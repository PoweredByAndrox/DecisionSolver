#pragma once
#ifndef __PHYSICS_H__
#define __PHYSICS_H__
#include "pch.h"

#include <PxPhysicsAPI.h>

using namespace physx;

PxVec3 ToPxVec3(Vector3 var);
Vector3 ToVec3(PxVec3 var);

Quaternion ToQuat(PxQuat var);
PxQuat ToQuaternion(Quaternion var);

PxExtendedVec3 ToExtended(Vector3 var);
Vector3 ToExtended(PxExtendedVec3 var);

class Models;
class SimpleLogic;
class Physics
{
public:
	HRESULT Init();

	void Simulation(float Timestep);

	void SetGravity(PxRigidDynamic *RigDyn, PxVec3 Vec3) { RigDyn->getScene()->setGravity(Vec3); }
	void SetMass(PxRigidDynamic *RigDyn, PxReal Mass) { RigDyn->setMass(Mass); }
	void AddTorque(PxRigidDynamic *RigDyn, PxVec3 Vec3, PxForceMode::Enum ForceMode) { RigDyn->addTorque(Vec3, ForceMode); }
	void AddForce(PxRigidDynamic *RigDyn, PxVec3 Vec3, PxForceMode::Enum ForceMode) { RigDyn->addForce(Vec3, ForceMode); }
//	void CreateJoint(PxRigidDynamic *RigDyn1, PxRigidDynamic *RigDyn2, PxVec3 OffSet)
//	{
//#if defined (_DEBUG)
//		PxFixedJoint *distanceJoint = PxFixedJointCreate(*gPhysics, RigDyn1, PxTransform(OffSet), RigDyn2, PxTransform(-OffSet));
//		distanceJoint->setProjectionLinearTolerance(0.5f);
//#endif
//		PxSphericalJoint *sphericalJoint = PxSphericalJointCreate(*gPhysics, RigDyn1, PxTransform(OffSet), RigDyn2,
//			PxTransform(-OffSet));
//		sphericalJoint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
//	}

	vector<PxRigidDynamic *> GetPhysDynamicObject() { return DynamicObjects; }
	vector<PxRigidStatic *> GetPhysStaticObject() { return StaticObjects; }

	PxVec3 GetObjPos(PxRigidDynamic *Obj) { return Obj->getGlobalPose().p; }

	void Destroy();

	bool IsPhysicsInit() { return IsInitPhysX; }
	void _createTriMesh(shared_ptr<Models> Model, bool stat_dyn);

	PxTriangleMesh *getTriMesh() { return triangleMesh; }
	PxScene *getScene() { return gScene; }
	PxPhysics *getPhysics() { return gPhysics; }
	PxMaterial *getMaterial() { return gMaterial; }
	PxRigidDynamic *getActrCamera() { return gActorCamera; }
#if defined (_DEBUG)
	PxPvdSceneClient *getClientPVD() { return pvdClient; }
#endif
	PxControllerManager *getContrlManager() { return ControllerManager; }

	void DestroyAllObj();

	void SpawnObject(PxVec3 Pos = PxVec3(0.f, 0.f, 0.f));
protected:
	// ***************
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;

	// ***************
	PxFoundation *gFoundation = nullptr;
	PxPhysics *gPhysics = nullptr;
	PxMaterial *gMaterial = nullptr;
	PxScene *gScene = nullptr;
	PxRigidStatic *gPlane = nullptr;
	PxRigidDynamic *gBox = nullptr, *gActorCamera = nullptr;
	PxCooking *gCooking = nullptr;
#if defined (_DEBUG)
	PxPvd *gPvd = nullptr;
	PxPvdSceneClient *pvdClient = nullptr;
	PxPvdTransport *transport = nullptr;
#endif

	// ***************
	HRESULT hr = S_OK;

	// ***************
	vector<PxRigidDynamic *> DynamicObjects;
	vector<PxRigidStatic *> StaticObjects;

	// ***************
	PxRigidDynamic *meshActor = nullptr;
	PxTriangleMesh *triangleMesh = nullptr;

	PxControllerManager *ControllerManager = nullptr;

	// ***************
		// Initialized bool variable
	bool IsInitPhysX = false;

	vector<shared_ptr<GeometricPrimitive>> Cobes;
	vector<PxRigidDynamic *> DynCobes;
};
#endif // !__PHYSICS_H__
