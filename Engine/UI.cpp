#include "pch.h"

#include "UI.h"

using namespace Engine;

HRESULT UI::Init(int Count, LPCWSTR texture)
{
	try
	{
		for (int i = 0; i < Count; i++)
		{
			Dialog *dial = new Dialog;
			DialogResourceManager *DRes = new DialogResourceManager;

			dial->Init(DRes, true, texture);

			V(DRes->OnD3D11CreateDevice(fs->GetFile(string("Font.dds"))->PathW.c_str(), fs->GetFile(string("UI.hlsl"))->PathW.c_str()));
			g_DialogResourceManager.push_back(DRes);
			g_Dialog.push_back(dial);
		}
		InitUI = true;
		return S_OK;
	}
	catch (const exception &)
	{
		DebugTrace("UI: Init is failed.\n");
		throw exception("Init is failed!!!");
		InitUI = false;
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddButton(Dialog *Dial, int ID, wstring Text, int X, int Y, int W, int H, int Key)
{
	try
	{
		Buttons.push_back(ID);
		Dial->AddButton(Buttons.back(), Text.c_str(), X, Y, W, H, Key);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddButton failed.\n");
		throw exception("AddButton failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddButton(Dialog *Dial, int ID, wstring Text, int X, int Y, int W, int H)
{
	try
	{
		Buttons.push_back(ID);
		Dial->AddButton(Buttons.back(), Text.c_str(), X, Y, W, H);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddButton failed.\n");
		throw exception("AddButton failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddButton(Dialog *Dial, int ID, wstring Text)
{
	try
	{
		//if (ID->size() != Text.size())
		//{
		//	DebugTrace("UI: AddButton failed. An equal number of ID elements and text is required.\n");
		//	throw exception("ID.size() != Text.size()!!!");
		//	return E_FAIL;
		//}
		Buttons.push_back(ID);
		Dial->AddButton(Buttons.back(), Text.c_str(), 35, iY, 125, 22, VK_F2);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddButton failed.\n");
		throw exception("AddButton failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddStatic(Dialog *Dial, int ID, wstring Text, int X, int Y, int W, int H, int Key)
{
	return E_NOTIMPL;
}

HRESULT UI::AddButton_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *Keys)
{
	try
	{
		if (ID->size() != Text->size() && Y->size() != Keys->size())
		{
			DebugTrace("UI: AddButton_Massive failed. An equal number of ID elements and TEXT is required.\n");
			throw exception("ID.size() != Text.size()!!!");
			return E_FAIL;
		}
		for (int i = 0; i < ID->size(); i++)
		{
			Buttons.push_back(ID->at(i));
			auto W = (Text->at(i).size() + Text->at(i).length()) * 5;
			Dial->AddButton(ID->at(i), Text->at(i).c_str(), 0, Y->at(i), W, 22, Keys->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddButton_Massive failed.\n");
		throw exception("AddButton_Massive failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddButton_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y)
{
	try
	{
		if (ID->size() != Text->size())
		{
			DebugTrace("UI: AddButton_Massive failed. An equal number of ID elements and text is required.\n");
			throw exception("ID.size() != Text.size()!!!");
			return E_FAIL;
		}
		for (int i = 0; i < ID->size(); i++)
		{
			Buttons.push_back(ID->at(i));
			Dial->AddButton(ID->at(i), Text->at(i).c_str(), 35, Y->at(i), 125, 22);
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddButton_Massive failed.\n");
		throw exception("AddButton_Massive failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddStatic_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H)
{
	try
	{
		if (ID->size() != Text->size() && Y->size() != X->size())
		{
			DebugTrace("UI: AddStatic_Massive failed. An equal number of ID elements and text and X\\Y is required.\n");
			throw exception("ID.size() != Text.size()!!!");
			return E_FAIL;
		}
		for (int i = 0; i < ID->size(); i++)
		{
			Statics.push_back(ID->at(i));
			Dial->AddStatic(ID->at(i), Text->at(i).c_str(), X->at(i), Y->at(i), Text->at(i).size(), H->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddStatic_Massive failed.\n");
		throw exception("AddStatic_Massive failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddStatic(Dialog *Dial, int ID, wstring Text, int X, int Y, int W, int H)
{
	return E_NOTIMPL;
}

HRESULT UI::AddStatic(Dialog *Dial, int ID, wstring Text)
{
	try
	{
		Statics.push_back(ID);
		Dial->AddStatic(Statics.back(), Text.c_str(), 35, 90, 60, 50);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddStatic failed.\n");
		throw exception("AddStatic failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

void UI::SetLocationButton(Dialog *Dial, int ID, int X, int Y, bool Align)
{
	if (!Align)
		Dial->GetButton(Buttons.at(ID))->SetLocation(X, Y);
	else
	{
		auto HeightBuff = DXUTGetDXGIBackBufferSurfaceDesc()->Width;
		auto WidthComponent = Dial->GetButton(Buttons.at(ID))->m_width + Dial->GetButton(Buttons.at(ID))->m_height;
		auto Cache = DXUTGetDXGIBackBufferSurfaceDesc()->Width - WidthComponent;
		auto ObjBtn = Dial->GetButton(Buttons.at(ID));
		if (ObjBtn)
			ObjBtn->SetLocation(Cache, Y);
	}
}

void UI::SetTextButton(Dialog *Dial, int ID, wstring *Text)
{
	Dial->GetButton(Buttons.at(ID))->SetText(Text->c_str());
}

void UI::SetTextButton(Dialog *Dial, int ID, wstring *Text, float Format[3])
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
			 Format[1], Format[2], Format[3]);
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, string *Text, float Format[3])
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
			 Format[1], Format[2], Format[3]);
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, wstring *Text, XMMATRIX Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
			 Format.r[4].m128_f32[1], Format.r[4].m128_f32[2], Format.r[4].m128_f32[3]);
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, string *Text, XMMATRIX Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
			 Format.r[4].m128_f32[1], Format.r[4].m128_f32[2], Format.r[4].m128_f32[3]);
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, wstring *Text, float Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"%.2f")).c_str()), Format);
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, string *Text, float Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("%.2f")).c_str(), Format);
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, wstring *Text, vector<size_t> Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(Text->c_str()), Format.data());
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, string *Text, vector<size_t> Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), Text->c_str(), Format.data());
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, wstring *Text, XMVECTOR Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
			 Format.m128_f32[1], Format.m128_f32[2], Format.m128_f32[3]);
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(Dialog *Dial, int ID, string *Text, XMVECTOR Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
			 Format.m128_f32[1], Format.m128_f32[2], Format.m128_f32[3]);
	Dial->GetButton(Buttons.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetLocationStatic(Dialog *Dial, int ID, int X, int Y, bool Align)
{
	Dial->GetStatic(Statics.at(ID))->SetLocation(X, Y);
}

void UI::SetTextStatic(Dialog *Dial, int ID, wstring *Text)
{
	Dial->GetStatic(Statics.at(ID))->SetText(Text->c_str());
}

void UI::SetTextStatic(Dialog *Dial, int ID, wstring *Text, float Format[3])
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
		Format[1], Format[2], Format[3]);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, string *Text, float Format[3])
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
		Format[1], Format[2], Format[3]);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, wstring *Text, XMMATRIX Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
		Format.r[4].m128_f32[1], Format.r[4].m128_f32[2], Format.r[4].m128_f32[3]);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, string *Text, XMMATRIX Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
		Format.r[4].m128_f32[1], Format.r[4].m128_f32[2], Format.r[4].m128_f32[3]);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, wstring *Text, float Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"%.2f")).c_str()), Format);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, string *Text, float Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("%.2f")).c_str(), Format);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, wstring *Text, vector<size_t> Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), 
	W2A(wstring(*Text + wstring(L"%Iu; Instances %Iu; Voices %Iu / %Iu / %Iu / %Iu;")).c_str()),
		Format.at(0), Format.at(1), Format.at(2),
		Format.at(3), Format.at(4), Format.at(5), Format.at(6));
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, string *Text, vector<size_t> Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), 
	string(*Text + string("%Iu; Instances %Iu; Voices %Iu / %Iu / %Iu / %Iu;")).c_str(), 
		Format.at(0), Format.at(1), Format.at(2),
		Format.at(3), Format.at(4), Format.at(5), Format.at(6));
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, wstring *Text, Vector3 *Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
		Format->x, Format->y, Format->z);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, string *Text, Vector3 *Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	string CacheText;

	snprintf(buff, sizeof(buff), (CacheText = string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f"))).c_str(),
		Format->x, Format->y, Format->z);
	auto ObjStatcs = Dial->GetStatic(Statics.at(ID));
	if (ObjStatcs)
	{
		ObjStatcs->SetText(A2W(buff));
		ObjStatcs->SetSize(Text->size(), ObjStatcs->m_height);
	}
}

void UI::SetLocationCheck(Dialog *Dial, int ID, int X, int Y, bool Align)
{
	if (!Align)
		Dial->GetCheckBox(ID)->SetLocation(X, Y);
	else
	{
		auto HeightBuff = DXUTGetDXGIBackBufferSurfaceDesc()->Width;
		auto WidthComponent = Dial->GetCheckBox(ID)->m_width + 400;
		auto Cache = DXUTGetDXGIBackBufferSurfaceDesc()->Width - WidthComponent;
		auto ObjCBK = Dial->GetCheckBox(ID);
		if (ObjCBK)
				ObjCBK->SetLocation(Cache, Y);
	}
}

HRESULT UI::AddSlider(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Min, int Max)
{
	try
	{
		Sliders.push_back(ID);
		Dial->AddSlider(Sliders.back(), X, Y, W, H, Min, Max);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddSlider failed.\n");
		throw exception("AddSlider failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddSlider_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Min, vector<int> *Max, vector<int> *DefValue)
{	
	try
	{
		for (int i = 0; i < ID->size(); i++)
		{
			Sliders.push_back(ID->at(i));
			Dial->AddSlider(Sliders.back(), X->at(i), Y->at(i), W->at(i), H->at(i), Min->at(i), 
							Max->at(i), DefValue->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddSlider_Mass failed.\n");
		throw exception("AddSlider_Mass failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddCheckBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked, int HotKey)
{
	try
	{
		CheckBoxs.push_back(ID);
		Dial->AddCheckBox(ID, Text->c_str(), X, Y, W, H, Checked, HotKey);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddCheckBox failed.\n");
		throw exception("AddCheckBox failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddCheckBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Checked, vector<int> *HotKey)
{
	try
	{
		if (ID->size() != Text->size() && Y->size() != X->size())
		{
			DebugTrace("UI: AddCheckBox_Massive failed. An equal number of ID elements and text and X\\Y is required.\n");
			throw exception("ID.size() != Text.size()!!!");
			return E_FAIL;
		}
		for (int i = 0; i < ID->size(); i++)
		{
			CheckBoxs.push_back(ID->at(i));
			Dial->AddCheckBox(ID->at(i), Text->at(i).c_str(), X->at(i), Y->at(i), W->at(i),
				H->at(i), Checked->at(i), HotKey->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddCheckBox_Mass failed.\n");
		throw exception("AddCheckBox_Mass failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddCheckBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked)
{
	try
	{
		CheckBoxs.push_back(ID);
		Dial->AddCheckBox(ID, Text->c_str(), X, Y, W, H, Checked);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddCheckBox failed.\n");
		throw exception("AddCheckBox failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddCheckBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Checked)
{
	try
	{
		if (ID->size() != Text->size() && Y->size() != X->size())
		{
			DebugTrace("UI: AddCheckBox_Massive failed. An equal number of ID elements and text and X\\Y is required.\n");
			throw exception("ID.size() != Text.size()!!!");
			return E_FAIL;
		}
		for (int i = 0; i < ID->size(); i++)
		{
			CheckBoxs.push_back(ID->at(i));
			Dial->AddCheckBox(ID->at(i), Text->at(i).c_str(), X->at(i), Y->at(i), W->at(i),
				H->at(i), Checked->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddCheckBox_Mass failed.\n");
		throw exception("AddCheckBox_Mass failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddCheckBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H)
{
	try
	{
		CheckBoxs.push_back(ID);
		auto W = (Text->size() + Text->length()) * 4;
		Dial->AddCheckBox(ID, Text->c_str(), X, Y, W, H);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddCheckBox failed.\n");
		throw exception("AddCheckBox failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddCheckBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H)
{
	try
	{
		if (ID->size() != Text->size() && Y->size() != X->size())
		{
			DebugTrace("UI: AddCheckBox_Massive failed. An equal number of ID elements and text and X\\Y is required.\n");
			throw exception("ID.size() != Text.size()!!!");
			return E_FAIL;
		}
		for (int i = 0; i < ID->size(); i++)
		{
			CheckBoxs.push_back(ID->at(i));
			Dial->AddCheckBox(ID->at(i), Text->at(i).c_str(), X->at(i), Y->at(i), W->at(i),
				H->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddCheckBox_Mass failed.\n");
		throw exception("AddCheckBox_Mass failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddComboBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked, int HotKey)
{
	try
	{
		ComboBoxs.push_back(ID);
		Dial->AddComboBox(ID, X, Y, W, H, Checked, HotKey);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddComboBox failed.\n");
		throw exception("AddComboBox failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddComboBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Checked, vector<int> *HotKey)
{
	try
	{
		for (int i = 0; i < ID->size(); i++)
		{
			ComboBoxs.push_back(ID->at(i));
			Dial->AddComboBox(ID->at(i), X->at(i), Y->at(i), W->at(i),
				H->at(i), Checked->at(i), HotKey->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddComboBox_Mass failed.\n");
		throw exception("AddComboBox_Mass failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddComboBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H, int Checked)
{
	try
	{
		ComboBoxs.push_back(ID);
		Dial->AddComboBox(ID, X, Y, W, H, Checked);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddComboBox failed.\n");
		throw exception("AddComboBox failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddComboBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H, vector<int> *Checked)
{
	try
	{
		for (int i = 0; i < ID->size(); i++)
		{
			ComboBoxs.push_back(ID->at(i));
			Dial->AddComboBox(ID->at(i), X->at(i), Y->at(i), W->at(i), H->at(i), Checked->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddComboBox_Mass failed.\n");
		throw exception("AddComboBox_Mass failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddComboBox(Dialog *Dial, int ID, wstring *Text, int X, int Y, int W, int H)
{
	try
	{
		ComboBoxs.push_back(ID);
		Dial->AddComboBox(ID, X, Y, W, H);
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddComboBox failed.\n");
		throw exception("AddComboBox failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddComboBox_Mass(Dialog *Dial, vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *W, vector<int> *H)
{
	try
	{
		for (int i = 0; i < ID->size(); i++)
		{
			ComboBoxs.push_back(ID->at(i));
			Dial->AddComboBox(ID->at(i), X->at(i), Y->at(i), W->at(i), H->at(i));
		}
	}
	catch (const exception&)
	{
		DebugTrace("UI: AddComboBox_Mass failed.\n");
		throw exception("AddComboBox_Mass failed!!!");
		return E_FAIL;
	}
	return S_OK;
}


void UI::SetTextStatic(Dialog *Dial, int ID, wstring *Text, size_t Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff),
		W2A(wstring(*Text + wstring(L"%d")).c_str()), Format);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(Dialog *Dial, int ID, string *Text, size_t Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff),
		string(*Text + string("%d")).c_str(), Format);
	Dial->GetStatic(Statics.at(ID))->SetText(A2W(buff));
}

int UI::getAllComponentsCount()
{
	vector<int> cache;
	for (int i = 0; i < Buttons.size(); i++)
		cache.push_back(Buttons.at(i));

	for (int i = 0; i < Statics.size(); i++)
		cache.push_back(Statics.at(i));

	for (int i = 0; i < CheckBoxs.size(); i++)
		cache.push_back(CheckBoxs.at(i));

	for (int i = 0; i < Sliders.size(); i++)
		cache.push_back(Sliders.at(i));

	for (int i = 0; i < ComboBoxs.size(); i++)
		cache.push_back(ComboBoxs.at(i));

	return cache.size();
}

void Engine::UI::Render(float Time, int ID)
{
	if (!ID)
		for (int i = 0; i < g_Dialog.size(); i++)
			g_Dialog.at(i)->OnRender(Time);
	else
		g_Dialog.at(ID)->OnRender(Time);
}

HRESULT Engine::UI::LoadXmlUI(LPCSTR File)
{
	doc = make_unique<tinyxml2::XMLDocument>();

	doc->LoadFile(File);
	if (doc->ErrorID() > 0)
	{
		StackTrace(doc->ErrorStr());
		throw exception("UI->LoatXmlUI()::doc->LoadFile() == 0!!!");
		return E_FAIL;
	}
	if (doc->Parse(fs->getDataFromFile(&string(File), true).c_str()) > 0)
	{
		throw exception(string(string("UI->LoatXmlUI()::doc->Parse: \n") + string(doc->ErrorStr())).c_str());
		return E_FAIL;
	}
	
	ProcessXML();

	return S_OK;
}

void Engine::UI::CheckType(LPCSTR OneType)
{
	if (strcmp(OneType, "Button") == 0)
	{
		Buttons.push_back(Buttons.size() + 1);
		return;
	}

	if (strcmp(OneType, "Static") == 0)
	{
		Statics.push_back(Statics.size() + 1);
		return;
	}

	if (strcmp(OneType, "Checkbox") == 0)
	{
		CheckBoxs.push_back(CheckBoxs.size() + 1);
		return;
	}

	if (strcmp(OneType, "Radiobutton") == 0)
	{
		RadioButtons.push_back(RadioButtons.size() + 1);
		return;
	}

	if (strcmp(OneType, "Combobox") == 0)
	{
		ComboBoxs.push_back(ComboBoxs.size() + 1);
		return;
	}

	if (strcmp(OneType, "Slider") == 0)
	{
		Sliders.push_back(Sliders.size() + 1);
		return;
	}

	if (strcmp(OneType, "Editbox") == 0)
	{
		EditBoxs.push_back(EditBoxs.size() + 1);
		return;
	}

	/*
		case CONTROL_IMEEDITBOX:
			if (strcmp(OneType, "Button") == 0)
				Buttons.push_back(Buttons.size() + 1);
			break;
	*/
	if (strcmp(OneType, "Listbox") == 0)
	{
		ListBoxs.push_back(ListBoxs.size() + 1);
		return;
	}

	if (strcmp(OneType, "Scrollbar") == 0)
	{
		ScrollBars.push_back(ScrollBars.size() + 1);
		return;
	}
}

void Engine::UI::ProcessXML()
{
	Element = { doc->RootElement() };
	if (!Element.front())
	{
		DebugTrace("UI->LoadXmlUI()::doc->RootElement() == nullptr!!!");
		throw exception("UI->LoadXmlUI()::doc->RootElement() == nullptr!!!");
		return;
	}

	// ********
		// GUI
	XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
	for (int i = 1; i < INT16_MAX; i++)
	{
		if (strcmp(FirstAttr->Name(), "id") == 0)
		{
			ID.push_back(FirstAttr->Value());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}

		if (strcmp(FirstAttr->Name(), "width") == 0)
		{
			W.push_back(FirstAttr->IntValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}

		if (strcmp(FirstAttr->Name(), "height") == 0)
		{
			H.push_back(FirstAttr->IntValue());
			FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
			if (!FirstAttr)
				break;
		}
	}
	Element.push_back(Element.back()->FirstChild()->ToElement());

	// ********
		// Other
	for (int i = 1; i < INT16_MAX; i++)
	{
		XMLAttribute *FirstAttr = const_cast<XMLAttribute *>(Element.back()->ToElement()->FirstAttribute());
		for (int i = 1; i < INT16_MAX; i++)
		{
			if (strcmp(FirstAttr->Name(), "id") == 0)
			{
				ID.push_back(FirstAttr->Value());
				CheckType(Element.back()->ToElement()->Value());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "text") == 0)
			{
				Text.push_back(FirstAttr->Value());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "x") == 0)
			{
				X.push_back(FirstAttr->IntValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "y") == 0)
			{
				Y.push_back(FirstAttr->IntValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "width") == 0)
			{
				W.push_back(FirstAttr->IntValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}

			if (strcmp(FirstAttr->Name(), "height") == 0)
			{
				H.push_back(FirstAttr->IntValue());
				FirstAttr = const_cast<XMLAttribute *>(FirstAttr->Next());
				if (!FirstAttr)
					break;
			}
		}
		if (Element.front()->LastChild()->Value() == Element.back()->Value())
			break;

		Element.push_back(Element.back()->NextSibling()->ToElement());
	}
}

void Engine::UI::ReloadXML(LPCSTR File)
{
	W.clear(); 
	H.clear();
	X.clear();
	Y.clear();
	ID.clear();
	Text.clear();
	Element.clear();

	LoadXmlUI(File);
}