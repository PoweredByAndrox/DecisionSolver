#include "pch.h"

#include "Audio.h"
#include "DXTK/Audio/WAVFileReader.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Physics.h"
#include "Camera.h"
#include "Console.h"

X3DAUDIO_VECTOR ToXV(Vector3 Some)
{
	X3DAUDIO_VECTOR Vec;
	Vec.x = Some.x;
	Vec.y = Some.y;
	Vec.z = Some.z;

	return Vec;
}

Vector3 ToVec(X3DAUDIO_VECTOR Some)
{
	Vector3 Vec;
	Vec.x = Some.x;
	Vec.y = Some.y;
	Vec.z = Some.z;

	return Vec;
}


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

	Listener->OrientFront = ToXV(Application->getCamera()->GetWorldAhead());
	Listener->OrientTop = ToXV(Application->getCamera()->GetWorldUp());
	Listener->Position = ToXV(Application->getCamera()->GetEyePt());

	Emitter->OrientFront = ToXV(Application->getCamera()->GetWorldAhead());
	Emitter->OrientTop = ToXV(Application->getCamera()->GetWorldUp());
	Emitter->Position = ToXV(Vector3::Zero);
	//Emitter->InnerRadius = 10.f;
	//Emitter->CurveDistanceScaler = FLT_MIN;

	InitSoundSystem = true;
}

void Audio::AddNewSound()
{
	auto ListSoundsFile = Application->getFS()->GetFileByType(_TypeOfFile::SOUNDS);
	for (size_t i = 0; i < ListSoundsFile.size(); i++)
	{
		soundEffect.push_back(make_unique<SoundEffect>(audEngine.get(), ListSoundsFile.at(i)->PathW.c_str()));

		sound.push_back(soundEffect.back()->CreateInstance(SoundEffectInstance_Use3D));
		if (!sound.back().operator bool())
		{
			DebugTrace("Sound_system: Error sound.push_back.\n");
			throw exception("sound[i] == null!!!");
		}
	}

	//Emitter->ChannelCount = sound.size();
}

void Audio::Update()
{
	if (AudioEngine::GetRendererDetails().empty())
	{
		Console::LogError("Audio: No Sound Devices Found!");
		return;
	}

	if (!audEngine->Update())
	{
		if (audEngine->IsCriticalError())
		{
			DebugTrace("Sound_system: Error IsCriticalError.\n");
			throw exception("audEngine: has critical errors with audio!");
		}
	}

	//Listener->Position = ToXV(Application->getCamera()->GetEyePt());
	Listener->Update(Application->getCamera()->GetEyePt(), Application->getCamera()->GetWorldAhead(), Application->getframeTime());

	if (!Application->getPhysics()->GetPhysDynamicObject().empty())
	{
		Emitter->Update(ToVec3(Application->getPhysics()->GetPhysDynamicObject().at(0)->getGlobalPose().p),
			Application->getCamera()->GetWorldUp(), Application->getframeTime());
	}

	for (size_t i = 0; i < sound.size(); i++)
	{
		sound.at(i)->Apply3D(*Listener, *Emitter, false);
	}
}

void Audio::doPlay()
{
	for (size_t i = 0; i < sound.size(); i++)
		sound.at(i)->Play(true);
}

void Audio::changeSoundVol(float Vol)
{
	for (size_t i = 0; i < sound.size(); i++)
		sound.at(i)->SetVolume(Vol);
}

void Audio::changeSoundPan(float Pan)
{
	for (size_t i = 0; i < sound.size(); i++)
		sound.at(i)->SetPan(Pan);
}

void Audio::doPause()
{
	for (size_t i = 0; i < sound.size(); i++)
		sound.at(i)->Pause();
}

void Audio::doResume()
{
	for (size_t i = 0; i < sound.size(); i++)
		sound.at(i)->Resume();
}

void Audio::doStop()
{
	for (size_t i = 0; i < sound.size(); i++)
		sound.at(i)->Stop();
}
