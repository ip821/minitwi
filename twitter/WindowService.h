// WindowService.h : Declaration of the CWindowService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;
using namespace std;

// CWindowService

class ATL_NO_VTABLE CWindowService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CWindowService, &CLSID_WindowService>,
	public IWindowService,
	public IPluginSupportNotifications,
	public IWindowEventSink
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
	END_COM_MAP()

private:
	struct WindowData
	{
		CComPtr<IWindow> m_pWindow;
		DWORD m_dwAdvice = 0;
	};

	CComPtr<IServiceProvider> m_pServiceProvider;
	map<HWND, WindowData> m_windows;

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OpenWindow)(HWND hWndParent, REFCLSID clsid, IVariantObject* pVariantObject);

	STDMETHOD(OnClosed)(HWND hWndSelf);
};

OBJECT_ENTRY_AUTO(__uuidof(WindowService), CWindowService)
