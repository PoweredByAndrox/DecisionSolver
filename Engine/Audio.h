#pragma once
#if !defined(__SOUND_SYSTEM_H__)
#define __SOUND_SYSTEM_H__
#include "pch.h"

#include "File_system.h"
#include <xaudio2.h>
#include <X3DAudio.h>

constexpr auto fourccRIFF = 'FFIR';
constexpr auto fourccDATA = 'atad';
constexpr auto fourccFMT = ' tmf';
constexpr auto fourccWAVE = 'EVAW';
constexpr auto fourccXWMA = 'AMWX';
constexpr auto fourccDPDS = 'sdpd';

class Audio
{
private:
	struct AudioFile
	{
	private:
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;
		IXAudio2SourceVoice *source = nullptr;
		IXAudio2SubmixVoice *SubMix = nullptr;

		X3DAUDIO_EMITTER Emitter;
		X3DAUDIO_CONE EmitterCone;
		X3DAUDIO_DISTANCE_CURVE Emitter_LFE_Curve;

		X3DAUDIO_DSP_SETTINGS DSPSettings;
		FLOAT32 matrixCoefficients[8];

		//	WAV Buffer Data!
		BYTE *pDataBuffer = nullptr;

		HRESULT findChunk(HANDLE file, DWORD fourcc, DWORD *ChunkSize, DWORD *ChunkDataPosition);
		HRESULT readChunkData(HANDLE file, void *buffer, DWORD bufferSize, DWORD bufferOffset);
		HRESULT loadWAVFile(string filename, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer);
	public:
		// For std::vector!
		auto getBUFFER() { return &buffer; }
		// For std::vector!
		auto getWFX() { return &wfx; }
		// For std::vector!
		auto getSOURCE() { return source; }

		HRESULT Load(string FName, int Channels);
		void Update(Vector3 pos, X3DAUDIO_HANDLE X3DInstance, X3DAUDIO_LISTENER Listener, DWORD dwCalcFlags, int Channels);
		void Destroy();
	};

public:
	HRESULT Init();
	void Update();

	void doPause();
	void doResume();
	void doStop();
	void doPlay();

	void ReleaseAudio();

	void changeSoundVol(float Vol);
	void changeSoundPan(float Pan);

	void changeSoundPos(Vector3 pos) { this->pos = pos; }
	Vector3 getSoundPosition() { return pos; }
	Audio() {}
	~Audio() {}

	// ************
	bool IsInitSoundSystem() { return InitSoundSystem; }
private:
	// ************
	bool InitSoundSystem = false;

	vector<shared_ptr<Audio::AudioFile>> AFiles;

	X3DAUDIO_HANDLE X3DInstance;
	X3DAUDIO_LISTENER Listener = { XMFLOAT3(0,0,0) };
	X3DAUDIO_CONE Listener_DirectionalCone;
	XAUDIO2_VOICE_DETAILS VOICE_Details;

	Vector3 pos = Vector3::Zero;

	DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
		| X3DAUDIO_CALCULATE_LPF_DIRECT /*| X3DAUDIO_CALCULATE_LPF_REVERB
		| X3DAUDIO_CALCULATE_REVERB*/;
};
#endif // !__SOUND_SYSTEM_H__