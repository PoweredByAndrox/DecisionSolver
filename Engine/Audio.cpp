#include "pch.h"

#include "Audio.h"
#include "../Audio/WAVFileReader.h"

void EngineNS::Audio::Init()
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;

#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif

	audEngine = make_unique<AudioEngine>(eflags);
	if (!audEngine.operator bool())
	{
		DebugTrace("Sound_system: Error audEngine = make_unique.\n");
		throw exception("audEngine == nullptr!!!");
		InitSoundSystem = false;
	}
	InitSoundSystem = true;
}

void EngineNS::Audio::AddNewSound()
{
	ListSoundsFile = Application->getFS()->getFilesInFolder(&wstring(L"Sounds"), false, false);
	for (int i = 0; i < ListSoundsFile.size(); i++)
	{
		soundEffect[i] = make_unique<SoundEffect>(audEngine.get(), ListSoundsFile.at(i).c_str());

		sound.push_back(soundEffect[i]->CreateInstance());
		if (!sound.at(i))
		{
			DebugTrace("Sound_system: Error sound.push_back.\n");
			throw exception("sound[i] == null!!!");
		}
	}
}

void EngineNS::Audio::Update()
{
	if (!audEngine->Update())
	{
		if (audEngine->IsCriticalError())
		{
			DebugTrace("Sound_system: Error IsCriticalError.\n");
			throw exception("audEngine: has critical errors with audio!");
		}
	}
}

void EngineNS::Audio::doPlay()
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->Play(true);
}

void EngineNS::Audio::changeSoundVol(float Vol)
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->SetVolume(Vol);
}

void EngineNS::Audio::changeSoundPan(float Pan)
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->SetPan(Pan);
}

void EngineNS::Audio::doPause()
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->Pause();
}

void EngineNS::Audio::doResume()
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->Resume();
}

void EngineNS::Audio::doStop()
{
	for (int i = 0; i < sound.size(); i++)
		sound.at(i)->Stop();
}
