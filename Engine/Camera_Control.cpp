#include "Camera_Control.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "PhysCamera.h"
#include "Camera.h"

PxCapsuleController *Camera_Control::C_Control = nullptr;

void Camera_Control::Init()
{
	PCam = make_shared<PhysCamera>();

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

Vector3 Camera_Control::Update(Vector3 camPos, float Time, Vector3 VDir)
{
	//PCam->Update();

	auto Jump = PCam->getJump();
	float jump_height = Jump->getHeight(Time),
		fScaleMove = Application->getCamera()->getMoveScale(), X = fScaleMove, Z = fScaleMove;
	PosControllerHead();

	targetKeyDisplacement.x *= X;
	targetKeyDisplacement.z *= Z;

	targetKeyDisplacement.y += (jump_height == 0.0f && Jump->getCanJump() ?
		Application->getPhysics()->getScene()->getGravity().y * Time :
		jump_height);

	const PxU32 flags = C_Control->move(targetKeyDisplacement, 0.0f, Time, PxControllerFilters(0));

	if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
		Jump->Stop();

	if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::Space))
		Jump->Start(5.5f);

	return HeadPos - VDir;
}

void Camera_Control::GetInput(Vector3 VDir)
{
	targetKeyDisplacement = ToPxVec3(VDir);
}