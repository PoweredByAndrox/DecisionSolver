#pragma once
#if !defined(__CAMERA_CONTROL_H__)
#define __CAMERA_CONTROL_H__
#include "pch.h"

#include "Physics.h"

class PhysCamera;
class Camera_Control
{
private:
	float booster(float camTargY, float somethingY, float Delta);

protected:
	PxBoxControllerDesc boxcDescActor;
	PxCapsuleControllerDesc capscDescActor;
	static PxCapsuleController *C_Control;

	PxVec3 targetKeyDisplacement;

	shared_ptr<PhysCamera> PCam;
	void PosControllerHead();
	Vector3 HeadPos = Vector3::Zero;
public:
	void Init();

	//Return the coordinates from Camera Into PhysX Sphere
	Vector3 Update(Vector3 camPos, float Time, Vector3 VDir);

	void GetInput(Vector3 VDir);

	shared_ptr<PhysCamera> getPCam() { return PCam; }

	auto getController() { return C_Control; }
	void setTargKey(PxVec3 Vec3) { targetKeyDisplacement = Vec3; }

	Vector3 getHeadPos() { return HeadPos; }
	//~Camera_Control()
	//{
		//SAFE_release(C_Control);
	//}
};
#endif
