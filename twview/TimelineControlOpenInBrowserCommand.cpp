#include "stdafx.h"
#include "TimelineControlOpenInBrowserCommand.h"
#include "TimelineControlCopyCommand.h"

#include "Plugins.h"
#include "..\twiconn\Plugins.h"

STDMETHODIMP CTimelineControlOpenInBrowserCommand::OnInitialized(IServiceProvider* pServiceProvider)
{
	return S_OK;
}

STDMETHODIMP CTimelineControlOpenInBrowserCommand::OnShutdown()
{
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	if (m_pVariantObject)
		m_pVariantObject.Release();
	return S_OK;
}

STDMETHODIMP CTimelineControlOpenInBrowserCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CHECK_E_POINTER(bstrText);
	*bstrText = CComBSTR(L"Open in browser\tShift+Enter").Detach();
	return S_OK;
}

STDMETHODIMP CTimelineControlOpenInBrowserCommand::InstallMenu(IMenu* pMenu)
{
	CHECK_E_POINTER(pMenu);
	RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_OPEN_BROWSER, this));
	return S_OK;
}

STDMETHODIMP CTimelineControlOpenInBrowserCommand::Invoke(REFGUID guidCommand)
{
	UNREFERENCED_PARAMETER(guidCommand);

	if (!m_pVariantObject)
		return S_OK;

	CComPtr<IVariantObject> pTempObject = m_pVariantObject;
	m_pVariantObject.Release();

	CComVar vUserName;
	RETURN_IF_FAILED(pTempObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserName));
    CComVar vObjectType;
    RETURN_IF_FAILED(pTempObject->GetVariantValue(ObjectModel::Metadata::Object::Type, &vObjectType));
	CComVar v;
	RETURN_IF_FAILED(pTempObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &v));
	if (vUserName.vt == VT_BSTR && v.vt == VT_BSTR)
	{
        auto strUrl = CTimelineControlCopyCommand::MakeTwitterUrl(vUserName.bstrVal, vObjectType.bstrVal, v.bstrVal);// L"https://twitter.com/" + CString(vUserName.bstrVal) + L"/status/" + CString(v.bstrVal);
		ShellExecute(NULL, L"open", strUrl, NULL, NULL, SW_SHOW);
	}

	return S_OK;
}

STDMETHODIMP CTimelineControlOpenInBrowserCommand::SetVariantObject(IVariantObject* pVariantObject)
{
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CTimelineControlOpenInBrowserCommand::SetColumnName(LPCTSTR lpszColumnName)
{
	m_strColumnName = lpszColumnName;
	return S_OK;
}

STDMETHODIMP CTimelineControlOpenInBrowserCommand::GetAccelerator(REFGUID guidCommand, TACCEL *pAccel)
{
	CHECK_E_POINTER(pAccel);
	*pAccel = { FVIRTKEY | FSHIFT, VK_RETURN, 0 }; //CTRL-ENTER
	return S_OK;
}