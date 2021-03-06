#pragma once

#include "twview_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "asyncmdl_contract_i.h"
#include "BaseTimeLineControl.h"

using namespace ATL;
using namespace std;

class ATL_NO_VTABLE CHomeTimeLineControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CHomeTimeLineControl, &CLSID_HomeTimeLineControl>,
	public CWindowImpl<CHomeTimeLineControl>,
	public CBaseTimeLineControl<CHomeTimeLineControl>,
	public IHomeTimeLineControl
{
public:
	CHomeTimeLineControl()
	{
	}

	DECLARE_NO_REGISTRY()
	BEGIN_COM_MAP(CHomeTimeLineControl)
		COM_INTERFACE_ENTRY(IHomeTimeLineControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications2)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
		COM_INTERFACE_ENTRY(IServiceProviderSupport)
		COM_INTERFACE_ENTRY(ITimelineControlSupport)
	END_COM_MAP()

	BEGIN_MSG_MAP(CHomeTimeLineControl)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus);
        DEFAULT_MESSAGE_HANDLER(OnMessage)
    END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

protected:
	virtual HRESULT Initializing() override;
	virtual HRESULT Initialized() override;
	virtual HRESULT ShuttingDown() override;
private:

public:
	STDMETHOD(GetText)(BSTR* pbstr);
};

OBJECT_ENTRY_AUTO(__uuidof(HomeTimeLineControl), CHomeTimeLineControl)