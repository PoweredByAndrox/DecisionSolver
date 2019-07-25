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

#include "Camera.h"

using namespace physx;

bool Picking::isPicked() const
{
	return mMouseJoint != nullptr;
}

void Picking::tick()
{
	if (mMouseJoint)
		moveActor(Application->getMouse()->GetState().x, Application->getMouse()->GetState().y);

		// PT: delete mouse actor one frame later to avoid crashes
	SAFE_release(mMouseActorToDelete);
}

void Picking::computeCameraRay(PxVec3 &Pos, PxVec3 &dir, PxI32 x, PxI32 y) const
{
	Matrix ProjMat = Application->getCamera()->GetProjMatrix(),
		ViewMat = Application->getCamera()->GetViewMatrix();

	float VP_Height = Application->getViewPort().Height, VP_Width = Application->getViewPort().Width;

	Vector3 nearVector = XMVector3Unproject(Vector3(float(x), float(VP_Height - y),
		0.0f), 0.0f, 0.0f, VP_Width, VP_Height, 0.0f, 1.0f, ProjMat, Matrix::Identity, ViewMat),

		farVector = XMVector3Unproject(Vector3(float(x), float(VP_Height - y),
			1.0f), 0.0f, 0.0f, VP_Width, VP_Height, 0.0f, 1.0f, ProjMat, Matrix::Identity, ViewMat),

		tmp;

	Pos = ToPxVec3(Application->getCamera()->GetEyePt());
	tmp = farVector - nearVector;
	dir = ToPxVec3(tmp).getNormalized();
}

bool Picking::pick(int x, int y)
{
	PxVec3 rayOrig, rayDir;
	computeCameraRay(rayOrig, rayDir, x, y);

	// raycast rigid bodies in scene
	PxRaycastHit hit; hit.shape = nullptr;
	PxRaycastBuffer hit1;

	Application->getPhysics()->getScene()->raycast(rayOrig, rayDir, 1000.f, hit1, PxHitFlag::ePOSITION);
	hit = hit1.block;

	if (hit.shape)
	{
		LPCSTR shapeName = hit.shape->getName();
		PxRigidActor *actor = hit.actor;
		PX_ASSERT(actor);
		mSelectedActor = static_cast<PxRigidActor*>(actor->is<PxRigidDynamic>());

		if (!mSelectedActor)
			mSelectedActor = static_cast<PxRigidActor*>(actor->is<PxArticulationLink>());
	}
	else
		mSelectedActor = 0;

	if (mSelectedActor)
	{
		Application->SetWireFrame(true);
		//if its a dynamic rigid body, joint it for dragging purposes:
		grabActor(hit.position, rayOrig);
	}
	else
		Application->SetWireFrame(false);

#ifdef VISUALIZE_PICKING_RAYS
	Ray ray;
	ray.origin = rayOrig;
	ray.dir = rayDir;
	mRays.push_back(ray);
#endif
	return true;
}

void Picking::ReleasePick()
{
	// let go any picked actor
	if (mMouseJoint)
	{
		mMouseJoint->release();
		mMouseJoint = nullptr;

		PX_ASSERT(!mMouseActorToDelete);
		mMouseActorToDelete = mMouseActor;	// PT: instead, we mark for deletion next frame
	}
}

//----------------------------------------------------------------------------//

void Picking::grabActor(const PxVec3& worldImpact, const PxVec3& rayOrigin)
{
	if (!mSelectedActor || (mSelectedActor->getType() != PxActorType::eRIGID_DYNAMIC
			&& mSelectedActor->getType() != PxActorType::eARTICULATION_LINK))
		return;

	//create a shape less actor for the mouse
	{
		mMouseActor = Application->getPhysics()->getPhysics()->createRigidDynamic(PxTransform(worldImpact, PxQuat(PxIdentity)));
		mMouseActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
		mMouseActor->setMass(1.0f);
		mMouseActor->setMassSpaceInertiaTensor(PxVec3(1.0f, 1.0f, 1.0f));

		Application->getPhysics()->getScene()->addActor(*mMouseActor);
	}
	PxRigidActor *pickedActor = static_cast<PxRigidActor *>(mSelectedActor);

#if USE_D6_JOINT_FOR_MOUSE
	mMouseJoint = PxD6JointCreate(*Application->getPhysics()->getPhysics(),
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

	mDistanceToPicked = (worldImpact.multiply(rayOrigin)).magnitude();
}

//----------------------------------------------------------------------------//

void Picking::moveActor(int x, int y)
{
	if (!mMouseActor)
		return;

	PxVec3 rayOrig, rayDir;
	computeCameraRay(rayOrig, rayDir, x, y);

	const PxVec3 pos = rayOrig + 10.0f * PxVec3(rayDir.x, -rayDir.y, rayDir.z);

	mMouseActor->setKinematicTarget(PxTransform(pos, PxQuat(PxIdentity)));
}
