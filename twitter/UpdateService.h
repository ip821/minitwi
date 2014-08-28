// UpdateService.h : Declaration of the CUpdateService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "asyncsvc_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace std;

// CUpdateService

class ATL_NO_VTABLE CUpdateService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUpdateService, &CLSID_UpdateService>,
	public IUpdateService,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IDownloadServiceEventSink,
	public IInitializeWithControlImpl
{
public:
	CUpdateService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_UPDATESERVICE)


	BEGIN_COM_MAP(CUpdateService)
		COM_INTERFACE_ENTRY(IUpdateService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

private:
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ITimerService> m_pTimerService;
	CComPtr<IDownloadService> m_pDownloadService;
	DWORD m_dwAdviceThreadService = 0;
	DWORD m_dwAdviceDownloadService = 0;
	BOOL m_bUpdateAvailable = TRUE;
	CString m_strUpdatePath;
	mutex m_mutex;

	void GetFolderPath(CString& strFolderPath);
	void GetSubDirs(CString strFolderPath, std::vector<std::wstring>& dirs);

public:
	static CString GetInstalledVersion();
	
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnDownloadComplete)(IVariantObject *pResult);

	STDMETHOD(IsUpdateAvailable)(BOOL* pbUpdateAvailable);
	STDMETHOD(RunUpdate)();
};

OBJECT_ENTRY_AUTO(__uuidof(UpdateService), CUpdateService)