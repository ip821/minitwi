#include "stdafx.h"
#include "TimelineControlCopyCommand.h"

STDMETHODIMP CTimelineControlCopyCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CHECK_E_POINTER(bstrText);
	*bstrText = CComBSTR(L"Copy text").Detach();
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::InstallMenu(IMenu* pMenu)
{
	CHECK_E_POINTER(pMenu);
	return pMenu->AddMenuCommand(GUID_NULL, CLSID_TimelineControlCopyCommand, this);
}

STDMETHODIMP CTimelineControlCopyCommand::Invoke(REFGUID guidCommand)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CComVariant v;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(CComBSTR(m_strColumnName), &v));
	{
		if (OpenClipboard(NULL))
		{
			if (EmptyClipboard())
			{
				std::wstring str;
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
