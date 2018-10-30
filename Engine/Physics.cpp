#include "pch.h"
#include "Physics.h"

HRESULT Physics::Init()
{
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);

	if (!gFoundation)
	{
		MessageBoxA(DXUTGetHWND(), "PxCreateFoundation failed! line 9", "ERROR!", MB_OK);
		return E_FAIL;
	}

	gPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
	
	if (gPhysicsSDK == NULL)
	{
		MessageBoxA(DXUTGetHWND(), "Error creating PhysX3 device, Exiting! line 19", "ERROR!", MB_OK);
		return E_FAIL;
	}

	PxSceneDesc sceneDesc(gPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysicsSDK->createScene(sceneDesc);
	
							//static friction, dynamic friction, restitution
	mMaterial = gPhysicsSDK->createMaterial(0.5, 0.5, 0.5);

	//1-Creating static plane
	PxTransform planePos = PxTransform(PxVec3(0.0f, 0, 0.0f), 
		PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));

	plane = gPhysicsSDK->createRigidStatic(planePos);
	plane->createShape(PxPlaneGeometry(), *mMaterial);
	gScene->addActor(*plane);

	PxTransform boxPos(PxVec3(0.0f, 10.0f, 0.0f));
	PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
	gBox = PxCreateDynamic(*gPhysicsSDK, boxPos, boxGeometry, *mMaterial, 1.0f);
	gScene->addActor(*gBox);

	return S_OK;
}

void Physics::Simulation()
{
	gScene->simulate(myTimestep);
	gScene->fetchResults(true);
	for (int i = 0; i <= 300; i++)
	{
		PxVec3 boxPos = gBox->getGlobalPose().p;
		char buff[100];
		snprintf(buff, sizeof(buff), "\nBox current Position (%f), (%f), (%f)\n", boxPos.x, boxPos.y, boxPos.z);
		std::string buffAsStdStr = buff;
		OutputDebugStringA(buffAsStdStr.c_str());
	}
}

void Physics::Destroy()
{
	gScene->release();
	gPhysicsSDK->release();
	gFoundation->release();
}
