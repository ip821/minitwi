// DownloadService.h : Declaration of the CDownloadService

#pragma once
#include "resource.h"       // main symbols
#include "twmdl_i.h"
#include "asyncmdl_contract_i.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CDownloadService

class ATL_NO_VTABLE CDownloadService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDownloadService, &CLSID_DownloadService>,
	public IDownloadService,
	public IThreadServiceEventSink,
	public IPluginSupportNotifications,
	public IConnectionPointContainerImpl<CDownloadService>,
	public IConnectionPointImpl<CDownloadService, &__uuidof(IDownloadServiceEventSink)>
{
public:
	CDownloadService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_DOWNLOADSERVICE)

	BEGIN_COM_MAP(CDownloadService)
		COM_INTERFACE_ENTRY(IDownloadService)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
	END_COM_MAP()

	BEGIN_CONNECTION_POINT_MAP(CDownloadService)
		CONNECTION_POINT_ENTRY(__uuidof(IDownloadServiceEventSink))
	END_CONNECTION_POINT_MAP()

private:
	CComPtr<IThreadPoolService> m_pThreadPoolService;
	DWORD m_dwAdvice = 0;
	unordered_set<wstring> m_urls;
	boost::mutex m_mutex;

	HRESULT Fire_OnDownloadComplete(IVariantObject *pResult);
	static size_t WriteCallback(char *ptr, size_t size, size_t nmemb, void *userdata);

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(AddDownload)(IVariantObject* pVariantObject);

	STDMETHOD(SuspendDownloads)();
	STDMETHOD(ResumeDownloads)();
};

OBJECT_ENTRY_AUTO(__uuidof(DownloadService), CDownloadService)
