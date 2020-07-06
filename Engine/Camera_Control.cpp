#include "Camera_Control.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "PhysCamera.h"
#include "Camera.h"
#include "Audio.h"
#include "File_system.h"

PxCapsuleController *Camera_Control::C_Control = nullptr;
string jmpSnd, dwnSnd;

void Camera_Control::Init()
{
	PCam = make_shared<PhysCamera>();
	auto Obj = Application->getFS()->GetFile("Start Jump");
	Application->getSound()->AddNewFile(Obj->PathA, false);
	jmpSnd = Obj->FileA;

	Obj = Application->getFS()->GetFile("Stop Jump");
	Application->getSound()->AddNewFile(Obj->PathA, false);
	dwnSnd = Obj->FileA;

	//capscDescActor->density = cDescActor.ProxyDensity;
	//capscDescActor->scaleCoeff = cDescActor.ProxyScale;
	capscDescActor.material = Application->getPhysics()->getMaterial();
	capscDescActor.position = ToExtended(Vector3(0.f, 5.f, 0.f));
	capscDescActor.height = 1.f;
	capscDescActor.radius = 0.5f;
	//capscDescActor->slopeLimit = cDescActor.SlopeLimit;
	capscDescActor.contactOffset = 0.01f;
	capscDescActor.stepOffset = 0.01f;
	capscDescActor.reportCallback = nullptr;
	capscDescActor.behaviorCallback = nullptr;

	C_Control = static_cast<PxCapsuleController *>
		(Application->getPhysics()->getContrlManager()->createController(capscDescActor));
	if (!C_Control)
	{
		// ToDo Exception Of "Cannot Create a Phys Control Camera"
	}

	PxRigidDynamic *actor = C_Control->getActor();
	if (actor)
	{
		if (actor->getNbShapes())
		{
			PxShape *ctrlShape = nullptr;
			actor->getShapes(&ctrlShape, 1);
			ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		}
	}
}

void Camera_Control::PosControllerHead()
{
	PxRigidActor *charActor = C_Control->getActor();
	if (!charActor)
		return;

	PxShape *shape = nullptr;
	charActor->getShapes(&shape, 1);
	if (!shape)
		return;

	PxCapsuleGeometry geom;
	shape->getCapsuleGeometry(geom);

	HeadPos = ToVec3(toVec3(C_Control->getPosition())) +
		Vector3(0.f, geom.halfHeight + geom.radius, 0.f);
}
//PCam->Update(); // Stand up and crouch

ToDo("Run By Shift");
Vector3 Camera_Control::Update(Vector3 camPos, float Time, Vector3 VDir)
{
	if (!C_Control) return camPos;

	auto Jump = PCam->getJump();
	float MGr = Application->getPhysics()->getScene()->getGravity().y;
	if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::Space) && Jump->getCanJump())
	{
		if (!jmpSnd.empty())
			Application->getSound()->doPlay(jmpSnd);
		Jump->Start(5.5f);
	}

	float jump_height = Jump->getHeight(Time),
		fScaleMove = Application->getCamera()->getMoveScale();
	
	targetKeyDisplacement.y = jump_height;

	flags = C_Control->move(targetKeyDisplacement, 0.0f, Time, PxControllerFilters(0));

	if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		if (!dwnSnd.empty() && targetKeyDisplacement.y <= 0.0f && !Jump->getCanJump())
			Application->getSound()->doPlay(dwnSnd);
		Jump->Stop();
	}

	PosControllerHead();
	return HeadPos + VDir;
}

void Camera_Control::GetInput(Vector3 VDir)
{
	targetKeyDisplacement = ToPxVec3(VDir);
}