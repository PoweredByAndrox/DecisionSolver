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
//#if defined (DEBUG)
//		PxFixedJoint *distanceJoint = PxFixedJointCreate(*gPhysics, RigDyn1, PxTransform(OffSet), RigDyn2, PxTransform(-OffSet));
//		distanceJoint->setProjectionLinearTolerance(0.5f);
//#endif
//		PxSphericalJoint *sphericalJoint = PxSphericalJointCreate(*gPhysics, RigDyn1, PxTransform(OffSet), RigDyn2,
//			PxTransform(-OffSet));
//		sphericalJoint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
//	}

	vector<PxRigidDynamic *> GetPhysDynamicObject() { return DynamicObjects; }
	vector<PxRigidStatic *> GetPhysStaticObject() { return StaticObjects; }

	void AddNewActor(Vector3 Pos, Vector3 Geom, float Mass, float SizeModel = 1.f);

	PxVec3 GetObjPos(PxRigidDynamic *Obj) { return Obj->getGlobalPose().p; }

	void Destroy();

	bool IsPhysicsInit() { return IsInitPhysX; }
	//void _createTriMesh(Models *Model, bool stat_dyn);

	PxTriangleMesh *getTriMesh() { if (triangleMesh) return triangleMesh; return nullptr; }
	PxScene *getScene() { if (gScene) return gScene; return nullptr; }
	PxPhysics *getPhysics() { if (gPhysics) return gPhysics; return nullptr; }
	PxMaterial *getMaterial() { if (gMaterial) return gMaterial; return nullptr; }
	PxRigidDynamic *getActrCamera() { if (gActorCamera) return gActorCamera; return nullptr; }
#if defined (DEBUG)
	PxPvdSceneClient *getClientPVD() { if (pvdClient) return pvdClient; return nullptr; }
#endif
	PxControllerManager *getContrlManager() { return ControllerManager; }

	void ClearAllObj()
	{
		while (!DynamicObjects.empty())
		{
			SAFE_release(DynamicObjects.at(0));
			DynamicObjects.erase(DynamicObjects.begin());
		}
		Cobes.clear();
	}

	//void SetPhysicsForCamera(Vector3 Pos, Vector3 Geom);
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
#if defined (DEBUG)
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

	PxVec3 ConstrainToBoundary(PxVec3 Pos, PxVec3 Min, PxVec3 Max);
};
#endif // !__PHYSICS_H__
