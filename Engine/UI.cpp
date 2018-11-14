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
		DebugTrace("UI: Init failed. Line: 13\n");
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
		DebugTrace("UI: AddButton failed. Line: 46\n");
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
			DebugTrace("UI: AddButton_Massive failed. An equal number of ID elements and text is required. Line: 64\n");
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
		DebugTrace("UI: AddButton_Massive failed. Line: 77\n");
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
			DebugTrace("UI: AddStatic_Massive failed. An equal number of ID elements and text is required. Line: 90\n");
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
		DebugTrace("UI: AddStatic_Massive failed. Line: 105\n");
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
		DebugTrace("UI: AddStatic failed. Line: 132\n");
		throw std::exception("AddStatic failed!!!");
		return E_FAIL;
	}
	return S_OK;
}
