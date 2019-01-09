#pragma once
#ifndef __CAMERA_H__
#define __CAMERA_H__
#include "pch.h"

#include "Physics.h"

namespace Engine
{
	enum D3DUtil_CameraKeys
	{
		CAM_STRAFE_LEFT = 0,
		CAM_STRAFE_RIGHT,
		CAM_MOVE_FORWARD,
		CAM_MOVE_BACKWARD,
		CAM_MOVE_UP,
		CAM_MOVE_DOWN,
		CAM_RESET,
		CAM_CONTROLDOWN,
		CAM_MAX_KEYS,
		CAM_UNKNOWN = 0xFF
	};

#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08
	class Jump;
	class Camera: public Physics
	{
	public:
		Camera() {}

		HRESULT Init(Physics *PhysX)
		{
			// Setup the view matrix
			SetViewParams(Vector3::Zero, Vector3(0.0f, 0.0f, 1.0f));

			// Setup the projection matrix
			SetProjParams(XM_PI / 4, 1.0f, 1.0f, 1000.0f);

			GetCursorPos(&m_ptLastMousePosition);

			SetRect(&m_rcDrag, LONG_MIN, LONG_MIN, LONG_MAX, LONG_MAX);

			try
			{
				this->PhysX.reset(PhysX);
				PhysX->SetPhysicsForCamera(Vector3(0.f, 0.f, 0.f), Vector3(1.f, 1.f, 1.f));
			}
			catch (const exception&)
			{
				throw exception("Catch in Camera->Init!!!");
				return E_FAIL;
			}

			return S_OK;
		}

		virtual LRESULT HandleMessages(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

			// Functions to change camera matrices
		virtual void Reset();
		virtual void SetViewParams(_In_ Vector3 vEyePt, _In_ Vector3 vLookatPt);
		virtual void SetProjParams(_In_ float fFOV, _In_ float fAspect, _In_ float fNearPlane, _In_ float fFarPlane);

			// Functions to change behavior
		virtual void SetDragRect(_In_ const RECT& rc) { m_rcDrag = rc; }
		void SetInvertPitch(_In_ bool bInvertPitch) { m_bInvertPitch = bInvertPitch; }
		void SetDrag(_In_ bool bMovementDrag, _In_ float fTotalDragTimeToZero = 0.25f)
		{
			m_bMovementDrag = bMovementDrag;
			m_fTotalDragTimeToZero = fTotalDragTimeToZero;
		}
		void SetEnableYAxisMovement(_In_ bool bEnableYAxisMovement) { m_bEnableYAxisMovement = bEnableYAxisMovement; }
		void SetEnablePositionMovement(_In_ bool bEnablePositionMovement) { m_bEnablePositionMovement = bEnablePositionMovement; }
		void SetClipToBoundary(_In_ bool bClipToBoundary, _In_opt_ Vector3 pvMinBoundary, _In_opt_ Vector3 pvMaxBoundary)
		{
			m_bClipToBoundary = bClipToBoundary;
				m_vMinBoundary = pvMinBoundary;
				m_vMaxBoundary = pvMaxBoundary;
		}
		void SetScalers(_In_ float fRotationScaler = 0.01f, _In_ float fMoveScaler = 5.0f)
		{
			m_fRotationScaler = fRotationScaler;
			m_fMoveScaler = fMoveScaler;
		}
		void SetNumberOfFramesToSmoothMouseData(_In_ int nFrames) { if (nFrames > 0) m_fFramesToSmoothMouseData = (float)nFrames; }
		void SetResetCursorAfterMove(_In_ bool bResetCursorAfterMove) { m_bResetCursorAfterMove = bResetCursorAfterMove; }

			// Functions to get state
		Matrix GetViewMatrix() const { return DirectX::XMLoadFloat4x4(&m_mView); }
		Matrix GetProjMatrix() const { return DirectX::XMLoadFloat4x4(&m_mProj); }
	//	Vector3 GetEyePt() const { return DirectX::XMLoadFloat3(&m_vEye); }
		Vector3 GetLookAtPt() const { return DirectX::XMLoadFloat3(&m_vLookAt); }
		float GetNearClip() const { return m_fNearPlane; }
		float GetFarClip() const { return m_fFarPlane; }

		bool IsBeingDragged() const { return (m_bMouseLButtonDown || m_bMouseMButtonDown || m_bMouseRButtonDown); }
		bool IsMouseLButtonDown() const { return m_bMouseLButtonDown; }
		bool IsMouseMButtonDown() const { return m_bMouseMButtonDown; }
		bool sMouseRButtonDown() const { return m_bMouseRButtonDown; }
	protected:
		virtual D3DUtil_CameraKeys MapKey(_In_ UINT nKey);

		bool IsKeyDown(_In_ BYTE key) const { return((key & KEY_IS_DOWN_MASK) == KEY_IS_DOWN_MASK); }
		bool WasKeyDown(_In_ BYTE key) const { return((key & KEY_WAS_DOWN_MASK) == KEY_WAS_DOWN_MASK); }

		void ChangeFOV(float m_nMouseWheelDelta)
		{
			m_fFOV += m_nMouseWheelDelta;
		}

		Vector3 ConstrainToBoundary(_In_ Vector3 v)
		{
			Vector3 vMin = XMLoadFloat3(&m_vMinBoundary);
			Vector3 vMax = XMLoadFloat3(&m_vMaxBoundary);

			return XMVectorClamp(v, vMin, vMax);
		}

		void UpdateMouseDelta();
		void UpdateVelocity(_In_ float fElapsedTime);
		void GetInput(_In_ bool bGetKeyboardInput, _In_ bool bGetMouseInput, _In_ bool bGetGamepadInput);

		Matrix m_mView = {},                    // View matrix 
			   m_mProj = {};                    // Projection matrix

		DXUT_GAMEPAD m_GamePad[DXUT_MAX_CONTROLLERS] = {};  // XInput controller state
		Vector3 m_vGamePadLeftThumb = { 0.f, 0.f, 0.f },
				m_vGamePadRightThumb = { 0.f, 0.f, 0.f };
		double m_GamePadLastActive[DXUT_MAX_CONTROLLERS] = {};

		BYTE m_aKeys[CAM_MAX_KEYS] = {};             // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
		Vector3 m_vKeyboardDirection = { 0.f, 0.f, 0.f }; // Direction vector of keyboard input
		POINT m_ptLastMousePosition;            // Last absolute position of mouse cursor
		int m_nCurrentButtonMask = 0,               // mask of which buttons are down
			m_nMouseWheelDelta = 0,                 // Amount of middle wheel scroll (+/-) 
			m_cKeysDown = 0;                        // Number of camera keys that are down.

		Vector2 m_vMouseDelta = { 0.f, 0.f };        // Mouse relative delta smoothed over a few frames
		Vector3 m_vDefaultEye = { 0.f, 0.f, 0.f },        // Default camera eye position
				m_vDefaultLookAt = { 0.f, 0.f, 0.f },     // Default LookAt position
				m_vEye = { 0.f, 0.f, 0.f },               // Camera eye position
				m_vLookAt = { 0.f, 0.f, 0.f };            // LookAt position

		float m_fCameraYawAngle = 0.f,                // Yaw angle of camera
			  m_fCameraPitchAngle = 0.f,              // Pitch angle of camera
			  m_fFramesToSmoothMouseData = 2.0f;       // Number of frames to smooth mouse data over

		RECT m_rcDrag;                          // Rectangle within which a drag can be initiated.
		Vector3 m_vVelocity = { 0.f, 0.f, 0.f },          // Velocity of camera
				m_vVelocityDrag = { 0.f, 0.f, 0.f };      // Velocity drag force

		float m_fDragTimer = 0.f,                     // Countdown timer to apply drag
			  m_fTotalDragTimeToZero = 0.25f;           // Time it takes for velocity to go from full to 0

		Vector2 m_vRotVelocity = { 0.f, 0.f };       // Velocity of camera

		float m_fFOV = 0.f,                           // Field of view
			  m_fAspect = 0.f,                        // Aspect ratio
			  m_fNearPlane = 0.f,                     // Near plane
			  m_fFarPlane = 1.0f,						// Far plane

			  m_fRotationScaler = 0.01f,                // Scaler for rotation
			  m_fMoveScaler = 5.f;                    // Scaler for movement

		bool m_bMouseLButtonDown = false,               // True if left button is down 
			 m_bMouseMButtonDown = false,               // True if middle button is down 
			 m_bMouseRButtonDown = false,               // True if right button is down 
			 m_bMovementDrag = false,                   // If true, then camera movement will slow to a stop otherwise movement is instant
			 m_bInvertPitch = false,                    // Invert the pitch axis
			 m_bEnablePositionMovement = true,         // If true, then the user can translate the camera/model 
			 m_bEnableYAxisMovement = true,            // If true, then camera can move in the y-axis
			 m_bClipToBoundary = false,                 // If true, then the camera will be clipped to the boundary
			 m_bResetCursorAfterMove = false,           // If true, the class will reset the cursor position so that the cursor always has space to move 
			 ChangeFieldOfView = false;

		Vector3 m_vMinBoundary = { -1.f, -1.f, -1.f },       // Min point in clip boundary
				m_vMaxBoundary = { 1.f, 1.f, 1.f };       // Max point in clip boundary

		public:
		void FrameMove(_In_ float fElapsedTime);
		void SetRotateButtons(_In_ bool bLeft, _In_ bool bMiddle, _In_ bool bRight, _In_ bool bRotateWithoutButtonDown = false);

		Matrix GetWorldMatrix() const { return DirectX::XMLoadFloat4x4(&m_mCameraWorld); }

		Vector3 GetWorldRight() const { return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&m_mCameraWorld._11)); }
		Vector3 GetWorldUp() const { return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&m_mCameraWorld._21)); }
		Vector3 GetWorldAhead() const { return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&m_mCameraWorld._31)); }
		Vector3 GetEyePt() const { return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&m_mCameraWorld._41)); }

		float getFOV() { return m_fFOV; }

		void SetChangeFOV(bool FOV) { ChangeFieldOfView = FOV; }

		void setPosCam(_In_ Vector3 Pos)
		{
			if (m_vEye != Pos)
			{
				m_vEye = Pos;

				Vector3 vLookAt = m_vEye + GetWorldAhead();
				Matrix mView = XMMatrixLookAtLH(m_vEye, vLookAt, GetWorldUp()),
					mCameraWorld = XMMatrixInverse(nullptr, mView);
				XMStoreFloat3(&m_vLookAt, vLookAt);
				XMStoreFloat4x4(&m_mView, mView);
				XMStoreFloat4x4(&m_mCameraWorld, mCameraWorld);

				return;
			}
			m_vEye = Pos;

		}
		void setPosCam(_In_ float Y)
		{
			if (m_vEye.y != Y)
			{
				m_vEye.y = Y;

				Vector3 vLookAt = m_vEye + GetWorldAhead();
				Matrix mView = XMMatrixLookAtLH(m_vEye, vLookAt, GetWorldUp()),
					mCameraWorld = XMMatrixInverse(nullptr, mView);
				XMStoreFloat3(&m_vLookAt, vLookAt);
				XMStoreFloat4x4(&m_mView, mView);
				XMStoreFloat4x4(&m_mCameraWorld, mCameraWorld);

				return;
			}
			m_vEye.y = Y;

		}
	protected:
		Matrix m_mCameraWorld;

		int m_nActiveButtonMask = 0x07;
		bool m_bRotateWithoutButtonDown = false;

		unique_ptr<Physics> PhysX;
	};

	class Frustum
	{
	public:
		void ConstructFrustum(float screenDepth, Matrix projectionMatrix, Matrix viewMatrix);

		bool CheckPoint(float x, float y, float z);
		bool CheckCube(float xCenter, float yCenter, float zCenter, float size);
		bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
		bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);

	private:
		Vector3 m_planes[6];
	};
};
#endif // !__CAMERA_H__
