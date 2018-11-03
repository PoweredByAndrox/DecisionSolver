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

	PxTransform planePos = PxTransform(PxVec3(0.0f, 0, 0.0f), 
		PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));

	plane = gPhysicsSDK->createRigidStatic(planePos);
	plane->createShape(PxPlaneGeometry(), *mMaterial);
	gScene->addActor(*plane);

	PxTransform boxPos(PxVec3(0.0f, 10.0f, 0.0f));
	PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
	gBox = PxCreateDynamic(*gPhysicsSDK, boxPos, boxGeometry, *mMaterial, 1.0f);
	gScene->addActor(*gBox);

	NumberRigDyn.push_back(gBox);

	return S_OK;
}

void Physics::Simulation()
{
	gScene->simulate(myTimestep);
	gScene->fetchResults(true);
#ifdef DEBUG
	for (int i = 0; i <= 300; i++)
	{
		PxVec3 boxPos = gBox->getGlobalPose().p;
		char buff[100];
		snprintf(buff, sizeof(buff), "\nBox current Position (%f), (%f), (%f)\n", boxPos.x, boxPos.y, boxPos.z);
		std::string buffAsStdStr = buff;
		OutputDebugStringA(buffAsStdStr.c_str());
	}
#endif
}

void Physics::SetGravity(PxRigidDynamic *RigDyn, PxVec3 Vec3)
{
	RigDyn->getScene()->setGravity(Vec3);
}

void Physics::SetMass(PxRigidDynamic *RigDyn, PxReal Mass)
{
	RigDyn->setMass(Mass);
}

void Physics::AddTorque(PxRigidDynamic *RigDyn, PxVec3 Vec3, PxForceMode::Enum ForceMode)
{
	RigDyn->addTorque(Vec3, ForceMode);
}

PxActor* Physics::GenTriangleMesh(PxVec3 pos, vector<short> indices, vector<float> vertices)
{
	int NumVerticies = vertices.size() / 3;
	int NumTriangles = indices.size() / 3;

	//Create pointer for vertices  
	PxVec3* verts = new PxVec3[NumVerticies];
	int ii = -1;
	for (int i = 0; i < NumVerticies; i++)
	{
		++ii;
		verts[i].x = vertices[ii];
		verts[i].y = vertices[++ii];
		verts[i].z = vertices[++ii];
	}

	//Create pointer for indices  
	PxU16 *tris = new PxU16[indices.size()];
	for (int i = indices.size() - 1; i >= 0; i--)
		tris[i] = i;

	// Build physical model  
	//PxTriangleMeshDesc TriMeshDesc;
	//TriMeshDesc.numVertices = NumVerticies;
	//TriMeshDesc.numTriangles = NumTriangles;
	//TriMeshDesc.pointStrideBytes = sizeof(PxVec3);
	//TriMeshDesc.triangleStrideBytes = 3 * sizeof(PxU16);
	//TriMeshDesc.points = verts;
	//TriMeshDesc.triangles = tris;
	//TriMeshDesc.flags = NX_MF_16_BIT_INDICES;//| NX_MF_FLIPNORMALS ;  
	//PxTriangleMeshShapeDesc ShapeDesc;
	//PxInitCooking();

	// Cooking from memory  
	//MemoryWriteBuffer buf;
	//bool status = PxCookTriangleMesh(TriMeshDesc, buf);
	//ShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
	//PxActorDesc actorDesc;
	//actorDesc.shapes.pushBack(&ShapeDesc);
	//actorDesc.globalPose.t = pos;
	//PxActor* act = gScene->createActor(actorDesc);
	delete[] verts;
	delete[] tris;
	return nullptr;
}

vector<PxRigidDynamic*> Physics::GetPhysObject()
{
	return NumberRigDyn;
}

void Physics::Destroy()
{
	gScene->release();
	gPhysicsSDK->release();
	gFoundation->release();
}
