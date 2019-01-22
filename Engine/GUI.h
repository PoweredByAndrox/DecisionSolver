//--------------------------------------------------------------------------------------
// File: DXUTgui.h
//
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=320437
//--------------------------------------------------------------------------------------
#pragma once

#include <usp10.h>
#include <dimm.h>
#include <WICTextureLoader.h>

#include "File_system.h"
#include "Shaders.h"


#ifdef DXUT_AUTOLIB
#pragma comment( lib, "usp10.lib" )
#endif

namespace Engine
{
	using namespace std;
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//--------------------------------------------------------------------------------------
	// Defines and macros 
	//--------------------------------------------------------------------------------------
#define EVENT_BUTTON_CLICKED                0x0101
#define EVENT_COMBOBOX_SELECTION_CHANGED    0x0201
#define EVENT_RADIOBUTTON_CHANGED           0x0301
#define EVENT_CHECKBOX_CHANGED              0x0401
#define EVENT_SLIDER_VALUE_CHANGED          0x0501
#define EVENT_SLIDER_VALUE_CHANGED_UP       0x0502

#define EVENT_EDITBOX_STRING                0x0601
// EVENT_EDITBOX_CHANGE is sent when the listbox content changes
// due to user input.
#define EVENT_EDITBOX_CHANGE                0x0602
#define EVENT_LISTBOX_ITEM_DBLCLK           0x0701
// EVENT_LISTBOX_SELECTION is fired off when the selection changes in
// a single selection list box.
#define EVENT_LISTBOX_SELECTION             0x0702
#define EVENT_LISTBOX_SELECTION_END         0x0703


//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
	class DialogResourceManager;
	class Control;
	class Button;
	class Static;
	class CheckBox;
	class RadioButton;
	class ComboBox;
	class Slider;
	class EditBox;
	class ListBox;
	class ScrollBar;
	class Element;
	struct ElementHolder;
	struct TextureNode;
	struct FontNode;
	
	typedef void (CALLBACK *PCALLBACKGUIEVENT)(_In_ UINT nEvent, _In_ int nControlID, _In_ Control *pControl, _In_opt_ void* pUserContext);

	//--------------------------------------------------------------------------------------
	// Enums for pre-defined control types
	//--------------------------------------------------------------------------------------
	enum CONTROL_TYPE
	{
		CONTROL_BUTTON,
		CONTROL_STATIC,
		CONTROL_CHECKBOX,
		CONTROL_RADIOBUTTON,
		CONTROL_COMBOBOX,
		CONTROL_SLIDER,
		CONTROL_EDITBOX,
		CONTROL_IMEEDITBOX,
		CONTROL_LISTBOX,
		CONTROL_SCROLLBAR,
	};

	enum CONTROL_STATE
	{
		STATE_NORMAL = 0,
		STATE_DISABLED,
		STATE_HIDDEN,
		STATE_FOCUS,
		STATE_MOUSEOVER,
		STATE_PRESSED,
	};

#define MAX_CONTROL_STATES 6

	struct BlendColor
	{
		void Init(_In_ DWORD defaultColor, _In_ DWORD disabledColor = D3DCOLOR_ARGB(200, 128, 128, 128), _In_ DWORD hiddenColor = 0);
		void Blend(_In_ UINT iState, _In_ float fElapsedTime, _In_ float fRate = 0.7f);

		DWORD States[MAX_CONTROL_STATES]; // Modulate colors for all possible control states
		Vector4 Current;

		void SetCurrent(DWORD color);
	};


	//-----------------------------------------------------------------------------
	// Contains all the display tweakables for a sub-control
	//-----------------------------------------------------------------------------
	class Element
	{
	public:
		void SetTexture(_In_ UINT texture, _In_ RECT *prcTexture, _In_ DWORD defaultTextureColor = D3DCOLOR_ARGB(255, 255, 255, 255));
		void SetFont(_In_ UINT font, _In_ DWORD defaultFontColor = D3DCOLOR_ARGB(255, 255, 255, 255), DWORD textFormat = DT_CENTER | DT_VCENTER);

		void Refresh();

		UINT iTexture,         // Index of the texture for this Element 
			iFont;             // Index of the font for this Element
		DWORD dwTextFormat;     // The format argument to DrawText 

		RECT rcTexture;         // Bounding rect of this element on the composite texture

		BlendColor TextureColor,
			FontColor;
	};


	//-----------------------------------------------------------------------------
	// All controls must be assigned to a dialog, which handles
	// input and rendering for the controls.
	//-----------------------------------------------------------------------------
	class Dialog
	{
		friend class DialogResourceManager;

	public:
		Dialog() noexcept;
		~Dialog();

		// Need to call this now
		void Init(_In_ DialogResourceManager *pManager, _In_ bool bRegisterDialog = true);
		void Init(_In_ DialogResourceManager *pManager, _In_ bool bRegisterDialog,
			_In_z_ LPCWSTR pszControlTextureFilename);
		void Init(_In_ DialogResourceManager *pManager, _In_ bool bRegisterDialog,
			_In_z_ LPCWSTR szControlTextureResourceName, _In_ HMODULE hControlTextureResourceModule);

		// Windows message handler
		bool MsgProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

		// Control creation
		HRESULT AddStatic(_In_ int ID, _In_z_ LPCWSTR strText, _In_ int x, _In_ int y, _In_ int width, _In_ int height, _In_ bool bIsDefault = false,
			_Out_opt_ Static **ppCreated = nullptr);
		HRESULT AddButton(_In_ int ID, _In_z_ LPCWSTR strText, _In_ int x, _In_ int y, _In_ int width, _In_ int height, _In_ UINT nHotkey = 0,
			_In_ bool bIsDefault = false, _Out_opt_ Button **ppCreated = nullptr);
		HRESULT AddCheckBox(_In_ int ID, _In_z_ LPCWSTR strText, _In_ int x, _In_ int y, _In_ int width, _In_ int height, _In_ bool bChecked = false,
			_In_ UINT nHotkey = 0, _In_ bool bIsDefault = false, _Out_opt_ CheckBox **ppCreated = nullptr);
		HRESULT AddRadioButton(_In_ int ID, _In_ UINT nButtonGroup, _In_z_ LPCWSTR strText, _In_ int x, _In_ int y, _In_ int width,
			_In_ int height, _In_ bool bChecked = false, _In_ UINT nHotkey = 0, _In_ bool bIsDefault = false,
			_Out_opt_ RadioButton** ppCreated = nullptr);
		HRESULT AddComboBox(_In_ int ID, _In_ int x, _In_ int y, _In_ int width, _In_ int height, _In_ UINT nHotKey = 0, _In_ bool bIsDefault = false,
			_Out_opt_ ComboBox** ppCreated = nullptr);
		HRESULT AddSlider(_In_ int ID, _In_ int x, _In_ int y, _In_ int width, _In_ int height, _In_ int min = 0, _In_ int max = 100, _In_ int value = 50,
			_In_ bool bIsDefault = false, _Out_opt_ Slider **ppCreated = nullptr);
		//      AddIMEEditBox has been renamed into DXUTguiIME.cpp as CDXUTIMEEditBox::CreateIMEEditBox
		HRESULT AddEditBox(_In_ int ID, _In_z_ LPCWSTR strText, _In_ int x, _In_ int y, _In_ int width, _In_ int height, _In_ bool bIsDefault = false,
			_Out_opt_ EditBox** ppCreated = nullptr);
		HRESULT AddListBox(_In_ int ID, _In_ int x, _In_ int y, _In_ int width, _In_ int height, _In_ DWORD dwStyle = 0,
			_Out_opt_ ListBox **ppCreated = nullptr);
		HRESULT AddControl(_In_ Control *pControl);
		HRESULT InitControl(_In_ Control *pControl);

		// Control retrieval
		Static *GetStatic(_In_ int ID) const
		{
			return reinterpret_cast<Static *>(GetControl(ID, CONTROL_STATIC));
		}
		Button *GetButton(_In_ int ID) const
		{
			return reinterpret_cast<Button *>(GetControl(ID, CONTROL_BUTTON));
		}
		CheckBox *GetCheckBox(_In_ int ID) const
		{
			return reinterpret_cast<CheckBox *>(GetControl(ID, CONTROL_CHECKBOX));
		}
		RadioButton *GetRadioButton(_In_ int ID) const
		{
			return reinterpret_cast<RadioButton *>(GetControl(ID, CONTROL_RADIOBUTTON));
		}
		ComboBox *GetComboBox(_In_ int ID) const
		{
			return reinterpret_cast<ComboBox *>(GetControl(ID, CONTROL_COMBOBOX));
		}
		Slider *GetSlider(_In_ int ID) const
		{
			return reinterpret_cast<Slider *>(GetControl(ID, CONTROL_SLIDER));
		}
		EditBox *GetEditBox(_In_ int ID) const
		{
			return reinterpret_cast<EditBox *>(GetControl(ID, CONTROL_EDITBOX));
		}
		ListBox *GetListBox(_In_ int ID) const
		{
			return reinterpret_cast<ListBox *>(GetControl(ID, CONTROL_LISTBOX));
		}

		Control *GetControl(_In_ int ID) const;
		Control *GetControl(_In_ int ID, _In_ UINT nControlType) const;
		Control *GetControlAtPoint(_In_ const POINT &pt) const;

		bool GetControlEnabled(_In_ int ID) const;
		void SetControlEnabled(_In_ int ID, _In_ bool bEnabled);

		void ClearRadioButtonGroup(_In_ UINT nGroup);
		void ClearComboBox(_In_ int ID);

		// Access the default display Elements used when adding new controls
		HRESULT SetDefaultElement(_In_ UINT nControlType, _In_ UINT iElement, _In_ Element *pElement);
		Element* GetDefaultElement(_In_ UINT nControlType, _In_ UINT iElement) const;

		// Methods called by controls
		void SendEvent(_In_ UINT nEvent, _In_ bool bTriggeredByUser, _In_ Control *pControl);
		void RequestFocus(_In_ Control *pControl);

		// Render helpers
		HRESULT DrawRect(_In_ const RECT *pRect, _In_ DWORD color);
		HRESULT DrawSprite(_In_ Element *pElement, _In_ const RECT *prcDest, _In_ float fDepth);
		HRESULT DrawSprite11(_In_ Element *pElement, _In_ const RECT *prcDest, _In_ float fDepth);
		HRESULT CalcTextRect(_In_z_ LPCWSTR strText, _In_ Element *pElement, _In_ const RECT *prcDest, _In_ int nCount = -1);
		HRESULT DrawTextGUI(_In_z_ LPCWSTR strText, _In_ Element *pElement, _In_ const RECT *prcDest, _In_ bool bShadow = false, _In_ bool bCenter = false);

		// Attributes
		bool GetVisible() const { return m_bVisible; }
		void SetVisible(_In_ bool bVisible) { m_bVisible = bVisible; }
		bool GetMinimized() const { return m_bMinimized; }
		void SetMinimized(_In_ bool bMinimized) { m_bMinimized = bMinimized; }
		void SetBackgroundColors(_In_ DWORD colorAllCorners) { SetBackgroundColors(colorAllCorners, colorAllCorners, colorAllCorners, colorAllCorners); }
		void SetBackgroundColors(_In_ DWORD colorTopLeft, _In_ DWORD colorTopRight, _In_ DWORD colorBottomLeft, _In_ DWORD colorBottomRight);
		void EnableCaption(_In_ bool bEnable) { m_bCaption = bEnable; }
		int GetCaptionHeight() const { return m_nCaptionHeight; }
		void SetCaptionHeight(_In_ int nHeight) { m_nCaptionHeight = nHeight; }
		void SetCaptionText(_In_ const WCHAR *pwszText) { wcscpy_s(m_wszCaption, sizeof(m_wszCaption) / sizeof(m_wszCaption[0]), pwszText); }
		void GetLocation(_Out_ POINT &Pt) const
		{
			Pt.x = m_x;
			Pt.y = m_y;
		}
		void SetLocation(_In_ int x, _In_ int y)
		{
			m_x = x;
			m_y = y;
		}
		void SetSize(_In_ int width, _In_ int height)
		{
			m_width = width;
			m_height = height;
		}
		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

		static void WINAPI SetRefreshTime(_In_ float fTime) { s_fTimeRefresh = fTime; }

		static Control *WINAPI GetNextControl(_In_ Control *pControl);
		static Control *WINAPI GetPrevControl(_In_ Control *pControl);

		void RemoveControl(_In_ int ID);
		void RemoveAllControls();

		// Sets the callback used to notify the app of control events
		void SetCallback(_In_ PCALLBACKGUIEVENT pCallback, _In_opt_ void* pUserContext = nullptr);
		void EnableNonUserEvents(_In_ bool bEnable) { m_bNonUserEvents = bEnable; }
		void EnableKeyboardInput(_In_ bool bEnable) { m_bKeyboardInput = bEnable; }
		void EnableMouseInput(_In_ bool bEnable) { m_bMouseInput = bEnable; }
		bool IsKeyboardInputEnabled() const { return m_bKeyboardInput; }

		// Device state notification
		void Refresh();
		HRESULT OnRender(_In_ float fElapsedTime);

		// Shared resource access. Indexed fonts and textures are shared among
		// all the controls.
		HRESULT SetFont(_In_ UINT index, _In_z_ LPCWSTR strFaceName, _In_ LONG height, _In_ LONG weight);
		FontNode *GetFont(_In_ UINT index) const;

		HRESULT SetTexture(_In_ UINT index, _In_z_ LPCWSTR strFilename);
		HRESULT SetTexture(_In_ UINT index, _In_z_ LPCWSTR strResourceName, _In_ HMODULE hResourceModule);
		TextureNode *GetTexture(_In_ UINT index) const;

		DialogResourceManager *GetManager() const { return m_pManager; }

		static void WINAPI  ClearFocus();
		void FocusDefaultControl();

		bool m_bNonUserEvents;
		bool m_bKeyboardInput;
		bool m_bMouseInput;

		//************
			// Getting ALL Controls In Current Dialog
		auto getAllControls() { if (!m_Controls.empty()) return m_Controls; return vector<Control *>{nullptr}; }


		//************
			// Get Next Dialog From Current
		auto getNextDialog() { if (m_pNextDialog) return m_pNextDialog; }


		//************
			// Get Previous Dialogs From Current
		auto getPrevDialogs() { if (m_pPrevDialog) return m_pPrevDialog; }

		//************
			// Get Control Mouse Over
		auto getControlMouseOver() { return m_pControlMouseOver; }
	private:
		int m_nDefaultControlID;

		static double s_fTimeRefresh;
		double m_fTimeLastRefresh;

		// Initialize default Elements
		void InitDefaultElements();

		// Windows message handlers
		void OnMouseMove(_In_ const POINT &pt);
		void OnMouseUp(_In_ const POINT &pt);

		void SetNextDialog(_In_ Dialog *pNextDialog);

		// Control events
		bool OnCycleFocus(_In_ bool bForward);

		static Control *s_pControlFocus;        // The control which has focus
		static Control *s_pControlPressed;      // The control currently pressed

		Control *m_pControlMouseOver;           // The control which is hovered over

		bool m_bVisible,
			m_bCaption,
			m_bMinimized,
			m_bDrag;
		WCHAR m_wszCaption[256];

		int m_x,
			m_y,
			m_width,
			m_height,
			m_nCaptionHeight;

		DWORD m_colorTopLeft,
			m_colorTopRight,
			m_colorBottomLeft,
			m_colorBottomRight;

		DialogResourceManager *m_pManager;
		PCALLBACKGUIEVENT m_pCallbackEvent;
		void* m_pCallbackEventUserContext;

		vector<int> m_Textures;   // Index into m_TextureCache;
		vector<int> m_Fonts;      // Index into m_FontCache;

		vector<Control *> m_Controls;
		vector<ElementHolder *> m_DefaultElements;

		Element m_CapElement;  // Element for the caption

		Dialog *m_pNextDialog;
		Dialog *m_pPrevDialog;
	};

	//--------------------------------------------------------------------------------------
	// Structs for shared resources
	//--------------------------------------------------------------------------------------
	struct TextureNode
	{
		bool bFileSource;  // True if this texture is loaded from a file. False if from resource.
		HMODULE hResourceModule;
		int nResourceID;   // Resource ID. If 0, string-based ID is used and stored in strFilename.
		WCHAR strFilename[MAX_PATH];
		DWORD dwWidth,
			  dwHeight;
		ID3D11Texture2D* pTexture11;
		ID3D11ShaderResourceView* pTexResView11;
	};

	struct FontNode
	{
		WCHAR strFace[MAX_PATH];
		LONG nHeight,
			nWeight;
	};

	struct SpriteVertex
	{
		Vector3 vPos;
		Vector4 vColor;
		Vector2 vTex;
	};

	//-----------------------------------------------------------------------------
	// Manages shared resources of dialogs
	//-----------------------------------------------------------------------------
	class DialogResourceManager: public Shaders
	{
	public:
		DialogResourceManager() noexcept;
		~DialogResourceManager();

		bool MsgProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);

		// D3D11 specific
		HRESULT OnD3D11CreateDevice(_In_ LPCWSTR UIPath, _In_ LPCWSTR ShaderFile);
		void OnD3D11ReleasingSwapChain();
		void OnD3D11DestroyDevice();
		void StoreD3D11State(_In_ ID3D11DeviceContext *pd3dImmediateContext);
		void RestoreD3D11State(_In_ ID3D11DeviceContext *pd3dImmediateContext);
		void ApplyRenderUI11(_In_ ID3D11DeviceContext *pd3dImmediateContext);
		void ApplyRenderUIUntex11(_In_ ID3D11DeviceContext *pd3dImmediateContext);
		void BeginSprites11();
		void EndSprites11(_In_ ID3D11Device *pd3dDevice, _In_ ID3D11DeviceContext *pd3dImmediateContext);

		ID3D11Device *GetD3D11Device() const { return m_pd3d11Device; }
		ID3D11DeviceContext *GetD3D11DeviceContext() const { return m_pd3d11DeviceContext; }

		FontNode *GetFontNode(_In_ size_t iIndex) const { return m_FontCache[iIndex]; }
		TextureNode *GetTextureNode(_In_ size_t iIndex) const { return m_TextureCache[iIndex]; }

		int AddFont(_In_z_ LPCWSTR strFaceName, _In_ LONG height, _In_ LONG weight);
		int AddTexture(_In_z_ LPCWSTR strFilename);
		int AddTexture(_In_z_ LPCWSTR strResourceName, _In_ HMODULE hResourceModule);

		bool RegisterDialog(_In_ Dialog *pDialog);
		void UnregisterDialog(_In_ Dialog *pDialog);
		void EnableKeyboardInputForAllDialogs();

		// Shared between all dialogs

		// D3D11
		// Shaders
		ID3D11VertexShader *m_pVSRenderUI11;
		ID3D11PixelShader *m_pPSRenderUI11, *m_pPSRenderUIUntex11;

		// States
		ID3D11DepthStencilState *m_pDepthStencilStateUI11;
		ID3D11RasterizerState *m_pRasterizerStateUI11;
		ID3D11BlendState *m_pBlendStateUI11;
		ID3D11SamplerState *m_pSamplerStateUI11;

		// Stored states
		ID3D11DepthStencilState *m_pDepthStencilStateStored11;
		UINT m_StencilRefStored11;
		ID3D11RasterizerState *m_pRasterizerStateStored11;
		ID3D11BlendState *m_pBlendStateStored11;
		float m_BlendFactorStored11[4];
		UINT m_SampleMaskStored11;
		ID3D11SamplerState *m_pSamplerStateStored11;

		ID3D11InputLayout *m_pInputLayout11;
		ID3D11Buffer *m_pVBScreenQuad11,

			// Sprite workaround
			*m_pSpriteBuffer11;
		UINT m_SpriteBufferBytes11;
		vector<SpriteVertex> m_SpriteVertices;

		vector<Dialog *> m_Dialogs;            // Dialogs registered

	protected:
		// D3D11 specific
		ID3D11Device *m_pd3d11Device;
		ID3D11DeviceContext *m_pd3d11DeviceContext;
		HRESULT CreateTexture11(_In_ UINT index);

		vector<TextureNode *> m_TextureCache;   // Shared textures
		vector<FontNode *> m_FontCache;         // Shared fonts

		unique_ptr<Shaders> shader = make_unique<Shaders>();
	};


	//-----------------------------------------------------------------------------
	// Base class for controls
	//-----------------------------------------------------------------------------
	class Control
	{
	public:
		Control(_In_opt_ Dialog *pDialog = nullptr) noexcept;
		virtual ~Control();

		virtual HRESULT OnInit() { return S_OK; }
		virtual void Refresh();
		virtual void Render(_In_ float fElapsedTime) { UNREFERENCED_PARAMETER(fElapsedTime); }

		// Windows message handler
		virtual bool MsgProc(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
		{
			UNREFERENCED_PARAMETER(uMsg);
			UNREFERENCED_PARAMETER(wParam);
			UNREFERENCED_PARAMETER(lParam);
			return false;
		}

		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
		{
			UNREFERENCED_PARAMETER(uMsg);
			UNREFERENCED_PARAMETER(wParam);
			UNREFERENCED_PARAMETER(lParam);
			return false;
		}
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam)
		{
			UNREFERENCED_PARAMETER(uMsg);
			UNREFERENCED_PARAMETER(pt);
			UNREFERENCED_PARAMETER(wParam);
			UNREFERENCED_PARAMETER(lParam);
			return false;
		}

		virtual bool CanHaveFocus() { return false; }
		virtual void OnFocusIn() { m_bHasFocus = true; }
		virtual void OnFocusOut() { m_bHasFocus = false; }
		virtual void OnMouseEnter() { m_bMouseOver = true; }
		virtual void OnMouseLeave() { m_bMouseOver = false; }
		virtual void OnHotkey() { }

		virtual bool ContainsPoint(_In_ const POINT &pt) { return PtInRect(&m_rcBoundingBox, pt) != 0; }

		virtual void SetEnabled(_In_ bool bEnabled) { m_bEnabled = bEnabled; }
		virtual bool GetEnabled() const { return m_bEnabled; }
		virtual void SetVisible(_In_ bool bVisible) { m_bVisible = bVisible; }
		virtual bool GetVisible() const { return m_bVisible; }

		UINT GetType() const { return m_Type; }

		int GetID() const { return m_ID; }
		void SetID(_In_ int ID) { m_ID = ID; }

		void SetLocation(_In_ int x, _In_ int y)
		{
			m_x = x;
			m_y = y;
			UpdateRects();
		}
		void SetSize(int width, int height)
		{
			m_width = width;
			m_height = height;
			UpdateRects();
		}

		void SetHotkey(_In_ UINT nHotkey) { m_nHotkey = nHotkey; }
		UINT GetHotkey() const { return m_nHotkey; }

		void SetUserData(_In_opt_ void* pUserData) { m_pUserData = pUserData; }
		void* GetUserData() const { return m_pUserData; }

		virtual void SetTextColor(_In_ DWORD Color);
		Element *GetElement(_In_ UINT iElement) const { return m_Elements[iElement]; }
		HRESULT SetElement(_In_ UINT iElement, _In_ Element *pElement);

		bool m_bVisible,                // Shown/hidden flag
			m_bMouseOver,              // Mouse pointer is above control
			m_bHasFocus,               // Control has input focus
			m_bIsDefault;              // Is the default control

		   // Size, scale, and positioning members
		int m_x, m_y, m_width, m_height;

		// These members are set by the container
		Dialog *m_pDialog;    // Parent container
		UINT m_Index;              // Index within the control list

		vector<Element *> m_Elements;  // All display elements
	protected:
		virtual void UpdateRects();

		int m_ID;                 // ID number
		CONTROL_TYPE m_Type;  // Control type, set once in constructor  
		UINT m_nHotkey;            // Virtual key code for this control's hotkey
		void* m_pUserData;         // Data associated with this control that is set by user.

		bool m_bEnabled;           // Enabled/disabled flag

		RECT m_rcBoundingBox;      // Rectangle defining the active region of the control
	};

	//-----------------------------------------------------------------------------
	// Contains all the display information for a given control type
	//-----------------------------------------------------------------------------
	struct ElementHolder
	{
		UINT nControlType, iElement;

		Element element;
	};

	//-----------------------------------------------------------------------------
	// Static control
	//-----------------------------------------------------------------------------
	class Static: public Control
	{
	public:
		Static(_In_opt_ Dialog *pDialog = nullptr) noexcept;

		virtual void Render(_In_ float fElapsedTime) override;
		virtual bool ContainsPoint(_In_ const POINT &pt) override
		{
			UNREFERENCED_PARAMETER(pt);
			return false;
		}

		HRESULT GetTextCopy(_Out_writes_(bufferCount) LPCWSTR strDest, _In_ UINT bufferCount) const;
		LPCWSTR GetText() const { return m_strText; }
		HRESULT SetText(_In_z_ LPCWSTR strText);

	protected:
		WCHAR m_strText[MAX_PATH];      // Window text  
	};


	//-----------------------------------------------------------------------------
	// Button control
	//-----------------------------------------------------------------------------
	class Button: public Static
	{
	public:
		Button(_In_opt_ Dialog *pDialog = nullptr) noexcept;

		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual void OnHotkey() override
		{
			if (m_pDialog->IsKeyboardInputEnabled()) m_pDialog->RequestFocus(this);
			m_pDialog->SendEvent(EVENT_BUTTON_CLICKED, true, this);
		}

		virtual bool ContainsPoint(_In_ const POINT &pt) override
		{
			return PtInRect(&m_rcBoundingBox, pt) != 0;
		}
		virtual bool CanHaveFocus() override
		{
			return (m_bVisible && m_bEnabled);
		}

		virtual void Render(_In_ float fElapsedTime) override;

		//********
			// Get Press Bool
		auto getPressed() { return m_bPressed; }
	protected:
		bool m_bPressed;
	};


	//-----------------------------------------------------------------------------
	// CheckBox control
	//-----------------------------------------------------------------------------
	class CheckBox: public Button
	{
	public:
		CheckBox(_In_opt_ Dialog *pDialog = nullptr) noexcept;

		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual void OnHotkey() override
		{
			if (m_pDialog->IsKeyboardInputEnabled()) m_pDialog->RequestFocus(this);
			SetCheckedInternal(!m_bChecked, true);
		}

		virtual bool ContainsPoint(_In_ const POINT &pt) override;
		virtual void UpdateRects() override;

		virtual void Render(_In_ float fElapsedTime) override;

		bool GetChecked() const { return m_bChecked; }
		void SetChecked(_In_ bool bChecked) { SetCheckedInternal(bChecked, false); }
	protected:
		virtual void SetCheckedInternal(_In_ bool bChecked, _In_ bool bFromInput);

		bool m_bChecked;
		RECT m_rcButton, m_rcText;
	};


	//-----------------------------------------------------------------------------
	// RadioButton control
	//-----------------------------------------------------------------------------
	class RadioButton: public CheckBox
	{
	public:
		RadioButton(_In_opt_ Dialog *pDialog = nullptr) noexcept;

		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual void OnHotkey() override
		{
			if (m_pDialog->IsKeyboardInputEnabled()) m_pDialog->RequestFocus(this);
			SetCheckedInternal(true, true, true);
		}

		void SetChecked(_In_ bool bChecked, _In_ bool bClearGroup = true) { SetCheckedInternal(bChecked, bClearGroup, false); }
		void SetButtonGroup(_In_ UINT nButtonGroup) { m_nButtonGroup = nButtonGroup; }
		UINT GetButtonGroup() const { return m_nButtonGroup; }
	protected:
		virtual void SetCheckedInternal(_In_ bool bChecked, _In_ bool bClearGroup, _In_ bool bFromInput);
		UINT m_nButtonGroup;
	};


	//-----------------------------------------------------------------------------
	// Scrollbar control
	//-----------------------------------------------------------------------------
	class ScrollBar: public Control
	{
	public:
		ScrollBar(_In_opt_ Dialog *pDialog = nullptr) noexcept;
		virtual ~ScrollBar();

		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool MsgProc(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;

		virtual void Render(_In_ float fElapsedTime) override;
		virtual void UpdateRects() override;

		void SetTrackRange(_In_ int nStart, _In_ int nEnd);
		int GetTrackPos() const { return m_nPosition; }
		void SetTrackPos(_In_ int nPosition)
		{
			m_nPosition = nPosition;
			Cap();
			UpdateThumbRect();
		}
		int GetPageSize() const { return m_nPageSize; }
		void SetPageSize(_In_ int nPageSize)
		{
			m_nPageSize = nPageSize;
			Cap();
			UpdateThumbRect();
		}

		void Scroll(_In_ int nDelta);    // Scroll by nDelta items (plus or minus)
		void ShowItem(_In_ int nIndex);  // Ensure that item nIndex is displayed, scroll if necessary
	protected:
		// ARROWSTATE indicates the state of the arrow buttons.
		// CLEAR            No arrow is down.
		// CLICKED_UP       Up arrow is clicked.
		// CLICKED_DOWN     Down arrow is clicked.
		// HELD_UP          Up arrow is held down for sustained period.
		// HELD_DOWN        Down arrow is held down for sustained period.
		enum ARROWSTATE
		{
			CLEAR,
			CLICKED_UP,
			CLICKED_DOWN,
			HELD_UP,
			HELD_DOWN
		};

		void UpdateThumbRect();
		void Cap();  // Clips position at boundaries. Ensures it stays within legal range.

		bool m_bShowThumb,
			m_bDrag;
		RECT m_rcUpButton,
			m_rcDownButton,
			m_rcTrack,
			m_rcThumb;
		int m_nPosition,  // Position of the first displayed item
			m_nPageSize,  // How many items are displayable in one page
			m_nStart,     // First item
			m_nEnd;       // The index after the last item
		POINT m_LastMouse;// Last mouse position
		ARROWSTATE m_Arrow; // State of the arrows
		double m_dArrowTS;  // Timestamp of last arrow event.
	};


	//-----------------------------------------------------------------------------
	// ListBox control
	//-----------------------------------------------------------------------------
	struct ListBoxItem
	{
		WCHAR strText[256];
		void* pData;

		RECT rcActive;
		bool bSelected;
	};

	class ListBox: public Control
	{
	public:
		ListBox(_In_opt_ Dialog *pDialog = nullptr) noexcept;
		virtual ~ListBox();

		virtual HRESULT OnInit() override
		{
			return m_pDialog->InitControl(&m_ScrollBar);
		}
		virtual bool CanHaveFocus() override
		{
			return (m_bVisible && m_bEnabled);
		}
		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)  override;
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam)  override;
		virtual bool MsgProc(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)  override;

		virtual void Render(_In_ float fElapsedTime) override;
		virtual void UpdateRects() override;

		DWORD GetStyle() const { return m_dwStyle; }
		size_t GetSize() const { return m_Items.size(); }
		void SetStyle(_In_ DWORD dwStyle) { m_dwStyle = dwStyle; }
		int GetScrollBarWidth() const { return m_nSBWidth; }
		void SetScrollBarWidth(_In_ int nWidth)
		{
			m_nSBWidth = nWidth;
			UpdateRects();
		}
		void SetBorder(_In_ int nBorder, _In_ int nMargin)
		{
			m_nBorder = nBorder;
			m_nMargin = nMargin;
		}
		HRESULT AddItem(_In_z_ LPCWSTR wszText, _In_opt_ void* pData);
		HRESULT InsertItem(_In_ int nIndex, _In_z_ LPCWSTR wszText, _In_opt_ void* pData);
		void    RemoveItem(_In_ int nIndex);
		void    RemoveAllItems();

		ListBoxItem *GetItem(_In_ int nIndex) const;
		int              GetSelectedIndex(_In_ int nPreviousSelected = -1) const;
		ListBoxItem *GetSelectedItem(_In_ int nPreviousSelected = -1) const
		{
			return GetItem(GetSelectedIndex(nPreviousSelected));
		}
		void             SelectItem(_In_ int nNewIndex);

		enum STYLE
		{
			MULTISELECTION = 1
		};

	protected:
		RECT m_rcText,     // Text rendering bound
			m_rcSelection; // Selection box bound
		ScrollBar m_ScrollBar;
		int m_nSBWidth,
			m_nBorder,
			m_nMargin,
			m_nTextHeight;  // Height of a single line of text
		DWORD m_dwStyle;    // List box style
		int m_nSelected,    // Index of the selected item for single selection list box
			m_nSelStart;    // Index of the item where selection starts (for handling multi-selection)
		bool m_bDrag;       // Whether the user is dragging the mouse to select

		vector<ListBoxItem *> m_Items;
	};


	//-----------------------------------------------------------------------------
	// ComboBox control
	//-----------------------------------------------------------------------------
	struct ComboBoxItem
	{
		WCHAR strText[256];
		void* pData;

		RECT rcActive;
		bool bVisible;
	};

	class ComboBox : public Button
	{
	public:
		ComboBox(_In_opt_ Dialog *pDialog = nullptr) noexcept;
		virtual ~ComboBox();

		virtual void SetTextColor(_In_ DWORD Color) override;
		virtual HRESULT OnInit() override
		{
			return m_pDialog->InitControl(&m_ScrollBar);
		}

		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual void OnHotkey() override;

		virtual bool CanHaveFocus() override
		{
			return (m_bVisible && m_bEnabled);
		}
		virtual void OnFocusOut() override;
		virtual void Render(_In_ float fElapsedTime) override;

		virtual void UpdateRects() override;

		HRESULT AddItem(_In_z_ LPCWSTR strText, _In_opt_ void* pData);
		void RemoveAllItems();
		void RemoveItem(_In_ UINT index);
		bool ContainsItem(_In_z_ LPCWSTR strText, _In_ UINT iStart = 0);
		int FindItem(_In_z_ LPCWSTR strText, _In_ UINT iStart = 0) const;
		void* GetItemData(_In_z_ LPCWSTR strText) const;
		void* GetItemData(_In_ int nIndex) const;
		void SetDropHeight(_In_ UINT nHeight)
		{
			m_nDropHeight = nHeight;
			UpdateRects();
		}
		int     GetScrollBarWidth() const { return m_nSBWidth; }
		void    SetScrollBarWidth(_In_ int nWidth)
		{
			m_nSBWidth = nWidth;
			UpdateRects();
		}

		int GetSelectedIndex() const { return m_iSelected; }
		void* GetSelectedData() const;
		ComboBoxItem *GetSelectedItem() const;

		UINT GetNumItems() { return static_cast<UINT>(m_Items.size()); }
		ComboBoxItem *GetItem(_In_ UINT index) { return m_Items[index]; }

		HRESULT SetSelectedByIndex(_In_ UINT index);
		HRESULT SetSelectedByText(_In_z_ LPCWSTR strText);
		HRESULT SetSelectedByData(_In_ void* pData);

	protected:
		int m_iSelected,
			m_iFocused,
			m_nDropHeight,
			m_nSBWidth;
		ScrollBar m_ScrollBar;

		bool m_bOpened;

		RECT m_rcText,
			m_rcButton,
			m_rcDropdown,
			m_rcDropdownText;

		vector<ComboBoxItem *> m_Items;
	};


	//-----------------------------------------------------------------------------
	// Slider control
	//-----------------------------------------------------------------------------
	class Slider : public Control
	{
	public:
		Slider(_In_opt_ Dialog *pDialog = nullptr) noexcept;

		virtual bool ContainsPoint(_In_ const POINT &pt) override;
		virtual bool CanHaveFocus() override
		{
			return (m_bVisible && m_bEnabled);
		}
		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam) override;

		virtual void UpdateRects() override;

		virtual void Render(_In_ float fElapsedTime) override;

		void SetValue(int nValue) { SetValueInternal(nValue, false); }
		int GetValue() const { return m_nValue; }

		void GetRange(_Out_ int& nMin, _Out_ int& nMax) const
		{
			nMin = m_nMin;
			nMax = m_nMax;
		}
		void SetRange(_In_ int nMin, _In_ int nMax);

	protected:
		void SetValueInternal(_In_ int nValue, _In_ bool bFromInput);
		int ValueFromPos(_In_ int x);

		int m_nValue,

			m_nMin,
			m_nMax,

			m_nDragX,      // Mouse position at start of drag
			m_nDragOffset, // Drag offset from the center of the button
			m_nButtonX;

		bool m_bPressed;
		RECT m_rcButton;
	};


	//-----------------------------------------------------------------------------
	// CUniBuffer class for the edit control
	//-----------------------------------------------------------------------------
	class UniBuffer
	{
	public:
		UniBuffer(_In_ int nInitialSize = 1) noexcept;
		~UniBuffer();

		size_t GetBufferSize() const { return m_nBufferSize; }
		bool SetBufferSize(_In_ int nSize);
		int GetTextSize() const { return (int)wcslen(m_pwszBuffer); }
		LPCWSTR GetBuffer() const { return m_pwszBuffer; }
		const WCHAR& operator[](_In_ int n) const { return m_pwszBuffer[n]; }
		WCHAR& operator[](_In_ int n);
		FontNode *GetFontNode() const { return m_pFontNode; }
		void SetFontNode(_In_opt_ FontNode *pFontNode) { m_pFontNode = pFontNode; }
		void Clear();

		bool InsertChar(_In_ int nIndex, _In_ WCHAR wChar);
		// Inserts the char at specified index. If nIndex == -1, insert to the end.

		bool RemoveChar(_In_ int nIndex);
		// Removes the char at specified index. If nIndex == -1, remove the last char.

		bool InsertString(_In_ int nIndex, _In_z_ LPCWSTR pStr, _In_ int nCount = -1);
		// Inserts the first nCount characters of the string pStr at specified index.  If nCount == -1, the entire string is inserted. If nIndex == -1, insert to the end.

		bool SetText(_In_z_ LPCWSTR wszText);

		// Uniscribe
		bool CPtoX(_In_ int nCP, _In_ bool bTrail, _Out_ int *pX);
		bool XtoCP(_In_ int nX, _Out_ int *pCP, _Out_ int *pnTrail);
		void GetPriorItemPos(_In_ int nCP, _Out_ int *pPrior);
		void GetNextItemPos(_In_ int nCP, _Out_ int* pPrior);

	private:
		HRESULT Analyse();      // Uniscribe -- Analyse() analyses the string in the buffer

		WCHAR* m_pwszBuffer;    // Buffer to hold text
		int m_nBufferSize;   // Size of the buffer allocated, in characters

		// Uniscribe-specific
		FontNode *m_pFontNode;          // Font node for the font that this buffer uses
		bool m_bAnalyseRequired;            // True if the string has changed since last analysis.
		SCRIPT_STRING_ANALYSIS m_Analysis;  // Analysis for the current string
	};


	//-----------------------------------------------------------------------------
	// EditBox control
	//-----------------------------------------------------------------------------
	class EditBox: public Control
	{
	public:
		EditBox(_In_opt_ Dialog *pDialog = nullptr) noexcept;
		virtual ~EditBox();

		virtual bool HandleKeyboard(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool HandleMouse(_In_ UINT uMsg, _In_ const POINT &pt, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual bool MsgProc(_In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) override;
		virtual void UpdateRects() override;
		virtual bool CanHaveFocus() override { return (m_bVisible && m_bEnabled); }
		virtual void Render(_In_ float fElapsedTime) override;
		virtual void OnFocusIn() override;

		void SetText(_In_z_ LPCWSTR wszText, _In_ bool bSelected = false);
		LPCWSTR GetText() const { return m_Buffer.GetBuffer(); }
		size_t GetTextLength() const { return m_Buffer.GetTextSize(); }  // Returns text length in chars excluding nul.
		HRESULT GetTextCopy(_Out_writes_(bufferCount) LPCWSTR strDest, _In_ UINT bufferCount) const;
		void ClearText();

		virtual void SetTextColor(_In_ DWORD Color) override { m_TextColor = Color; }  // Text color
		void SetSelectedTextColor(_In_ DWORD Color) { m_SelTextColor = Color; }  // Selected text color
		void SetSelectedBackColor(_In_ DWORD Color) { m_SelBkColor = Color; }  // Selected background color
		void SetCaretColor(_In_ DWORD Color) { m_CaretColor = Color; }  // Caret color
		void SetBorderWidth(_In_ int nBorder)
		{
			m_nBorder = nBorder;
			UpdateRects();
		}  // Border of the window
		void SetSpacing(_In_ int nSpacing)
		{
			m_nSpacing = nSpacing;
			UpdateRects();
		}
		void ParseFloatArray(_In_reads_(nCount) float *pNumbers, _In_ int nCount);
		void SetTextFloatArray(_In_reads_(nCount) const float *pNumbers, _In_ int nCount);

	protected:
		void PlaceCaret(_In_ int nCP);
		void DeleteSelectionText();
		void ResetCaretBlink();
		void CopyToClipboard();
		void PasteFromClipboard();

		UniBuffer m_Buffer;     // Buffer to hold text
		int m_nBorder,      // Border of the window
			m_nSpacing;     // Spacing between the text and the edge of border
		RECT m_rcText,       // Bounding rectangle for the text
			m_rcRender[9];  // Convenient rectangles for rendering elements
		double m_dfBlink,      // Caret blink time in milliseconds
			m_dfLastBlink;  // Last timestamp of caret blink
		bool m_bCaretOn,     // Flag to indicate whether caret is currently visible
			m_bInsertMode;  // If true, control is in insert mode. Else, overwrite mode.
		int m_nCaret,       // Caret position, in characters
			m_nSelStart,    // Starting position of the selection. The caret marks the end.
			m_nFirstVisible;// First visible character in the edit control
		DWORD m_TextColor,    // Text color
			m_SelTextColor, // Selected text color
			m_SelBkColor,   // Selected background color
			m_CaretColor;   // Caret color

		   // Mouse-specific
		bool m_bMouseDrag;       // True to indicate drag in progress

		// Static
		static bool s_bHideCaret;   // If true, we don't render the caret.
	};

	//-----------------------------------------------------------------------------
	HRESULT InitFont11(_In_ ID3D11Device *pd3d11Device, _In_ ID3D11InputLayout *pInputLayout, LPCWSTR UIPath);
	void EndFont11();
	void EndText11(_In_ ID3D11Device *pd3dDevice, _In_ ID3D11DeviceContext *pd3d11DeviceContext);

	void BeginText11();

	void DrawText11(_In_ ID3D11Device *pd3dDevice, _In_ ID3D11DeviceContext *pd3d11DeviceContext,
		_In_z_ LPCWSTR strText, _In_ const RECT &rcScreen, _In_ Vector4 vFontColor,
		_In_ float fBBWidth, _In_ float fBBHeight, _In_ bool bCenter);
	HRESULT WINAPI CreateGUITextureFromInternalArray(_Outptr_ ID3D11Texture2D **ppTexture);
}