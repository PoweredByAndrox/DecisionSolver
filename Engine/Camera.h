#pragma once
#if !defined(__CAMERA_H__)
#define __CAMERA_H__
#include "pch.h"

#include "Camera_Control.h"

class Camera: public Camera_Control
{
public:
	Camera() {}

	HRESULT Init(float W, float H, float FOV = 1.0f);

	// Functions to change camera matrices
	virtual void Reset();
	virtual void SetViewParams(Vector3 vEyePt, Vector3 vLookatPt);
	virtual void SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane);

	void SetFreeMoveCam(bool FreeMove) { FreeCamMove = FreeMove; m_bEnableYAxisMovement = true; }

	// Functions to change behavior
	void SetInvertPitch(bool bInvertPitch) { m_bInvertPitch = bInvertPitch; }
	void SetDrag(bool bMovementDrag, float fTotalDragTimeToZero = 0.25f)
	{
		m_bMovementDrag = bMovementDrag;
		m_fTotalDragTimeToZero = fTotalDragTimeToZero;
	}
	void SetEnableYAxisMovement(bool bEnableYAxisMovement) { m_bEnableYAxisMovement = bEnableYAxisMovement; }
	void SetEnablePositionMovement(bool bEnablePositionMovement) { m_bEnablePositionMovement = bEnablePositionMovement; }
	void SetClipToBoundary(bool bClipToBoundary, Vector3 pvMinBoundary, Vector3 pvMaxBoundary)
	{
		m_bClipToBoundary = bClipToBoundary;
		m_vMinBoundary = pvMinBoundary;
		m_vMaxBoundary = pvMaxBoundary;
	}
	void SetScalers(float fRotationScaler = 0.01f, float fMoveScaler = 5.0f)
	{
		m_fRotationScaler = fRotationScaler;
		m_fMoveScaler = fMoveScaler;
	}
	void SetNumberOfFramesToSmoothMouseData(int nFrames);
	void SetResetCursorAfterMove(bool bResetCursorAfterMove) { m_bResetCursorAfterMove = bResetCursorAfterMove; }

	void setDrawCursor(bool IsDraw) { ShowCursor(IsDraw); }

	// Functions to get state
	Matrix GetViewMatrix() const;
	Matrix GetProjMatrix() const;
	Vector3 GetLookAtPt() const;
	float GetNearClip() const;
	float GetFarClip() const;
	float getMoveScale() const;
protected:
	void UpdateMouseDelta();
	void UpdateVelocity(float fElapsedTime);
	void GetInput(bool bGetKeyboardInput, bool bGetGamepadInput);

	Vector3 vWorldUp = Vector3::Zero, vWorldAhead = Vector3::Zero;

	Matrix m_mView = {},                    // View matrix 
		m_mProj = {},						// Projection matrix
		mCameraRot = {};

	Vector3 m_vGamePadLeftThumb = Vector3::Zero,
		m_vGamePadRightThumb = Vector3::Zero,
		m_vKeyboardDirection = Vector3::Zero; // Direction vector of keyboard input

	Vector2 m_vMouseDelta = Vector2::Zero;        // Mouse relative delta smoothed over a few frames
	Vector3 m_vDefaultEye = Vector3::Zero,        // Default camera eye position
		m_vDefaultLookAt = Vector3::Zero,     // Default LookAt position
		m_vEye = Vector3::Zero,               // Camera eye position
		m_vLookAt = Vector3::Zero;            // LookAt position

	float m_fCameraYawAngle = 0.f,                // Yaw angle of camera
		m_fCameraPitchAngle = 0.f,              // Pitch angle of camera
		m_fFramesToSmoothMouseData = 10.0f;       // Number of frames to smooth mouse data over

	Vector3 m_vVelocity = Vector3::Zero,          // Velocity of camera
		m_vVelocityDrag = Vector3::Zero;      // Velocity drag force

	float m_fDragTimer = 0.f,                     // Countdown timer to apply drag
		m_fTotalDragTimeToZero = 0.25f;           // Time it takes for velocity to go from full to 0

	Vector2 m_vRotVelocity = Vector2::Zero;       // Velocity of camera

	float m_fFOV = 0.f,                           // Field of view
		m_fAspect = 0.f,                        // Aspect ratio
		m_fNearPlane = 0.f,                     // Near plane
		m_fFarPlane = 1.0f,						// Far plane

		m_fRotationScaler = 0.f,                // Scaler for rotation
		m_fMoveScaler = 0.f;                    // Scaler for movement

	bool m_bMovementDrag = false,                   // If true, then camera movement will slow to a stop otherwise movement is instant
		m_bInvertPitch = false,                    // Invert the pitch axis
		m_bEnablePositionMovement = true,         // If true, then the user can translate the camera/model 
		m_bEnableYAxisMovement = true,            // If true, then camera can move in the y-axis
		m_bClipToBoundary = false,                 // If true, then the camera will be clipped to the boundary
		m_bResetCursorAfterMove = false,           // If true, the class will reset the cursor position so that the cursor always has space to move 
		FreeCamMove = false;						// Free Camera Move (aka Fly Free)

	Vector3 m_vMinBoundary = { -1.f, -1.f, -1.f },       // Min point in clip boundary
		m_vMaxBoundary = { 1.f, 1.f, 1.f };       // Max point in clip boundary
	Vector2 m_ptLastMousePosition = Vector2::Zero,     // Last absolute position of mouse cursor
		ptCurMousePos = Vector2::Zero,
		ptCurMouseDelta = Vector2::Zero;

public:
	void FrameMove(float fElapsedTime);

	Matrix GetWorldMatrix() const;

	Vector3 GetWorldRight() const;
	Vector3 GetWorldUp() const;
	Vector3 GetWorldAhead() const;
	Vector3 GetEyePt() const;

	void SetCameraControlButtons(bool LeftM, bool RightM, bool WithoutButtons)
	{
		Left = LeftM;
		Right = RightM;
		WithoutButton = WithoutButtons;
	}

	auto GetCCT() { return C_CT; }
protected:
	Matrix m_mCameraWorld = {};

	bool Left = false, Right = false, WithoutButton = false;

	shared_ptr<Camera_Control> C_CT;
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
#endif // !__CAMERA_H__
