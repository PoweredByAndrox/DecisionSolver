#include "pch.h"

#include "Physics.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Camera.h"
#include "Console.h"
#include "DebugDraw.h"
#include "SimpleLogic.h"

HRESULT Physics::Init()
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	if (!gFoundation)
	{
		Engine::LogError("Physics::Init->PxCreateFoundation Is nullptr!",
			"Physics::Init->PxCreateFoundation Is nullptr!",
			"Physics: Something is wrong with create PhysX Foundation!");
		IsInitPhysX = false;
		return E_FAIL;
	}

#if defined (_DEBUG)
	gPvd = PxCreatePvd(*gFoundation);
	if (!gPvd)
	{
		Engine::LogError("Physics::Init->PxCreatePvd() Was Triggered!",
			"Physics::Init->PxCreatePvd() Was Triggered!", "Physics: Something is wrong with create PVD!");
		IsInitPhysX = false;
		return E_FAIL;
	}

	transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//transport = PxDefaultPvdFileTransportCreate("sample.pxd2");
	if (!transport)
	{
		Engine::LogError("Physics::Init->PxDefaultPvdSocketTransportCreate Failed!",
			"Physics::Init->PxDefaultPvdSocketTransportCreate Is Failed!",
			"Physics: Something is wrong with create PVD Socket!");
		IsInitPhysX = false;
		return E_FAIL;
	}

	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true,
#if defined (_DEBUG)
		gPvd
#else
		nullptr
#endif 
	);
	if (!gPhysics)
	{
		Engine::LogError("Physics::Init->gPhysics Is nullptr!", "Physics::Init->gPhysics Is nullptr!",
			"Physics: Something is wrong with create PhysX!");
		return E_FAIL;
		IsInitPhysX = false;
	}

	PxInitExtensions(*gPhysics,
#if defined (_DEBUG)
		gPvd
#else
		nullptr
#endif 
	);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
	sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;

	gScene = gPhysics->createScene(sceneDesc);
	if (!gScene)
	{
		Engine::LogError("Physics::Init->gScene Is nullptr!", "Physics::Init->gScene Is nullptr!",
			"Physics: Something is wrong with create PhysX Scene!");
		IsInitPhysX = false;
		return E_FAIL;
	}

	ControllerManager = PxCreateControllerManager(*gScene);
	if (!ControllerManager)
	{
		Engine::LogError("Physics::Init->ControllerManager Is nullptr!",
			"Physics::Init->ControllerManager Is nullptr!",
			"Physics: Something is wrong with create PhysX Contoller Manager!");
		IsInitPhysX = false;
		return E_FAIL;
	}

#if defined (_DEBUG)
	pvdClient = gScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
#endif

							//static friction, dynamic friction, restitution
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	if (!gMaterial)
	{
		Engine::LogError("Physics::Init->gMaterial Is nullptr!",
			"Physics::Init->gMaterial Is nullptr!", "Physics: Something is wrong with create PhysX Material!");
		IsInitPhysX = false;
		return E_FAIL;
	}

	PxTransform planePos = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxHalfPi,
		PxVec3(0.0f, 0.0f, 1.0f)));
	gPlane = gPhysics->createRigidStatic(planePos);
	if (!gPlane)
	{
		Engine::LogError("Physics::Init->gPlane Is nullptr!", "Physics::Init->gPlane Is nullptr!",
			"Physics: Something is wrong with create PhysX Plane!");
		IsInitPhysX = false;
		return E_FAIL;
	}

	gScene->addActor(*PxRigidActorExt::createExclusiveShape(*gPlane, PxPlaneGeometry(),
		*gMaterial)->getActor());

	StaticObjects.push_back(gPlane);

	PxCookingParams params(gPhysics->getTolerancesScale());
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildGPUData = true;
	gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, params);
	if (!gCooking)
	{
		Engine::LogError("Physics::Init->gCooking Is nullptr!",
			"Physics::Init->gCooking Is nullptr!", "Physics: Something is wrong with create PhysX Cook!");
		IsInitPhysX = false;
		return E_FAIL;
	}

	IsInitPhysX = true;
	return S_OK;
}

void Physics::Simulation(float Timestep)
{
	//Render
	for (size_t i = 0; i < Cobes.size(); i++)
	{
		for (auto it: DynCobes)
		{
			PxQuat aq = it->getGlobalPose().q;
			PxVec3 pos = it->getGlobalPose().p;
			Cobes.at(i)->Draw(Matrix::CreateFromQuaternion(
				Quaternion(aq.x, aq.y, aq.z, aq.w)) * Matrix::CreateTranslation(Vector3(pos.x, pos.y, pos.z)),
				Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix(),
				Colors::DarkSeaGreen, nullptr, Application->IsWireFrame());
		}
	}
	
	if (!Application->IsSimulatePhysics())
	{
		gScene->simulate(Timestep);
		gScene->fetchResults(true);

		if (Application->getDebugDraw().operator bool())
		{
			BoundingBox box;
			box.Center = Vector3::Zero;
			box.Extents = Vector3(100.f, 50.f, 100.f);
			//Application->getDebugDraw()->Draw(box, (Vector4)Colors::DarkGoldenrod);
		}
	}
}

ToDo("Replace Code Below To ConvexMesh")
#include "Models.h"
void Physics::_createTriMesh(shared_ptr<Models> Model, bool stat_dyn)
{
	auto Meshes = Model->getMeshes();

	vector<PxVec3> verts;
	vector<PxU32> indies;

	for (size_t i = 0; i < Meshes.size(); i++)
	{
		auto Obj = Meshes.at(i)->getVertices();
		for (size_t i1 = 0; i1 < Obj.size(); i1++)
			verts.push_back(ToPxVec3(Obj.at(i1).Pos));

		auto Obj1 = Meshes.at(i)->getIndices();
		for (size_t i1 = 0; i1 < Obj1.size(); i1++)
			indies.push_back(Obj1.at(i1));
	}

	PxTriangleMeshDesc TriMeshDesc;
	TriMeshDesc.points.count = verts.size();
	TriMeshDesc.points.stride = sizeof(PxVec3);
	TriMeshDesc.points.data = &(verts)[0];

	TriMeshDesc.triangles.count = indies.size() / 3;
	TriMeshDesc.triangles.stride = 3 * sizeof(PxU32);
	TriMeshDesc.triangles.data = &(indies)[0];
	if (!TriMeshDesc.isValid())
	{
//		DebugTrace("Physics: TriMeshDesc.isValid failed.\n");
		throw exception("TriMeshDesc.isValid == false!!!");
		return;
	}

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = gCooking->cookTriangleMesh(TriMeshDesc, writeBuffer, &result);
	if (!status)
	{
		throw exception("PhysX->_createTriMesh: cookTriangleMesh == false!!!");
		return;
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	triangleMesh = gPhysics->createTriangleMesh(readBuffer);
	if (!triangleMesh)
	{
//		DebugTrace("PhysX->_createTriMesh: triangleMesh failed.\n");
		throw exception("triangleMesh == nullptr!!!");
		return;
	}
	if (stat_dyn)
	{
		PxRigidStatic *TriMesh = gPhysics->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
		gScene->addActor(*TriMesh);
		StaticObjects.push_back(TriMesh);
	}
	if (!stat_dyn)
	{
		PxRigidDynamic *TriMesh = gPhysics->createRigidDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
		gScene->addActor(*PxRigidActorExt::createExclusiveShape(*TriMesh, PxSphereGeometry(10.f), *gMaterial)->getActor());
		DynamicObjects.push_back(TriMesh);
	}
}

void Physics::DestroyAllObj()
{
	while (!DynamicObjects.empty())
	{
		SAFE_release(DynamicObjects.at(0));
		DynamicObjects.erase(DynamicObjects.begin());
	}
	while (!StaticObjects.empty())
	{
		SAFE_release(StaticObjects.at(0));
		StaticObjects.erase(StaticObjects.begin());
	}
	Cobes.clear();
}

void Physics::SpawnObject(PxVec3 Pos)
{
	gBox = PxCreateDynamic(*gPhysics, PxTransform(Pos), PxBoxGeometry(PxVec3(5.f, 5.f, 5.f)), *gMaterial, 1.0f);

	gBox->setMass(4.f);
	
	gScene->addActor(*gBox);
	Cobes.push_back(GeometricPrimitive::CreateCube(Application->getDeviceContext(), 5.f, false));
	DynCobes.push_back(gBox);
}

void Physics::Destroy()
{
	DestroyAllObj();
	SAFE_release(ControllerManager);
//	SAFE_release(gPlane);
	SAFE_release(gScene);
	SAFE_release(gMaterial);
	SAFE_release(gPhysics);

#if defined (_DEBUG)

	if (gPvd)
	{
		PxPvdTransport *transport = gPvd->getTransport();
		SAFE_release(transport);
	}
	pvdClient = nullptr;
	SAFE_release(gPvd);
#endif
	SAFE_release(gCooking);
	SAFE_release(gFoundation);
}

PxVec3 ToPxVec3(Vector3 var)
{
	return PxVec3(var.x, var.y, var.z);
}

Vector3 ToVec3(PxVec3 var)
{
	return Vector3(var.x, var.y, var.z);
}

Quaternion ToQuat(PxQuat var)
{
	return Quaternion(var.x, var.y, var.z, var.w);
}

PxQuat ToQuaternion(Quaternion var)
{
	return PxQuat(var.x, var.y, var.z, var.w);
}

PxExtendedVec3 ToExtended(Vector3 var)
{
	return PxExtendedVec3(var.x, var.y, var.z);
}

Vector3 ToExtended(PxExtendedVec3 var)
{
	return Vector3((float)var.x, (float)var.y, (float)var.z);
}
