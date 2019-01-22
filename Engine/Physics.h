#pragma once
#ifndef __PHYSICS_H__
#define __PHYSICS_H__
#include "pch.h"

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>

#include <foundation/PxSimpleTypes.h>
#include <foundation/PxVec3.h>
#include <foundation/PxFoundation.h>

#include <extensions/PxD6Joint.h>
#include <extensions/PxSphericalJoint.h>
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxShapeExt.h>
#include <extensions/PxSimpleFactory.h>
#include <extensions/PxRaycastCCD.h>

#include <PxSimulationEventCallback.h>

#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerBehavior.h>

#include "DirectXHelpers.h"
#include "GeometricPrimitive.h"
#include "Effects.h"
#include "Models.h"

#if defined(DEBUG) && !defined(_M_X64)
#pragma comment(lib, "PhysX3DEBUG_x86.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x86.lib")
#pragma comment(lib, "PxFoundationDEBUG_x86.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PxPvdSDKDEBUG_x86.lib")
#pragma comment(lib, "SceneQueryDEBUG.lib")
#pragma comment(lib, "SimulationControllerDEBUG.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x86.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x86.lib")
#pragma comment(lib, "PxTaskDEBUG_x86")
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
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PxFoundation_x86.lib")
#pragma comment(lib, "PxPvdSDK_x86.lib")
#pragma comment(lib, "SceneQuery.lib")
#pragma comment(lib, "SimulationController.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")
#pragma comment(lib, "PhysX3Cooking_x86.lib")
#pragma comment(lib, "PxTask_x86")
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

namespace Engine
{
#define _SAFE_RELEASE(p) { if (p) { (p)->release(); (p) = nullptr; } }


	class Physics: public Models
	{
	public:
		HRESULT Init();

		void Simulation(bool StopIT, float Timestep);

		void SetGravity(PxRigidDynamic *RigDyn, PxVec3 Vec3) { RigDyn->getScene()->setGravity(Vec3); }
		void SetMass(PxRigidDynamic *RigDyn, PxReal Mass) { RigDyn->setMass(Mass); }
		void AddTorque(PxRigidDynamic *RigDyn, PxVec3 Vec3, PxForceMode::Enum ForceMode) { RigDyn->addTorque(Vec3, ForceMode); }
		void AddForce(PxRigidDynamic *RigDyn, PxVec3 Vec3, PxForceMode::Enum ForceMode) { RigDyn->addForce(Vec3, ForceMode); }
		void CreateJoint(PxRigidDynamic *RigDyn1, PxRigidDynamic *RigDyn2, PxVec3 OffSet)
		{
#ifndef DEBUG
			PxFixedJoint *distanceJoint = PxFixedJointCreate(*gPhysics, RigDyn1, PxTransform(OffSet), RigDyn2, PxTransform(-OffSet));
			distanceJoint->setProjectionLinearTolerance(0.5f);
#endif
			PxSphericalJoint *sphericalJoint = PxSphericalJointCreate(*gPhysics, RigDyn1, PxTransform(OffSet), RigDyn2,
				PxTransform(-OffSet));
			sphericalJoint->setSphericalJointFlag(PxSphericalJointFlag::eLIMIT_ENABLED, true);
		}

		vector<PxRigidDynamic *> GetPhysDynamicObject() { return DynamicObjects; }
		vector<PxRigidStatic *> GetPhysStaticObject() { return StaticObjects; }
		//	void GenTriangleMesh(PxVec3 pos, vector<VERTEX> indices, vector<UINT> vertices);

		void AddNewActor(Vector3 Pos, Vector3 Geom, float Mass);

		PxVec3 GetObjPos(PxRigidDynamic *Obj) { return Obj->getGlobalPose().p; }

		void Destroy();

		bool IsPhysicsInit() { return IsInitPhysX; }
		void _createTriMesh(Models *Model);

		PxTriangleMesh *getTriMesh() { if (triangleMesh) return triangleMesh; return nullptr; }
		PxScene *getScene() { if (gScene) return gScene; return nullptr; }
		PxPhysics *getPhysics() { if (gPhysics) return gPhysics; return nullptr; }
		PxMaterial *getMaterial() { if (gMaterial) return gMaterial; return nullptr; }
		PxRigidDynamic *getActrCamera() { if (gActorCamera) return gActorCamera; return nullptr; }
		PxPvdSceneClient *getClientPVD() { if (pvdClient) return pvdClient; return nullptr; }

		void ClearAllObj()
		{
			if (!DynamicObjects.empty())
				while (DynamicObjects.size() != 0)
				{
					DynamicObjects[0]->release();
					DynamicObjects.erase(DynamicObjects.begin());
					if (DynamicObjects.size() == 0)
						DynamicObjects.clear();
				}
		}

		void Release()
		{
			SAFE_RELEASE(Device);
			if (DeviceCon)
			{
				DeviceCon->ClearState();
				DeviceCon->Flush();
				SAFE_RELEASE(DeviceCon);
			}
		}

		void SetPhysicsForCamera(Vector3 Pos, Vector3 Geom);

		Physics() {}
		~Physics() {}
	private:
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
		void GetD3DDevice() { if (!Device) Device = DXUTGetD3D11Device(); }
		void GetD3DDeviceCon() { if (!DeviceCon) DeviceCon = DXUTGetD3D11DeviceContext(); }
		ID3D11Device *Device = nullptr;
		ID3D11DeviceContext *DeviceCon = nullptr;

		// ***************
			// Initialized bool variable
		bool IsInitPhysX = false;
	};
};
#endif // !__PHYSICS_H__
