#include "Camera_Control.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"
#include "PhysCamera.h"
#include "Camera.h"

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

	C_Control = static_cast<PxCapsuleController*>
		(Application->getPhysics()->getContrlManager()->createController(capscDescActor));
	if(!C_Control)
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
	PxShape *shape = nullptr;
	charActor->getShapes(&shape, 1);
	PxCapsuleGeometry geom;
	shape->getCapsuleGeometry(geom);

	HeadPos = ToVec3(toVec3(C_Control->getPosition())) +
		Vector3(0.f, geom.halfHeight + geom.radius, 0.f);
}

Vector3 Camera_Control::Update(Vector3 camPos, float Time, Vector3 VDir)
{
	//PCam->Update();

	float jump_height = PCam->getJump()->getHeight(Time);

#if defined (NEVER)
	if (jump_height != 0.0f)
		OutputDebugStringA((string("\nJumpHeight: ") + to_string(jump_height) + string("\n")).c_str());
#endif

	targetKeyDisplacement *= Application->getCamera()->getMoveScale();
	targetKeyDisplacement *= Time;

	targetKeyDisplacement.y += (jump_height == 0.0f ? Application->getPhysics()->getScene()->getGravity().y * Time :
		booster(HeadPos.y, jump_height, Time * 0.5f));

	const PxU32 flags = C_Control->move(targetKeyDisplacement, 0.1f, Time, PxControllerFilters(0));

	if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN)
		PCam->getJump()->Stop();

	if (Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::Space))
		PCam->getJump()->Start(10.0f);

	PosControllerHead();
	HeadPos.y += booster(HeadPos.y, 0.3f, Time * 0.2f); // Also plus our cam position (y ~ 0.81f)

	return HeadPos - VDir;
}

void Camera_Control::GetInput(Vector3 VDir)
{
	targetKeyDisplacement = ToPxVec3(VDir);
}

float Camera_Control::booster(float camTargY, float somethingY, float Delta)
{
	if (Delta < 0.0f)
		Delta = 0.0f;
	else if (Delta > 1.0f)
		Delta = 1.0f;
	return (camTargY * Delta + somethingY * (1.0f - Delta));
}