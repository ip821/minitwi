#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"

using namespace ATL;
using namespace IP;

class ATL_NO_VTABLE COpenUrlService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COpenUrlService, &CLSID_FormsService>,
	public IOpenUrlService,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public ITimelineControlEventSink,
	public IDownloadServiceEventSink
{
public:

	COpenUrlService()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(COpenUrlService)
		COM_INTERFACE_ENTRY(IOpenUrlService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ITimelineControlEventSink)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IWindowService> m_pWindowService;
	CComPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<IFormsService> m_pFormsService;
	CComPtr<IFormManager> m_pFormManager;
	CComPtr<IDownloadService> m_pDownloadService;

	DWORD m_dwAdviceTimelineControl = 0;
	DWORD m_dwAdviceDownloadService = 0;

	STDMETHOD(OpenTwitViewForm)(IVariantObject* pVariantObject);
	STDMETHOD(OpenUserInfoForm)(IVariantObject* pVariantObject);

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	METHOD_EMPTY(STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject));
	STDMETHOD(OnColumnClick)(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject);
	STDMETHOD(OnItemDoubleClick)(IVariantObject* pVariantObject);

	STDMETHOD(OnDownloadComplete)(IVariantObject *pResult);
};

OBJECT_ENTRY_AUTO(__uuidof(OpenUrlService), COpenUrlService)