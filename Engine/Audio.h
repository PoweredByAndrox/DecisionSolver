#pragma once
#if !defined(__SOUND_SYSTEM_H__)
#define __SOUND_SYSTEM_H__
#include "pch.h"

#include "File_system.h"
#include <xaudio2.h>
#include <X3DAudio.h>
#include <mmsystem.h>

class Audio
{
private:
	class XAudio2Callback: public IXAudio2VoiceCallback
	{
	public:
		HANDLE handle;
		XAudio2Callback():
			handle(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE)) {}

		virtual ~XAudio2Callback()
		{
			CloseHandle(handle);
		}
		void __stdcall OnStreamEnd() override
		{
			SetEvent(handle);
		}
		void __stdcall OnBufferStart(void *pBufferContext) override
		{
			SetEvent(handle);
		}

		void __stdcall OnVoiceProcessingPassStart(UINT32 BytesRequired) override {}
		void __stdcall OnVoiceProcessingPassEnd() override {}
		void __stdcall OnBufferEnd(void* pBufferContext) override {}
		void __stdcall OnLoopEnd(void* pBufferContext) override {}
		void __stdcall OnVoiceError(void* pBufferContext, HRESULT Error) override {}
	};

	class EngineCallBack: public IXAudio2EngineCallback
	{
	public:
		void __stdcall OnProcessingPassEnd() override {}
		void __stdcall OnProcessingPassStart() override {}
		void __stdcall OnCriticalError(HRESULT Error) override {}
	};

	EngineCallBack engineCallBack;

	class AudioFile
	{
	private:
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;
		IXAudio2SourceVoice *source;
		IXAudio2SubmixVoice *SubMix;

		XAudio2Callback voiceCallBack;

		//	WAV Buffer Data!
		vector<BYTE> pDataBuffer;
		WAVEFORMATEX WaveFormEx;
		HMMIO mmio;
		MMCKINFO riff, fmt, data;

		HRESULT loadWAVFile(string filename, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer);
	public:
		AudioFile(string FName, int Channels)
		{
			Load(FName, Channels);
		}
		AudioFile() {}
		~AudioFile() {}

		// For std::vector!
		auto getBUFFER() { return &buffer; }
		// For std::vector!
		auto getWFX() { return &wfx; }
		// For std::vector!
		auto getSOURCE() { return source; }

		auto getWaveFormatEx() { return WaveFormEx; }

		HRESULT Load(string FName, int Channels);
		void Update(Vector3 pos, X3DAUDIO_HANDLE X3DInstance, X3DAUDIO_LISTENER Listener,
			X3DAUDIO_EMITTER Emitter, X3DAUDIO_CONE EmitterCone, X3DAUDIO_DSP_SETTINGS DSPSettings, DWORD dwCalcFlags);
		void Destroy();
	};

public:
	HRESULT Init();
	void Update();

	static void doPause();
	static void doResume();
	static void doStop();
	static void doPlay();

	void ReleaseAudio();

	static void changeSoundVol(float Vol);
	void changeSoundPan(float Pan);

	void changeSoundPos(Vector3 pos) { this->pos = pos; }
	Vector3 getSoundPosition() { return pos; }
	Audio() {}
	~Audio() { ReleaseAudio(); }

	// ************
	bool IsInitSoundSystem() { return InitSoundSystem; }

	static void PlayFile(string File, bool NeedFind);
private:
	// ************
	bool InitSoundSystem = false;

	static vector<unique_ptr<Audio::AudioFile>> AFiles;
	static XAUDIO2_VOICE_DETAILS VOICE_Details;

	X3DAUDIO_HANDLE X3DInstance;
	X3DAUDIO_LISTENER Listener = { XMFLOAT3(0,0,0) };
	X3DAUDIO_CONE Listener_DirectionalCone;

	X3DAUDIO_EMITTER Emitter;
	X3DAUDIO_CONE EmitterCone;
	X3DAUDIO_DISTANCE_CURVE Emitter_LFE_Curve;

	X3DAUDIO_DSP_SETTINGS DSPSettings;

	Vector3 pos = Vector3::Zero;

	DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
		| X3DAUDIO_CALCULATE_LPF_DIRECT /*| X3DAUDIO_CALCULATE_LPF_REVERB
		| X3DAUDIO_CALCULATE_REVERB*/;
};
#endif // !__SOUND_SYSTEM_H__