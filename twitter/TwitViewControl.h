#pragma once

#include "twitter_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "asyncsvc_contract_i.h"
#include "BaseTimeLineControl.h"

using namespace ATL;
using namespace std;

class ATL_NO_VTABLE CTwitViewControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTwitViewControl, &CLSID_HomeTimeLineControl>,
	public CWindowImpl<CTwitViewControl>,
	public CBaseTimeLineControl<CTwitViewControl>,
	public ITwitViewControl,
	public IInitializeWithVariantObject
{
public:
	CTwitViewControl()
	{
	}

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP(CTwitViewControl)
		COM_INTERFACE_ENTRY(ITwitViewControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IServiceProviderSupport)
		COM_INTERFACE_ENTRY(ITimelineControlSupport)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
	END_COM_MAP()

	BEGIN_MSG_MAP(CTwitViewControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		bHandled = TRUE;
		lResult = OnMessage(uMsg, wParam, lParam, bHandled);
		if (bHandled)
			return TRUE;
	END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		if (m_hWnd)
			DestroyWindow();
	}

private:

public:
	METHOD_EMPTY(STDMETHOD(GetText)(BSTR* pbstr));
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(TwitViewControl), CTwitViewControl)