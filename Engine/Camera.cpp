#include "pch.h"
#include "Camera.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

using namespace DirectX;

HRESULT Camera::Init(float W, float H, float FOV)
{
		// Setup the view matrix
	SetViewParams(Vector3(0, 3, 0), Vector3(1.0f, 0.0f, 0.0f));

		// Setup the projection matrix
	SetProjParams(FOV, W / H, 1.0f, 1000.0f);

	if (Application->getPhysics().operator bool())
	{
		C_CT = make_shared<Camera_Control>();
		C_CT->Init();
	}

	return S_OK;
}

XMMATRIX XMMatrixLookToLH_New(FXMVECTOR EyePosition, FXMVECTOR EyeDirectionOld, FXMVECTOR UpDirection)
{
	XMVECTOR EyeDirection = XMVectorSubtract(EyeDirectionOld, EyePosition);

	assert(!XMVector3IsInfinite(EyeDirection));
	assert(!XMVector3IsInfinite(UpDirection));

	XMVECTOR R2 = XMVector3Normalize(EyeDirection);

	XMVECTOR R0 = XMVector3Cross(UpDirection, R2);
	R0 = XMVector3Normalize(R0);

	XMVECTOR R1 = XMVector3Cross(R2, R0);

	XMVECTOR NegEyePosition = XMVectorNegate(EyePosition);

	XMVECTOR D0 = XMVector3Dot(R0, NegEyePosition);
	XMVECTOR D1 = XMVector3Dot(R1, NegEyePosition);
	XMVECTOR D2 = XMVector3Dot(R2, NegEyePosition);

	XMMATRIX M;
	M.r[0] = XMVectorSelect(D0, R0, g_XMSelect1110.v);
	M.r[1] = XMVectorSelect(D1, R1, g_XMSelect1110.v);
	M.r[2] = XMVectorSelect(D2, R2, g_XMSelect1110.v);
	M.r[3] = g_XMIdentityR3.v;

	M = XMMatrixTranspose(M);

	return M;
}

void Camera::SetViewParams(Vector3 vEyePt, Vector3 vLookatPt)
{
	XMStoreFloat3(&m_vEye, vEyePt);
	XMStoreFloat3(&m_vLookAt, vLookatPt);

	if (!C_CT.operator bool())
	{
		XMStoreFloat3(&m_vDefaultEye, vEyePt);
		XMStoreFloat3(&m_vDefaultLookAt, vLookatPt);
	}
		// Calc the view matrix
	m_mView = XMMatrixLookToLH_New(vEyePt, vLookatPt, Vector3::Up);

	Vector3 zBasis;
	zBasis = vLookatPt;

	m_fCameraPitchAngle = -atan2f(zBasis.y, sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x));
	m_fCameraYawAngle = atan2f(zBasis.x, zBasis.z);
}

XMMATRIX XM_CALLCONV PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
{
	float    SinFov;
	float    CosFov;
	XMScalarSinCos(&SinFov, &CosFov, 0.5f * FovAngleY);

	float fRange = FarZ / (FarZ - NearZ);
	// Note: This is recorded on the stack
	float Height = CosFov / SinFov;
	XMVECTOR rMem = {
		Height / AspectRatio,
		Height,
		fRange,
		-fRange * NearZ
	};
	// Copy from memory to SSE register
	XMVECTOR vValues = rMem;
	XMVECTOR vTemp = _mm_setzero_ps();
	// Copy x only
	vTemp = _mm_move_ss(vTemp, vValues);
	// CosFov / SinFov,0,0,0
	XMMATRIX M;
	M.r[0] = vTemp;
	// 0,Height / AspectRatio,0,0
	vTemp = vValues;
	vTemp = _mm_and_ps(vTemp, g_XMMaskY);
	M.r[1] = vTemp;
	// x=fRange,y=-fRange * NearZ,0,1.0f
	vTemp = _mm_setzero_ps();
	vValues = _mm_shuffle_ps(vValues, g_XMIdentityR3, _MM_SHUFFLE(3, 2, 3, 2));
	// 0,0,fRange,1.0f
	vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(3, 0, 0, 0));
	M.r[2] = vTemp;
	// 0,0,-fRange * NearZ,0.0f
	vTemp = _mm_shuffle_ps(vTemp, vValues, _MM_SHUFFLE(2, 1, 0, 0));
	M.r[3] = vTemp;
	return M;
}

void Camera::SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane)
{
		// Set attributes for the projection matrix
	m_fFOV = fFOV;
	m_fAspect = fAspect;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;
	
	XMStoreFloat4x4(&m_mProj, PerspectiveFovLH(fFOV, fAspect, fNearPlane, fFarPlane));
}

void Camera::Teleport(Vector3 NewPos, Vector3 NewLook, bool NoTPPhysx)
{
	if (C_CT.operator bool() && !NoTPPhysx && (NewPos != Vector3::Zero))
		C_CT->getController()->setPosition(PxExtendedVec3(NewPos.x, NewPos.y, NewPos.z));
	if ((isnan<float>(NewPos.x) || isinf<float>(NewPos.x)) &&
		(isnan<float>(NewPos.y) || isinf<float>(NewPos.y)) &&
		(isnan<float>(NewPos.z) || isinf<float>(NewPos.z)))
		NewPos = Vector3::Zero;
	if ((isnan<float>(NewLook.x) || isinf<float>(NewLook.x)) &&
		(isnan<float>(NewLook.y) || isinf<float>(NewLook.y)) &&
		(isnan<float>(NewLook.z) || isinf<float>(NewLook.z)))
		NewLook = Vector3::Zero;
	if ((isnan<float>(m_vEye.x) || isinf<float>(m_vEye.x)) &&
		(isnan<float>(m_vEye.y) || isinf<float>(m_vEye.y)) &&
		(isnan<float>(m_vEye.z) || isinf<float>(m_vEye.z)))
		Reset();

	SetViewParams(NewPos, NewLook);
}

void Camera::SetNumberOfFramesToSmoothMouseData(int nFrames)
{
	if (nFrames > 0)
		m_fFramesToSmoothMouseData = (float)nFrames;
}

Matrix Camera::GetViewMatrix() const
{
	return m_mView;
}

Matrix Camera::GetProjMatrix() const
{
	return m_mProj;
}

Matrix Camera::GetRotMatrix() const
{
	return mCameraRot;
}

Vector3 Camera::GetLookAtPt() const
{
	return m_vLookAt;
}

float Camera::GetNearClip() const
{
	return m_fNearPlane;
}

float Camera::GetFarClip() const
{
	return m_fFarPlane;
}

float Camera::getMoveScale()
{
	return m_fMoveScaler;
}

float Camera::getRotateScale()
{
	return m_fRotationScaler;
}

void Camera::GetInput(bool bGetKeyboardInput, bool bGetGamepadInput)
{
	if (!DisableCameraCtrl && Application->getKeyboard()->IsConnected())
	{
		m_vKeyboardDirection = Vector3::Zero;
		float speed = 2.0f * Application->getframeTime();

		if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::W))
			if (!FreeCamMove)
				m_vKeyboardDirection += speed * -mCameraRot.Forward();
			else
				m_vKeyboardDirection.z += speed;

		if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::S))
			if (!FreeCamMove)
				m_vKeyboardDirection -= speed * -mCameraRot.Forward();
			else
				m_vKeyboardDirection.z -= speed;

		if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::D))
			if (!FreeCamMove)
				m_vKeyboardDirection += speed * mCameraRot.Right();
			else
				m_vKeyboardDirection.x += speed;

		if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::A))
			if (!FreeCamMove)
				m_vKeyboardDirection -= speed * mCameraRot.Right();
			else
				m_vKeyboardDirection.x -= speed;

		if (m_bEnableYAxisMovement && FreeCamMove)
		{
			if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::E))
				m_vKeyboardDirection.y += speed;

			if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::Q))
				m_vKeyboardDirection.y -= speed;
		}

		if (!FreeCamMove && C_CT.operator bool())
			C_CT->setTargKey(ToPxVec3(m_vKeyboardDirection));
	}
	else
	{
		m_vKeyboardDirection = Vector3::Zero;
		if (!FreeCamMove && C_CT.operator bool())
			C_CT->setTargKey(ToPxVec3(m_vKeyboardDirection));
	}

	if (!DisableCameraCtrl && ((Application->getMouse()->GetState().leftButton && Left) ||
		(Application->getMouse()->GetState().rightButton && Right)) != WithoutButton)
		UpdateMouseDelta();
}

void Camera::UpdateMouseDelta()
{
		// Get current position of mouse
	ptCurMousePos.x = static_cast<float>(Application->getMouse()->GetState().x);
	ptCurMousePos.y = static_cast<float>(Application->getMouse()->GetState().y);

		// Calc how far it's moved since last frame
	ptCurMouseDelta = ptCurMousePos - m_ptLastMousePosition;

		// Record current position for next time
	m_ptLastMousePosition = ptCurMousePos;

	if (m_bResetCursorAfterMove)
	{
		Vector2 ptCenter = Vector2(static_cast<float>(Application->getWorkAreaSize(Application->GetHWND()).x),
			static_cast<float>(Application->getWorkAreaSize(Application->GetHWND()).y)) / 2.f;

		::SetCursorPos(static_cast<int>(ptCenter.x), static_cast<int>(ptCenter.y));
		m_ptLastMousePosition = ptCenter;
	}

	float fPercentOfNew = 1.0f / m_fFramesToSmoothMouseData,
		  fPercentOfOld = 1.0f - fPercentOfNew;
	m_vMouseDelta = m_vMouseDelta * fPercentOfOld + ptCurMouseDelta * fPercentOfNew;

	m_vRotVelocity = m_vMouseDelta * m_fRotationScaler;
}

void Camera::UpdateVelocity(float fElapsedTime)
{
	Vector3 vGamePadRightThumb = XMVectorSet(m_vGamePadRightThumb.x, -m_vGamePadRightThumb.z, 0, 0),

		vMouseDelta = XMLoadFloat2(&m_vMouseDelta),
		vRotVelocity = vMouseDelta * m_fRotationScaler + vGamePadRightThumb * 0.02f;

	XMStoreFloat2(&m_vRotVelocity, vRotVelocity);

	Vector3 vKeyboardDirection = XMLoadFloat3(&m_vKeyboardDirection),
		vGamePadLeftThumb = XMLoadFloat3(&m_vGamePadLeftThumb),
		vAccel = vKeyboardDirection + vGamePadLeftThumb;

	vAccel = XMVector3Normalize(vAccel);

		// Scale the acceleration vector
	vAccel *= (Application->getKeyboard()->GetState().LeftShift ? m_fMoveScaler + 15.f : m_fMoveScaler);

	if (m_bMovementDrag)
	{
			// Is there any acceleration this frame?
		if (XMVectorGetX(XMVector3LengthSq(vAccel)) > 0)
		{
			XMStoreFloat3(&m_vVelocity, vAccel);
			m_fDragTimer = m_fTotalDragTimeToZero;
			XMStoreFloat3(&m_vVelocityDrag, vAccel / m_fDragTimer);
		}
		else
		{
				// If no key being pressed, then slowly decrease velocity to 0
			if (m_fDragTimer > 0)
			{
					// Drag until timer is <= 0
				Vector3 vVelocity = XMLoadFloat3(&m_vVelocity), vVelocityDrag = XMLoadFloat3(&m_vVelocityDrag);

				vVelocity -= vVelocityDrag * fElapsedTime;
				XMStoreFloat3(&m_vVelocity, vVelocity);
				m_fDragTimer -= fElapsedTime;
			}
			else
					// Zero velocity
				m_vVelocity = Vector3::Zero;
		}
	}
	else
			// No drag, so immediately change the velocity
		XMStoreFloat3(&m_vVelocity, vAccel);
}

void Camera::Reset()
{
	//SetViewParams(m_vDefaultEye, m_vDefaultLookAt);
	Teleport(m_vDefaultEye, m_vDefaultLookAt);
}

#include "DebugDraw.h"
//shared_ptr<GrabThing> GThing = make_shared<GrabThing>();
void Camera::FrameMove(float fElapsedTime)
{
	if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::Home))
		Reset();
	//else if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::G))
	//	GThing->Grab();
	//else if (Application->getTrackerKeyboard().IsKeyPressed(Keyboard::Keys::F))
	//	GThing->CheckType(-1);
	//else if (Application->getKeyboard()->GetState().IsKeyDown(Keyboard::Keys::B))
	//	GThing->Drop();

		// Get keyboard/mouse/gamepad input
	GetInput(m_bEnablePositionMovement, true);

		// Get amount of velocity based on the keyboard input and drag (if any)
	UpdateVelocity(fElapsedTime);

		// If rotating the camera
	if (!DisableCameraCtrl && ((Application->getMouse()->GetState().leftButton && Left ||
		Application->getMouse()->GetState().rightButton && Right)
		!= WithoutButton) || m_vGamePadRightThumb.x != 0 || m_vGamePadRightThumb.z != 0)
	{
			// Update the pitch & yaw angle based on mouse movement
		float fYawDelta = m_vRotVelocity.x,
			fPitchDelta = m_vRotVelocity.y;

			// Invert pitch if requested
		if (m_bInvertPitch)
			fPitchDelta = -fPitchDelta;

		m_fCameraPitchAngle += fPitchDelta;
		m_fCameraYawAngle += fYawDelta;

			// Limit pitch to straight up or straight down
		m_fCameraPitchAngle = max(-XM_PIDIV2, m_fCameraPitchAngle);
		m_fCameraPitchAngle = min(XM_PIDIV2, m_fCameraPitchAngle);
	}

		// Make a rotation matrix based on the camera's yaw & pitch
	mCameraRot = XMMatrixRotationRollPitchYaw(m_fCameraPitchAngle, m_fCameraYawAngle, 0.0f);

		// Transform vectors based on camera's rotation matrix
	vWorldUp = XMVector3TransformCoord(g_XMIdentityR1, mCameraRot);
	vWorldAhead = XMVector3TransformCoord(g_XMIdentityR2, mCameraRot);

		// Transform the position delta by the camera's rotation
	if (!m_bEnableYAxisMovement)
		mCameraRot = XMMatrixRotationRollPitchYaw(0.0f, m_fCameraYawAngle, 0.0f);

	Vector3 vEye = XMLoadFloat3(&m_vEye);

	if (Application->getPhysics().operator bool() && C_CT.operator bool() && !FreeCamMove)
		vEye = C_CT->Update(XMVector3TransformCoord(m_vVelocity * fElapsedTime, mCameraRot), fElapsedTime, Vector3::Up);
	else
		vEye += XMVector3TransformCoord(m_vVelocity * fElapsedTime, mCameraRot);

	if (m_bClipToBoundary)
		vEye = ConstrainToBoundary(m_vEye, m_vMinBoundary, m_vMaxBoundary);
	XMStoreFloat3(&m_vEye, vEye);

		// Update the lookAt position based on the eye position
	m_vLookAt = vEye + vWorldAhead;

		// Update the view matrix
	m_mView = XMMatrixLookToLH_New(vEye, m_vLookAt, vWorldUp);

	m_mCameraWorld = m_mView.Invert();
}

Matrix Camera::GetWorldMatrix() const
{
	return XMLoadFloat4x4(&m_mCameraWorld);
}

Vector3 Camera::GetWorldRight() const
{
	return XMLoadFloat3(reinterpret_cast<const Vector3 *>(&m_mCameraWorld._11));
}

Vector3 Camera::GetWorldUp() const
{
	return vWorldUp;
}

Vector3 Camera::GetWorldAhead() const
{
	return vWorldAhead;
}

Vector3 Camera::GetEyePt() const
{
	return XMLoadFloat3(reinterpret_cast<const Vector3 *>(&m_mCameraWorld._41));
}

void Frustum::ConstructFrustum(float screenDepth, Matrix projectionMatrix, Matrix viewMatrix)
{
	float zMinimum = 0.f, r = 0.f;
	Matrix matrix;

	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	matrix = XMMatrixMultiply(viewMatrix, projectionMatrix);

	auto a = matrix._14 + matrix._13;
	auto b = matrix._24 + matrix._23;
	auto c = matrix._34 + matrix._33;
	auto d = matrix._44 + matrix._43;
	m_planes[0] = XMVectorSet(a, b, c, d);
	m_planes[0] = XMPlaneNormalize(m_planes[0]);

	a = matrix._14 - matrix._13;
	b = matrix._24 - matrix._23;
	c = matrix._34 - matrix._33;
	d = matrix._44 - matrix._43;
	m_planes[1] = XMVectorSet(a, b, c, d);
	m_planes[1] = XMPlaneNormalize(m_planes[1]);

	a = matrix._14 + matrix._11;
	b = matrix._24 + matrix._21;
	c = matrix._34 + matrix._31;
	d = matrix._44 + matrix._41;
	m_planes[2] = XMVectorSet(a, b, c, d);
	m_planes[2] = XMPlaneNormalize(m_planes[2]);

	a = matrix._14 - matrix._11;
	b = matrix._24 - matrix._21;
	c = matrix._34 - matrix._31;
	d = matrix._44 - matrix._41;
	m_planes[3] = XMVectorSet(a, b, c, d);
	m_planes[3] = XMPlaneNormalize(m_planes[3]);

	a = matrix._14 - matrix._12;
	b = matrix._24 - matrix._22;
	c = matrix._34 - matrix._32;
	d = matrix._44 - matrix._42;
	m_planes[4] = XMVectorSet(a, b, c, d);
	m_planes[4] = XMPlaneNormalize(m_planes[4]);

	a = matrix._14 + matrix._12;
	b = matrix._24 + matrix._22;
	c = matrix._34 + matrix._32;
	d = matrix._44 + matrix._42;
	m_planes[5] = XMVectorSet(a, b, c, d);
	m_planes[5] = XMPlaneNormalize(m_planes[5]);
}

bool Frustum::CheckPoint(float x, float y, float z)
{
	for (int i = 0; i < 6; i++)
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(x, y, z, 1.0f))) < 0.0f)
			return false;

	return true;
}

bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float size)
{
	for (int i = 0; i < 6; i++)
	{
		float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter - size), (yCenter - size), (zCenter - size), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter + size), (yCenter - size), (zCenter - size), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter - size), (yCenter + size), (zCenter - size), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter + size), (yCenter + size), (zCenter - size), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter - size), (yCenter - size), (zCenter + size), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter + size), (yCenter - size), (zCenter + size), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter - size), (yCenter + size), (zCenter + size), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter + size), (yCenter + size), (zCenter + size), 1.0f)));
		if (ret >= 0.0f)
			continue;

		return false;
	}

	return true;
}

bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter, yCenter, zCenter, 1.0f)));
		if (ret < -radius)
			return false;
	}

	return true;
}

bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	for (int i = 0; i < 6; i++)
	{
		float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter - zSize), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter - zSize), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter - zSize), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter - xSize), (yCenter - ySize), (zCenter + zSize), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter - zSize), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter + xSize), (yCenter - ySize), (zCenter + zSize), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter - xSize), (yCenter + ySize), (zCenter + zSize), 1.0f)));
		if (ret >= 0.0f)
			continue;

		ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((xCenter + xSize), (yCenter + ySize), (zCenter + zSize), 1.0f)));
		if (ret >= 0.0f)
			continue;

		return false;
	}

	return true;
}