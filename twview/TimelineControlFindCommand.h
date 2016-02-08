// CopyCommand.h : Declaration of the CTimelineControlFindCommand

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace IP;

// CTimelineControlFindCommand

class ATL_NO_VTABLE CTimelineControlFindCommand :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CTimelineControlFindCommand, &CLSID_TimelineControlFindCommand>,
    public ICommand2,
    public IAcceleratorSupport,
    public IInitializeWithControlImpl,
    public IPluginSupportNotifications
{
public:
    CTimelineControlFindCommand()
    {
    }

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CTimelineControlFindCommand)
        COM_INTERFACE_ENTRY(ICommand)
        COM_INTERFACE_ENTRY(ICommand2)
        COM_INTERFACE_ENTRY(IAcceleratorSupport)
        COM_INTERFACE_ENTRY(IInitializeWithControl)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications)
    END_COM_MAP()

public:

    STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText);
    STDMETHOD(InstallMenu)(IMenu* pMenu);
    STDMETHOD(Invoke)(REFGUID guidCommand);
    STDMETHOD(GetAccelerator)(REFGUID guidCommand, TACCEL *pAccel);

    STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
    STDMETHOD(OnShutdown)();

    STDMETHOD(GetEnabled)(REFGUID guidCommand, BOOL *pbEnabled);
    METHOD_EMPTY(STDMETHOD(GetHBITMAP)(REFGUID guidCommand, HBITMAP *pHBITMAP));
    METHOD_EMPTY(STDMETHOD(InstallToolbar)(IToolbar *pToolbar));
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineControlFindCommand), CTimelineControlFindCommand)
