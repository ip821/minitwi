#pragma once

using namespace ATL;
using namespace std;

#include "twitter_i.h"
#include "..\model-libs\asyncsvc\asyncsvc_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"

class ATL_NO_VTABLE CTwitViewRepliesService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTwitViewRepliesService, &CLSID_TwitViewRepliesService>,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public ITwitViewRepliesService,
	public IInitializeWithVariantObject,
	public IMsgHandler,
	public IInitializeWithControlImpl
{
public:
	CTwitViewRepliesService()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTwitViewRepliesService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(ITwitViewRepliesService)
		COM_INTERFACE_ENTRY(ITimelineService)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<ITimelineControl> m_pTimelineControl;
	CComPtr<IVariantObject> m_pVariantObject;

	TIME_ZONE_INFORMATION m_tz;
	DWORD m_dwAdvice = 0;
	mutex m_mutex;

	bool m_bUpdating = false;
	int m_cAttemptCount = 0;
	const int MAX_ATTEMPT_COUNT = 10;
	CComBSTR m_bstrMaxId;
	bool m_bParentRetroived = false;

public:
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(SetTimelineControl)(ITimelineControl *pTimelineControl);

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);

	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult);
};

OBJECT_ENTRY_AUTO(__uuidof(TwitViewRepliesService), CTwitViewRepliesService)