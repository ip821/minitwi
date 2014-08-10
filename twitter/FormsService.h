// FormsService.h : Declaration of the CFormsService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CFormsService

class ATL_NO_VTABLE CFormsService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CFormsService, &CLSID_FormsService>,
	public IFormsService,
	public IPluginSupportNotifications,
	public IPluginSupportNotifications2,
	public IInitializeWithControlImpl
{
public:
	CFormsService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_FORMSSERVICE)

	BEGIN_COM_MAP(CFormsService)
		COM_INTERFACE_ENTRY(IFormsService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications2)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

public:

	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnInitializing)(IServiceProvider* pServiceProvider);
};

OBJECT_ENTRY_AUTO(__uuidof(FormsService), CFormsService)
