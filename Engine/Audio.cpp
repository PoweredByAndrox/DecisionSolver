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

ToDo("Error Hanging!")
HRESULT Audio::AudioFile::loadWAVFile(string filename, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer)
{
	mmio = mmioOpenA(const_cast<LPSTR>(filename.c_str()), NULL, MMIO_READ);

	if (!mmio)
		return HRESULT_FROM_WIN32(GetLastError());

	riff.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioDescend(mmio, &riff, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		Engine::LogError("Audio: " + filename + ": Isn't a WAV File",
			filename + ": Isn't a WAV File",
			"Something is wrong with Audio: " + filename + ": Isn't a WAV File");
		return HRESULT_FROM_WIN32(GetLastError());
	}

	fmt.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioDescend(mmio, &fmt, &riff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
	{
		Engine::LogError("Audio: " + filename + ": Hasn't a FMT",
			filename + ": Hasn't a FMT",
			"Something is wrong with Audio: " + filename + ": Hasn't a FMT");
		return HRESULT_FROM_WIN32(GetLastError());
	}

	LONG readSize = mmioRead(mmio, reinterpret_cast<HPSTR>(&WaveFormEx), fmt.cksize);
	switch (readSize)
	{
	case 0:
		MessageBoxA(NULL, "FMT0", "Error", MB_OK);
		break;
	case -1:
		MessageBoxA(NULL, "FMT-1", "Error", MB_OK);
		break;
	default:
		if ((unsigned)readSize != fmt.cksize)
		{
			MessageBoxA(NULL, "DEF_FMT", "Error", MB_OK);
			return false;
		}
	}

	mmioAscend(mmio, &fmt, SEEK_SET);

	data.ckid = mmioFOURCC('d', 'a', 't', 'a');

	if (mmioDescend(mmio, &data, &riff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
		Engine::LogError((boost::format("Audio::LoadWAV()-> This File: %s Has a Bad Data.")
			% filename.c_str()).str(),
			"Audio::LoadWAV() failed!!!", "Sound: Something is wrong with Load WAV File! It Has a Bad Data");

	pDataBuffer.resize(data.cksize);

	if (mmioRead(
		mmio,
		reinterpret_cast<HPSTR>(&pDataBuffer[0]), data.cksize) != (signed)data.cksize)
		Engine::LogError((boost::format("Audio::LoadWAV()-> This File: %s Isn't a WAV File.") % filename).str(),
			"Audio::LoadWAV() failed!!!", "Sound: Something is wrong with Load WAV File! It Isn't a WAV File");
	
	mmioClose(mmio, MMIO_FHOPEN);

	buffer.AudioBytes = pDataBuffer.size();
	buffer.pAudioData = pDataBuffer.data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	return S_OK;
}
HRESULT Audio::AudioFile::Load(string FName, int Channels)
{
	SecureZeroMemory(&buffer, sizeof(buffer));
	SecureZeroMemory(&WaveFormEx, sizeof(WaveFormEx));

	EngineTrace(loadWAVFile(FName, wfx, buffer));

	if (Repeat)
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	EngineTrace(pXAudio2->CreateSourceVoice(&source, &WaveFormEx));

	source->Stop();
	source->FlushSourceBuffers();
	source->SubmitSourceBuffer(&buffer, nullptr);
	return S_OK;
}

void Audio::AudioFile::Update()
{
	source->SetOutputMatrix(NULL, DSPSettings.SrcChannelCount, DSPSettings.DstChannelCount,
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

	EngineTrace(pXAudio2->CreateMasteringVoice(&master));

	master->GetVoiceDetails(&VOICE_Details);
	pXAudio2->GetDeviceDetails(0, &DEVICE_Details);

	X3DAudioInitialize(DEVICE_Details.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, X3DInstance);
	if (!Lnr.operator bool()) Lnr = make_shared<Listerner>();

	InitSoundSystem = true;

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
		if (path(File).filename().string() != It.second)
		{
			Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, Repeat),
				path(File).filename().string()));
			Src.back().first->Init();
		}
	}
}

void Audio::DeleteSound(string ID)
{
	if (ID.empty()) return;
	for (size_t i = 0; i < Src.size(); i++)
	{
		if (path(ID).filename().string() != Src.at(i).second)
		{
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
		for (auto It: Src)
		{
			if (path(File).filename().string() != It.second)
			{
				Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, Repeat),
					path(File).filename().string()));
				Src.back().first->Init();
			}
		}
	}
}

void Audio::Update(Vector3 CamPos, Vector3 CamAhead, Vector3 CamUp)
{
	if (!X3DInstance || Src.empty() || !Lnr.operator bool()
		|| !Lnr->GetListener())
		return;

#if defined(_DEBUG)
	master->GetVoiceDetails(&VOICE_Details);
#endif

	for (auto ASrc: Src)
	{
		Lnr->Update(CamPos, CamAhead, CamUp);

		if (!ASrc.first || !ASrc.first->GetEmitter()) return;

		ASrc.first->Update();

		DSPSettings.SrcChannelCount = 2; DSPSettings.DstChannelCount = DEVICE_Details.OutputFormat.Format.nChannels;

		X3DAudioCalculate(X3DInstance, Lnr->GetListener(), ASrc.first->GetEmitter(), ASrc.first->GetCalcFlags(),
			&DSPSettings);
		ASrc.first->GetAUDFile()->Update();
	}
}

void Audio::doPlay()
{
	for (auto It: Src)
	{
		It.first->Play();
	}
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
	//if (g_audioState.pSubmixVoice)
	//{
	//	g_audioState.pSubmixVoice->DestroyVoice();
	//	g_audioState.pSubmixVoice = NULL;
	//}
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
	//SecureZeroMemory(&Emitter, sizeof(X3DAUDIO_EMITTER));
	//SecureZeroMemory(&EmitterCone, sizeof(X3DAUDIO_CONE));
	//SecureZeroMemory(&Emitter_LFE_Curve, sizeof(X3DAUDIO_DISTANCE_CURVE));
}

void Audio::changeSoundVol(float Vol)
{
	//for (size_t i = 0; i < AFiles.size(); i++)
	//{
	//	AFiles.at(i)->getSOURCE()->SetVolume(Vol);
	//}
}

void Audio::changeSoundPan(float Pan)
{
	//for (size_t i = 0; i < sound.size(); i++)
	//	sound.at(i)->SetPan(Pan);
}

void Audio::PlayFile(string File, bool RepeatIt, bool NeedFind)
{
	if (File.empty())
	{
		Engine::LogError("Something is wrong with Engine::PlayFile",
			"Something is wrong with Engine::PlayFile",
			(boost::format("LUA (Audio):\nFile: %s Doesn't Exist") % File).str());
		return;
	}

	for (auto It: Src)
	{
		It.first->Stop();
		It.first->Destroy();
	}
	Src.clear();

	if (NeedFind)
	{
		auto SoundFile = Application->getFS()->GetFile(File);
		if (SoundFile.operator bool())
		{
			Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, RepeatIt),
				path(File).filename().string()));
			Src.back().first->Init();
		}
	}
	else // If We Have A Full Path To File!!!
	{
		Src.push_back(make_pair(make_shared<Source>(File, VOICE_Details.InputChannels, RepeatIt),
			path(File).filename().string()));
		Src.back().first->Init();
	}
}

void Audio::doPause()
{
	for (auto It: Src)
	{
		It.first->Pause();
	}
}

void Audio::doStop()
{
	for (auto It: Src)
	{
		It.first->Stop();
	}
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
	EngineTrace(Code = AUDFile->getSOURCE()->Start());
	if (Code == S_OK)
	{
		isPause = false;
		isPlay = true;
	}

	return Code;
}

HRESULT Audio::Source::Pause()
{
	ToDo("Do Play Or Pause This");
	HRESULT Code = S_OK;
	UINT isPlay = pVoiceState.BuffersQueued;
	if (AUDFile->getSOURCE() && isPlay != 0) // If It's not Playing
	{
		EngineTrace(Code = AUDFile->getSOURCE()->Stop());
		if (Code == S_OK)
		{
			isPause = true;
			isPlay = false;
		}
	}

	return Code;
}

HRESULT Audio::Source::Stop()
{
	HRESULT Code = S_OK;
	EngineTrace(Code = AUDFile->getSOURCE()->Stop());
	AUDFile->getSOURCE()->FlushSourceBuffers();
	AUDFile->getSOURCE()->SubmitSourceBuffer(AUDFile->getBUFFER(), nullptr);
	if (Code == S_OK)
	{
		isPause = false;
		isPlay = false;
	}

	return Code;
}

void Audio::Source::Destroy()
{
	SecureZeroMemory(&Emitter, sizeof(X3DAUDIO_EMITTER));
	SecureZeroMemory(&EmitterCone, sizeof(X3DAUDIO_CONE));

	AUDFile->Destroy();
}
