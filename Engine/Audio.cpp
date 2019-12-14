#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Console.h"
#include "Camera.h"

#include "Audio.h"
#include <wrl.h>

IXAudio2MasteringVoice *master = nullptr;
Microsoft::WRL::ComPtr<IXAudio2> audio;
XAUDIO2_VOICE_STATE pVoiceState;

vector<unique_ptr<Audio::AudioFile>> Audio::AFiles;
XAUDIO2_VOICE_DETAILS Audio::VOICE_Details;
XAUDIO2_DEVICE_DETAILS Audio::DEVICE_Details;

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
	SecureZeroMemory(&wfx, sizeof(wfx));

	EngineTrace(loadWAVFile(FName, wfx, buffer));

	EngineTrace(audio->CreateSourceVoice(&source, &WaveFormEx));

	source->Stop();
	source->FlushSourceBuffers();
	source->SubmitSourceBuffer(&buffer, nullptr);
	return S_OK;
}

void Audio::AudioFile::Update(Vector3 pos, X3DAUDIO_HANDLE X3DInstance, X3DAUDIO_LISTENER Listener,
	X3DAUDIO_EMITTER Emitter, X3DAUDIO_CONE EmitterCone, X3DAUDIO_DSP_SETTINGS DSPSettings, DWORD dwCalcFlags)
{
	source->SetOutputMatrix(NULL, DSPSettings.SrcChannelCount, DSPSettings.DstChannelCount,
		DSPSettings.pMatrixCoefficients);
	source->SetFrequencyRatio(DSPSettings.DopplerFactor);

	source->GetState(&pVoiceState);
}
HRESULT Audio::Init()
{
	SecureZeroMemory(&Listener, sizeof(X3DAUDIO_LISTENER));
	SecureZeroMemory(&Listener_DirectionalCone, sizeof(X3DAUDIO_CONE));
	SecureZeroMemory(&Emitter, sizeof(X3DAUDIO_EMITTER));
	SecureZeroMemory(&EmitterCone, sizeof(X3DAUDIO_CONE));
	SecureZeroMemory(&Emitter_LFE_Curve, sizeof(X3DAUDIO_DISTANCE_CURVE));
	SecureZeroMemory(&DSPSettings, sizeof(X3DAUDIO_DSP_SETTINGS));

	EngineTrace(XAudio2Create(&audio));

	EngineTrace(audio->CreateMasteringVoice(&master));

	master->GetVoiceDetails(&VOICE_Details);
	audio->GetDeviceDetails(0, &DEVICE_Details);

	X3DAudioInitialize(DEVICE_Details.OutputFormat.dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, X3DInstance);

	auto SoundFiles = Application->getFS()->GetFileByType(_TypeOfFile::SOUNDS);
	for (size_t i = 0; i < SoundFiles.size(); i++)
	{
		AFiles.push_back(make_unique<AudioFile>(SoundFiles.at(i)->PathA, VOICE_Details.InputChannels));
	}

	Listener_DirectionalCone.InnerAngle = X3DAUDIO_PI * 5.0f / 6.0f;
	Listener_DirectionalCone.OuterAngle = X3DAUDIO_PI * 11.0f / 6.0f;
	Listener_DirectionalCone.InnerVolume = 1.0f;
	Listener_DirectionalCone.OuterVolume = 0.75f;
	Listener_DirectionalCone.OuterLPF = 0.25f;
	Listener_DirectionalCone.InnerReverb = 0.708f;
	Listener_DirectionalCone.OuterReverb = 1.0f;
	Listener.pCone = &Listener_DirectionalCone;

	InitSoundSystem = true;

	return S_OK;
}

void Audio::Update()
{
	if (!X3DInstance)
		return;

	master->GetVoiceDetails(&VOICE_Details);

	Vector3
		Eye = Application->getCamera()->GetEyePt(),
		WorldAhead = Application->getCamera()->GetWorldAhead(),
		WorldUp = Application->getCamera()->GetWorldUp();
	X3DAUDIO_VECTOR
		posCam = { Eye.x, Eye.y, Eye.z },
		WAh = { WorldAhead.x, WorldAhead.y, WorldAhead.z },
		WUp = { WorldUp.x, WorldUp.y, WorldUp.z };

	Listener.OrientFront = WAh;
	Listener.OrientTop = WUp;
	Listener.Position = posCam;

	Emitter.Position = pos;
	EmitterCone.OuterVolume = 1.0f;
	EmitterCone.OuterLPF = 1.0f;
	EmitterCone.OuterReverb = 1.0f;
	Emitter.pCone = &EmitterCone;

	Emitter.InnerRadius = 50.f;
	Emitter.InnerRadiusAngle = 1.0f;
	Emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE *)&X3DAudioDefault_LinearCurve;

	Emitter.ChannelCount = VOICE_Details.InputChannels;
	Emitter.ChannelRadius = 1.0f;
	Emitter.CurveDistanceScaler = 50.f;
	float EmitterAzimuths[1] = { 0.0f };
	Emitter.pChannelAzimuths = EmitterAzimuths;

	Emitter.Position = { 0, 0, 0 };
	Emitter.OrientFront = { 0, 0, 1 };
	Emitter.OrientTop = { 0, 1, 0 };

	DSPSettings.SrcChannelCount = DSPSettings.DstChannelCount = Emitter.ChannelCount;

	if (!DSPSettings.pMatrixCoefficients)
	{
		DSPSettings.pMatrixCoefficients =
			new FLOAT32[DSPSettings.SrcChannelCount * DSPSettings.DstChannelCount];
		memset(DSPSettings.pMatrixCoefficients, 0,
			sizeof(FLOAT32) * (DSPSettings.SrcChannelCount * DSPSettings.DstChannelCount));
	}

	X3DAudioCalculate(X3DInstance, &Listener, &Emitter, dwCalcFlags, &DSPSettings);

	for (size_t i = 0; i < AFiles.size(); i++)
	{
		AFiles.at(i)->Update(Vector3(pos.x, pos.y, pos.z), X3DInstance, Listener, Emitter, EmitterCone,
			DSPSettings, dwCalcFlags);
	}
}

void Audio::doPlay()
{
	for (size_t i = 0; i < AFiles.size(); i++)
	{
		AFiles.at(i)->getSOURCE()->Start();
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
	{
		source->DestroyVoice();
		source = nullptr;
	}

	SecureZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
	SecureZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));
}

void Audio::ReleaseAudio()
{
	SAFE_DELETE(DSPSettings.pMatrixCoefficients);

	doStop();
	for (size_t i = 0; i < AFiles.size(); i++)
	{
		AFiles.at(i)->Destroy();
	}
	//if (g_audioState.pSubmixVoice)
	//{
	//	g_audioState.pSubmixVoice->DestroyVoice();
	//	g_audioState.pSubmixVoice = NULL;
	//}

	if (master)
	{
		master->DestroyVoice();
		master = nullptr;
	}

	if (audio)
	{
		audio->StopEngine();
		audio = nullptr;
	}

	SecureZeroMemory(&DSPSettings, sizeof(X3DAUDIO_DSP_SETTINGS));
	SecureZeroMemory(&Emitter, sizeof(X3DAUDIO_EMITTER));
	SecureZeroMemory(&EmitterCone, sizeof(X3DAUDIO_CONE));
	SecureZeroMemory(&Emitter_LFE_Curve, sizeof(X3DAUDIO_DISTANCE_CURVE));
}

void Audio::changeSoundVol(float Vol)
{
	for (size_t i = 0; i < AFiles.size(); i++)
	{
		AFiles.at(i)->getSOURCE()->SetVolume(Vol);
	}
}

void Audio::changeSoundPan(float Pan)
{
	//for (size_t i = 0; i < sound.size(); i++)
	//	sound.at(i)->SetPan(Pan);
}

void Audio::PlayFile(string File, bool NeedFind)
{
	if (File.empty())
	{
		Engine::LogError("Something is wrong with Engine::PlayFile",
			"Something is wrong with Engine::PlayFile",
			(boost::format("LUA (Audio):\nFile: %s Doesn't Exist") % File).str());
		return;
	}

	for (size_t i = 0; i < AFiles.size(); i++)
	{
		AFiles.at(i)->Destroy();
	}
	AFiles.clear();

	if (NeedFind)
	{
		auto SoundFile = Application->getFS()->GetFile(File);
		if (SoundFile.operator bool())
			AFiles.push_back(make_unique<AudioFile>(SoundFile->PathA, VOICE_Details.InputChannels));
	}
	else // If We Have A Full Path To File!!!
		AFiles.push_back(make_unique<AudioFile>(File, VOICE_Details.InputChannels));
}

void Audio::doPause()
{
	for (size_t i = 0; i < AFiles.size(); i++)
	{
		UINT isPlay = pVoiceState.BuffersQueued;
		if (AFiles.at(i)->getSOURCE() && isPlay != 0)
			AFiles.at(i)->getSOURCE()->Stop();
	}

	//for (size_t i = 0; i < sound.size(); i++)
	//	sound.at(i)->Pause();
}

void Audio::doResume()
{
	//for (size_t i = 0; i < sound.size(); i++)
	//	sound.at(i)->Resume();
}

void Audio::doStop()
{
	for (size_t i = 0; i < AFiles.size(); i++)
	{
		AFiles.at(i)->getSOURCE()->Stop();
		AFiles.at(i)->getSOURCE()->FlushSourceBuffers();
		AFiles.at(i)->getSOURCE()->SubmitSourceBuffer(AFiles.at(i)->getBUFFER(), nullptr);
	}
}
