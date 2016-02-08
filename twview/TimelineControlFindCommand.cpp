#include "stdafx.h"
#include "TimelineControlFindCommand.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

STDMETHODIMP CTimelineControlFindCommand::OnInitialized(IServiceProvider* pServiceProvider)
{
    return S_OK;
}

STDMETHODIMP CTimelineControlFindCommand::OnShutdown()
{
    RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
    return S_OK;
}

STDMETHODIMP CTimelineControlFindCommand::GetEnabled(REFGUID guidCommand, BOOL *pbEnabled)
{
    CHECK_E_POINTER(pbEnabled);
    if (IsEqualGUID(guidCommand, COMMAND_FIND))
    {
        *pbEnabled = TRUE;
    }
    return S_OK;
}

STDMETHODIMP CTimelineControlFindCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
    UNREFERENCED_PARAMETER(guidCommand);
    CHECK_E_POINTER(bstrText);
    if (guidCommand == COMMAND_FIND)
        *bstrText = CComBSTR(L"Find\tCtrl+F").Detach();
    return S_OK;
}

STDMETHODIMP CTimelineControlFindCommand::InstallMenu(IMenu* pMenu)
{
    CHECK_E_POINTER(pMenu);
    RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_FIND, this));
    return S_OK;
}

STDMETHODIMP CTimelineControlFindCommand::Invoke(REFGUID guidCommand)
{
    if (IsEqualGUID(guidCommand, COMMAND_FIND))
    {
        CComQIPtr<ITimelineControl> m_pTimelineControl = m_pControl;
        RETURN_IF_FAILED(m_pTimelineControl->ToggleSearch());
    }
    return S_OK;
}

STDMETHODIMP CTimelineControlFindCommand::GetAccelerator(REFGUID guidCommand, TACCEL *pAccel)
{
    CHECK_E_POINTER(pAccel);
    if (guidCommand == COMMAND_FIND)
        *pAccel = { FVIRTKEY | FCONTROL, 0x46, 0 }; //CTRL-F
    return S_OK;
}