#include "stdafx.h"
#include "TimelineControlCopyCommand.h"
#include "Plugins.h"

STDMETHODIMP CTimelineControlCopyCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CHECK_E_POINTER(bstrText);
	if (guidCommand == COMMAND_COPY_TEXT)
		*bstrText = CComBSTR(L"Copy").Detach();
	else if (guidCommand == COMMAND_COPY_URL)
		*bstrText = CComBSTR(L"Copy link").Detach();
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::InstallMenu(IMenu* pMenu)
{
	CHECK_E_POINTER(pMenu);
	RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_COPY_TEXT, this));
	RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_COPY_URL, this));
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::Invoke(REFGUID guidCommand)
{
	UNREFERENCED_PARAMETER(guidCommand);

	if (!m_pVariantObject)
		return S_OK;

	CComPtr<IVariantObject> pTempObject = m_pVariantObject;
	m_pVariantObject.Release();

	if (OpenClipboard(NULL))
	{
		if (EmptyClipboard())
		{
			std::wstring str;

			if (IsEqualGUID(guidCommand, COMMAND_COPY_URL))
			{
				CComVariant vUserName;
				RETURN_IF_FAILED(pTempObject->GetVariantValue(VAR_TWITTER_USER_NAME, &vUserName));
				CComVariant v;
				RETURN_IF_FAILED(pTempObject->GetVariantValue(VAR_ID, &v));
				if (vUserName.vt == VT_BSTR && v.vt == VT_BSTR)
				{
					str = L"https://twitter.com/" + CString(vUserName.bstrVal) + L"/status/" + CString(v.bstrVal);
				}
			}
			else if (IsEqualGUID(guidCommand, COMMAND_COPY_TEXT))
			{
				CComVariant vUserDisplayName;
				RETURN_IF_FAILED(pTempObject->GetVariantValue(VAR_TWITTER_USER_DISPLAY_NAME, &vUserDisplayName));
				CComVariant vUserName;
				RETURN_IF_FAILED(pTempObject->GetVariantValue(VAR_TWITTER_USER_NAME, &vUserName));
				CComVariant vText;
				RETURN_IF_FAILED(pTempObject->GetVariantValue(VAR_TWITTER_TEXT, &vText));
				if (vUserDisplayName.vt == VT_BSTR)
					str = vUserDisplayName.bstrVal;
				if (vUserName.vt == VT_BSTR)
					str += L" @" + CString(vUserName.bstrVal);
				if (vText.vt == VT_BSTR)
					str += L"\n" + CString(vText.bstrVal);
			}

			size_t nLength = _tcslen(str.c_str());
			size_t nByteOfBuffer = (nLength + 1) * sizeof(TCHAR);
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, nByteOfBuffer);
			if (NULL == hGlobal)
			{
				CloseClipboard();
				return S_OK;
			}

			LPTSTR pBuf = (LPTSTR)GlobalLock(hGlobal);
			if (NULL == pBuf)
			{
				GlobalFree(hGlobal);
				hGlobal = NULL;
				return HRESULT_FROM_WIN32(GetLastError());
			}

			_tcscpy_s(pBuf, nLength + 1, str.c_str());
			SetClipboardData(CF_UNICODETEXT, hGlobal);
			GlobalUnlock(hGlobal);
			CloseClipboard();
		}
	}
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::SetVariantObject(IVariantObject* pVariantObject)
{
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::SetColumnName(LPCTSTR lpszColumnName)
{
	m_strColumnName = lpszColumnName;
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::GetAccelerator(REFGUID guidCommand, TACCEL *pAccel)
{
	CHECK_E_POINTER(pAccel);
	if (guidCommand == COMMAND_COPY_TEXT)
		*pAccel = { FVIRTKEY | FCONTROL, 0x43, 0 }; //CTRL-C
	else if (guidCommand == COMMAND_COPY_URL)
		*pAccel = { FVIRTKEY | FCONTROL | FSHIFT, 0x43, 0 };//CTRL-SHIFT-C
	return S_OK;
}