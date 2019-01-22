 /*
 This code contains NVIDIA Confidential Information and is disclosed to you
 under a form of NVIDIA software license agreement provided separately to you.

 Notice
 NVIDIA Corporation and its licensors retain all intellectual property and
 proprietary rights in and to this software and related documentation and
 any modifications thereto. Any use, reproduction, disclosure, or
 distribution of this software and related documentation without an express
 license agreement from NVIDIA Corporation is strictly prohibited.

 ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
 NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
 THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
 MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.

 Information and code furnished is believed to be accurate and reliable.
 However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
 information or for any infringement of patents or other rights of third parties that may
 result from its use. No license is granted by implication or otherwise under any patent
 or patent rights of NVIDIA Corporation. Details are subject to change without notice.
 This code supersedes and replaces all information previously supplied.
 NVIDIA Corporation products are not authorized for use as critical
 components in life support devices or systems without express written approval of
 NVIDIA Corporation.

 Copyright (c) 2008-2018 NVIDIA Corporation. All rights reserved.
 Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
 Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  
*/

#include "pch.h"

#include "Picking.h"

using namespace Engine;	// PT: please DO NOT indent the whole file
using namespace physx;

Picking::Picking() :
	mSelectedActor(nullptr),
	mMouseJoint(nullptr),
	mMouseActor(nullptr),
	mMouseActorToDelete(nullptr),
	mDistanceToPicked(0.0f),
	mMouseScreenX(0),
	mMouseScreenY(0) 
{}

Picking::~Picking() {}

bool Picking::isPicked() const
{
	return mMouseJoint != 0;
}

void Picking::moveCursor(PxI32 x, PxI32 y)
{
	mMouseScreenX = x;
	mMouseScreenY = y;
}

/*void Picking::moveCursor(PxReal deltaDepth)
{
	const PxReal range[2] = { 0.0f, 1.0f };

	const PxReal r = (range[1] - range[0]);
	const PxReal d = (mMouseDepth - range[0])/r;
	const PxReal delta = deltaDepth*0.02f*(1.0f - d);

	mMouseDepth = PxClamp(mMouseDepth + delta, range[0], range[1]); 
}*/

void Picking::tick()
{
	if (mMouseJoint)
		moveActor(mMouseScreenX, mMouseScreenY);

	// PT: delete mouse actor one frame later to avoid crashes
	_SAFE_RELEASE(mMouseActorToDelete);
}

void Picking::computeCameraRay(PxVec3& orig, PxVec3& dir, PxI32 x, PxI32 y) const
{
	Vector3 vPickRayDir, vPickRayOrig, v;

	const Matrix pmatProj = camera->GetProjMatrix();
	Matrix mWorldView = camera->GetWorldMatrix() * camera->GetViewMatrix();
	Matrix m;

	POINT ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(DXUTGetHWND(), &ptCursor);

		// Compute the vector of the pick ray in screen space
	v.x = -(((2.f * ptCursor.x) / DXUTGetDXGIBackBufferSurfaceDesc()->Width)) / pmatProj._11;
	v.y = -(((2.f * ptCursor.y) / DXUTGetDXGIBackBufferSurfaceDesc()->Height) - 1) / pmatProj._22;
	v.z = 2.f;

		// Get the inverse view matrix
	m = XMMatrixInverse(NULL, mWorldView);

		// Transform the screen space pick ray into 3D space
	vPickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	vPickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	vPickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

	orig = PxVec3(vPickRayOrig.x, vPickRayOrig.y, vPickRayOrig.z);
	dir = PxVec3(vPickRayDir.x, vPickRayDir.y, vPickRayDir.z).getNormalized();
}

bool Picking::pick(int x, int y)
{
	PxScene *scene = nullptr;
	scene = PhysX->getScene();

	PxVec3 rayOrig, rayDir;
	computeCameraRay(rayOrig, rayDir, x, y);

	// raycast rigid bodies in scene
	PxRaycastHit hit; hit.shape = nullptr;
	PxRaycastBuffer hit1;

	scene->raycast(rayOrig, rayDir, PX_MAX_F32, hit1, PxHitFlag::ePOSITION);
	hit = hit1.block;

	if (hit.shape)
	{
		const char* shapeName = hit.shape->getName();
		PxRigidActor* actor = hit.actor;
		PX_ASSERT(actor);
		mSelectedActor = static_cast<PxRigidActor*>(actor->is<PxRigidDynamic>());

		if (!mSelectedActor)
			mSelectedActor = static_cast<PxRigidActor*>(actor->is<PxArticulationLink>());

		//ML::this is very useful to debug some collision problem
		PxTransform t = actor->getGlobalPose();
		PX_UNUSED(t);
		//shdfnd::printFormatted("id = %i\n PxTransform transform(PxVec3(%f, %f, %f), PxQuat(%f, %f, %f, %f))\n", reinterpret_cast<size_t>(actor->userData), t.p.x, t.p.y, t.p.z, t.q.x, t.q.y, t.q.z, t.q.w);
	}
	else
		mSelectedActor = 0;

	if (mSelectedActor)
		//if its a dynamic rigid body, joint it for dragging purposes:
		grabActor(hit.position, rayOrig);

#ifdef VISUALIZE_PICKING_RAYS
	Ray ray;
	ray.origin = rayOrig;
	ray.dir = rayDir;
	mRays.push_back(ray);
#endif
	return true;
}


//----------------------------------------------------------------------------//

PxActor *Picking::letGo()
{
	// let go any picked actor
	if (mMouseJoint)
	{
		mMouseJoint->release();
		mMouseJoint = nullptr;

		PX_ASSERT(!mMouseActorToDelete);
		mMouseActorToDelete = mMouseActor;	// PT: instead, we mark for deletion next frame
	}

	PxActor *returnedActor = mSelectedActor;

	mSelectedActor = nullptr;

	return returnedActor;
}

//----------------------------------------------------------------------------//

void Picking::grabActor(const PxVec3& worldImpact, const PxVec3& rayOrigin)
{
	if (!mSelectedActor || (mSelectedActor->getType() != PxActorType::eRIGID_DYNAMIC
			&& mSelectedActor->getType() != PxActorType::eARTICULATION_LINK))
		return;

	PxScene *scene = nullptr;
	scene = PhysX->getScene();

	PxPhysics *physics = nullptr;
	physics = PhysX->getPhysics();

	//create a shape less actor for the mouse
	{
		mMouseActor = physics->createRigidDynamic(PxTransform(worldImpact, PxQuat(PxIdentity)));
		mMouseActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		mMouseActor->setMass(1.0f);
		mMouseActor->setMassSpaceInertiaTensor(PxVec3(1.0f, 1.0f, 1.0f));

		scene->addActor(*mMouseActor);
	}
	PxRigidActor* pickedActor = static_cast<PxRigidActor*>(mSelectedActor);

#if USE_D6_JOINT_FOR_MOUSE
	mMouseJoint = PxD6JointCreate(*physics,
		mMouseActor,
		PxTransform(PxIdentity),
		pickedActor,
		PxTransform(pickedActor->getGlobalPose().transformInv(worldImpact)));
	mMouseJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
	mMouseJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
	mMouseJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
#elif USE_SPHERICAL_JOINT_FOR_MOUSE
	mMouseJoint = PxSphericalJointCreate(physics,
		mMouseActor,
		PxTransform(PxIdentity),
		pickedActor,
		PxTransform(pickedActor->getGlobalPose().transformInv(worldImpact)));
#else
	mMouseJoint = PxDistanceJointCreate(physics,
		mMouseActor,
		PxTransform(PxIdentity),
		pickedActor,
		PxTransform(pickedActor->getGlobalPose().transformInv(worldImpact)));
	mMouseJoint->setMaxDistance(0.0f);
	mMouseJoint->setMinDistance(0.0f);
	mMouseJoint->setDistanceJointFlags(PxDistanceJointFlag::eMAX_DISTANCE_ENABLED);
#endif

	mDistanceToPicked = (worldImpact - rayOrigin).magnitude();
}

//----------------------------------------------------------------------------//

void Picking::moveActor(int x, int y)
{
	if (!mMouseActor)
		return;

	PxVec3 rayOrig, rayDir;
	computeCameraRay(rayOrig, rayDir, x, y);

	const PxVec3 pos = rayOrig + mDistanceToPicked * rayDir;

	mMouseActor->setKinematicTarget(PxTransform(pos, PxQuat(PxIdentity)));
}
