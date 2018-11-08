#include "pch.h"
#include "Physics.h"

#define SAFE_RELEASE(p) { if (p) { (p)->release(); (p) = nullptr; } }


HRESULT Physics::Init() //vector<Mesh> *_Mesh
{
	try
	{
		gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
		if (gFoundation == nullptr)
		{
			MessageBoxA(DXUTGetHWND(), "PxCreateFoundation failed, Exiting! line 9", "ERROR!", MB_OK);
			return E_FAIL;
			IsInitPhysX = false;
		}

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale());
		if (gPhysics == nullptr)
		{
			MessageBoxA(DXUTGetHWND(), "Error in creating PhysX3 device, Exiting! line 17", "ERROR!", MB_OK);
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
			MessageBoxA(DXUTGetHWND(), "Error in gPhysics->createScene(), Exiting! line 29", "ERROR!", MB_OK);
			return E_FAIL;
			IsInitPhysX = false;
		}
		//static friction, dynamic friction, restitution
		gMaterial = gPhysics->createMaterial(0.5, 0.5, 0.5);
		if (!gMaterial)
		{
			MessageBoxA(DXUTGetHWND(), "Error in gPhysics->createMaterial(), Exiting! line 37", "ERROR!", MB_OK);
			return E_FAIL;
			IsInitPhysX = false;
		}
		PxTransform planePos = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));

		gPlane = gPhysics->createRigidStatic(planePos);
		if (!gPlane)
		{
			MessageBoxA(DXUTGetHWND(), "Error in gPhysics->createRigidStatic(), Exiting! line 46", "ERROR!", MB_OK);
			return E_FAIL;
			IsInitPhysX = false;
		}
		gPlane->createShape(PxPlaneGeometry(), *gMaterial);
		gScene->addActor(*gPlane);

		//vector<VertexPositionNormalTexture> vertices = 
		//{
		//	VertexPositionNormalTexture::VertexPositionNormalTexture(
		//	XMFLOAT3(_Mesh->data()->vertices.data()->X,
		//		_Mesh->data()->vertices.data()->Y,
		//		_Mesh->data()->vertices.data()->Z),
		//	XMFLOAT3(_Mesh->data()->vertices.data()->X,
		//		_Mesh->data()->vertices.data()->Y,
		//		_Mesh->data()->vertices.data()->Z),
		//	XMFLOAT2(_Mesh->data()->vertices.data()->texcoord.x,
		//		_Mesh->data()->vertices.data()->texcoord.y)) 
		//};

		m_shape = GeometricPrimitive::CreateBox(DXUTGetD3D11DeviceContext(), 
			Vector3(1.5f, 1.5f, 1.5f));
			//vertices,
			//vector<uint16_t>(_Mesh->data()->indices.size()));
		PxTransform boxPos(PxVec3(0.5f, 5.0f, 0.5f));
		PxBoxGeometry boxGeometry(PxVec3(0.5f, 0.5f, 0.5f));
		gBox = PxCreateDynamic(*gPhysics, boxPos, boxGeometry, *gMaterial, 1.0f);

		gScene->addActor(*gBox);
		DynamicObjects.push_back(gBox);
		
		PxCookingParams params(gPhysics->getTolerancesScale());
		params.meshWeldTolerance = 0.001f;
		params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
		params.buildGPUData = true; //Enable GRB data being produced in cooking.
		gCooking = PxCreateCooking(PX_PHYSICS_VERSION, *gFoundation, params);
		if (!gCooking)
		{
			MessageBoxA(DXUTGetHWND(), "Error in gPhysics->createRigidStatic(), Exiting! line 60", "ERROR!", MB_OK);
			return E_FAIL;
			IsInitPhysX = false;
		}
		IsInitPhysX = true;

		getDeviceD3D();
		getDeviceConD3D();
		return S_OK;
	}
	catch (const std::exception&)
	{
		IsInitPhysX = false;
		return E_FAIL;
	}
}

void Physics::Simulation(Matrix World, Matrix View, Matrix Proj)
{
	Matrix _World;
	gScene->simulate(Timestep);
	gScene->fetchResults(true);

	PxShape* shapes[128];
	const PxU32 nbShapes = gBox->getNbShapes();
	gBox->getShapes(shapes, nbShapes);

	const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[0], *gBox));
	PxVec4 coll;
	coll = shapePose.column0;
	_World._11 = coll.x;
	_World._12 = coll.y;
	_World._13 = coll.z;
	_World._14 = coll.w;
	coll = shapePose.column1;
	_World._21 = coll.x;
	_World._22 = coll.y;
	_World._23 = coll.z;
	_World._24 = coll.w;
	coll = shapePose.column2;
	_World._31 = coll.x;
	_World._32 = coll.y;
	_World._33 = coll.z;
	_World._34 = coll.w;
	coll = shapePose.column3;
	_World._41 = coll.x;
	_World._42 = coll.y;
	_World._43 = coll.z;
	_World._44 = coll.w;

	m_shape->Draw(_World, View, Proj);

#if defined(Never)
	for (int i = 0; i <= 300; i++)
	{
		PxVec3 boxPos = DynamicObjects[0]->getGlobalPose().p;
		char buff[100];
		snprintf(buff, sizeof(buff), "\nBox current Position (%f), (%f), (%f)\n", boxPos.x, boxPos.y, boxPos.z);
		std::string buffAsStdStr = buff;
		OutputDebugStringA(buffAsStdStr.c_str());
	}
#endif
}

void Physics::_createConvexMesh()
{
	int NumVerticies = _Mesh->data()->vertices.size();
	int NumTriangles = _Mesh->data()->indices.size() / 3;


	PxVec3* verts = new PxVec3[NumVerticies / 3];
	int ii = -1;
	for (int i = 0; i < NumVerticies / 3; i++)
	{
		++ii;
		verts[i].x = _Mesh->data()->vertices[ii].X;
		verts[i].y = _Mesh->data()->vertices[++ii].Y;
		verts[i].z = _Mesh->data()->vertices[++ii].Z;
	}

	PxU16 *tris = new PxU16[_Mesh->data()->indices.size()];
	for (int i = _Mesh->data()->indices.size() - 1; i >= 0; i--)
		tris[i] = _Mesh->data()->indices[i];

	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = NumVerticies;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = &_Mesh->data()->vertices[3];
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

	bool Valid_ConvexMesh = convexDesc.isValid();


	PxTriangleMeshDesc TriMeshDesc;
	TriMeshDesc.points.count = NumVerticies;
	TriMeshDesc.points.stride = sizeof(PxVec3);
	TriMeshDesc.points.data = &_Mesh->data()->vertices[6]; // XYZ
	TriMeshDesc.triangles.count = NumTriangles;
	TriMeshDesc.triangles.data = &_Mesh->data()->indices[3];
	TriMeshDesc.triangles.stride = 3 * sizeof(PxU32);
	TriMeshDesc.flags = PxMeshFlag::Enum::e16_BIT_INDICES;//eCOMPUTE_CONVEX;

	bool Valid_TriMesh = TriMeshDesc.isValid();

	meshActor = gPhysics->createRigidDynamic(PxTransform(1.0f, 1.0f, 1.0f));
	PxShape* meshShape;
	if (!meshActor)
	{
		MessageBoxA(DXUTGetHWND(), "Error in create createRigidDynamic! line: 137. PhysX", "Error", MB_OK);
		return;
	}
	meshActor->setRigidBodyFlag(PxRigidBodyFlag::Enum::eKINEMATIC, true);

	//PxDefaultMemoryInputData readBuffer(buf.getData(), buf.getSize());
	triangleMesh = gCooking->createTriangleMesh(TriMeshDesc, gPhysics->getPhysicsInsertionCallback());
	PxDefaultMemoryOutputStream buf;
	bool status = gCooking->cookTriangleMesh(TriMeshDesc, buf);

	if (!triangleMesh)
	{
		MessageBoxA(DXUTGetHWND(), "Error in create createTriangleMesh! line: 149. PhysX", "Error", MB_OK);
		return;
	}
	PxTriangleMeshGeometry triGeom;
	triGeom.triangleMesh = triangleMesh;
	meshShape = meshActor->createShape(triGeom, *gMaterial);

	gScene->addActor(*meshActor);

	//DynamicObjects.push_back(meshActor);

	PxConvexMeshCookingResult::Enum result;
	PxDefaultMemoryOutputStream buf2;

	if (!gCooking->cookConvexMesh(convexDesc, buf2, &result))
	{
		MessageBoxA(DXUTGetHWND(), "Error in cookConvexMesh! line: 165. PhysX", "Error", MB_OK);
		return;
	}
	PxDefaultMemoryInputData input(buf2.getData(), buf2.getSize());
	PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);

	PxConvexMeshGeometry convexGeom = PxConvexMeshGeometry(convexMesh);
	auto convexShape = meshActor->createShape(convexGeom, *gMaterial);
	convexShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	PxGeometryHolder geom = convexShape->getGeometry();

	PxRigidDynamic* newActor = PxCreateDynamic(*gPhysics, 
	PxTransform(PxVec3(_Mesh->data()->vertices.data()->X,
		_Mesh->data()->vertices.data()->Y,
		_Mesh->data()->vertices.data()->Z)), *convexShape, PxReal(1.0f));
	if (!newActor)
	{
		MessageBoxA(DXUTGetHWND(), "Error in newActor = PxCreateDynamic! line: 177. PhysX", "Error", MB_OK);
		return;
	}
	newActor->setRigidBodyFlags(PxRigidBodyFlag::Enum::eENABLE_CCD | PxRigidBodyFlag::Enum::eENABLE_POSE_INTEGRATION_PREVIEW | PxRigidBodyFlag::Enum::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES);
}

void Physics::GenTriangleMesh(PxVec3 pos, vector<Mesh::VERTEX> indices, vector<UINT> vertices)
{
	//int NumVerticies = vertices.size() / 3;
	//int NumTriangles = indices.size() / 3;

	////Create pointer for vertices  
	//PxVec3* verts = new PxVec3[NumVerticies];
	//int ii = -1;
	//for (int i = 0; i < NumVerticies; i++)
	//{
	//	++ii;
	//	verts[i].x = indices[ii].X;
	//	verts[i].y = indices[++ii].Y;
	//	verts[i].z = indices[++ii].Z;
	//}

	////Create pointer for indices  
	//PxU16 *tris = new PxU16[vertices.size()];
	//for (int i = vertices.size() - 1; i >= 0; i--)
	//	tris[i] = vertices[i];
	
	// Build physical model 
	//PxShape* meshShape;
	//if (meshActor)
	//{
	//	meshActor->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, true);

	//	PxTriangleMeshGeometry triGeom;
	//	triGeom.triangleMesh = triangleMesh;
	//	meshShape = meshActor->createShape(triGeom, defaultMaterial);
	//	getScene().addActor(*meshActor);
	//}

	//PxDefaultMemoryOutputStream writeBuffer;
	//bool status = gCooking->cookConvexMesh(meshDesc, writeBuffer);
	//if (!status)
	//	return;


	//SAFE_DELETE_ARRAY(verts);
	//SAFE_DELETE_ARRAY(tris);
}

void Physics::Destroy()
{
	SAFE_RELEASE(gScene);
	SAFE_RELEASE(gPhysics);
	SAFE_RELEASE(gFoundation);
	SAFE_RELEASE(gCooking);
}
