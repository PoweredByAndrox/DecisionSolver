#ifndef __CAMERA_H__
#define __CAMERA_H__
#pragma once

namespace Engine
{
#include "pch.h"
	class CD3DArcBall
	{
	public:
		CD3DArcBall() noexcept;

		// Functions to change behavior
		void Reset();
		void SetTranslationRadius(_In_ float fRadiusTranslation)
		{
			m_fRadiusTranslation = fRadiusTranslation;
		}
		void SetWindow(_In_ INT nWidth, _In_ INT nHeight, _In_ float fRadius = 0.9f)
		{
			m_nWidth = nWidth;
			m_nHeight = nHeight;
			m_fRadius = fRadius;
			m_vCenter.x = float(m_nWidth) / 2.0f;
			m_vCenter.y = float(m_nHeight) / 2.0f;
		}
		void SetOffset(_In_ INT nX, _In_ INT nY) { m_Offset.x = nX; m_Offset.y = nY; }

		void OnBegin(_In_ int nX, _In_ int nY);   // start the rotation (pass current mouse position)
		void OnMove(_In_ int nX, _In_ int nY);    // continue the rotation (pass current mouse position)
		void OnEnd();                               // end the rotation 

		LRESULT HandleMessages(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

		// Functions to get/set state
		Matrix GetRotationMatrix() const
		{
			using namespace DirectX;
			Vector3 q = XMLoadFloat4(&m_qNow);
			return DirectX::XMMatrixRotationQuaternion(q);
		}
		Matrix GetTranslationMatrix() const { return DirectX::XMLoadFloat4x4(&m_mTranslation); }
		Matrix GetTranslationDeltaMatrix() const { return DirectX::XMLoadFloat4x4(&m_mTranslationDelta); }
		bool IsBeingDragged() const { return m_bDrag; }
		Vector3 GetQuatNow() const { return DirectX::XMLoadFloat4(&m_qNow); }
		void SetQuatNow(_In_ Vector3& q) { DirectX::XMStoreFloat4(&m_qNow, q); }

		static Vector3 QuatFromBallPoints(_In_ Vector3 vFrom, _In_ Vector3 vTo)
		{
			using namespace DirectX;

			Vector3 dot = XMVector3Dot(vFrom, vTo);
			Vector3 vPart = XMVector3Cross(vFrom, vTo);
			return XMVectorSelect(dot, vPart, g_XMSelect1110);
		}

	protected:
		DirectX::XMFLOAT4X4 m_mRotation;        // Matrix for arc ball's orientation
		DirectX::XMFLOAT4X4 m_mTranslation;     // Matrix for arc ball's position
		DirectX::XMFLOAT4X4 m_mTranslationDelta;// Matrix for arc ball's position

		POINT m_Offset;                         // window offset, or upper-left corner of window
		INT m_nWidth;                           // arc ball's window width
		INT m_nHeight;                          // arc ball's window height
		Vector2 m_vCenter;            // center of arc ball 
		float m_fRadius;                        // arc ball's radius in screen coords
		float m_fRadiusTranslation;             // arc ball's radius for translating the target

		Vector4 m_qDown;              // Quaternion before button down
		Vector4 m_qNow;               // Composite quaternion for current drag
		bool m_bDrag;                           // Whether user is dragging arc ball

		POINT m_ptLastMouse;                    // position of last mouse point
		Vector3 m_vDownPt;            // starting point of rotation arc
		Vector3 m_vCurrentPt;         // current point of rotation arc

		Vector3 ScreenToVector(_In_ float fScreenPtX, _In_ float fScreenPtY)
		{
			// Scale to screen
			float x = -(fScreenPtX - m_Offset.x - m_nWidth / 2) / (m_fRadius * m_nWidth / 2);
			float y = (fScreenPtY - m_Offset.y - m_nHeight / 2) / (m_fRadius * m_nHeight / 2);

			float z = 0.0f;
			float mag = x * x + y * y;

			if (mag > 1.0f)
			{
				float scale = 1.0f / sqrtf(mag);
				x *= scale;
				y *= scale;
			}
			else
				z = sqrtf(1.0f - mag);

			return DirectX::XMVectorSet(x, y, z, 0);
		}
	};

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

	class CBaseCamera
	{
	public:
		CBaseCamera() noexcept;

		virtual LRESULT HandleMessages(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
		virtual void FrameMove(_In_ float fElapsedTime) = 0;

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
		void SetClipToBoundary(_In_ bool bClipToBoundary, _In_opt_ DirectX::XMFLOAT3* pvMinBoundary, _In_opt_ DirectX::XMFLOAT3* pvMaxBoundary)
		{
			m_bClipToBoundary = bClipToBoundary;
			if (pvMinBoundary) m_vMinBoundary = *pvMinBoundary;
			if (pvMaxBoundary) m_vMaxBoundary = *pvMaxBoundary;
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
		Vector3 GetEyePt() const { return DirectX::XMLoadFloat3(&m_vEye); }
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

		Vector3 ConstrainToBoundary(_In_ Vector3 v)
		{
			using namespace DirectX;

			Vector3 vMin = XMLoadFloat3(&m_vMinBoundary);
			Vector3 vMax = XMLoadFloat3(&m_vMaxBoundary);

			return XMVectorClamp(v, vMin, vMax);
		}

		void UpdateMouseDelta();
		void UpdateVelocity(_In_ float fElapsedTime);
		void GetInput(_In_ bool bGetKeyboardInput, _In_ bool bGetMouseInput, _In_ bool bGetGamepadInput);

		DirectX::XMFLOAT4X4 m_mView;                    // View matrix 
		DirectX::XMFLOAT4X4 m_mProj;                    // Projection matrix

		DXUT_GAMEPAD m_GamePad[DXUT_MAX_CONTROLLERS];  // XInput controller state
		Vector3 m_vGamePadLeftThumb;
		Vector3 m_vGamePadRightThumb;
		double m_GamePadLastActive[DXUT_MAX_CONTROLLERS];

		int m_cKeysDown;                        // Number of camera keys that are down.
		BYTE m_aKeys[CAM_MAX_KEYS];             // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
		Vector3 m_vKeyboardDirection; // Direction vector of keyboard input
		POINT m_ptLastMousePosition;            // Last absolute position of mouse cursor
		int m_nCurrentButtonMask;               // mask of which buttons are down
		int m_nMouseWheelDelta;                 // Amount of middle wheel scroll (+/-) 
		Vector2 m_vMouseDelta;        // Mouse relative delta smoothed over a few frames
		float m_fFramesToSmoothMouseData;       // Number of frames to smooth mouse data over
		Vector3 m_vDefaultEye;        // Default camera eye position
		Vector3 m_vDefaultLookAt;     // Default LookAt position
		Vector3 m_vEye;               // Camera eye position
		Vector3 m_vLookAt;            // LookAt position
		float m_fCameraYawAngle;                // Yaw angle of camera
		float m_fCameraPitchAngle;              // Pitch angle of camera

		RECT m_rcDrag;                          // Rectangle within which a drag can be initiated.
		Vector3 m_vVelocity;          // Velocity of camera
		Vector3 m_vVelocityDrag;      // Velocity drag force
		float m_fDragTimer;                     // Countdown timer to apply drag
		float m_fTotalDragTimeToZero;           // Time it takes for velocity to go from full to 0
		Vector2 m_vRotVelocity;       // Velocity of camera

		float m_fFOV;                           // Field of view
		float m_fAspect;                        // Aspect ratio
		float m_fNearPlane;                     // Near plane
		float m_fFarPlane;                      // Far plane

		float m_fRotationScaler;                // Scaler for rotation
		float m_fMoveScaler;                    // Scaler for movement

		bool m_bMouseLButtonDown;               // True if left button is down 
		bool m_bMouseMButtonDown;               // True if middle button is down 
		bool m_bMouseRButtonDown;               // True if right button is down 
		bool m_bMovementDrag;                   // If true, then camera movement will slow to a stop otherwise movement is instant
		bool m_bInvertPitch;                    // Invert the pitch axis
		bool m_bEnablePositionMovement;         // If true, then the user can translate the camera/model 
		bool m_bEnableYAxisMovement;            // If true, then camera can move in the y-axis
		bool m_bClipToBoundary;                 // If true, then the camera will be clipped to the boundary
		bool m_bResetCursorAfterMove;           // If true, the class will reset the cursor position so that the cursor always has space to move 

		Vector3 m_vMinBoundary;       // Min point in clip boundary
		Vector3 m_vMaxBoundary;       // Max point in clip boundary
	};

	class CFirstPersonCamera: public CBaseCamera
	{
	public:
		CFirstPersonCamera() noexcept;

		virtual void FrameMove(_In_ float fElapsedTime) override;
		void SetRotateButtons(_In_ bool bLeft, _In_ bool bMiddle, _In_ bool bRight, _In_ bool bRotateWithoutButtonDown = false);

		Matrix GetWorldMatrix() const { return DirectX::XMLoadFloat4x4(&m_mCameraWorld); }

		Vector3 GetWorldRight() const { return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&m_mCameraWorld._11)); }
		Vector3 GetWorldUp() const { return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&m_mCameraWorld._21)); }
		Vector3 GetWorldAhead() const { return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&m_mCameraWorld._31)); }
		Vector3 GetEyePt() const { return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&m_mCameraWorld._41)); }
		
		void setPosCam(_In_ Vector3 Pos) 
		{
			SetViewParams(Pos, Pos + GetWorldAhead());
		}

		void setPosCam(_In_ float Y)
		{
			SetViewParams(Vector3(m_vEye.x, Y, m_vEye.z), Vector3(m_vEye.x, Y, m_vEye.z) + GetWorldAhead());
		}

		//***********
			// To control the movement of the terrain
		float getXPos() { return GetEyePt().x; }
		float getYPos() { return GetEyePt().y; }
		float getZPos() { return GetEyePt().z; }

	protected:
		DirectX::XMFLOAT4X4 m_mCameraWorld;

		int m_nActiveButtonMask;
		bool m_bRotateWithoutButtonDown;
	};
};
#endif // !__CAMERA_H__
