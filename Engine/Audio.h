#pragma once
#ifndef __SOUND_SYSTEM_H__
#define __SOUND_SYSTEM_H__
#include "File_system.h"
#include "pch.h"
#include <Audio.h>

#pragma comment(lib, "DirectXTKAudioDX.lib")

namespace Engine
{
	class Audio : public File_system
	{
	public:
		void Init();
		void AddNewSound();
		void Update();

		// ************
			// Needed for UI (main menu)
		void doPause();
		void doResume();
		void doStop();
		void doPlay();

		// ************
		void changeSoundVol(float Vol);
		void changeSoundPan(float Pan);

		Audio() {}
		~Audio() {}

		// ************
		bool IsInitSounSystem() { return InitSoundSystem; }
		auto *getStaticsSound() { return &audEngine.get()->GetStatistics(); }

	private:
		// ************
		bool InitSoundSystem = false;

		// ************
		unique_ptr<AudioEngine> audEngine;
		unique_ptr<SoundEffect> soundEffect[FILENAME_MAX];
		vector<unique_ptr<SoundEffectInstance>> sound;
		vector<wstring> ListSoundsFile;

		HRESULT hr = S_OK;
	};
};

#endif // !__SOUND_SYSTEM_H__