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

PxVec3 Picking::unProject(int x, int y, float depth) const
{
	PxU32 windowWidth = Application->getWorkAreaSize(Application->GetHWND()).x;
	PxU32 windowHeight = Application->getWorkAreaSize(Application->GetHWND()).y;

	const PxF32 outX = (float)x / (float)windowWidth;
	const PxF32 outY = (float)y / (float)windowHeight;

	Vector3 Done;//= Application->UnProject(outX * 2, outY * 2, depth * 2);
	return PxVec3(Done.x, Done.y, Done.z);
}
void Picking::project(const PxVec3 &v, int &xi, int &yi, float &depth) const
{
	Vector3 pos;//= Application->Project(v.x, v.y, v.z);
		//* Map x, y and z to range 0-1 */
	pos.x = (pos.x + 1) * 0.5f;
	pos.y = (pos.y + 1) * 0.5f;
	pos.z = (pos.z + 1) * 0.5f;

	PxU32 windowWidth = Application->getWorkAreaSize(Application->GetHWND()).x;
	PxU32 windowHeight = Application->getWorkAreaSize(Application->GetHWND()).y;

		/* Map x,y to viewport */
	pos.x *= windowWidth;
	pos.y *= windowHeight;

	depth = (float)pos.z;

	xi = (int)(pos.x + 0.5);
	yi = (int)(pos.y + 0.5);
}

void Picking::computeCameraRay(PxVec3 &Pos, PxVec3 &dir, PxI32 x, PxI32 y) const
{
	const Matrix pmatProj = Application->getCamera()->GetProjMatrix();

	// Compute the vector of the pick ray in screen space
	Vector3 v;
	v.x = (+2.0f * x /  - 1.0f) / pmatProj._11;
	v.y = (-2.0f * y /  + 1.0f) / pmatProj._22;
	v.z = 1.0f;
//	vPickRayDir = Vector3(v.x, v.y, 0.0f);
//	vPickRayPos = Application->getCamera()->GetEyePt();
	Vector3 rayOrigin = XMVector3Unproject(Vector4(x, y, 0.f, 1.f), Application->getViewPort().TopLeftX, Application->getViewPort().TopLeftY,
		Application->getViewPort().Width,
		Application->getViewPort().Height, 0.f, 1000.f, Application->getCamera()->GetProjMatrix(),
		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetWorldMatrix());
	Vector3 rayDirection = XMVector3Unproject(Vector4(x, y, 1.f, 1.f), Application->getViewPort().TopLeftX, Application->getViewPort().TopLeftY,
		Application->getViewPort().Width,
		Application->getViewPort().Height, 0.f, 1000.f, Application->getCamera()->GetProjMatrix(),
		Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetWorldMatrix());

	Pos = ToPxVec3(rayOrigin);
	dir = ToPxVec3(rayDirection - rayOrigin).getNormalized();
}

bool Picking::pick(int x, int y)
{
	PxVec3 rayOrig, rayDir;
	computeCameraRay(rayOrig, rayDir, x, y);

	// raycast rigid bodies in scene
	PxRaycastHit hit; hit.shape = nullptr;
	PxRaycastBuffer hit1;

	Application->getPhysics()->getScene()->raycast(rayOrig, rayDir, 100000.f, hit1, PxHitFlag::ePOSITION);
	hit = hit1.block;

	if (hit.shape)
	{
		LPCSTR shapeName = hit.shape->getName();
		PxRigidActor *actor = hit.actor;
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

PxActor *Picking::ReleasePick()
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

	const PxVec3 pos = rayOrig + mDistanceToPicked * rayDir;

	mMouseActor->setKinematicTarget(PxTransform(pos, PxQuat(PxIdentity)));
}
