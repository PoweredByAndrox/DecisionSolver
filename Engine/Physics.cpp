#include "pch.h"

#include "Physics.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "Camera.h"
#include "Console.h"
#include "DebugDraw.h"

shared_ptr<StepTimer> TM1 = make_shared<StepTimer>();
HRESULT Physics::Init()
{
	try
	{
		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
			gDefaultErrorCallback);
		if (!gFoundation)
		{

#if defined (DEBUG)
			DebugTrace("Physics::Init->PxCreateFoundation Is nullptr!");
#endif 
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->PxCreateFoundation Is nullptr!");
#endif
			Console::LogError("Physics: Something is wrong with create PhysX Foundation!");
			IsInitPhysX = false;
			return E_FAIL;
		}

#if defined (DEBUG)
		gPvd = PxCreatePvd(*gFoundation);
		if (!gPvd)
		{
			DebugTrace("Physics::Init->PxCreatePvd() Was Triggered!");
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->PxCreatePvd() Was Triggered!");
#endif
			Console::LogError("Physics: Something is wrong with create PVD!");
			IsInitPhysX = false;
			return E_FAIL;
		}

		transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
		//transport = PxDefaultPvdFileTransportCreate("sample.pxd2");
		if (!transport)
		{
			DebugTrace("Physics::Init->PxDefaultPvdSocketTransportCreate Is Failed!");
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->PxDefaultPvdSocketTransportCreate Is Failed!");
#endif
			Console::LogError("Physics: Something is wrong with create PVD Socket!");
			IsInitPhysX = false;
			return E_FAIL;
		}

		gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif
		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true,
#if defined (DEBUG)
			gPvd
#else
			nullptr
#endif 
);
		if (!gPhysics)
		{
#if defined (DEBUG)
			DebugTrace("Physics::Init->gPhysics Is nullptr!");
#endif 
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->gPhysics Is nullptr!");
#endif
			Console::LogError("Physics: Something is wrong with create PhysX!");
			return E_FAIL;
			IsInitPhysX = false;
		}

		PxInitExtensions(*gPhysics,
#if defined (DEBUG)
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
#if defined (DEBUG)
			DebugTrace("Physics::Init->gScene Is nullptr!");
#endif 
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->gScene Is nullptr!");
#endif
			Console::LogError("Physics: Something is wrong with create PhysX Scene!");
			IsInitPhysX = false;
			return E_FAIL;
		}

		ControllerManager = PxCreateControllerManager(*gScene);
		if (!ControllerManager)
		{
#if defined (DEBUG)
			DebugTrace("Physics::Init->ControllerManager Is nullptr!");
#endif 
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->ControllerManager Is nullptr!");
#endif
			Console::LogError("Physics: Something is wrong with create PhysX Contoller Manager!");
		}

#if defined (DEBUG)
		pvdClient = gScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
#endif

		// Params (below)
			//static friction, dynamic friction, restitution
		gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
		if (!gMaterial)
		{
#if defined (DEBUG)
			DebugTrace("Physics::Init->gMaterial Is nullptr!");
#endif 
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->gMaterial Is nullptr!");
#endif
			Console::LogError("Physics: Something is wrong with create PhysX Material!");
			IsInitPhysX = false;
			return E_FAIL;
		}

		PxTransform planePos = PxTransform(PxVec3(0.0f, 0.0f, 0.0f), PxQuat(PxHalfPi,
			PxVec3(0.0f, 0.0f, 1.0f)));
		gPlane = gPhysics->createRigidStatic(planePos);
		if (!gPlane)
		{
#if defined (DEBUG)
			DebugTrace("Physics::Init->gPlane Is nullptr!");
#endif 
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->gPlane Is nullptr!");
#endif
			Console::LogError("Physics: Something is wrong with create PhysX Plane!");
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
#if defined (DEBUG)
			DebugTrace("Physics::Init->gCooking Is nullptr!");
#endif 
#if defined (ExceptionWhenEachError)
			throw exception("Physics::Init->gCooking Is nullptr!");
#endif
			Console::LogError("Physics: Something is wrong with create PhysX Cook!");
			IsInitPhysX = false;
			return E_FAIL;
		}

		TM1->SetFixedTimeStep(true);

		IsInitPhysX = true;
		return S_OK;
	}
	catch (const exception &Catch)
	{
#if defined (DEBUG)
		DebugTrace(string(string("Physics::Init->catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif 
#if defined (ExceptionWhenEachError)
		throw exception(string(string("Physics::Init->catch() Was Triggered!\nReturn Error Text:")
			+ Catch.what()).c_str());
#endif
		Console::LogError(string(string("Physics: Something is wrong with Init Function!"\
			"\nReturn Error Text:")
			+ Catch.what()).c_str());
		return E_FAIL;
	}
}

PxVec3 Physics::ConstrainToBoundary(PxVec3 Pos, PxVec3 Min, PxVec3 Max)
{
	Vector3 vMin = ToVec3(Min),
		vMax = ToVec3(Max),
		ConvertPos = ToVec3(Pos);

	ConvertPos.Clamp(vMin, vMax);
	return ToPxVec3(ConvertPos);
}

ToDo("TEST LOGIC!!!")
float Temp = 0.05f;
void Physics::Simulation(float Timestep)
{
	TM1->Tick();

	for (size_t i = 0; i < Cobes.size(); i++)
	{
		vector<PxQuat> aq;
		vector<PxVec3> pos;
		auto PhysObj = DynamicObjects;

		for (size_t i1 = 0; i1 < PhysObj.size(); i1++)
		{
			aq.push_back(PhysObj.at(i1)->getGlobalPose().q);
			pos.push_back(PhysObj.at(i1)->getGlobalPose().p);

			Cobes.at(i)->Draw(Matrix::CreateFromQuaternion(
				Quaternion(aq.at(i1).x, aq.at(i1).y, aq.at(i1).z, aq.at(i1).w)) *
				Matrix::CreateTranslation(Vector3(pos.at(i1).x, pos.at(i1).y, pos.at(i1).z)),
				Application->getCamera()->GetViewMatrix(), Application->getCamera()->GetProjMatrix(),
				Colors::DarkSeaGreen, nullptr, Application->IsWireFrame());

			if (GetAsyncKeyState(VK_OEM_PLUS))
				Temp += 0.05f;
			else if (GetAsyncKeyState(VK_OEM_MINUS))
			{
				Temp -= 0.05f;
				if (Temp <= 0.0f)
					Temp = 0.05f;
			}

			TM1->SetTargetElapsedSeconds(Temp);

			float time = (float)TM1->GetElapsedSeconds();

			OutputDebugStringA((boost::format("\nTargSec: %f, Time: %f") % Temp % time).str().c_str());
			auto P = PhysObj.at(i)->getGlobalPose();
		
			OutputDebugStringA((boost::format(" Pos: X: %f, Y: %f, Z: %f\n")
				% P.p.x% P.p.y% P.p.z).str().c_str());

			PhysObj.at(i)->setGlobalPose(PxTransform(ConstrainToBoundary(PxVec3(P.p.x + time, P.p.y, P.p.z),
				PxVec3(-100.f, -100.f, -100.f), PxVec3(100.f, 100.f, 100.f)), PxIdentity));
		}
	}

	if (!Application->IsSimulatePhysics())
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

	for (size_t i = 0; i < Meshes.size(); i++)
	{
		auto Obj = Meshes.at(i).vertices;
		for (size_t i1 = 0; i1 < Obj.size(); i1++)
			verts.push_back(ToPxVec3(Obj.at(i1).Position));

		auto Obj1 = Meshes.at(i).indices;
		for (size_t i1 = 0; i1 < Obj1.size(); i1++)
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
	ClearAllObj();

#if defined (DEBUG)
	if (gPvd)
	{
		if (gPvd->getTransport() && gPvd->getTransport()->isConnected())
			gPvd->getTransport()->flush();
		if (gPvd->isConnected())
			gPvd->disconnect();
		//gPvd->release();
		if (transport)
		{
			if (transport->isConnected())
				transport->disconnect();
		}
	}
#endif
	if (ControllerManager)
		ControllerManager->release();
	if (gCooking)
		gCooking->release();
	if (gScene)
		gScene->release();
	if (gPhysics)
		gPhysics->release();
	if (gFoundation)
		gFoundation->release();
}

void Physics::AddNewActor(Vector3 Pos, Vector3 Geom, float Mass, float SizeModel)
{
	if (SizeModel <= 0)
		SizeModel = 10.f;
	if (Mass <= 0)
		Mass = 10.f;
	if (Geom == Vector3::Zero)
		Geom = Vector3::One;

	gBox = PxCreateDynamic(*gPhysics, PxTransform(PxVec3(Pos.x, Pos.y, Pos.z)),
		PxBoxGeometry(PxVec3(Geom.x, Geom.y, Geom.z)), *gMaterial, 1.0f);

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

PxExtendedVec3 ToExtended(Vector3 var)
{
	return PxExtendedVec3(var.x, var.y, var.z);
}

Vector3 ToExtended(PxExtendedVec3 var)
{
	return Vector3((float)var.x, (float)var.y, (float)var.z);
}
