// CopyCommand.h : Declaration of the CTimelineControlCopyCommand

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace IP;

// CTimelineControlCopyCommand

class ATL_NO_VTABLE CTimelineControlCopyCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineControlCopyCommand, &CLSID_TimelineControlCopyCommand>,
	public ICommand2,
	public IAcceleratorSupport,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications
{
public:
	CTimelineControlCopyCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTimelineControlCopyCommand)
		COM_INTERFACE_ENTRY(ICommand)
		COM_INTERFACE_ENTRY(ICommand2)
		COM_INTERFACE_ENTRY(IAcceleratorSupport)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

private:
	CComPtr<IVariantObject> m_pVariantObject;

    CString MakeTwitterUrl(BSTR bstrUserName, BSTR bstrTwitterId);
    CString MakeMediaString(CString& strUrl);
public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText);
	STDMETHOD(InstallMenu)(IMenu* pMenu);
	STDMETHOD(Invoke)(REFGUID guidCommand);
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
	STDMETHOD(GetAccelerator)(REFGUID guidCommand, TACCEL *pAccel);
	
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(GetEnabled)(REFGUID guidCommand, BOOL *pbEnabled);
	METHOD_EMPTY(STDMETHOD(GetHBITMAP)(REFGUID guidCommand, HBITMAP *pHBITMAP));
	METHOD_EMPTY(STDMETHOD(InstallToolbar)(IToolbar *pToolbar));
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineControlCopyCommand), CTimelineControlCopyCommand)
