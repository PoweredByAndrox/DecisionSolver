#pragma once
#if !defined(__SOUND_SYSTEM_H__)
#define __SOUND_SYSTEM_H__
#include "pch.h"

#include "DXSDKAudio2.h"
#include "X3DAudio.h"

class Audio
{
private:
	class AudioFile
	{
	private:
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer = {};
		IXAudio2SourceVoice *source;
		IXAudio2SubmixVoice *SubMix;

		//	WAV Buffer Data!
		vector<BYTE> pDataBuffer;
		WAVEFORMATEX WaveFormEx;
		HMMIO mmio;
		MMCKINFO riff, fmt, data;

		bool Repeat = false;

		HRESULT loadWAVFile(string filename, WAVEFORMATEXTENSIBLE &wfx, XAUDIO2_BUFFER &buffer);
	public:
		AudioFile(string FName, int Channels, bool Repeat): Repeat(Repeat)
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

		bool IsItRepeat() { return Repeat; }

		HRESULT Load(string FName, int Channels);
		void Update();
		void Destroy();
	};
	// Has All Sound And Update Them In 3D Positional
	struct Source
	{
	public:
		Source(string FName, int Channels, bool Repeat = true);
		~Source() {}

		void Init();
		void Update();
		auto GetEmitter() { return &Emitter; }
		Vector3 getSoundPosition() { return Vector3(pos.x, pos.y, pos.z); }
		DWORD GetCalcFlags() { return dwCalcFlags; }
		shared_ptr<AudioFile> GetAUDFile() { return AUDFile; }

		void setSoundPosition(Vector3 pos) { this->pos = X3DAUDIO_VECTOR{ pos.x, pos.y, pos.z }; }

		HRESULT Play();
		HRESULT Pause();
		HRESULT Stop();

		bool IsPlay() { return isPlay; }
		bool IsPause() { return isPause; }

		void Destroy();
	private:
		X3DAUDIO_EMITTER Emitter = {};
		X3DAUDIO_CONE EmitterCone = {};
		X3DAUDIO_DISTANCE_CURVE Emitter_LFE_Curve = {};
		X3DAUDIO_VECTOR pos = { 0, 0, 0 };
		shared_ptr<AudioFile> AUDFile;
		DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DOPPLER
			| X3DAUDIO_CALCULATE_LPF_DIRECT /*| X3DAUDIO_CALCULATE_LPF_REVERB
			| X3DAUDIO_CALCULATE_REVERB*/;

		bool isPlay = false, isPause = true;
	};
	// Listener it has only one cuz it's a camera
	struct Listerner
	{
	public:
		Listerner() { Init(); }
		~Listerner() {}

		void Init();
		void Update(Vector3 CamPos, Vector3 CamAhead, Vector3 CamUp);

		auto GetListener() { return &Listener; }
	private:
		X3DAUDIO_LISTENER Listener = { };
		X3DAUDIO_CONE Listener_DirectionalCone = {};
	};
	static vector<pair<shared_ptr<Source>, string>> Src;
	static shared_ptr<Listerner> Lnr;

public:
	HRESULT Init();
	
	void AddNewFile(string File, bool Repeat);
	void AddNewFile(vector<string> Files, bool Repeat);
	void DeleteSound(string ID);

	void Update(Vector3 CamPos, Vector3 CamAhead, Vector3 CamUp);

	static void doPause();
	static void doStop();
	static void doPlay();

	void ReleaseAudio();

	static void changeSoundVol(float Vol);
	void changeSoundPan(float Pan);

	vector<pair<shared_ptr<Source>, string>> getAllSources() { return Src; }
	shared_ptr<Listerner> getListerner() { return Lnr; }

	bool IsInitSoundSystem() { return InitSoundSystem; }

	static void PlayFile(string File, bool RepeatIt, bool NeedFind);
private:
	bool InitSoundSystem = false;
	static IXAudio2 *pXAudio2;

	static XAUDIO2_VOICE_DETAILS VOICE_Details;
	static XAUDIO2_DEVICE_DETAILS DEVICE_Details;

	static X3DAUDIO_HANDLE X3DInstance;

	static X3DAUDIO_DSP_SETTINGS DSPSettings;
};
#endif // !__SOUND_SYSTEM_H__