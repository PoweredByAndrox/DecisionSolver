#include "pch.h"

#include "Audio.h"
#include "DXTK/Audio/WAVFileReader.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Physics.h"
#include "Camera.h"
#include "Console.h"

void Audio::Init()
{
	eflags = AudioEngine_Default;

#if defined(DEBUG) || defined(_DEBUG)
	eflags = eflags | AudioEngine_Debug;
#endif

	audEngine = make_unique<AudioEngine>(eflags);
	if (!audEngine.operator bool())
	{
		DebugTrace("Sound_system: Error audEngine = make_unique.\n");
		throw exception("audEngine == nullptr!!!");
		InitSoundSystem = false;
	}
	USES_CONVERSION;

	enumList = AudioEngine::GetRendererDetails();
	if (enumList.empty())
		Console::LogError("Audio: No Sound Devices Found!");
	else
	{
		for (auto it = enumList.cbegin(); it != enumList.cend(); ++it)
		{
			Console::LogInfo((boost::format("Audio: Found Sound Devices:\nID: %s,\nDescription: %s")
				% W2A(it->deviceId.c_str()) % W2A(it->description.c_str())).str());

			// it->deviceId.c_str() - this is the device/end-point identifier you can
			//     pass as a parameter to AudioEngine
			// it->description.c_str() - this is a textual description
		}
	}

	Listener = make_unique<AudioListener>();
	Emitter = make_unique<AudioEmitter>();
	
	Listener->SetOrientation(Vector3::Forward, Vector3::Up);
	Emitter->SetOrientation(Vector3::Forward, Vector3::Up);

	Emitter->InnerRadius = 10.f;

	InitSoundSystem = true;
}

void Audio::AddNewSound()
{
	auto ListSoundsFile = Application->getFS()->GetFileByType(_TypeOfFile::SOUNDS);
	for (int i = 0; i < ListSoundsFile.size(); i++)
	{
		soundEffect.push_back(make_unique<SoundEffect>(audEngine.get(), ListSoundsFile.at(i)->PathW.c_str()));

		sound.push_back(soundEffect.back()->CreateInstance(SoundEffectInstance_Use3D));
		if (!sound.back().operator bool())
		{
			DebugTrace("Sound_system: Error sound.push_back.\n");
			throw exception("sound[i] == null!!!");
		}
	}

	Emitter->ChannelCount = sound.size();
}

void Audio::Update()
{
	enumList = AudioEngine::GetRendererDetails();
	if (enumList.empty())
	{
		Console::LogError("Audio: No Sound Devices Found!");
		return;
	}

	Listener->Update(Application->getCamera()->GetEyePt(), Vector3::Up, 10.f);

	for (int i = 0; i < sound.size(); i++)
	{
		sound.at(i)->Apply3D(*Listener, *Emitter, false);
	}

	if (!Application->getPhysics()->GetPhysDynamicObject().empty())
		//Emitter->SetPosition(ToVec3(Application->getPhysics()->GetPhysDynamicObject().at(0)->getGlobalPose().p));
		Emitter->Update(ToVec3(Application->getPhysics()->GetPhysDynamicObject().at(0)->getGlobalPose().p), Vector3::Up, 10.f);

	if (!audEngine->Update())
	{
		if (audEngine->IsCriticalError())
		{
			DebugTrace("Sound_system: Error IsCriticalError.\n");
			throw exception("audEngine: has critical errors with audio!");
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
