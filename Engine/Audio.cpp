#include "pch.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Console.h"
#include "Camera.h"

#include "Audio.h"

IXAudio2MasteringVoice *master = nullptr;
IXAudio2 *audio = nullptr;

HRESULT Audio::AudioFile::findChunk(HANDLE file, DWORD fourcc, DWORD *ChunkSize, DWORD *ChunkDataPosition)
{
	if (SetFilePointer(file, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType = 0ul,
		dwChunkDataSize = 0ul,
		dwRIFFDataSize = 0ul,
		dwFileType = 0ul,
		bytesRead = 0ul,
		dwOffset = 0ul;

	HRESULT hr = S_OK;
	while (hr == S_OK)
	{
		DWORD dwRead = 0ul;
		if (!ReadFile(file, &dwChunkType, sizeof(DWORD), &dwRead, nullptr))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (!ReadFile(file, &dwChunkDataSize, sizeof(DWORD), &dwRead, nullptr))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (!ReadFile(file, &dwFileType, sizeof(DWORD), &dwRead, nullptr))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(file, dwChunkDataSize, nullptr, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			*ChunkSize = dwChunkDataSize;
			*ChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize)
			return S_FALSE;
	}

	return S_OK;
}
HRESULT Audio::AudioFile::readChunkData(HANDLE file, void *buffer, DWORD bufferSize, DWORD bufferOffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(file, bufferOffset, nullptr, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwRead = 0ul;
	if (!ReadFile(file, buffer, bufferSize, &dwRead, nullptr))
		hr = HRESULT_FROM_WIN32(GetLastError());

	return hr;
}
HRESULT Audio::AudioFile::loadWAVFile(string filename, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer)
{
	HANDLE file = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
		OPEN_EXISTING, 0, nullptr);

	if (INVALID_HANDLE_VALUE == file)
		return HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(file, 0, nullptr, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD chunkSize = 0ul,
		chunkPosition = 0ul,
		fileType = 0ul;
	EngineTrace(findChunk(file, fourccRIFF, &chunkSize, &chunkPosition));

	EngineTrace(readChunkData(file, &fileType, sizeof(DWORD), chunkPosition));
	if (fileType != fourccWAVE)
		return S_FALSE;

	EngineTrace(findChunk(file, fourccFMT, &chunkSize, &chunkPosition));
	EngineTrace(readChunkData(file, &wfx, chunkSize, chunkPosition));

	EngineTrace(findChunk(file, fourccDATA, &chunkSize, &chunkPosition));
	pDataBuffer = (BYTE *)malloc(chunkSize);
	EngineTrace(readChunkData(file, pDataBuffer, chunkSize, chunkPosition));

	buffer.AudioBytes = chunkSize;
	buffer.pAudioData = pDataBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	return S_OK;
}

HRESULT Audio::AudioFile::Load(string FName, int Channels)
{
	EngineTrace(loadWAVFile(FName, wfx, buffer));
	EngineTrace(audio->CreateSourceVoice(&source, (WAVEFORMATEX *)&wfx));

	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	EngineTrace(source->SubmitSourceBuffer(&buffer));

	DSPSettings.SrcChannelCount = 2;
	DSPSettings.DstChannelCount = Channels;
	DSPSettings.pMatrixCoefficients = new FLOAT32(1.f);

	ZeroMemory(&Emitter, sizeof(X3DAUDIO_EMITTER));
	ZeroMemory(&EmitterCone, sizeof(X3DAUDIO_CONE));
	EmitterCone.OuterVolume = 1.0f;
	EmitterCone.OuterLPF = 1.0f;
	EmitterCone.OuterReverb = 1.0f;
	Emitter.pCone = &EmitterCone;

	Emitter.InnerRadius = 50.f;
	Emitter.InnerRadiusAngle = X3DAUDIO_PI / 4.0f;
	Emitter.pVolumeCurve = (X3DAUDIO_DISTANCE_CURVE *)&X3DAudioDefault_LinearCurve;

	Emitter.ChannelCount = 2;
	Emitter.ChannelRadius = 1.0f;
	Emitter.CurveDistanceScaler = 50.f;
	FLOAT32 EmitterAzimuths[2];
	Emitter.pChannelAzimuths = EmitterAzimuths;

	Emitter.Position = { 0, 0, 0 };
	Emitter.OrientFront = { 0, 0, 1 };
	Emitter.OrientTop = { 0, 1, 0 };

	return S_OK;
}

void Audio::AudioFile::Update(Vector3 pos, X3DAUDIO_HANDLE X3DInstance, X3DAUDIO_LISTENER Listener,
	DWORD dwCalcFlags, int Channels)
{
	if (!X3DInstance)
		return;

	Emitter.Position = pos;

	X3DAudioCalculate(X3DInstance, &Listener, &Emitter, dwCalcFlags, &DSPSettings);
	source->SetFrequencyRatio(DSPSettings.DopplerFactor);
	source->SetOutputMatrix(master, Channels, DSPSettings.DstChannelCount, DSPSettings.pMatrixCoefficients);
}

HRESULT Audio::Init()
{
	USES_CONVERSION;
	EngineTrace(XAudio2Create(&audio, XAUDIO2_STOP_ENGINE_WHEN_IDLE | XAUDIO2_1024_QUANTUM));

#if defined (DEBUG)
	XAUDIO2_DEBUG_CONFIGURATION debug = { 0 };
	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	debug.BreakMask = XAUDIO2_LOG_ERRORS;
	audio->SetDebugConfiguration(&debug, 0);
#endif

	DWORD dwChannelMask = 0ul;
	EngineTrace(audio->CreateMasteringVoice(&master));

	master->GetVoiceDetails(&VOICE_Details);
	EngineTrace(master->GetChannelMask(&dwChannelMask));
	X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, X3DInstance);

	auto SoundFiles = Application->getFS()->GetFileByType(_TypeOfFile::SOUNDS);
	for (size_t i = 0; i < SoundFiles.size(); i++)
	{
		AFiles.push_back(make_shared<AudioFile>());
		AFiles.back()->Load(SoundFiles.at(i)->PathA, VOICE_Details.InputChannels);
	}

	ZeroMemory(&Listener, sizeof(X3DAUDIO_LISTENER));
	ZeroMemory(&Listener_DirectionalCone, sizeof(X3DAUDIO_CONE));

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
	X3DAUDIO_VECTOR posCam = { Application->getCamera()->GetEyePt().x,
		Application->getCamera()->GetEyePt().y,
		Application->getCamera()->GetEyePt().z },
		WAh = { Application->getCamera()->GetWorldAhead().x,Application->getCamera()->GetWorldAhead().y,
	   Application->getCamera()->GetWorldAhead().z },

		WUp = { Application->getCamera()->GetWorldUp().x,Application->getCamera()->GetWorldUp().y,
	Application->getCamera()->GetWorldUp().z };

	Listener.OrientFront = WAh;
	Listener.OrientTop = WUp;
	Listener.Position = posCam;

	for (size_t i = 0; i < AFiles.size(); i++)
	{
		AFiles.at(i)->Update(pos, X3DInstance, Listener, dwCalcFlags, VOICE_Details.InputChannels);
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
	if (SubMix)
	{
		SubMix->DestroyVoice();
		SubMix = nullptr;
	}
	if (source)
	{
		source->DestroyVoice();
		source = nullptr;
	}

	ZeroMemory(&buffer, sizeof(XAUDIO2_BUFFER));
	ZeroMemory(&wfx, sizeof(WAVEFORMATEXTENSIBLE));
	ZeroMemory(&DSPSettings, sizeof(X3DAUDIO_DSP_SETTINGS));
	ZeroMemory(&Emitter, sizeof(X3DAUDIO_EMITTER));
	ZeroMemory(&EmitterCone, sizeof(X3DAUDIO_CONE));
	ZeroMemory(&Emitter_LFE_Curve, sizeof(X3DAUDIO_DISTANCE_CURVE));

	if (pDataBuffer)
	{
		free(pDataBuffer);
		pDataBuffer = nullptr;
	}
}

void Audio::ReleaseAudio()
{
	doStop();
	for (size_t i = 0; i < AFiles.size(); i++)
	{
		AFiles.at(i)->Destroy();
	}

	AFiles.~vector();
	//if (g_audioState.pSubmixVoice)
	//{
	//	g_audioState.pSubmixVoice->DestroyVoice();
	//	g_audioState.pSubmixVoice = NULL;
	//}

	//if (audio)
	//{
	//	audio->StopEngine();
	//	audio = nullptr;
	//}
	//if (master)
	//{
	//	master->DestroyVoice();
	//	master = nullptr;
	//}
}

void Audio::changeSoundVol(float Vol)
{
	//for (size_t i = 0; i < sound.size(); i++)
	//	sound.at(i)->SetVolume(Vol);
}

void Audio::changeSoundPan(float Pan)
{
	//for (size_t i = 0; i < sound.size(); i++)
	//	sound.at(i)->SetPan(Pan);
}

void Audio::doPause()
{
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
	}
}
