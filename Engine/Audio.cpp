#include "pch.h"
#include "Audio.h"
#include "../Audio/WAVFileReader.h"

void Audio::Init()
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;

#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif

	audEngine = make_unique<AudioEngine>(eflags);
	if (audEngine == nullptr)
	{
		DebugTrace("Sound_system: Error audEngine = make_unique. Line: 14\n");
		throw exception("audEngine == nullptr!!!");
		InitSoundSystem = false;
	}
	InitSoundSystem = true;
}

void Audio::AddNewSound()
{
	ListSoundsFile = getFilesInFolder(&wstring(L"Sounds"), false, false);
	for (int i = 0; i < ListSoundsFile.size(); i++)
	{
		soundEffect[i] = make_unique<SoundEffect>(audEngine.get(), ListSoundsFile[i].c_str());

		sound.push_back(soundEffect[i]->CreateInstance());
		if (!sound[i])
		{
			DebugTrace("Sound_system: Error sound.push_back. Line: 31\n");
			throw exception("sound[i] == null!!!");
		}
	}
}

void Audio::Update()
{
	if (!audEngine->Update())
	{
		if (audEngine->IsCriticalError())
		{
			DebugTrace("Sound_system: Error IsCriticalError. Line: 43\n");
			throw exception("audEngine: has critical errors audio!");
		}
	}
}

void Audio::doPlay()
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->Play(true);
}

void Audio::changeSoundVol(float Vol)
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->SetVolume(Vol);
}

void Audio::changeSoundPan(float Pan)
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->SetPan(Pan);
}

void Audio::doPause()
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->Pause();
}

void Audio::doResume()
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->Resume();
}

void Audio::doStop()
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->Stop();
}
