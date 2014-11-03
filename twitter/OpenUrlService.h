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
	public IFormManagerEventSink,
	public IMsgFilter
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
		COM_INTERFACE_ENTRY(IFormManagerEventSink)
		COM_INTERFACE_ENTRY(IMsgFilter)
	END_COM_MAP()

private:
	CComQIPtr<IMainWindow> m_pMainWindow;
	CComPtr<IMessageLoop> m_pMessageLoop;
	CComPtr<IServiceProvider> m_pServiceProvider;
	CComPtr<IWindowService> m_pWindowService;
	CComPtr<ITimelineControl> m_pHomeTimelineControl;
	CComPtr<ITimelineControl> m_pSearchTimelineControl;
	CComPtr<IFormsService> m_pFormsService;
	CComPtr<IFormManager> m_pFormManager;
	DWORD m_dwAdviceHomeTimelineControl = 0;
	DWORD m_dwAdviceSearchTimelineControl = 0;
	DWORD m_dwAdviceFormManager = 0;
	DWORD m_dwAdviceTimelineControlInUserInfoControl = 0;
	DWORD m_dwAdviceTimelineControlInTwitViewControl = 0;

	STDMETHOD(OpenTwitViewForm)(IVariantObject* pVariantObject);
	STDMETHOD(OpenUserInfoForm)(IVariantObject* pVariantObject);

private:

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	METHOD_EMPTY(STDMETHOD(OnItemRemoved)(IVariantObject *pItemObject));
	STDMETHOD(OnColumnClick)(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject);
	STDMETHOD(OnItemDoubleClick)(IVariantObject* pVariantObject);

	STDMETHOD(OnActivate)(IControl *pControl);
	STDMETHOD(OnDeactivate)(IControl *pControl);

	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *bResult);
};

OBJECT_ENTRY_AUTO(__uuidof(OpenUrlService), COpenUrlService)