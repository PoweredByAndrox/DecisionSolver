#include "pch.h"

#include "Physics.h"

HRESULT Engine::Physics::Init()
{
	try
	{
		gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (!gFoundation)
		{
			DebugTrace("Physics: PxCreateFoundation failed.\n");
			throw exception("PxCreateFoundation == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		gPvd = PxCreatePvd(*gFoundation);
		if (!gPvd)
		{
			DebugTrace("Physics: gPvd failed.\n");
			throw exception("gPvd == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		//transport = PxDefaultPvdFileTransportCreate("sample.pxd2");
		if (!transport)
		{
			DebugTrace("Physics: Transport failed.\n");
			throw exception("Transport == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);
		if (!gPhysics)
		{
			DebugTrace("Physics: gPhysics failed.\n");
			throw exception("gPhysics == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;
		sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;

		gScene = gPhysics->createScene(sceneDesc);
		if (!gScene)
		{
			DebugTrace("Physics: gScene failed.\n");
			throw exception("gScene == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		pvdClient = gScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

			//static friction, dynamic friction, restitution
		gMaterial = gPhysics->createMaterial(0.5, 0.5, 0.6);
		if (!gMaterial)
		{
			DebugTrace("Physics: gMaterial failed.\n");
			throw exception("gMaterial == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		PxTransform planePos = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
		gPlane = gPhysics->createRigidStatic(planePos);
		if (!gPlane)
		{
			DebugTrace("Physics: gPlane failed.\n");
			throw exception("gPlane == nullptr!!!");
			return E_FAIL;
		}

		gPlane->createShape(PxPlaneGeometry(), *gMaterial);
		gScene->addActor(*gPlane);

		StaticObjects.push_back(gPlane);
		
		PxCookingParams params(gPhysics->getTolerancesScale());
		params.meshWeldTolerance = 0.001f;
		params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
		params.buildGPUData = true;
		gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, params);
		if (!gCooking)
		{
			DebugTrace("Physics: gCooking failed.\n");
			throw exception("gCooking == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		IsInitPhysX = true;

		GetD3DDevice();
		GetD3DDeviceCon();
		return S_OK;
	}
	catch (const exception&)
	{
		DebugTrace("Physics: Init failed.\n");
		throw exception("PhysX initialization error!!!");
		return E_FAIL;
	}
}

void Engine::Physics::Simulation(bool StopIT, float Timestep)
{
	if (!StopIT) 
	{
		gScene->simulate(Timestep);
		gScene->fetchResults(true);

		if (gPvd->isConnected())
			transport->flush();
	}
}

void Engine::Physics::_createTriMesh(Models *Model)
{
	auto Meshes = Model->getMeshes();

	for (int i = 0; i < Meshes.size(); i++)
	{
		auto ObjVertx = Meshes.at(i).vertices;
		auto ObjIndx = Meshes.at(i).indices;

		PxVec3 *verts = new PxVec3[ObjVertx.size()];
		for (int i = 0; i < ObjVertx.size(); i++)
		{
			verts[i].x = ObjVertx[i].Position.x;
			verts[i].y = ObjVertx[i].Position.y;
			verts[i].z = ObjVertx[i].Position.z;
		}

		vector<PxU32> tris; tris.resize(ObjIndx.size() /3);
		for (int i = tris.size() - 1; i >= 0; i--)
			tris[i] = ObjIndx.at(i);

		PxTriangleMeshDesc TriMeshDesc;
		TriMeshDesc.points.count = ObjVertx.size();
		TriMeshDesc.points.stride = sizeof(PxVec3);
		TriMeshDesc.points.data = verts;

		TriMeshDesc.triangles.count = tris.size() /3;
		TriMeshDesc.triangles.stride = 3 * sizeof(PxU32);
		TriMeshDesc.triangles.data = tris.data();
		if (!TriMeshDesc.isValid())
		{
			DebugTrace("Physics: TriMeshDesc.isValid failed.\n");
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
			DebugTrace("PhysX->_createTriMesh: triangleMesh failed.\n");
			throw exception("triangleMesh == nullptr!!!");
			return;
		}

		PxTriangleMeshGeometry geom(triangleMesh);
		PxRigidStatic *groundMesh = gPhysics->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
		groundMesh->attachShape(*PxRigidActorExt::createExclusiveShape(*groundMesh, PxTriangleMeshGeometry(triangleMesh), *gMaterial));

		gScene->addActor(*groundMesh);

		StaticObjects.push_back(groundMesh);
	}
}

void Engine::Physics::SetPhysicsForCamera(Vector3 Pos, Vector3 Geom) // Position Camera // Geometry to default
{
	gActorCamera = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(Pos.x, Pos.y, Pos.z)), PxBoxGeometry(PxVec3(Geom.x, Geom.y, Geom.z)), *gMaterial, 1.0f);
	gActorCamera->setMass(5.0f);
//	gScene->addActor(*gActorCamera);

	PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0.f, 0.f, 1.f)));
	PxShape *aCapsuleShape = PxRigidActorExt::createExclusiveShape(*gActorCamera, PxCapsuleGeometry(1.5f, 1.f), *gMaterial);
	aCapsuleShape->setLocalPose(relativePose);
	PxRigidBodyExt::updateMassAndInertia(*gActorCamera, 0.5f);

	gScene->addActor(*gActorCamera);

	// DynamicObjects.push_back(gActorCamera);
}

void Engine::Physics::Destroy()
{
	if (gPvd)
	{
		PxPvdTransport *transport = gPvd->getTransport();
		if (transport->isConnected())
			transport->flush();
		gPvd->disconnect();
		gPvd->release();
		transport->release();
	}
	_SAFE_RELEASE(gCooking);
	_SAFE_RELEASE(gScene);
	_SAFE_RELEASE(gPhysics);
	_SAFE_RELEASE(gFoundation);
}

void Engine::Physics::AddNewActor(Vector3 Pos, Vector3 Geom, float Mass)
{
	gBox = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(Pos.x, Pos.y, Pos.z)), PxBoxGeometry(PxVec3(Geom.x, Geom.y, Geom.z)), *gMaterial, 1.0f);

	gBox->setMass(Mass);

	gScene->addActor(*gBox);
	DynamicObjects.push_back(gBox);
}