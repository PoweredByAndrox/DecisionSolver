// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2018 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "pch.h"

//#include "PxScene.h"
//#include "PxQueryReport.h"
//#include "PxBatchQueryDesc.h"
//#include "extensions/PxJoint.h"
//#include "PxRigidDynamic.h"
//#include "extensions/PxDistanceJoint.h"
//#include "extensions/PxSphericalJoint.h"
//#include "PxArticulationLink.h"
//#include "PxShape.h"
#include "Picking.h"

//#if PX_UNIX_FAMILY
//#include <stdio.h>
//#endif

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
	// compute picking ray
//	const PxVec3 rayOrig = unProject(x, y, 0.0f);	// PT: what the frell is that?
	const PxVec3 rayOrig = PxVec3(Camera->GetEyePt().x, Camera->GetEyePt().y, Camera->GetEyePt().z);
	const PxVec3 rayDir = (unProject(x, y, 1.0f) - rayOrig).getNormalized();

	orig = rayOrig;
	dir = rayDir;
}

bool Picking::pick(int x, int y)
{
	
	auto *scene = PhysX->getScene();

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

Engine::PxActor* Picking::letGo()
{
	// let go any picked actor
	if (mMouseJoint)
	{
		mMouseJoint->release();
		mMouseJoint = nullptr;

		//	SAFE_RELEASE(mMouseActor);			// PT: releasing immediately crashes
		PX_ASSERT(!mMouseActorToDelete);
		mMouseActorToDelete = mMouseActor;	// PT: instead, we mark for deletion next frame
	}

	PxActor* returnedActor = mSelectedActor;

	mSelectedActor = nullptr;

	return returnedActor;
}

//----------------------------------------------------------------------------//

void Picking::grabActor(const PxVec3& worldImpact, const PxVec3& rayOrigin)
{
	if (!mSelectedActor
		|| (mSelectedActor->getType() != PxActorType::eRIGID_DYNAMIC
			&& mSelectedActor->getType() != PxActorType::eARTICULATION_LINK))
		return;

	auto *scene = PhysX->getScene();
	auto *physics = PhysX->getPhysics();

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

//----------------------------------------------------------------------------//

PxVec3 Picking::unProject(int x, int y, float depth) const
{
	const auto projection = Camera->GetProjMatrix();
	//const PxTransform view = getCamera().getViewMatrix().getInverse();
	auto view = Camera->GetViewMatrix().Invert();

	const PxF32 outX = (float)x / (float)DXUTGetWindowWidth();
	const PxF32 outY = (float)y / (float)DXUTGetWindowHeight();

	return //unproject(projection, view, 
		PxVec3(outX * 2 - 1, outY * 2 - 1, depth * 2 - 1);
}

//----------------------------------------------------------------------------//

void Picking::project(const physx::PxVec3& v, int& xi, int& yi, float& depth) const
{
	auto projection = Camera->GetProjMatrix();
	//const PxTransform 
	auto view = Camera->GetViewMatrix().Invert();

/*
	Vector3 ray_origin = Vector3(0, 0, 0);
	for (float x = -1.f; x <= 1.f; x += 2.f / W)
	{
		for (float y = -1.f; y <= 1.f; y += 2.f / H) 
		{
			Vector3 ray_direction = Normalize(Vector3(x, y, -1.f)) - ray_origin;
			Vector3 ray_in_model = Unproject(ray_direction, 0.f, 0.f, 
											 width, height, znear, zfar, 
											 proj, view, model);
		}
	}
*/

	//Vector4 screenPoint(v.x, v.y, v.z, 1);

	//Matrix invprojview = -(view * Camera->GetProjMatrix());

	//Vector4 nearPoint = XMVectorMultiply(invprojview, screenPoint);
	//if (nearPoint.w) nearPoint *= 1.0f / nearPoint.w;
	//PxVec3 pos(nearPoint.x, nearPoint.y, nearPoint.z);

	/////* Map x, y and z to range 0-1 */
	//pos.x = (pos.x + 1) * 0.5f;
	//pos.y = (pos.y + 1) * 0.5f;
	//pos.z = (pos.z + 1) * 0.5f;

	///* Map x,y to viewport */
	//pos.x *= DXUTGetWindowWidth();
	//pos.y *= DXUTGetWindowHeight();

	//depth = (float)pos.z;

	//xi = (int)(pos.x + 0.5);
	//yi = (int)(pos.y + 0.5);
}
