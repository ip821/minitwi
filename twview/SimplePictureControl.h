// SimplePictureControl.h : Declaration of the CSimplePictureControl

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;

// CSimplePictureControl

class ATL_NO_VTABLE CSimplePictureControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSimplePictureControl, &CLSID_SimplePictureControl>,
	public CWindowImpl<CSimplePictureControl>,
	public IControl,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications,
	public IAnimationServiceEventSink,
	public IThemeSupport
{
public:
	DECLARE_WND_CLASS(L"SimplePictureControl")
	CSimplePictureControl();
	~CSimplePictureControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSimplePictureControl)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IAnimationServiceEventSink)
		COM_INTERFACE_ENTRY(IThemeSupport)
	END_COM_MAP()

	BEGIN_MSG_MAP(CSimplePictureControl)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnForwardMessage)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnForwardMessage)
	END_MSG_MAP()

private:
 	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IAnimationService> m_pAnimationService;
	CComPtr<ITheme> m_pTheme;
	CComBSTR m_bstrUrl;

	CString m_strLastErrorMsg;

	const DWORD STEPS = 25;
	DWORD m_dwAdviceAnimationService = 0;

	HRESULT Fire_OnClosed(HWND hWnd);

	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	STDMETHOD(GetHWND)(HWND* phWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *bResult);

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnAnimationStep)(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep);

	STDMETHOD(SetTheme)(ITheme *pTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(SimplePictureControl), CSimplePictureControl)
