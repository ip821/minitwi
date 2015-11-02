// ShowMoreSupportService.h : Declaration of the CShowMoreSupportService

#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"
#include "twconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CShowMoreSupportService

class ATL_NO_VTABLE CShowMoreSupportService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CShowMoreSupportService, &CLSID_ShowMoreSupportService>,
	public IThreadServiceEventSink,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public ITimelineControlEventSink
{
public:
	CShowMoreSupportService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CShowMoreSupportService)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(ITimelineControlEventSink)
	END_COM_MAP()

private:
	CComPtr<ISettings> m_pSettings;
	CComPtr<IThreadService> m_pThreadServiceUpdateService;
	CComPtr<IThreadService> m_pThreadServiceShowMoreService;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComQIPtr<ITimelineControl> m_pTimelineControl;
	DWORD m_dwAdviceThreadServiceUpdateService = 0;
	DWORD m_dwAdviceThreadServiceShowMoreService = 0;
	DWORD m_dwAdviceTimelineControl = 0;
	BOOL m_bShowMoreRunning = 0;

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnColumnClick)(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject);
	METHOD_EMPTY(STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject));
	METHOD_EMPTY(STDMETHOD(OnItemDoubleClick)(IVariantObject* pVariantObject));
};

OBJECT_ENTRY_AUTO(__uuidof(ShowMoreSupportService), CShowMoreSupportService)
