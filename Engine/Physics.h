#pragma once
#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include <PxPhysicsAPI.h> //PhysX main header file
#include <PxPhysics.h>
#include <extensions/PxExtensionsAPI.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxShapeExt.h>
#include <extensions/PxSimpleFactory.h>
#include <PxRigidStatic.h> 
#include <PxSimulationEventCallback.h>

#include <foundation/PxFoundation.h>

#ifdef _DEBUG
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
#else
#pragma comment(lib, "PhysX3_x86.lib")
#pragma comment(lib, "PhysX3Common_x86.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PxFoundation_x86.lib")
#pragma comment(lib, "PxPvdSDK_x86.lib")
#pragma comment(lib, "SceneQuery.lib")
#pragma comment(lib, "SimulationController.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")
#pragma comment(lib, "PhysX3Cooking_x86.lib")
#endif

using namespace std;
using namespace physx;

class Physics
{
public:
	HRESULT Init();
	void Simulation();

	void Destroy();

	Physics() {}
	~Physics() {}

private:
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxFoundation *gFoundation = nullptr;
	PxPhysics *gPhysicsSDK = nullptr;
	PxMaterial* mMaterial = nullptr;
	PxScene* gScene = nullptr;
	PxRigidStatic* plane = nullptr;
	PxRigidDynamic*gBox = nullptr;

	PxReal myTimestep = 1.0f / 60.0f;
};
#endif // !__PHYSICS_H__
