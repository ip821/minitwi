#include "stdafx.h"
#include "TimelineControlCopyCommand.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

STDMETHODIMP CTimelineControlCopyCommand::OnInitialized(IServiceProvider* pServiceProvider)
{
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::OnShutdown()
{
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	if (m_pVariantObject)
		m_pVariantObject.Release();
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::GetEnabled(REFGUID guidCommand, BOOL *pbEnabled)
{
	CHECK_E_POINTER(pbEnabled);
	if (IsEqualGUID(guidCommand, COMMAND_COPY_IMAGE_URL))
	{
		*pbEnabled = FALSE;
		CComVar vColumnObject;
		ASSERT_IF_FAILED(m_pVariantObject->GetVariantValue(ObjectModel::Metadata::Table::Column::Object, &vColumnObject));
		if (vColumnObject.vt == VT_UNKNOWN)
		{
			CComQIPtr<IColumnsInfoItem> pColumnsInfoItem = vColumnObject.punkVal;
			ATLASSERT(pColumnsInfoItem);
			CComBSTR bstrColumnName;
			pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Column::Name, &bstrColumnName);
			*pbEnabled =
				bstrColumnName == Twitter::Connection::Metadata::TweetObject::Image ||
				bstrColumnName == Twitter::Connection::Metadata::UserObject::Image;
		}
	}
	else
	{
		*pbEnabled = TRUE;
	}
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CHECK_E_POINTER(bstrText);
	if (guidCommand == COMMAND_COPY_TEXT)
		*bstrText = CComBSTR(L"Copy\tCtrl+C").Detach();
	else if (guidCommand == COMMAND_COPY_URL)
		*bstrText = CComBSTR(L"Copy link\tCtrl+Shift+C").Detach();
	else if (guidCommand == COMMAND_COPY_IMAGE_URL)
		*bstrText = CComBSTR(L"Copy image link").Detach();
	return S_OK;
}

STDMETHODIMP CTimelineControlCopyCommand::InstallMenu(IMenu* pMenu)
{
	CHECK_E_POINTER(pMenu);
	RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_COPY_TEXT, this));
	RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_COPY_URL, this));
	RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_COPY_IMAGE_URL, this));
	return S_OK;
}

CString CTimelineControlCopyCommand::MakeTwitterUrl(BSTR bstrUserName, BSTR bstrObjectType, BSTR bstrTwitterId)
{
    if(CComBSTR(bstrObjectType) == Twitter::Connection::Metadata::TweetObject::TypeId)
        return L"https://twitter.com/" + CString(bstrUserName) + L"/status/" + CString(bstrTwitterId);
    else if(CComBSTR(bstrObjectType) == Twitter::Connection::Metadata::UserObject::TypeId)
        return L"https://twitter.com/" + CString(bstrUserName);
    return L"";
}

CString CTimelineControlCopyCommand::MakeMediaString(CString& strUrl)
{
    CString strResultString;
    strResultString.Format(L"Media: %s", strUrl);
    return strResultString;
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

            CComVar vUserName;
            RETURN_IF_FAILED(pTempObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserName));
            CComVar vTwitterId;
            RETURN_IF_FAILED(pTempObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vTwitterId));
            CComVar vObjectType;
            RETURN_IF_FAILED(pTempObject->GetVariantValue(ObjectModel::Metadata::Object::Type, &vObjectType));

			if (IsEqualGUID(guidCommand, COMMAND_COPY_URL))
			{
				if (vUserName.vt == VT_BSTR && vTwitterId.vt == VT_BSTR && vObjectType.vt == VT_BSTR)
				{
					str = MakeTwitterUrl(vUserName.bstrVal, vObjectType.bstrVal, vTwitterId.bstrVal);
				}
			}
			else if (IsEqualGUID(guidCommand, COMMAND_COPY_TEXT))
			{
				CComVar vUserDisplayName;
				RETURN_IF_FAILED(pTempObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::DisplayName, &vUserDisplayName));
				CComVar vText;
				RETURN_IF_FAILED(pTempObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::NormalizedText, &vText));

				if (vUserDisplayName.vt == VT_BSTR)
					str = vUserDisplayName.bstrVal;
				if (vUserName.vt == VT_BSTR)
					str += L" @" + CString(vUserName.bstrVal);
				if (vText.vt == VT_BSTR)
				{
					str += L"\n" + CString(vText.bstrVal);

					CComVar vMediaUrls;
					RETURN_IF_FAILED(pTempObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vMediaUrls));

					if (vMediaUrls.vt == VT_UNKNOWN)
					{
						CComQIPtr<IObjArray> pMediaUrlsCollection = vMediaUrls.punkVal;
						if (pMediaUrlsCollection)
						{
							UINT uiCount = 0;
							RETURN_IF_FAILED(pMediaUrlsCollection->GetCount(&uiCount));

                            if (uiCount)
                            {
                                str += L"\n";
                                if (uiCount == 1)
                                {
                                    CComPtr<IVariantObject> pMediaUrlObject;
                                    RETURN_IF_FAILED(pMediaUrlsCollection->GetAt(0, __uuidof(IVariantObject), (LPVOID*)&pMediaUrlObject));
                                    CComVar vUrlText;
                                    RETURN_IF_FAILED(pMediaUrlObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &vUrlText));
                                    if (vUrlText.vt == VT_BSTR)
                                    {
                                        str += MakeMediaString(CString(vUrlText.bstrVal));
                                    }
                                }
                                else if (vUserName.vt == VT_BSTR && vTwitterId.vt == VT_BSTR && vObjectType.vt == VT_BSTR)
                                {
                                    str += MakeMediaString(MakeTwitterUrl(vUserName.bstrVal, vObjectType.bstrVal, vTwitterId.bstrVal));
                                }
                            }
						}
					}
				}
			}
			else if (IsEqualGUID(guidCommand, COMMAND_COPY_IMAGE_URL))
			{
				CComVar vColumnObject;
				ASSERT_IF_FAILED(pTempObject->GetVariantValue(ObjectModel::Metadata::Table::Column::Object, &vColumnObject));
				CComQIPtr<IColumnsInfoItem> pColumnsInfoItem = vColumnObject.punkVal;
				ATLASSERT(pColumnsInfoItem);
				CComBSTR bstr;
				RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Connection::Metadata::MediaObject::MediaUrl, &bstr));
				CString strUrl(bstr);
				strUrl.Replace(L"_normal", L"");
				str = strUrl;
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

STDMETHODIMP CTimelineControlCopyCommand::GetAccelerator(REFGUID guidCommand, TACCEL *pAccel)
{
	CHECK_E_POINTER(pAccel);
	if (guidCommand == COMMAND_COPY_TEXT)
		*pAccel = { FVIRTKEY | FCONTROL, 0x43, 0 }; //CTRL-C
	else if (guidCommand == COMMAND_COPY_URL)
		*pAccel = { FVIRTKEY | FCONTROL | FSHIFT, 0x43, 0 };//CTRL-SHIFT-C
	return S_OK;
}