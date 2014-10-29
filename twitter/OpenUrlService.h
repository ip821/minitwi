#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"

using namespace ATL;

class ATL_NO_VTABLE COpenUrlService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<COpenUrlService, &CLSID_FormsService>,
	public IOpenUrlService,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public ITimelineControlEventSink,
	public ITabbedControlEventSink
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
		COM_INTERFACE_ENTRY(ITabbedControlEventSink)
	END_COM_MAP()

private:
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IWindowService> m_pWindowService;
	CComPtr<ITimelineControl> m_pTimelineControl;
	CComQIPtr<ITabbedControl> m_pTabbedControl;
	DWORD m_dwAdviceTimelineControl = 0;
	DWORD m_dwAdviceTabbedControl = 0;
	DWORD m_dwAdviceTimelineControlInUserInfoControl = 0;
	DWORD m_dwAdviceTimelineControlInTwitViewControl = 0;

private:
	STDMETHOD(OpenUserInfo)(IVariantObject* pVariantObject);
	STDMETHOD(OpenTwitView)(IVariantObject* pVariantObject);
	STDMETHOD(CopyImages)(IControl* pControl);

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	METHOD_EMPTY(STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject));
	STDMETHOD(OnColumnClick)(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject);
	STDMETHOD(OnItemDoubleClick)(IVariantObject* pVariantObject);

	STDMETHOD(OnActivate)(IControl *pControl);
	STDMETHOD(OnDeactivate)(IControl *pControl);
	STDMETHOD(OnClose)(IControl *pControl);
};

OBJECT_ENTRY_AUTO(__uuidof(OpenUrlService), COpenUrlService)