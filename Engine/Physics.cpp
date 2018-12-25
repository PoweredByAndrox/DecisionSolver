#include "pch.h"
#include "Physics.h"

HRESULT Engine::Physics::Init(Models *Model)
{
	try
	{
		this->Model.reset(Model);

		gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (gFoundation == nullptr)
		{
			DebugTrace("Physics: PxCreateFoundation failed.\n");
			throw exception("PxCreateFoundation == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		//gPvd = PxCreatePvd(*gFoundation);
		//PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		//gPvd->connect(*transport, PxPvdInstrumentationFlag::ePROFILE);

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);
		if (gPhysics == nullptr)
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

		PxTransform boxPos(PxVec3(0.5f, 5.0f, 0.5f));
		PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
		gBox = PxCreateDynamic(*gPhysics, boxPos, boxGeometry, *gMaterial, 1.0f);

		gScene->addActor(*gBox);
		DynamicObjects.push_back(gBox);
		
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
	}
}

void Engine::Physics::_createTriMesh()
{
	auto ObjVertx = Model->getVertices();
	auto ObjIndx = Model->getIndices();

	PxU32 NumVerticies = ObjVertx.size();
	PxU32 NumTriangles = ObjIndx.size() / 3;

	vector<PxVec3> verts; verts.resize(NumVerticies / 3);

	/*
	int ii = -1;
	for (int i = 0; i < NumVerticies / 3; i++)
	{
		++ii;
		verts[i].x = ObjVertx[ii].Position.x;
		verts[i].y = ObjVertx[++ii].Position.y;
		verts[i].z = ObjVertx[++ii].Position.z;
	}

	PxU32 *tris = new PxU32[NumTriangles];
	for (int i = NumTriangles - 1; i >= 0; i--)
		tris[i] = ObjIndx.at(i);
	*/

	PxTriangleMeshDesc TriMeshDesc;
	TriMeshDesc.points.count = ObjVertx.size();
	TriMeshDesc.points.stride = sizeof(PxVec3);
	TriMeshDesc.points.data = ObjVertx.data();

	TriMeshDesc.triangles.count = ObjIndx.size();
	TriMeshDesc.triangles.stride = 3 * sizeof(PxU32);
	TriMeshDesc.triangles.data = ObjIndx.data();
	if (!TriMeshDesc.isValid())
	{
		DebugTrace("Physics: TriMeshDesc.isValid failed.\n");
		throw exception("TriMeshDesc.isValid == false!!!");
		return;
	}

	PxCookingParams params(gPhysics->getTolerancesScale());
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eFORCE_32BIT_INDICES;
	params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
	gCooking->setParams(params);

	triangleMesh = gCooking->createTriangleMesh(TriMeshDesc, gPhysics->getPhysicsInsertionCallback());
	if (!triangleMesh)
	{
		DebugTrace("Physics: triangleMesh failed.\n");
		throw exception("triangleMesh == nullptr!!!");
		return;
	}

	PxTriangleMeshGeometry geom(triangleMesh);
	PxRigidStatic *groundMesh = gPhysics->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
	groundMesh->attachShape(*PxRigidActorExt::createExclusiveShape(*groundMesh, PxTriangleMeshGeometry(triangleMesh), *gMaterial));

	gScene->addActor(*groundMesh);

	StaticObjects.push_back(groundMesh);
}

void Engine::Physics::Destroy()
{
	//if (gPvd)
	//{
	//	PxPvdTransport *transport = gPvd->getTransport();
	//	gPvd->release();
	//	transport->release();
	//}
	_SAFE_RELEASE(gCooking);
	_SAFE_RELEASE(gScene);
	_SAFE_RELEASE(gPhysics);
	_SAFE_RELEASE(gFoundation);
}

bool CM = false;

void Engine::Physics::AddNewActor(Vector3 Pos, Vector3 Geom)
{
	PxTransform boxPos(PxVec3(Pos.x, Pos.y, Pos.z));
	PxBoxGeometry boxGeometry(PxVec3(Geom.x, Geom.y, Geom.z));
	gBox = PxCreateDynamic(*gPhysics, boxPos, boxGeometry, *gMaterial, 1.0f);

	gScene->addActor(*gBox);
	DynamicObjects.push_back(gBox);

	if (!CM)
	{
		_createTriMesh();
		CM = true;
	}
}