#pragma once

#include "twview_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "asyncmdl_contract_i.h"
#include "BaseTimeLineControl.h"

using namespace ATL;
using namespace std;

class ATL_NO_VTABLE CFollowingControl :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CFollowingControl, &CLSID_FollowingControl>,
    public CWindowImpl<CFollowingControl>,
    public CBaseTimeLineControl<CFollowingControl>,
    public IInitializeWithVariantObject,
    public IFollowingControl
{
public:
    CFollowingControl()
    {
    }

    DECLARE_NO_REGISTRY()
    BEGIN_COM_MAP(CFollowingControl)
        COM_INTERFACE_ENTRY(IControl)
        COM_INTERFACE_ENTRY(IControl2)
        COM_INTERFACE_ENTRY(IFollowingControl)
        COM_INTERFACE_ENTRY(IThemeSupport)
        COM_INTERFACE_ENTRY(IInitializeWithControl)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications2)
        COM_INTERFACE_ENTRY(IInitializeWithSettings)
        COM_INTERFACE_ENTRY(IServiceProviderSupport)
        COM_INTERFACE_ENTRY(ITimelineControlSupport)
        COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
    END_COM_MAP()

    BEGIN_MSG_MAP(CUserInfoControl)
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
    virtual HRESULT OnActivateInternal() override;
    virtual HRESULT ShuttingDown() override;

private:

public:
    METHOD_EMPTY(STDMETHOD(GetText)(BSTR* pbstr));
    STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
};

OBJECT_ENTRY_AUTO(__uuidof(FollowingControl), CFollowingControl)