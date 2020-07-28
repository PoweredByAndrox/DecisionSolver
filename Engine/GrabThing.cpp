#include "pch.h"
#include "GrabThing.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

#include "Console.h"
#include "File_system.h"
#include "Physics.h"
#include "Camera.h"
#include "PhysCamera.h"

PxFixedJoint *joint = nullptr;

void GrabThing::CheckType(/*int ID*/)
{
	if (!Application->getPhysics() || !Application->getCamera()) return;
	Application->getPhysics()->SpawnObject(ToPxVec3(Application->getCamera()->GetEyePt() + Vector3(0, 2.5f, -5.f)));
}

void GrabThing::Drop(/*int ID*/)
{
	SAFE_release(joint);
}

void GrabThing::Grab()
{
	auto mActor = Application->getCamera()->getPCam()->getController();
	if (!Application->getCamera()->getPCam() || !mActor) return;

	PxRigidDynamic *actor = mActor->getActor();

	PxI32 attachNum = 5;
	PxVec3 footPos = toVec3(mActor->getFootPosition());

	PxOverlapHit hitBuffer[5] = {};
	PxMemZero(&hitBuffer, sizeof(hitBuffer));
	PxFilterData fd(2ul, 0, 0, 0);
	PxOverlapBuffer buf(hitBuffer, 5);

	Application->getPhysics()->getScene()->overlap(PxSphereGeometry(3.f), PxTransform(footPos), buf,
		PxQueryFilterData(fd, PxQueryFlag::eANY_HIT));
	
		PxU32 hitNum = buf.getNbAnyHits();
		if (hitNum == 0)
			return;
		
		PxShape *capsuleShape = nullptr;
		actor->getShapes(&capsuleShape, 1);
		const PxCapsuleGeometry &capGeom = capsuleShape->getGeometry().capsule();

		attachNum = hitNum == (PxU32)-1 ? attachNum : hitNum;
		
		PxF32 boxExtent = 0.4f;

		PxTransform	lastTransform = PxTransform(actor->getGlobalPose().q);
		PxVec3 hookPos = PxVec3(0.0f, capGeom.halfHeight + capGeom.radius + boxExtent * 2, capGeom.radius * 2.0f);

		for (PxI32 i = 0; i < attachNum; ++i)
		{
			PxRigidDynamic *attachedActor = hitBuffer[i].actor->is<PxRigidDynamic>();
			//setCollisionGroup(attachedActor, PICKING_COLLISION_GROUP);

			joint = PxFixedJointCreate(*Application->getPhysics()->getPhysics(), actor, lastTransform,
				attachedActor, PxTransform(hookPos));
			joint->setConstraintFlag(PxConstraintFlag::eCOLLISION_ENABLED, false);
			//mFixedJoints.push_back(joint);

			hookPos.y += (boxExtent*1.5f);
		}
	
}
