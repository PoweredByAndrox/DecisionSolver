#include "pch.h"

#include "Physics.h"

vector<shared_ptr<GeometricPrimitive>> Cobes;

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "CommonStates.h"
#include "PrimitiveBatch.h"
#include <Effects.h>
#include "Camera.h"

HRESULT Physics::Init()
{
	try
	{
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
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

		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
		if (!gPhysics)
		{
			DebugTrace("Physics: gPhysics failed.\n");
			throw exception("gPhysics == nullptr!!!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		PxInitExtensions(*gPhysics, gPvd);

		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
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

		PxTransform planePos = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
		gPlane = gPhysics->createRigidStatic(planePos);
		if (!gPlane)
		{
			DebugTrace("Physics: gPlane failed.\n");
			throw exception("gPlane == nullptr!!!");
			return E_FAIL;
		}

		gScene->addActor(*PxRigidActorExt::createExclusiveShape(*gPlane, PxPlaneGeometry(), *gMaterial)->getActor());

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
		return S_OK;
	}
	catch (const exception&)
	{
		DebugTrace("Physics: Init failed.\n");
		throw exception("PhysX initialization error!!!");
		return E_FAIL;
	}
}

void Physics::Simulation(float Timestep)
{
	for (int i = 0; i < Cobes.size(); i++)
	{
		vector<PxQuat> aq;
		vector<PxVec3> pos;
		auto PhysObj = DynamicObjects;

		for (int i1 = 0; i1 < PhysObj.size(); i1++)
		{
			aq.push_back(PhysObj.at(i1)->getGlobalPose().q);
			pos.push_back(PhysObj.at(i1)->getGlobalPose().p);

			Cobes.at(i)->Draw(Matrix::CreateFromQuaternion(Quaternion(aq.at(i1).x, aq.at(i1).y, aq.at(i1).z, aq.at(i1).w)) *
				Matrix::CreateTranslation(Vector3(pos.at(i1).x, pos.at(i1).y, pos.at(i1).z)),
				Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix(), Colors::DarkSeaGreen,
				nullptr, Application->IsWireFrame());
		}
	}

	if (!Application->PausePhysics())
	{
		gScene->simulate(Timestep);
		gScene->fetchResults(true);
	}
}

/*
void Physics::_createTriMesh(Models *Model, bool stat_dyn)
{
	auto Meshes = Model;

	vector<PxVec3> verts;
	vector<PxU32> tris;

	for (int i = 0; i < Meshes.size(); i++)
	{
		auto Obj = Meshes.at(i).vertices;
		for (int i1 = 0; i1 < Obj.size(); i1++)
			verts.push_back(ToPxVec3(Obj.at(i1).Position));

		auto Obj1 = Meshes.at(i).indices;
		for (int i1 = 0; i1 < Obj1.size(); i1++)
			tris.push_back(Obj1.at(i1));
	}

	PxTriangleMeshDesc TriMeshDesc;
	TriMeshDesc.points.count = tris.size();
	TriMeshDesc.points.stride = sizeof(PxVec3);
	TriMeshDesc.points.data = verts.data();

	TriMeshDesc.triangles.count = tris.size() / 3;
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
	if (stat_dyn)
	{
		PxRigidStatic *TriMesh = gPhysics->createRigidStatic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
		gScene->addActor(*TriMesh); //PxRigidActorExt::createExclusiveShape(*TriMesh, PxTriangleMeshGeometry(triangleMesh), *gMaterial)->getActor());
		StaticObjects.push_back(TriMesh);
	}
	if (!stat_dyn)
	{
		PxRigidDynamic *TriMesh = gPhysics->createRigidDynamic(PxTransform(PxVec3(0.0f, 0.0f, 0.0f)));
		gScene->addActor(*PxRigidActorExt::createExclusiveShape(*StaticObjects.at(1), PxSphereGeometry(10.f), *gMaterial)->getActor());
		DynamicObjects.push_back(TriMesh);
	}
}
void Physics::SetPhysicsForCamera(Vector3 Pos, Vector3 Geom) // Position Camera // Geometry to default
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
*/

void Physics::Destroy()
{
	if (gPvd)
	{
		if (gPvd->getTransport() && gPvd->getTransport()->isConnected())
			gPvd->getTransport()->flush();
		gPvd->disconnect();
		gPvd->release();
		transport->release();
	}
	SAFE_release(gCooking);
	SAFE_release(gScene);
	SAFE_release(gPhysics);
	SAFE_release(gFoundation);
}

void Physics::AddNewActor(Vector3 Pos, Vector3 Geom, float Mass, float SizeModel)
{
	if (SizeModel <= 0)
		SizeModel = 10.f;
	if (Mass <= 0)
		Mass = 10.f;
	if (Geom == Vector3::Zero)
		Geom = Vector3::One;

	gBox = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(Pos.x, Pos.y, Pos.z)), PxBoxGeometry(PxVec3(Geom.x, Geom.y, Geom.z)), *gMaterial, 1.0f);

	gBox->setMass(Mass);

	gScene->addActor(*gBox);
	Cobes.push_back(GeometricPrimitive::CreateCube(Application->getDeviceContext(), SizeModel, false));

	DynamicObjects.push_back(gBox);
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
