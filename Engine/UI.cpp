#include "pch.h"
#include "UI.h"

HRESULT UI::Init()
{
	try
	{
		g_HUD.Init(&g_DialogResourceManager);
		InitUI = true;
		return S_OK;
	}
	catch (const exception ex)
	{
		DebugTrace("UI: Init failed. Line: 12\n");
		throw std::exception("Init == nullptr!!!");
		InitUI = false;
		return E_FAIL;
	}
}

HRESULT UI::AddButton(int ID, wstring Text, int X, int Y, int W, int H, int Key)
{
	return S_OK;
}

HRESULT UI::AddButton(int ID, wstring Text, int X, int Y, int W, int H)
{
	return S_OK;
}

HRESULT UI::AddButton(int ID, wstring Text)
{
	try
	{
		//if (ID->size() != Text.size())
		//{
		//	DebugTrace("UI: AddButton failed. An equal number of ID elements and text is required. Line: 36\n");
		//	throw std::exception("ID.size() != Text.size()!!!");
		//	return E_FAIL;
		//}
		ObjButton.push_back(ID);
		ObjNameButton.push_back(Text);
		g_HUD.AddButton(ObjButton.back(), ObjNameButton.back().c_str(), 35, iY, 125, 22, VK_F2);
	}
	catch (const std::exception&)
	{
		DebugTrace("UI: AddButton failed. Line: 45\n");
		throw std::exception("AddButton failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddStatic(int ID, wstring Text, int X, int Y, int W, int H, int Key)
{
	return E_NOTIMPL;
}

HRESULT UI::AddButton_Mass(vector<int> *ID, vector<wstring> *Text, vector<int> *X, vector<int> *Y, vector<int> *Keys)
{
	try
	{
		if (ID->size() != Text->size() && Y->size() != Keys->size())
		{
			DebugTrace("UI: AddButton_Massive failed. An equal number of ID elements and text is required. Line: 63\n");
			throw std::exception("ID.size() != Text.size()!!!");
			return E_FAIL;
		}
		for (int i = 0; i < ID->size(); i++)
		{
			ObjButton.push_back(ID->at(i));
			ObjNameButton.push_back(Text->at(i));
			g_HUD.AddButton(ID->at(i), Text->at(i).c_str(), 35, Y->at(i), 125, 22, Keys->at(i));
		}
	}
	catch (const std::exception&)
	{
		DebugTrace("UI: AddButton_Massive failed. Line: 76\n");
		throw std::exception("AddButton_Massive failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddStatic_Mass(vector<int>* ID, vector<wstring>* Text, vector<int>* X, vector<int>* Y)
{
	try
	{
		if (ID->size() != Text->size() && Y->size() != ID->size())
		{
			DebugTrace("UI: AddStatic_Massive failed. An equal number of ID elements and text is required. Line: 89\n");
			throw std::exception("ID.size() != Text.size()!!!");
			return E_FAIL;
		}
		for (int i = 0; i < ID->size(); i++)
		{
			ObjStatic.push_back(ID->at(i));
			ObjNameStatic.push_back(Text->at(i));
			g_HUD.AddStatic(ID->at(i), Text->at(i).c_str(), 35, Y->at(i), 60, 50);
		}
	}
	catch (const std::exception&)
	{
		DebugTrace("UI: AddStatic_Massive failed. Line: 102\n");
		throw std::exception("AddStatic_Massive failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT UI::AddStatic(int ID, wstring Text, int X, int Y, int W, int H)
{
	return E_NOTIMPL;
}

HRESULT UI::AddStatic(int ID, wstring Text)
{
	try
	{
		//if (ID.size() != Text.size())
		//{
		//	DebugTrace("UI: AddStatic failed. An equal number of ID elements and text is required. Line: 67\n");
		//	throw std::exception("ID.size() != Text.size()!!!");
		//	return E_FAIL;
		//}
		ObjStatic.push_back(ID);
		ObjNameStatic.push_back(Text);
		g_HUD.AddStatic(ObjStatic.back(), ObjNameStatic.back().c_str(), 35, 90, 60, 50);
	}
	catch (const std::exception&)
	{
		DebugTrace("UI: AddStatic failed. Line: 130\n");
		throw std::exception("AddStatic failed!!!");
		return E_FAIL;
	}
	return S_OK;
}

void UI::SetLocationButton(int ID, int X, int Y)
{
	g_HUD.GetButton(ObjButton.at(ID))->SetLocation(X, Y);
}

void UI::SetTextButton(int ID, wstring *Text)
{
	g_HUD.GetButton(ObjButton.at(ID))->SetText(Text->c_str());
}

void UI::SetTextButton(int ID, wstring *Text, float Format[3])
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
			 Format[1], Format[2], Format[3]);
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, string *Text, float Format[3])
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
			 Format[1], Format[2], Format[3]);
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, wstring *Text, XMMATRIX Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
			 Format.r[4].m128_f32[1], Format.r[4].m128_f32[2], Format.r[4].m128_f32[3]);
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, string *Text, XMMATRIX Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
			 Format.r[4].m128_f32[1], Format.r[4].m128_f32[2], Format.r[4].m128_f32[3]);
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, wstring *Text, float Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"%.2f")).c_str()), Format);
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, string *Text, float Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("%.2f")).c_str(), Format);
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, wstring *Text, vector<size_t> Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(Text->c_str()), Format.data());
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, string *Text, vector<size_t> Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), Text->c_str(), Format.data());
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, wstring *Text, XMVECTOR Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
			 Format.m128_f32[1], Format.m128_f32[2], Format.m128_f32[3]);
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetTextButton(int ID, string *Text, XMVECTOR Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
			 Format.m128_f32[1], Format.m128_f32[2], Format.m128_f32[3]);
	g_HUD.GetButton(ObjButton.at(ID))->SetText(A2W(buff));
	ZeroMemory(buff, sizeof(buff));
}

void UI::SetLocationStatic(int ID, int X, int Y)
{
	g_HUD.GetStatic(ObjStatic.at(ID))->SetLocation(X, Y);
}

void UI::SetTextStatic(int ID, wstring *Text)
{
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(Text->c_str());
}

void UI::SetTextStatic(int ID, wstring *Text, float Format[3])
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
		Format[1], Format[2], Format[3]);
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, string *Text, float Format[3])
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
		Format[1], Format[2], Format[3]);
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, wstring *Text, XMMATRIX Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
		Format.r[4].m128_f32[1], Format.r[4].m128_f32[2], Format.r[4].m128_f32[3]);
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, string *Text, XMMATRIX Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
		Format.r[4].m128_f32[1], Format.r[4].m128_f32[2], Format.r[4].m128_f32[3]);
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, wstring *Text, float Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"%.2f")).c_str()), Format);
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, string *Text, float Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("%.2f")).c_str(), Format);
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, wstring *Text, vector<size_t> Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), 
	W2A(wstring(*Text + wstring(L"%Iu; Instances %Iu; Voices %Iu / %Iu / %Iu / %Iu;")).c_str()),
		Format.at(0), Format.at(1), Format.at(2),
		Format.at(3), Format.at(4), Format.at(5), Format.at(6));
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, string *Text, vector<size_t> Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), 
	string(*Text + string("%Iu; Instances %Iu; Voices %Iu / %Iu / %Iu / %Iu;")).c_str(), 
		Format.at(0), Format.at(1), Format.at(2),
		Format.at(3), Format.at(4), Format.at(5), Format.at(6));
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, wstring *Text, XMVECTOR Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), W2A(wstring(*Text + wstring(L"X:%.1f, Y:%.1f, Z:%.1f")).c_str()),
		Format.m128_f32[1], Format.m128_f32[2], Format.m128_f32[3]);
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}

void UI::SetTextStatic(int ID, string *Text, XMVECTOR Format)
{
	char buff[FILENAME_MAX];
	USES_CONVERSION;
	snprintf(buff, sizeof(buff), string(*Text + string("X:%.1f, Y:%.1f, Z:%.1f")).c_str(),
		Format.m128_f32[1], Format.m128_f32[2], Format.m128_f32[3]);
	g_HUD.GetStatic(ObjStatic.at(ID))->SetText(A2W(buff));
}
