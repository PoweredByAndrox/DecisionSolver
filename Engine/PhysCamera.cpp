#include "PhysCamera.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

void PhysCamera::TryStandup()
{
	// overlap with upper part
	//if (capsuleDesc.getType() == PxControllerShapeType::eBOX)
	//{
	//}
	if (capscDescActor.getType() == PxControllerShapeType::eCAPSULE)
	{
		PxScene *scene = Application->getPhysics()->getScene();
		PxSceneReadLock scopedLock(*scene);

		PxCapsuleController* capsuleCtrl = static_cast<PxCapsuleController*>(C_Control);

		PxReal r = capsuleCtrl->getRadius();
		PxReal dh = capscDescActor.height - 2.5f/*capsulecDescActor.CrouchHeight*/ - 2 * r;
		PxCapsuleGeometry geom(r, dh*.5f);

		PxExtendedVec3 position = C_Control->getPosition();
		PxVec3 pos((float)position.x, (float)position.y + capscDescActor.height * .5f + r, (float)position.z);
		PxQuat orientation(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f));

		PxOverlapBuffer hit;
		if (scene->overlap(geom, PxTransform(pos, orientation), hit, PxQueryFilterData(PxQueryFlag::eANY_HIT |
			PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC)))
		{
			state = State::Crouch;
			return;
		}
	}

	// if no hit, we can stand up
	//resizeStanding();

	state = State::Standup;

	//mDoStandup = false;
	//mIsCrouching = false;
}

void PhysCamera::Update()
{
	//if (state != State::Standup)
	//	TryStandup();
	//const PxExtendedVec3 &newPos = C_Control->getPosition();
	//if (state == State::Standup)
	//	;
	//else if (state == State::Crouch)
	//	;
}

void PhysCamera::Jump::Start(PxF32 Force)
{
	if (!CanJump)
		return;
	
	JumpTimes = 0.0f;
	ForceJump = Force;
	CanJump = false;
}

void PhysCamera::Jump::Stop()
{
	if (CanJump)
		return;
	CanJump = true;
	ForceJump = 0.0f;
}

//#include "Console.h"
float PhysCamera::Jump::getHeight(float elapsedTime)
{
	if (CanJump)
	{
		IsOnGround = true;
		IsInAir = false;
		return 0.0f;
	}
	else
	{
		IsInAir = true;
		IsOnGround = false;
	}

	//Console::LogInfo((boost::format("\nJump elapsedTime: %f") % elapsedTime).str().c_str());
	float MainGravity = Application->getPhysics()->getScene()->getGravity().y;

	JumpTimes += elapsedTime;
	//OutputDebugStringA(("\nJumpTimes: " + to_string(JumpTimes) + "\n").c_str());

	const float Result = (-MainGravity * -JumpTimes)
		* JumpTimes*JumpTimes + ForceJump*JumpTimes;
	//OutputDebugStringA((string("\nResult: ") + to_string(Result) + string("\n")).c_str());

	return Result * elapsedTime;
}
