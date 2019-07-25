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

#pragma once
#ifndef __PICKING_H__
#define __PICKING_H__
#include "pch.h"

#include "Physics.h"

#define USE_D6_JOINT_FOR_MOUSE 1
#define USE_SPHERICAL_JOINT_FOR_MOUSE 0
//#define VISUALIZE_PICKING_RAYS

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

class Picking
{
public:
	Picking() {}
	~Picking() {}

	PX_FORCE_INLINE void UpdatePick() { pick(Application->getMouse()->GetState().x, Application->getMouse()->GetState().y); }
	bool isPicked() const;
	bool pick(int x, int y);
	void computeCameraRay(PxVec3 &Pos, PxVec3 &dir, PxI32 x, PxI32 y) const;

	void ReleasePick();
	void tick();

#ifdef VISUALIZE_PICKING_RAYS
	struct Ray
	{
		PxVec3 origin;
		PxVec3 dir;
	};
	PX_FORCE_INLINE	const vector<Ray> &getRays() const { return mRays; }
#endif

private:
	void grabActor(const PxVec3 &worldImpact, const PxVec3 &rayOrigin);
	void moveActor(int x, int y);

	PxActor *mSelectedActor = nullptr;

#if USE_D6_JOINT_FOR_MOUSE
	PxD6Joint *mMouseJoint = nullptr;
#elif USE_SPHERICAL_JOINT_FOR_MOUSE
	PxSphericalJoint *mMouseJoint = nullptr;
#else
	PxDistanceJoint *mMouseJoint = nullptr;
#endif
	PxRigidDynamic *mMouseActor = nullptr,
		*mMouseActorToDelete = nullptr;
	PxReal mDistanceToPicked = 0.f;

#ifdef VISUALIZE_PICKING_RAYS
	vector<Ray>	mRays;
#endif
};
#endif // !__PICKING_H__
