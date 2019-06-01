#include "pch.h"

#include "Camera.h"

class Engine;
extern shared_ptr<Engine> Application;
#include "Engine.h"

using namespace DirectX;

void Camera::SetViewParams(Vector3 vEyePt, Vector3 vLookatPt)
{
	XMStoreFloat3(&m_vEye, vEyePt);
	XMStoreFloat3(&m_vDefaultEye, vEyePt);

	XMStoreFloat3(&m_vLookAt, vLookatPt);
	XMStoreFloat3(&m_vDefaultLookAt, vLookatPt);

		// Calc the view matrix
	m_mView = XMMatrixLookAtLH(vEyePt, vLookatPt, Vector3::Up);

	Vector3 zBasis;
	zBasis = vLookatPt;

	m_fCameraPitchAngle = -atan2f(zBasis.y, sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x));
	m_fCameraYawAngle = atan2f(zBasis.x, zBasis.z);
}

void Camera::SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane)
{
		// Set attributes for the projection matrix
	m_fFOV = fFOV;
	m_fAspect = fAspect;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;

	XMStoreFloat4x4(&m_mProj, XMMatrixPerspectiveFovLH(fFOV, fAspect, fNearPlane, fFarPlane));
}

void Camera::GetInput(bool bGetKeyboardInput, bool bGetGamepadInput)
{
	if (bGetKeyboardInput)
	{
		m_vKeyboardDirection = Vector3::Zero;

		if (Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::W))
			m_vKeyboardDirection.z += 1.0f;

		if (Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::S))
			m_vKeyboardDirection.z -= 1.0f;

		if (m_bEnableYAxisMovement)
		{
			if (Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::E))
				m_vKeyboardDirection.y += 1.0f;

			if (Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::Q))
				m_vKeyboardDirection.y -= 1.0f;
		}

		if (Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::D))
			m_vKeyboardDirection.x += 1.0f;

		if (Application->getKeyboard()->GetState().IsKeyDown(DirectX::Keyboard::Keys::A))
			m_vKeyboardDirection.x -= 1.0f;
	}

	if (((Application->getMouse()->GetState().leftButton && Left) || (Application->getMouse()->GetState().rightButton && Right))
		!= WithoutButton)
		UpdateMouseDelta();
	/*
	if (bGetGamepadInput)
	{
		m_vGamePadLeftThumb = Vector3::Zero;
		m_vGamePadRightThumb = Vector3::Zero;

			// Get controller state
		for (DWORD iUserIndex = 0; iUserIndex < DXUT_MAX_CONTROLLERS; iUserIndex++)
		{
			DXUTGetGamepadState(iUserIndex, &m_GamePad[iUserIndex], true, true);

				// Mark time if the controller is in a non-zero state
			if (m_GamePad[iUserIndex].wButtons || m_GamePad[iUserIndex].sThumbLX || m_GamePad[iUserIndex].sThumbLY ||
				m_GamePad[iUserIndex].sThumbRX || m_GamePad[iUserIndex].sThumbRY || m_GamePad[iUserIndex].bLeftTrigger ||
				m_GamePad[iUserIndex].bRightTrigger)
				m_GamePadLastActive[iUserIndex] = DXUTGetTime();
		}

			// Find out which controller was non-zero last
		int iMostRecentlyActive = -1;
		double fMostRecentlyActiveTime = 0.0f;
		for (DWORD iUserIndex = 0; iUserIndex < DXUT_MAX_CONTROLLERS; iUserIndex++)
			if (m_GamePadLastActive[iUserIndex] > fMostRecentlyActiveTime)
			{
				fMostRecentlyActiveTime = m_GamePadLastActive[iUserIndex];
				iMostRecentlyActive = iUserIndex;
			}

			// Use the most recent non-zero controller if its connected
		if (iMostRecentlyActive >= 0 && m_GamePad[iMostRecentlyActive].bConnected)
		{
			m_vGamePadLeftThumb.x = m_GamePad[iMostRecentlyActive].fThumbLX;
			m_vGamePadLeftThumb.y = 0.0f;
			m_vGamePadLeftThumb.z = m_GamePad[iMostRecentlyActive].fThumbLY;

			m_vGamePadRightThumb.x = m_GamePad[iMostRecentlyActive].fThumbRX;
			m_vGamePadRightThumb.y = 0.0f;
			m_vGamePadRightThumb.z = m_GamePad[iMostRecentlyActive].fThumbRY;
		}
	}
	*/
}

void Camera::UpdateMouseDelta()
{
		// Get current position of mouse
	ptCurMousePos.x = Application->getMouse()->GetState().x;
	ptCurMousePos.y = Application->getMouse()->GetState().y;

		// Calc how far it's moved since last frame
	ptCurMouseDelta = ptCurMousePos - m_ptLastMousePosition;

		// Record current position for next time
	m_ptLastMousePosition = ptCurMousePos;

	if (m_bResetCursorAfterMove)
	{
		Vector2 ptCenter = Vector2(Application->getWorkAreaSize(Application->GetHWND()).x,
			Application->getWorkAreaSize(Application->GetHWND()).y);

		ptCenter /= 2;
		SetCursorPos(ptCenter.x, ptCenter.y);
		m_ptLastMousePosition = ptCenter;
	}

	float fPercentOfNew = 1.0f / m_fFramesToSmoothMouseData,
		  fPercentOfOld = 1.0f - fPercentOfNew;
	m_vMouseDelta = m_vMouseDelta * fPercentOfOld + ptCurMouseDelta * fPercentOfNew;

	m_vRotVelocity = m_vMouseDelta * m_fRotationScaler;
}

void Camera::UpdateVelocity(_In_ float fElapsedTime)
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
	vAccel *= m_fMoveScaler;

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
	SetViewParams(m_vDefaultEye, m_vDefaultLookAt);
}

void Camera::FrameMove(_In_ float fElapsedTime)
{
	if (GetAsyncKeyState(VK_LSHIFT))
		SetScalers(0.010f, 6.f * 15.f);
	else
		SetScalers(0.010f, 6.0f);

	if (Application->getTrackerKeyboard().IsKeyPressed(DirectX::Keyboard::Keys::Home))
		Reset();

	// Get keyboard/mouse/gamepad input
	GetInput(m_bEnablePositionMovement, true);

	// Get amount of velocity based on the keyboard input and drag (if any)
	UpdateVelocity(fElapsedTime);

	// If rotating the camera 
	if (((Application->getMouse()->GetState().leftButton && Left || Application->getMouse()->GetState().rightButton && Right)
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
		m_fCameraPitchAngle = max(-XM_PI / 2.0f, m_fCameraPitchAngle);
		m_fCameraPitchAngle = min(+XM_PI / 2.0f, m_fCameraPitchAngle);
	}

		// Make a rotation matrix based on the camera's yaw & pitch
	mCameraRot = XMMatrixRotationRollPitchYaw(m_fCameraPitchAngle, m_fCameraYawAngle, 0);

	// Transform vectors based on camera's rotation matrix
	vWorldUp = XMVector3TransformCoord(g_XMIdentityR1, mCameraRot), 
	vWorldAhead = XMVector3TransformCoord(g_XMIdentityR2, mCameraRot);

		// Transform the position delta by the camera's rotation 
	if (!m_bEnableYAxisMovement)
		mCameraRot = XMMatrixRotationRollPitchYaw(0.0f, m_fCameraYawAngle, 0.0f);

	m_vEye += XMVector3TransformCoord(m_vVelocity * fElapsedTime, mCameraRot);

	if (m_bClipToBoundary)
		m_vEye = ConstrainToBoundary(m_vEye);

		// Update the lookAt position based on the eye position
	m_vLookAt = m_vEye + vWorldAhead;

		// Update the view matrix
	m_mView = XMMatrixLookAtLH(m_vEye, m_vLookAt, vWorldUp);

	m_mCameraWorld = m_mView.Invert();
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