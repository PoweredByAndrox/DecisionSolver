#include "pch.h"

#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTres.h"
#include "Camera.h"

using namespace DirectX;

_Use_decl_annotations_
void Engine::Camera::SetViewParams(Vector3 vEyePt, Vector3 vLookatPt)
{
	XMStoreFloat3(&m_vEye, vEyePt);
	XMStoreFloat3(&m_vDefaultEye, vEyePt);

	XMStoreFloat3(&m_vLookAt, vLookatPt);
	XMStoreFloat3(&m_vDefaultLookAt, vLookatPt);

		// Calc the view matrix
	XMMATRIX mView = XMMatrixLookAtLH(vEyePt, vLookatPt, g_XMIdentityR1);
	XMStoreFloat4x4(&m_mView, mView);

	XMMATRIX mInvView = XMMatrixInverse(nullptr, mView);

	Vector3 zBasis;
	XMStoreFloat3(&zBasis, mInvView.r[2]);

	m_fCameraYawAngle = atan2f(zBasis.x, zBasis.z);
	float fLen = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
	m_fCameraPitchAngle = -atan2f(zBasis.y, fLen);
}

_Use_decl_annotations_
void Engine::Camera::SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane)
{
		// Set attributes for the projection matrix
	m_fFOV = fFOV;
	m_fAspect = fAspect;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;

	XMStoreFloat4x4(&m_mProj, XMMatrixPerspectiveFovLH(fFOV, fAspect, fNearPlane, fFarPlane));
}

_Use_decl_annotations_
LRESULT Engine::Camera::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	UNREFERENCED_PARAMETER(lParam);

	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		D3DUtil_CameraKeys mappedKey = MapKey((UINT)wParam);
		if (mappedKey != CAM_UNKNOWN)
			_Analysis_assume_(mappedKey < CAM_MAX_KEYS);
		if (FALSE == IsKeyDown(m_aKeys[mappedKey]))
		{
			m_aKeys[mappedKey] = KEY_WAS_DOWN_MASK | KEY_IS_DOWN_MASK;
			++m_cKeysDown;
		}
		break;
	}

	case WM_KEYUP:
	{
		D3DUtil_CameraKeys mappedKey = MapKey((UINT)wParam);
		if (mappedKey != CAM_UNKNOWN && (DWORD)mappedKey < 8)
		{
			m_aKeys[mappedKey] &= ~KEY_IS_DOWN_MASK;
			--m_cKeysDown;
		}
		break;
	}

	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	{
			// Compute the drag rectangle in screen coord.
		POINT ptCursor =
		{
			(short)LOWORD(lParam), (short)HIWORD(lParam)
		};

			// Update member var state
		if ((uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) && PtInRect(&m_rcDrag, ptCursor))
		{
			m_bMouseLButtonDown = true;
			m_nCurrentButtonMask |= MOUSE_LEFT_BUTTON;
		}
		if ((uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK) && PtInRect(&m_rcDrag, ptCursor))
		{
			m_bMouseMButtonDown = true;
			m_nCurrentButtonMask |= MOUSE_MIDDLE_BUTTON;
		}
		if ((uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK) && PtInRect(&m_rcDrag, ptCursor))
		{
			m_bMouseRButtonDown = true;
			m_nCurrentButtonMask |= MOUSE_RIGHT_BUTTON;
		}

		SetCapture(hWnd);
		GetCursorPos(&m_ptLastMousePosition);
		return TRUE;
	}

	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_LBUTTONUP:
	{
			// Update member var state
		if (uMsg == WM_LBUTTONUP)
		{
			m_bMouseLButtonDown = false;
			m_nCurrentButtonMask &= ~MOUSE_LEFT_BUTTON;
		}
		if (uMsg == WM_MBUTTONUP)
		{
			m_bMouseMButtonDown = false;
			m_nCurrentButtonMask &= ~MOUSE_MIDDLE_BUTTON;
		}
		if (uMsg == WM_RBUTTONUP)
		{
			m_bMouseRButtonDown = false;
			m_nCurrentButtonMask &= ~MOUSE_RIGHT_BUTTON;
		}

			// Release the capture if no mouse buttons down
		if (!m_bMouseLButtonDown && !m_bMouseRButtonDown && !m_bMouseMButtonDown)
			ReleaseCapture();
		break;
	}

	case WM_CAPTURECHANGED:
	{
		if ((HWND)lParam != hWnd)
		{
			if ((m_nCurrentButtonMask & MOUSE_LEFT_BUTTON) || (m_nCurrentButtonMask & MOUSE_MIDDLE_BUTTON) ||
				(m_nCurrentButtonMask & MOUSE_RIGHT_BUTTON))
			{
				m_bMouseLButtonDown = false;
				m_bMouseMButtonDown = false;
				m_bMouseRButtonDown = false;
				m_nCurrentButtonMask &= ~MOUSE_LEFT_BUTTON;
				m_nCurrentButtonMask &= ~MOUSE_MIDDLE_BUTTON;
				m_nCurrentButtonMask &= ~MOUSE_RIGHT_BUTTON;
				ReleaseCapture();
			}
		}
		break;
	}

	case WM_MOUSEWHEEL:
			// Update member var state
		m_nMouseWheelDelta = 0;
		m_nMouseWheelDelta += (int)GET_WHEEL_DELTA_WPARAM(wParam);

		ChangeFOV(m_nMouseWheelDelta);

		break;
	}
	return FALSE;
}

_Use_decl_annotations_
void Engine::Camera::GetInput(bool bGetKeyboardInput, bool bGetMouseInput, bool bGetGamepadInput)
{
	if (bGetKeyboardInput)
	{
		PxTransform Pos(0.f, 0.f, 0.f);
		m_vKeyboardDirection = Vector3(0.f, 0.f, 0.f);

			// Update acceleration vector based on keyboard state
		if (IsKeyDown(m_aKeys[CAM_MOVE_FORWARD]))
		{
			m_vKeyboardDirection.z += 1.0f;
			Pos.p.z = m_vKeyboardDirection.z;
		}

		if (IsKeyDown(m_aKeys[CAM_MOVE_BACKWARD]))
		{
			m_vKeyboardDirection.z -= 1.0f;
			Pos.p.z = m_vKeyboardDirection.z;
		}

		if (m_bEnableYAxisMovement)
		{
			if (IsKeyDown(m_aKeys[CAM_MOVE_UP]))
			{
				m_vKeyboardDirection.y += 1.0f;
				Pos.p.y = m_vKeyboardDirection.y;

			}

			if (IsKeyDown(m_aKeys[CAM_MOVE_DOWN]))
			{
				m_vKeyboardDirection.y -= 1.0f;
				Pos.p.y = m_vKeyboardDirection.y;
			}
		}

		if (IsKeyDown(m_aKeys[CAM_STRAFE_RIGHT]))
		{
			m_vKeyboardDirection.x += 1.0f;
			Pos.p.x = m_vKeyboardDirection.x;
		}

		if (IsKeyDown(m_aKeys[CAM_STRAFE_LEFT]))
		{
			m_vKeyboardDirection.x -= 1.0f;
			Pos.p.x = m_vKeyboardDirection.x;
		}
		//if (PhysX->getActrCamera())
		//	PhysX->getActrCamera()->setGlobalPose(Pos);

	}

	if (bGetMouseInput)
		UpdateMouseDelta();

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
}

void Engine::Camera::UpdateMouseDelta()
{
		// Get current position of mouse
	POINT ptCurMousePos;
	GetCursorPos(&ptCurMousePos);

		// Calc how far it's moved since last frame
	POINT ptCurMouseDelta;
	ptCurMouseDelta.x = ptCurMousePos.x - m_ptLastMousePosition.x;
	ptCurMouseDelta.y = ptCurMousePos.y - m_ptLastMousePosition.y;

		// Record current position for next time
	m_ptLastMousePosition = ptCurMousePos;

	if (m_bResetCursorAfterMove && DXUTIsActive())
	{
		POINT ptCenter;

			// Get the center of the current monitor
		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		DXUTGetMonitorInfo(DXUTMonitorFromWindow(DXUTGetHWND(), MONITOR_DEFAULTTONEAREST), &mi);
		ptCenter.x = (mi.rcMonitor.left + mi.rcMonitor.right) / 2;
		ptCenter.y = (mi.rcMonitor.top + mi.rcMonitor.bottom) / 2;
		SetCursorPos(ptCenter.x, ptCenter.y);
		m_ptLastMousePosition = ptCenter;
	}

	float fPercentOfNew = 1.0f / m_fFramesToSmoothMouseData, fPercentOfOld = 1.0f - fPercentOfNew;
	m_vMouseDelta.x = m_vMouseDelta.x * fPercentOfOld + ptCurMouseDelta.x * fPercentOfNew;
	m_vMouseDelta.y = m_vMouseDelta.y * fPercentOfOld + ptCurMouseDelta.y * fPercentOfNew;

	m_vRotVelocity.x = m_vMouseDelta.x * m_fRotationScaler;
	m_vRotVelocity.y = m_vMouseDelta.y * m_fRotationScaler;
}

void Engine::Camera::UpdateVelocity(_In_ float fElapsedTime)
{
	Vector3 vGamePadRightThumb = XMVectorSet(m_vGamePadRightThumb.x, -m_vGamePadRightThumb.z, 0, 0),

	vMouseDelta = XMLoadFloat2(&m_vMouseDelta), vRotVelocity = vMouseDelta * m_fRotationScaler + vGamePadRightThumb * 0.02f;

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

Engine::D3DUtil_CameraKeys Engine::Camera::MapKey(_In_ UINT nKey)
{
	switch (nKey)
	{
	case VK_CONTROL:
		return CAM_CONTROLDOWN;
	case VK_LEFT:
		return CAM_STRAFE_LEFT;
	case VK_RIGHT:
		return CAM_STRAFE_RIGHT;
	case VK_UP:
		return CAM_MOVE_FORWARD;
	case VK_DOWN:
		return CAM_MOVE_BACKWARD;
	case VK_PRIOR:
		return CAM_MOVE_UP;        // pgup
	case VK_NEXT:
		return CAM_MOVE_DOWN;      // pgdn

	case 'A':
		return CAM_STRAFE_LEFT;
	case 'D':
		return CAM_STRAFE_RIGHT;
	case 'W':
		return CAM_MOVE_FORWARD;
	case 'S':
		return CAM_MOVE_BACKWARD;
	case 'Q':
		return CAM_MOVE_DOWN;
	case 'E':
		return CAM_MOVE_UP;

case VK_NUMPAD4:
		return CAM_STRAFE_LEFT;
	case VK_NUMPAD6:
		return CAM_STRAFE_RIGHT;
	case VK_NUMPAD8:
		return CAM_MOVE_FORWARD;
	case VK_NUMPAD2:
		return CAM_MOVE_BACKWARD;
	case VK_NUMPAD9:
		return CAM_MOVE_UP;
	case VK_NUMPAD3:
		return CAM_MOVE_DOWN;

	case VK_HOME:
		return CAM_RESET;
	}
	return CAM_UNKNOWN;
}

void Engine::Camera::Reset()
{
	Vector3 vDefaultEye = XMLoadFloat3(&m_vDefaultEye), vDefaultLookAt = XMLoadFloat3(&m_vDefaultLookAt);

	SetViewParams(vDefaultEye, vDefaultLookAt);

	//if (PhysX->getActrCamera())
	//	PhysX->getActrCamera()->setGlobalPose(PxTransform(vDefaultEye.x, vDefaultEye.y, vDefaultEye.z));
}

void Engine::Camera::FrameMove(_In_ float fElapsedTime)
{
	if (DXUTGetGlobalTimer()->IsStopped())
		if (DXUTGetFPS() == 0.0f)
			fElapsedTime = 0;
		else
			fElapsedTime = 1.0f / DXUTGetFPS();

	if (IsKeyDown(m_aKeys[CAM_RESET]))
		Reset();

		// Get keyboard/mouse/gamepad input
	GetInput(m_bEnablePositionMovement, (m_nActiveButtonMask & m_nCurrentButtonMask) || m_bRotateWithoutButtonDown, true);

		// Get amount of velocity based on the keyboard input and drag (if any)
	UpdateVelocity(fElapsedTime);

		// Simple euler method to calculate position delta
	Vector3 vVelocity = XMLoadFloat3(&m_vVelocity), vPosDelta = vVelocity * fElapsedTime;

		// If rotating the camera 
	if ((m_nActiveButtonMask & m_nCurrentButtonMask) || m_bRotateWithoutButtonDown || m_vGamePadRightThumb.x != 0 ||
		m_vGamePadRightThumb.z != 0)
	{
			// Update the pitch & yaw angle based on mouse movement
		float fYawDelta = m_vRotVelocity.x, fPitchDelta = m_vRotVelocity.y;

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
	Matrix mCameraRot = XMMatrixRotationRollPitchYaw(m_fCameraPitchAngle, m_fCameraYawAngle, 0);

		// Transform vectors based on camera's rotation matrix
	Vector3 vWorldUp = XMVector3TransformCoord(g_XMIdentityR1, mCameraRot), vWorldAhead = XMVector3TransformCoord(g_XMIdentityR2, mCameraRot);

		// Transform the position delta by the camera's rotation 
	if (!m_bEnableYAxisMovement)
		mCameraRot = XMMatrixRotationRollPitchYaw(0.0f, m_fCameraYawAngle, 0.0f);
	Vector3 vPosDeltaWorld = XMVector3TransformCoord(vPosDelta, mCameraRot),
		
		// Move the eye position 
	vEye = m_vEye;
	vEye += vPosDeltaWorld;

	if (m_bClipToBoundary)
		vEye = ConstrainToBoundary(vEye);

	XMStoreFloat3(&m_vEye, vEye);

	//if (PhysX->getActrCamera())
	//	PhysX->getActrCamera()->setGlobalPose(PxTransform(vEye.x, vEye.y, vEye.z));

		// Update the lookAt position based on the eye position
	Vector3 vLookAt = vEye + vWorldAhead;
	XMStoreFloat3(&m_vLookAt, vLookAt);

		// Update the view matrix
	Matrix mView = XMMatrixLookAtLH(vEye, vLookAt, vWorldUp);
	XMStoreFloat4x4(&m_mView, mView);

	Matrix mCameraWorld = XMMatrixInverse(nullptr, mView);
	XMStoreFloat4x4(&m_mCameraWorld, mCameraWorld);
}

_Use_decl_annotations_
void Engine::Camera::SetRotateButtons(bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown)
{
	m_nActiveButtonMask = (bLeft ? MOUSE_LEFT_BUTTON : 0) | (bMiddle ? MOUSE_MIDDLE_BUTTON : 0) | (bRight ? MOUSE_RIGHT_BUTTON : 0);
	m_bRotateWithoutButtonDown = bRotateWithoutButtonDown;
}

void Engine::Frustum::ConstructFrustum(float screenDepth, Matrix projectionMatrix, Matrix viewMatrix)
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

bool Engine::Frustum::CheckPoint(float x, float y, float z)
{
	for (int i = 0; i < 6; i++)
		if (XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(x, y, z, 1.0f))) < 0.0f)
			return false;

	return true;
}

bool Engine::Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float size)
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

bool Engine::Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	for (int i = 0; i < 6; i++)
	{
		float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(xCenter, yCenter, zCenter, 1.0f)));
		if (ret < -radius)
			return false;
	}

	return true;
}

bool Engine::Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
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