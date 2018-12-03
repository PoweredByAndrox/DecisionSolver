#include "pch.h"
#include "Physics.h"

HRESULT Engine::Physics::Init()
{
	try
	{
		gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (gFoundation == nullptr)
		{
			DebugTrace("Physics: PxCreateFoundation failed. Line: 12\n");
			throw exception("PxCreateFoundation == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
		if (gPhysics == nullptr)
		{
			DebugTrace("Physics: gPhysics failed. Line: 21\n");
			throw exception("gPhysics == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		gScene = gPhysics->createScene(sceneDesc);
		if (!gScene)
		{
			DebugTrace("Physics: gScene failed. Line: 34\n");
			throw exception("gScene == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

			//static friction, dynamic friction, restitution
		gMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
		if (!gMaterial)
		{
			DebugTrace("Physics: gMaterial failed. Line: 44\n");
			throw exception("gMaterial == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}
		PxTransform planePos = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));

		gPlane = gPhysics->createRigidStatic(planePos);
		if (!gPlane)
		{
			DebugTrace("Physics: gPlane failed. Line: 54\n");
			throw exception("gPlane == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}
		gPlane->createShape(PxPlaneGeometry(), *gMaterial);
		gScene->addActor(*gPlane);

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
			DebugTrace("Physics: gCooking failed. Line: 76\n");
			throw exception("gCooking == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}
		IsInitPhysX = true;

		getDeviceD3D();
		getDeviceConD3D();
		return S_OK;
	}
	catch (const exception&)
	{
		DebugTrace("Physics: Init failed. Line: 86\n");
		throw exception("PhysX initialization error!!!");
		IsInitPhysX = false;
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

void Engine::Physics::_createConvexMesh()
{
	//int NumVerticies = vertices.size();
	//int NumTriangles = indices.size() / 3;

	//PxVec3* verts = new PxVec3[NumVerticies / 3];
	//int ii = -1;
	//for (int i = 0; i < NumVerticies / 3; i++)
	//{
	//	++ii;
	//	verts[i].x = vertices[ii].Position.x;
	//	verts[i].y = vertices[++ii].Position.y;
	//	verts[i].z = vertices[++ii].Position.z;
	//}

	//PxU16 *tris = new PxU16[indices.size()];
	//for (int i = indices.size() - 1; i >= 0; i--)
	//	tris[i] = indices[i];

	//PxConvexMeshDesc convexDesc;
	//convexDesc.points.count = NumVerticies;
	//convexDesc.points.stride = sizeof(PxVec3);
	//convexDesc.points.data = &vertices[3];
	//convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	//bool Valid_ConvexMesh = convexDesc.isValid();


	//PxTriangleMeshDesc TriMeshDesc;
	//TriMeshDesc.points.count = NumVerticies;
	//TriMeshDesc.points.stride = sizeof(PxVec3);
	//TriMeshDesc.points.data = &vertices[6];
	//TriMeshDesc.triangles.count = NumTriangles;
	//TriMeshDesc.triangles.data = &indices[3];
	//TriMeshDesc.triangles.stride = 3 * sizeof(PxU32);
	//TriMeshDesc.flags = PxMeshFlag::Enum::e16_BIT_INDICES;

	//bool Valid_TriMesh = TriMeshDesc.isValid();

	//meshActor = gPhysics->createRigidDynamic(PxTransform(1.0f, 1.0f, 1.0f));
	//PxShape* meshShape;
	//if (!meshActor)
	//{
	//	DebugTrace("Physics: meshActor failed. Line: 145\n");
	//	throw exception("meshActor == nullptr!!!");
	//	return;
	//}
	//meshActor->setRigidBodyFlag(PxRigidBodyFlag::Enum::eKINEMATIC, true);

	////PxDefaultMemoryInputData readBuffer(buf.getData(), buf.getSize());
	//triangleMesh = gCooking->createTriangleMesh(TriMeshDesc, gPhysics->getPhysicsInsertionCallback());
	//PxDefaultMemoryOutputStream buf;
	//bool status = gCooking->cookTriangleMesh(TriMeshDesc, buf);

	//if (!triangleMesh)
	//{
	//	DebugTrace("Physics: triangleMesh failed. Line: 158\n");
	//	throw exception("triangleMesh == nullptr!!!");
	//	return;
	//}
	//PxTriangleMeshGeometry triGeom;
	//triGeom.triangleMesh = triangleMesh;
	//meshShape = meshActor->createShape(triGeom, *gMaterial);

	//gScene->addActor(*meshActor);

	//PxConvexMeshCookingResult::Enum result;
	//PxDefaultMemoryOutputStream buf2;

	//if (!gCooking->cookConvexMesh(convexDesc, buf2, &result))
	//{
	//	DebugTrace("Physics: gCooking->cookConvexMesh failed. Line: 175\n");
	//	throw exception("gCooking->cookConvexMesh == nullptr!!!");
	//	return;
	//}
	//PxDefaultMemoryInputData input(buf2.getData(), buf2.getSize());
	//PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);

	//PxConvexMeshGeometry convexGeom = PxConvexMeshGeometry(convexMesh);
	//auto convexShape = meshActor->createShape(convexGeom, *gMaterial);
	//convexShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	//PxGeometryHolder geom = convexShape->getGeometry();

	//PxRigidDynamic* newActor = PxCreateDynamic(*gPhysics, 
	//PxTransform(PxVec3(vertices.data()->Position.x,
	//	vertices.data()->Position.y,
	//	vertices.data()->Position.z)), *convexShape, PxReal(1.0f));
	//if (!newActor)
	//{
	//	DebugTrace("Physics: newActor failed. Line: 193\n");
	//	throw exception("newActor == nullptr!!!");
	//	return;
	//}
	//newActor->setRigidBodyFlags(PxRigidBodyFlag::Enum::eENABLE_CCD | PxRigidBodyFlag::Enum::eENABLE_POSE_INTEGRATION_PREVIEW | PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES);
}

void Engine::Physics::Destroy()
{
	_SAFE_RELEASE(gScene); // Need To Rework It!
	_SAFE_RELEASE(gPhysics);
	_SAFE_RELEASE(gFoundation);
	_SAFE_RELEASE(gCooking);
}

void Engine::Physics::AddNewActor(Vector3 Pos, Vector3 Geom)
{
	PxTransform boxPos(PxVec3(Pos.x, Pos.y, Pos.z));
	PxBoxGeometry boxGeometry(PxVec3(Geom.x, Geom.y, Geom.z));
	gBox = PxCreateDynamic(*gPhysics, boxPos, boxGeometry, *gMaterial, 1.0f);

	gScene->addActor(*gBox);
	DynamicObjects.push_back(gBox);
}