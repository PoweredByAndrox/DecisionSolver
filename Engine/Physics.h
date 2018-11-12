#pragma once
#ifndef __PHYSICS_H__
#define __PHYSICS_H__
#include "pch.h"

#include <PxPhysicsAPI.h>
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

#include "DirectXHelpers.h"
#include "GeometricPrimitive.h"
#include "Effects.h"
#include "SimpleMath.h"
#include "Models.h"

#include "DirectXMath.h"
using namespace SimpleMath;


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
#pragma comment(lib, "DirectXTK.lib")
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
#pragma comment(lib, "DirectXTK.lib")
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
#pragma comment(lib, "DirectXTK.lib")
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
#pragma comment(lib, "DirectXTK.lib")
#endif

using namespace std;
using namespace physx;

class Physics: public Models
{
public:
	HRESULT Init(); //vector<Mesh> *_Mesh

	void Simulation(Matrix World, Matrix View, Matrix Proj);

	void SetGravity(PxRigidDynamic *RigDyn, PxVec3 Vec3) { RigDyn->getScene()->setGravity(Vec3); }
	void SetMass(PxRigidDynamic *RigDyn, PxReal Mass) { RigDyn->setMass(Mass); }
	void AddTorque(PxRigidDynamic *RigDyn, PxVec3 Vec3, PxForceMode::Enum ForceMode) { RigDyn->addTorque(Vec3, ForceMode); }

	vector<PxRigidDynamic*> GetPhysDynamiObject() { return DynamicObjects; }
	vector<PxRigidStatic*> GetPhysStaticObject() { return StaticObjects; }
//	void GenTriangleMesh(PxVec3 pos, vector<VERTEX> indices, vector<UINT> vertices);

	PxVec3 GetObjPos(PxRigidDynamic *Obj) { return Obj->getGlobalPose().p; }

	void Destroy();

	bool IsPhysicsInit() { return IsInitPhysX; }
	void _createConvexMesh();
		
	auto *getTriMesh() { return &triangleMesh; }

	Physics() {}
	~Physics() {}

private:
		// ***************
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	
		// ***************
	PxReal Timestep = 1.0f / 60.0f;

		// ***************
	std::unique_ptr<DirectX::GeometricPrimitive> m_shape;

		// ***************
	PxFoundation *gFoundation = nullptr;
	PxPhysics *gPhysics = nullptr;
	PxMaterial *gMaterial = nullptr;
	PxScene *gScene = nullptr;
	PxRigidStatic *gPlane = nullptr;
	PxRigidDynamic *gBox = nullptr;
	PxCooking *gCooking;

		// ***************
	HRESULT hr = S_OK;
	//vector<Mesh> *_Mesh;

		// ***************
	vector<PxRigidDynamic*> DynamicObjects;
	vector<PxRigidStatic*> StaticObjects;

		// ***************
	PxRigidDynamic *meshActor = nullptr;
	PxTriangleMesh *triangleMesh;
	
		// ***************
	void getDeviceD3D()
	{
		Device = DXUTGetD3D11Device();
	}
	void getDeviceConD3D()
	{
		DevCon = DXUTGetD3D11DeviceContext();
	}

		// ***************
			// Initialized bool variables
	bool IsInitPhysX = false;
	bool IsRenderBufferInit = false;

		// ***************
	ID3D11Buffer *VertexBuffer = nullptr, *IndexBuffer = nullptr;

		// ***************
	ID3D11DeviceContext *DevCon = DXUTGetD3D11DeviceContext();
	ID3D11Device *Device = DXUTGetD3D11Device();
};
#endif // !__PHYSICS_H__
