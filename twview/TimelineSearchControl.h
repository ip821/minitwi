// SettingsControl.h : Declaration of the CTimelineSearchControl

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "asyncmdl_contract_i.h"
#include "..\twiconn\twconn_contract_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CTimelineSearchControl

class ATL_NO_VTABLE CTimelineSearchControl :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CTimelineSearchControl, &CLSID_SettingsControl>,
    public CAxDialogImpl<CTimelineSearchControl>,
    public CDialogResize<CTimelineSearchControl>,
    public IPluginSupportNotifications,
    public IInitializeWithControlImpl,
    public IThemeSupport,
    public ITimelineSearchControl,
    public IConnectionPointContainerImpl<CTimelineSearchControl>,
    public IConnectionPointImpl<CTimelineSearchControl, &__uuidof(ITimelineSearchControlEventSink)>
{
public:
    CTimelineSearchControl()
    {
    }

    DECLARE_NO_REGISTRY()

    BEGIN_COM_MAP(CTimelineSearchControl)
        COM_INTERFACE_ENTRY(IControl)
        COM_INTERFACE_ENTRY(ITimelineSearchControl)
        COM_INTERFACE_ENTRY(IThemeSupport)
        COM_INTERFACE_ENTRY(IPluginSupportNotifications)
        COM_INTERFACE_ENTRY(IInitializeWithControl)
    END_COM_MAP()

    BEGIN_CONNECTION_POINT_MAP(CTimelineSearchControl)
        CONNECTION_POINT_ENTRY(__uuidof(ITimelineSearchControlEventSink))
    END_CONNECTION_POINT_MAP()

    BEGIN_DLGRESIZE_MAP(CTimelineSearchControl)
        DLGRESIZE_CONTROL(IDC_EDITTEXT, DLSZ_SIZE_X)
        DLGRESIZE_CONTROL(IDC_BUTTONGO, DLSZ_MOVE_X)
    END_DLGRESIZE_MAP()

    BEGIN_MSG_MAP(CTimelineSearchControl)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_HANDLER(IDC_BUTTONGO, BN_CLICKED, OnClickedGo)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        CHAIN_MSG_MAP(CDialogResize<CTimelineSearchControl>)
    END_MSG_MAP()

private:
    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnClickedGo(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    CComPtr<ITheme> m_pTheme;
    CComPtr<ISkinCommonControl> m_pSkinCommonControl;

    CEdit m_editText;
    CButton m_buttonGo;
    BOOL m_bActive = FALSE;

    void EnableControls(BOOL bEnable);
    STDMETHOD(DoSearch)();
    HRESULT Fire_OnSearch(BSTR bstrSearchPattern);

public:
    enum { IDD = IDD_TIMELINE_SEARCHCONTROL };

    STDMETHOD(GetHWND)(HWND *hWnd);
    STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
    STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

    STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
    STDMETHOD(OnShutdown)();

    STDMETHOD(SetTheme)(ITheme* pTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineSearchControl), CTimelineSearchControl)
