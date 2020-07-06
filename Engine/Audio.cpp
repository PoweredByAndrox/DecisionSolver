#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Console.h"
#include "Camera.h"
#include "File_system.h"

#include "Audio.h"

IXAudio2MasteringVoice *master = nullptr;
XAUDIO2_VOICE_STATE pVoiceState = {};
IXAudio2 *Audio::pXAudio2 = nullptr;

XAUDIO2_VOICE_DETAILS Audio::VOICE_Details = {};
XAUDIO2_DEVICE_DETAILS Audio::DEVICE_Details = {};

shared_ptr<Audio::Listerner> Audio::Lnr;
vector<pair<shared_ptr<Audio::Source>, string>> Audio::Src;

X3DAUDIO_HANDLE Audio::X3DInstance = {};
X3DAUDIO_DSP_SETTINGS Audio::DSPSettings = {};

class VoiceCallback: public IXAudio2VoiceCallback
{
public:
	HANDLE hBufferEndEvent;
	VoiceCallback(): hBufferEndEvent(CreateEventW(NULL, FALSE, FALSE, NULL)) {}
	~VoiceCallback() { CloseHandle(hBufferEndEvent); }

	//Called when the voice has just finished playing a contiguous audio stream.
	STDMETHOD_(void, OnStreamEnd) (THIS);

	//Unused methods are stubs
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS);
	STDMETHOD_(void, OnVoiceProcessingPassStart) (THIS_ UINT32 BytesRequired);
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext);
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error);
} voiceCallback;

HRESULT Audio::AudioFile::loadWAVFile(string filename, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer)
{
	mmio = mmioOpenA(const_cast<LPSTR>(filename.c_str()), nullptr, MMIO_READ);

	if (!mmio)
		return HRESULT_FROM_WIN32(ERROR_OPEN_FAILED);

	riff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioDescend(mmio, &riff, nullptr, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		Engine::LogError("Audio: " + filename + ": Isn't a WAV File",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Something is wrong with Audio: " + filename + ": Isn't a WAV File");
		return HRESULT_FROM_WIN32(E_FAIL);
	}

	fmt.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioDescend(mmio, &fmt, &riff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
	{
		Engine::LogError("Audio: " + filename + ": Hasn't a FMT",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Something is wrong with Audio: " + filename + ": Hasn't a FMT");
		return HRESULT_FROM_WIN32(E_FAIL);
	}

	LONG readSize = mmioRead(mmio, reinterpret_cast<HPSTR>(&WaveFormEx), fmt.cksize);
	switch (readSize)
	{
	case 0:
	{
		Engine::LogError("Audio: " + filename + ": Hasn't a FMT",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Something is wrong with Audio: " + filename + ": Has The Wrong FMT Size (= 0)");
		return HRESULT_FROM_WIN32(E_FAIL);
	}
	break;
	case -1:
	{
		Engine::LogError("Audio: " + filename + ": Hasn't a FMT",
			string(__FILE__) + ": " + to_string(__LINE__),
			"Something is wrong with Audio: " + filename + ": Has The Wrong FMT Size (= -1)");
		return HRESULT_FROM_WIN32(E_FAIL);
	}
	break;
	default:
		if ((unsigned)readSize != fmt.cksize)
		{
			Engine::LogError("Audio: " + filename + ": Hasn't a FMT",
				string(__FILE__) + ": " + to_string(__LINE__),
				"Something is wrong with Audio: " + filename + ": Has The Wrong FMT Size (readSize != fmt.cksize)");
			return HRESULT_FROM_WIN32(E_FAIL);
		}
	}

	mmioAscend(mmio, &fmt, SEEK_SET);

	data.ckid = mmioFOURCC('d', 'a', 't', 'a');

	if (mmioDescend(mmio, &data, &riff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
		Engine::LogError((boost::format("Audio::LoadWAV()-> This File: %s Has a Bad Data.") % filename.c_str()).str(),
			string(__FILE__) + ": " + to_string(__LINE__),
			"Sound: Something is wrong with Load WAV File! It Has a Bad Data");

	pDataBuffer.resize(data.cksize);

	if (mmioRead(
		mmio,
		reinterpret_cast<HPSTR>(&pDataBuffer[0]), data.cksize) != (signed)data.cksize)
		Engine::LogError((boost::format("Audio::LoadWAV()-> This File: %s Isn't a WAV File.") % filename).str(),
			string(__FILE__) + ": " + to_string(__LINE__),
			"Sound: Something is wrong with Load WAV File! It Isn't a WAV File");

	mmioClose(mmio, MMIO_FHOPEN);

	buffer.AudioBytes = pDataBuffer.size();
	buffer.pAudioData = pDataBuffer.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	return S_OK;
}

HRESULT Audio::hr = S_OK;
HRESULT Audio::AudioFile::Load(string FName, int Channels)
{
	if (FName.empty() || Channels == 0) return ERROR_BAD_ARGUMENTS;
	
	hr = S_OK;
	SecureZeroMemory(&buffer, sizeof(buffer));
	SecureZeroMemory(&WaveFormEx, sizeof(WaveFormEx));
	hr = loadWAVFile(FName, wfx, buffer);
	if (FAILED(hr))
		return hr;

	if (Repeat)
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	hr = pXAudio2->CreateSourceVoice(&source, &WaveFormEx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &voiceCallback);
	if (FAILED(hr))
		return hr;

	source->Stop();
	source->FlushSourceBuffers();
	source->SubmitSourceBuffer(&buffer, nullptr);
	return S_OK;
}

void Audio::AudioFile::Update()
{
	if (!source) return;
	source->SetOutputMatrix(nullptr, DSPSettings.SrcChannelCount, DSPSettings.DstChannelCount,
		DSPSettings.pMatrixCoefficients);
	source->SetFrequencyRatio(DSPSettings.DopplerFactor);

#if defined(_DEBUG)
	source->GetState(&pVoiceState);
#endif
}

HRESULT Audio::Init()
{
	if (InitSoundSystem) return S_OK;

	SecureZeroMemory(&DSPSettings, sizeof(X3DAUDIO_DSP_SETTINGS));
	EngineTrace(XAudio2Create(&pXAudio2));

	UINT32 pCound;
	pXAudio2->GetDeviceCount(&pCound);
	pXAudio2->GetDeviceDetails(0, &DEVICE_Details);

	if (pCound == 0) return S_OK; // DNS_ERROR_RRL_INVALID_WINDOW_SIZE

	EngineTrace(pXAudio2->CreateMasteringVoice(&master));

	master->GetVoiceDetails(&VOICE_Details);

	X3DAudioInitialize(DEVICE_Details.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, X3DInstance);
	if (!Lnr.operator bool()) Lnr = make_shared<Listerner>();

	DSPSettings.SrcChannelCount = 2; DSPSettings.DstChannelCount = DEVICE_Details.OutputFormat.Format.nChannels;

	InitSoundSystem = true;
	
	ResumeThread(voiceCallback.hBufferEndEvent);
	return S_OK;
}

void Audio::AddNewFile(string File, bool Repeat)
{
	if (File.empty()) return;
	if (Src.empty())
	{
		Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, Repeat),
			path(File).filename().string()));
		Src.back().first->Init();
		return;
	}
	for (auto It: Src)
	{
		if (path(File).filename().string() == It.second)
			return;
	}

	Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, Repeat),
		path(File).filename().string()));
	Src.back().first->Init();
}

void Audio::Remove(string ID)
{
	if (ID.empty()) return;
	for (size_t i = 0; i < Src.size(); i++)
	{
		if (Src.at(i).second == path(ID).filename().string())
		{
			Src.at(i).first->Stop();
			Src.at(i).first->Destroy();
			Src.erase(Src.begin() + i);
		}
	}
}

void Audio::AddNewFile(vector<string> Files, bool Repeat)
{
	for (auto File: Files)
	{
		if (File.empty()) continue;
		if (Src.empty())
		{
			Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, Repeat),
				path(File).filename().string()));
			Src.back().first->Init();
			return;
		}
		bool IfItBreak = false;
		for (auto It: Src)
		{
			if (path(File).filename().string() == It.second)
			{
				IfItBreak = true;
				break;
			}
		}

		if (!IfItBreak)
		{
			Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, Repeat),
				path(File).filename().string()));
			Src.back().first->Init();
		}
	}
}

void Audio::Update(Vector3 CamPos, Vector3 CamAhead, Vector3 CamUp)
{
	if (!X3DInstance || Src.empty() || !Lnr.operator bool()
		|| !Lnr->GetListener() || !master)
		return;

#if defined(_DEBUG)
	master->GetVoiceDetails(&VOICE_Details);
#endif

	for (auto ASrc: Src)
	{
		Lnr->Update(CamPos, CamAhead, CamUp);

		if (!ASrc.first || !ASrc.first->GetEmitter()) return;

		ASrc.first->Update();

		X3DAudioCalculate(X3DInstance, Lnr->GetListener(), ASrc.first->GetEmitter(), ASrc.first->GetCalcFlags(),
			&DSPSettings);
		ASrc.first->GetAUDFile()->Update();
	}
}

HRESULT Audio::doPlay()
{
	for (auto It: Src)
	{
		EngineTrace(It.first->Play());
	}

	return S_OK;
}

HRESULT Audio::doPlay(string Index)
{
	for (auto It: Src)
	{
		if (contains(It.second, Index))
			EngineTrace(It.first->Play());
	}

	return S_OK;
}

shared_ptr<Audio::Source> Audio::GetSound(string Index)
{
	for (auto It: Src)
	{
		if (contains(It.second, Index))
			return It.first;
	}
	return shared_ptr<Source>();
}

void Audio::AudioFile::Destroy()
{
	//if (SubMix)
	//{
	//	SubMix->DestroyVoice();
	//	SubMix = nullptr;
	//}
	if (source)
		source->DestroyVoice();

	SecureZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
	SecureZeroMemory(&WaveFormEx, sizeof(WAVEFORMATEX));
}

void Audio::ReleaseAudio()
{
	SAFE_DELETE_ARRAY(DSPSettings.pMatrixCoefficients);
	doStop();

	for (size_t i = 0; i < Src.size(); i++)
	{
		Src.at(i).first->Stop();
		Src.at(i).first->Destroy();
	}
	Src.clear();

	if (master)
		master->DestroyVoice();

	if (pXAudio2)
		pXAudio2->StopEngine();

	SecureZeroMemory(&DSPSettings, sizeof(X3DAUDIO_DSP_SETTINGS));
}

HRESULT Audio::changeVol(float Vol)
{
	for (auto It: Src)
	{
		EngineTrace(It.first->GetAUDFile()->getSOURCE()->SetVolume(Vol));
	}
	return S_OK;
}

HRESULT Audio::changePitch(float Pitch)
{
	for (auto It: Src)
	{
		EngineTrace(It.first->GetAUDFile()->getSOURCE()->SetFrequencyRatio(Pitch));
	}
	return S_OK;
}

HRESULT Audio::changePan(float Pan)
{
	float matrix[16];
	memset(matrix, 0, sizeof(float) * 16);

	if (DSPSettings.SrcChannelCount == 1)
	{
		// Mono panning
		float left = (Pan >= 0) ? (1.f - Pan) : 1.f;
		left = std::min<float>(1.f, left);
		left = std::max<float>(-1.f, left);

		float right = (Pan <= 0) ? (-Pan - 1.f) : 1.f;
		right = std::min<float>(1.f, right);
		right = std::max<float>(-1.f, right);

		matrix[0] = left;
		matrix[1] = right;
	}
	else if (DSPSettings.SrcChannelCount == 2)
	{
		// Stereo panning
		if (-1.f <= Pan && Pan <= 0.f)
		{
			matrix[0] = .5f * Pan + 1.f;    // .5 when Pan is -1, 1 when Pan is 0
			matrix[1] = .5f * -Pan;         // .5 when Pan is -1, 0 when Pan is 0
			matrix[2] = 0.f;                //  0 when Pan is -1, 0 when Pan is 0
			matrix[3] = Pan + 1.f;          //  0 when Pan is -1, 1 when Pan is 0
		}
		else
		{
			matrix[0] = -Pan + 1.f;         //  1 when Pan is 0,   0 when Pan is 1
			matrix[1] = 0.f;                //  0 when Pan is 0,   0 when Pan is 1
			matrix[2] = .5f * Pan;          //  0 when Pan is 0, .5f when Pan is 1
			matrix[3] = .5f * -Pan + 1.f;   //  1 when Pan is 0. .5f when Pan is 1
		}
	}

	for (auto It: Src)
	{
		EngineTrace(It.first->GetAUDFile()->getSOURCE()->SetOutputMatrix(nullptr,
			DSPSettings.SrcChannelCount, DSPSettings.DstChannelCount, matrix));
	}
	return S_OK;
}

HRESULT Audio::PlayFile(string File, bool RepeatIt, bool NeedFind)
{
	if (File.empty())
	{
		Engine::LogError("Something is wrong with Engine::PlayFile",
			string(__FILE__) + ": " + to_string(__LINE__),
			(boost::format("LUA (Audio):\nFile: %s Doesn't Exist") % File).str());
		return E_FAIL;
	}

	if (NeedFind)
	{
		auto SoundFile = Application->getFS()->GetFile(File);
		if (SoundFile.operator bool())
		{
			Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, RepeatIt),
				path(File).filename().string()));
			Src.back().first->Init();
			EngineTrace(Src.back().first->Play());
		}
	}
	else // If We Have A Full Path To File!!!
	{
		Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, RepeatIt),
			path(File).filename().string()));
		Src.back().first->Init();
		EngineTrace(Src.back().first->Play());
	}

	return S_OK;
}

HRESULT Audio::doStop()
{
	for (auto It: Src)
	{
		EngineTrace(It.first->Stop());
	}
	
	return S_OK;
}

HRESULT Audio::doStop(string Index)
{
	for (auto It: Src)
	{
		if (contains(It.second, Index))
			EngineTrace(It.first->Stop());
	}
	
	return S_OK;
}

void Audio::Listerner::Init()
{
	SecureZeroMemory(&Listener, sizeof(X3DAUDIO_LISTENER));
	SecureZeroMemory(&Listener_DirectionalCone, sizeof(X3DAUDIO_CONE));

	Listener_DirectionalCone.InnerAngle = X3DAUDIO_PI * 5.0f / 6.0f;
	Listener_DirectionalCone.OuterAngle = X3DAUDIO_PI * 11.0f / 6.0f;
	Listener_DirectionalCone.InnerVolume = 1.0f;
	Listener_DirectionalCone.OuterVolume = 0.75f;
	Listener_DirectionalCone.OuterLPF = 0.25f;
	Listener_DirectionalCone.InnerReverb = 0.708f;
	Listener_DirectionalCone.OuterReverb = 1.0f;
	Listener.pCone = &Listener_DirectionalCone;
}

void Audio::Listerner::Update(Vector3 CamPos, Vector3 CamAhead, Vector3 CamUp)
{
	Listener.OrientFront = X3DAUDIO_VECTOR{ CamAhead.x, CamAhead.y, CamAhead.z };
	Listener.OrientTop = X3DAUDIO_VECTOR{ CamUp.x, CamUp.y, CamUp.z };
	Listener.Position = X3DAUDIO_VECTOR{ CamPos.x, CamPos.y, CamPos.z };
}

Audio::Source::Source(string FName, int Channels, bool Repeat)
{
	AUDFile = make_shared<AudioFile>(FName, Channels, Repeat);
	Init();
}

void Audio::Source::Init()
{
	SecureZeroMemory(&Emitter, sizeof(X3DAUDIO_EMITTER));
	SecureZeroMemory(&EmitterCone, sizeof(X3DAUDIO_CONE));
	DSPSettings.pMatrixCoefficients = new FLOAT32[VOICE_Details.InputChannels * VOICE_Details.InputChannels];

	//SecureZeroMemory(&Emitter_LFE_Curve, sizeof(X3DAUDIO_DISTANCE_CURVE));
}

void Audio::Source::Update()
{
	EmitterCone.OuterVolume = 1.0f;
	EmitterCone.OuterLPF = 1.0f;
	EmitterCone.OuterReverb = 1.0f;
	Emitter.pCone = &EmitterCone;

	Emitter.InnerRadius = 25.f;
	Emitter.InnerRadiusAngle = 1.0f;
	Emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE *)&X3DAudioDefault_LinearCurve;

	Emitter.ChannelCount = DSPSettings.SrcChannelCount;
	Emitter.ChannelRadius = 1.0f;
	Emitter.CurveDistanceScaler = 30.f;
	float EmitterAzimuths[1] = { 0.0f };
	Emitter.pChannelAzimuths = EmitterAzimuths;

	Emitter.Position = pos;
	Emitter.OrientFront = { 0, 0, 1 };
	Emitter.OrientTop = { 0, 1, 0 };
}

HRESULT Audio::Source::Play()
{
	HRESULT Code = S_OK;
	EngineTrace(AUDFile->getSOURCE()->FlushSourceBuffers());
	EngineTrace(AUDFile->getSOURCE()->SubmitSourceBuffer(AUDFile->getBUFFER(), nullptr));
	EngineTrace(Code = AUDFile->getSOURCE()->Start());
	if (Code == S_OK)
	{
		isPlay = true;
		isStop = false;
	}

	return Code;
}

HRESULT Audio::Source::Stop()
{
	HRESULT Code = S_OK;
	EngineTrace(Code = AUDFile->getSOURCE()->Stop());
	EngineTrace(AUDFile->getSOURCE()->FlushSourceBuffers());
	EngineTrace(AUDFile->getSOURCE()->SubmitSourceBuffer(AUDFile->getBUFFER(), nullptr));
	if (Code == S_OK)
	{
		isPlay = false;
		isStop = true;
	}

	return Code;
}

void Audio::Source::Destroy()
{
	SecureZeroMemory(&Emitter, sizeof(X3DAUDIO_EMITTER));
	SecureZeroMemory(&EmitterCone, sizeof(X3DAUDIO_CONE));

	AUDFile->Destroy();
}

void VoiceCallback::OnStreamEnd(void)
{
	SetEvent(hBufferEndEvent);
}

void VoiceCallback::OnVoiceProcessingPassEnd()
{
}

void VoiceCallback::OnVoiceProcessingPassStart(UINT32 SamplesRequired)
{
}

void VoiceCallback::OnBufferEnd(void *pBufferContext)
{
	//Audio::UpdateSounds();
}

void VoiceCallback::OnBufferStart(void *pBufferContext)
{
}

void VoiceCallback::OnLoopEnd(void *pBufferContext)
{
}

void VoiceCallback::OnVoiceError(void *pBufferContext, HRESULT Error)
{
}
