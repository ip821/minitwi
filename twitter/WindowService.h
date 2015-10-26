// WindowService.h : Declaration of the CWindowService

#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

// CWindowService

class ATL_NO_VTABLE CWindowService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWindowService, &CLSID_WindowService>,
	public IWindowService,
	public IInitializeWithControlImpl,
	public IPluginSupportNotifications,
	public IWindowEventSink,
	public IInitializeWithSettings
{
public:
	CWindowService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_WINDOWSERVICE)

	BEGIN_COM_MAP(CWindowService)
		COM_INTERFACE_ENTRY(IWindowService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IWindowEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
	END_COM_MAP()

private:
	struct WindowData
	{
		CComPtr<IWindow> m_pWindow;
		DWORD m_dwAdvice = 0;
	};

	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<ISettings> m_pSettings;
	map<HWND, WindowData> m_windows;

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OpenWindow)(HWND hWndParent, REFCLSID clsid, IVariantObject* pVariantObject);

	STDMETHOD(OnClosed)(HWND hWndSelf);
	STDMETHOD(Load)(ISettings* pSettings);
};

OBJECT_ENTRY_AUTO(__uuidof(WindowService), CWindowService)
