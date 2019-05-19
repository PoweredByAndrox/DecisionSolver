#pragma once
#ifndef __PHYSICS_H__
#define __PHYSICS_H__
#include "pch.h"

#include <PxPhysicsAPI.h>

#if defined(DEBUG) && !defined(_M_X64)
#pragma comment(lib, "PhysX_32.lib")
#pragma comment(lib, "PhysXCommon_32.lib")
#pragma comment(lib, "PhysXFoundation_32.lib")
#pragma comment(lib, "PhysXExtensions_static_32.lib")
#pragma comment(lib, "PhysXPvdSDK_static_32.lib")
#pragma comment(lib, "SceneQuery_static_32.lib")
#pragma comment(lib, "SimulationController_static_32.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_32.lib")
#pragma comment(lib, "PhysXCooking_32.lib")
#pragma comment(lib, "PhysXTask_static_32.lib")
#elif defined(DEBUG) && defined(_M_X64)
#pragma comment(lib, "PhysX3DEBUG_x64.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib, "PxFoundationDEBUG_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x64.lib")
#pragma comment(lib, "SceneQueryDEBUG.lib")
#pragma comment(lib, "SimulationControllerDEBUG.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x64.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x64.lib")
#pragma comment(lib, "PxTaskDEBUG_x64")
#elif !defined(_M_X64)
#pragma comment(lib, "PhysX_32.lib")
#pragma comment(lib, "PhysXCommon_32.lib")
#pragma comment(lib, "PhysXFoundation_32.lib")
#pragma comment(lib, "PhysXExtensions_static_32.lib")
#pragma comment(lib, "PhysXPvdSDK_static_32.lib")
#pragma comment(lib, "SceneQuery_static_32.lib")
#pragma comment(lib, "SimulationController_static_32.lib")
#pragma comment(lib, "PhysXCharacterKinematic_static_32.lib")
#pragma comment(lib, "PhysXCooking_32.lib")
#pragma comment(lib, "PhysXTask_static_32.lib")
#elif defined(_M_X64)
#pragma comment(lib, "PhysX3CharacterKinematic_x64.lib")
#pragma comment(lib, "PhysX3Cooking_x64.lib")
#pragma comment(lib, "PxTask_x64")
#pragma comment(lib, "SceneQuery.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PxFoundation_x64.lib")
#pragma comment(lib, "PxPvdSDK_x64.lib")
#pragma comment(lib, "PhysX3_x64.lib")
#pragma comment(lib, "PxPvdSDK_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#endif
using namespace physx;

PxVec3 ToPxVec3(Vector3 var);
Vector3 ToVec3(PxVec3 var);

class Physics/*: public Models*/
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
//#if defined(DEBUG)
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
	PxPvdSceneClient *getClientPVD() { if (pvdClient) return pvdClient; return nullptr; }

	void ClearAllObj()
	{
		while (!DynamicObjects.empty())
		{
			SAFE_release(DynamicObjects.at(0));
			DynamicObjects.erase(DynamicObjects.begin());
		}
	}

	//void SetPhysicsForCamera(Vector3 Pos, Vector3 Geom);

	Physics() {}
	~Physics() {}
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
	PxPvd *gPvd = nullptr;
	PxPvdTransport *transport = nullptr;
	PxPvdSceneClient *pvdClient = nullptr;

	// ***************
	HRESULT hr = S_OK;

	// ***************
	vector<PxRigidDynamic *> DynamicObjects;
	vector<PxRigidStatic *> StaticObjects;

	// ***************
	PxRigidDynamic *meshActor = nullptr;
	PxTriangleMesh *triangleMesh = nullptr;

	// ***************
		// Initialized bool variable
	bool IsInitPhysX = false;
};
#endif // !__PHYSICS_H__
