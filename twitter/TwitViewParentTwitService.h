#pragma once

using namespace ATL;
using namespace std;

#include "twitter_i.h"
#include "..\model-libs\asyncsvc\asyncsvc_contract_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"

class ATL_NO_VTABLE CTwitViewParentTwitService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTwitViewParentTwitService, &CLSID_TwitViewParentTwitService>,
	public IPluginSupportNotifications,
	public IThreadServiceEventSink,
	public IInitializeWithSettings,
	public ITwitViewParentTwitService,
	public IInitializeWithControlImpl
{
public:
	CTwitViewParentTwitService()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTwitViewParentTwitService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(ITwitViewParentTwitService)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IThreadService> m_pThreadService;
	CComPtr<ISettings> m_pSettings;
	CComPtr<ITimelineControl> m_pTimelineControl;

	DWORD m_dwAdvice = 0;
	mutex m_mutex;

public:
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	STDMETHOD(OnRun)(IVariantObject *pResult);
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(SetTimelineControl)(ITimelineControl* pTimelineControl);
};

OBJECT_ENTRY_AUTO(__uuidof(TwitViewParentTwitService), CTwitViewParentTwitService)