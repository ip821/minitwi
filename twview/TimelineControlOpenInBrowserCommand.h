#pragma once

#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace IP;

// CTimelineControlOpenInBrowserCommand

class ATL_NO_VTABLE CTimelineControlOpenInBrowserCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineControlOpenInBrowserCommand, &CLSID_TimelineControlCopyCommand>,
	public ICommand,
	public IAcceleratorSupport,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IInitializeWithColumnName,
	public IPluginSupportNotifications
{
public:
	CTimelineControlOpenInBrowserCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTimelineControlOpenInBrowserCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
		COM_INTERFACE_ENTRY(IAcceleratorSupport)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithColumnName)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

private:
	CComPtr<IVariantObject> m_pVariantObject;
	CString m_strColumnName;
public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText);
	STDMETHOD(InstallMenu)(IMenu* pMenu);
	STDMETHOD(Invoke)(REFGUID guidCommand);
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
	STDMETHOD(SetColumnName)(LPCTSTR lpszColumnName);
	STDMETHOD(GetAccelerator)(REFGUID guidCommand, TACCEL *pAccel);
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineControlOpenInBrowserCommand), CTimelineControlOpenInBrowserCommand)
