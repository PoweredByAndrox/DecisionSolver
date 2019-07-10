#include "pch.h"

#include "Actor.h"

#include "Inc\GeometricPrimitive.h"
unique_ptr<DirectX::GeometricPrimitive> m_shape;
#include "DebugDraw.h"

void Actor::Update(float Time)
{
	if (Health == 0.0f && !IsGod)
		IsDead = true;
	else if (IsGod)
		Health = 999999.9999f;

	Position = Application->getCamera()->GetEyePt();

	Application->getCamera()->FrameMove(Time);

	Application->getCamera()->SetProjParams(XM_PI / 3.2f, ((float)Application->getWorkAreaSize(Application->GetHWND()).x /
		(float)Application->getWorkAreaSize(Application->GetHWND()).y), 0.1f, 1000.f);
}

extern bool DrawCamSphere;
void Actor::Render(float Time)
{
	Update(Time);

	auto PosCCT = Application->getCamera()->GetCCT();
	if (PosCCT.operator bool() && DrawCamSphere)
	{
		BoundingSphere sphere;
		sphere.Radius = PosCCT->getController()->getRadius();
		sphere.Center = ToExtended(PosCCT->getController()->getPosition());

#if defined (DEBUG)
		Application->getDebugDraw()->Draw(sphere, (Vector4)Colors::Crimson);
#else
		m_shape->Draw(Matrix::CreateTranslation(sphere.Center), Application->getCamera()->GetViewMatrix(),
			Application->getCamera()->GetWorldMatrix());
#endif
	}
}

HRESULT Actor::Init()
{
	//if (!DLG->IsInit())
	//	V(DLG->Init());
	//DLG->LoadFile(&string("For everything.xml"));
	//DLG->getMAReplices();

	Application->getCamera()->SetScalers(0.010f, 6.0f);
	Application->getCamera()->setCameraControlButtons(false, true, false);
	Application->getCamera()->SetResetCursorAfterMove(true);
	Application->getCamera()->SetEnableYAxisMovement(false);

	m_shape = GeometricPrimitive::CreateSphere(Application->getDeviceContext());

	InitClass = true;
	return S_OK;
}
