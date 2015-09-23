#include "stdafx.h"
#include "OpenUrlService.h"
#include "Plugins.h"

STDMETHODIMP COpenUrlService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_WindowService, &m_pWindowService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORMS_SERVICE, &m_pFormsService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &m_pFormManager));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	CComQIPtr<ITimelineControlSupport> pTimelineControlSupport = m_pControl;
	ATLASSERT(pTimelineControlSupport);
	RETURN_IF_FAILED(pTimelineControlSupport->GetTimelineControl(&m_pTimelineControl));
	RETURN_IF_FAILED(AtlAdvise(m_pTimelineControl, pUnk, __uuidof(ITimelineControlEventSink), &m_dwAdviceTimelineControl));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnShutdown()
{
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	RETURN_IF_FAILED(AtlUnadvise(m_pTimelineControl, __uuidof(ITimelineControlEventSink), m_dwAdviceTimelineControl));
	m_pTimelineControl.Release();
	m_pWindowService.Release();
	m_pServiceProvider.Release();
	m_pFormsService.Release();
	m_pFormManager.Release();
	return S_OK;
}

STDMETHODIMP COpenUrlService::OpenUserInfoForm(IVariantObject* pVariantObject)
{
	CComPtr<IControl> pControl;
	HRESULT hr = m_pFormManager->FindForm(CLSID_UserInfoControl, &pControl);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<IUserInfoControl> pUserInfoControl = pControl;
		ATLASSERT(pUserInfoControl);
		CComPtr<IVariantObject> pVariantObjectOpened;
		RETURN_IF_FAILED(pUserInfoControl->GetVariantObject(&pVariantObjectOpened));
		CComVar vUserNameOpened;
		RETURN_IF_FAILED(pVariantObjectOpened->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserNameOpened));
		CComVar vUserName;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserName));
		if (vUserName.vt == VT_BSTR && vUserNameOpened.vt == VT_BSTR && CComBSTR(vUserName.bstrVal) == CComBSTR(vUserNameOpened.bstrVal))
		{
			return S_OK;
		}
	}

	RETURN_IF_FAILED(m_pFormsService->OpenForm(CLSID_UserInfoControl, pVariantObject));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OpenSearchForm(IVariantObject* pVariantObject)
{
	CComPtr<IControl> pControl;
	HRESULT hr = m_pFormManager->FindForm(CLSID_SearchControl, &pControl);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<ISearchControl> pSearchControl = pControl;
		ATLASSERT(pSearchControl);
		CComPtr<IVariantObject> pVariantObjectOpened;
		RETURN_IF_FAILED(pSearchControl->GetVariantObject(&pVariantObjectOpened));
		CComVar vTextOpened;
		if (pVariantObjectOpened)
		{
			RETURN_IF_FAILED(pVariantObjectOpened->GetVariantValue(Twitter::Metadata::Object::Text, &vTextOpened));
		}
		CComVar vText;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Metadata::Object::Text, &vText));
		if (vText.vt == VT_BSTR && vTextOpened.vt == VT_BSTR && CComBSTR(vText.bstrVal) == CComBSTR(vTextOpened.bstrVal))
		{
			return S_OK;
		}
	}

	RETURN_IF_FAILED(m_pFormsService->OpenForm(CLSID_SearchControl, pVariantObject));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OpenTwitViewForm(IVariantObject* pVariantObject)
{
	CComPtr<IControl> pControl;
	HRESULT hr = m_pFormManager->FindForm(CLSID_TwitViewControl, &pControl);
	if (SUCCEEDED(hr))
	{
		CComQIPtr<ITwitViewControl> pTwitViewControl = pControl;
		ATLASSERT(pTwitViewControl);
		CComPtr<IVariantObject> pVariantObjectOpened;
		RETURN_IF_FAILED(pTwitViewControl->GetVariantObject(&pVariantObjectOpened));
		CComVar vIdOpened;
		RETURN_IF_FAILED(pVariantObjectOpened->GetVariantValue(ObjectModel::Metadata::Object::Id, &vIdOpened));
		CComVar vId;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
		if (vId.vt == VT_BSTR && vIdOpened.vt == VT_BSTR && CComBSTR(vId.bstrVal) == CComBSTR(vIdOpened.bstrVal))
		{
			return S_OK;
		}
	}
	RETURN_IF_FAILED(m_pFormsService->OpenForm(CLSID_TwitViewControl, pVariantObject));
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnItemDoubleClick(IVariantObject* pVariantObject)
{
	CComVar vObjectType;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Type, &vObjectType));
	if (vObjectType.vt == VT_BSTR)
	{
		if (CComBSTR(vObjectType.bstrVal) == Twitter::Connection::Metadata::TweetObject::TypeId)
		{
			RETURN_IF_FAILED(OpenTwitViewForm(pVariantObject));
		}
		else if (CComBSTR(vObjectType.bstrVal) == Twitter::Connection::Metadata::ListObject::TypeId)
		{
			RETURN_IF_FAILED(m_pFormsService->OpenForm(CLSID_ListTimelineControl, pVariantObject));
		}
	}
	return S_OK;
}

STDMETHODIMP COpenUrlService::OnColumnClick(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	CHECK_E_POINTER(pColumnsInfoItem);

	CComBSTR bstrColumnName;
	RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Column::Name, &bstrColumnName));

	CString strUrl;
	if (CComBSTR(bstrColumnName) == Twitter::Connection::Metadata::UserObject::DisplayName ||
		CComBSTR(bstrColumnName) == Twitter::Connection::Metadata::UserObject::Name)
	{
		CComVar vUserObject;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::UserObject, &vUserObject));
		if (vUserObject.vt == VT_UNKNOWN)
		{
			CComQIPtr<IVariantObject> pObj = vUserObject.punkVal;
			ATLASSERT(pObj);
			RETURN_IF_FAILED(OpenUserInfoForm(pObj));
		}
	}

	if (CComBSTR(bstrColumnName) == Twitter::Connection::Metadata::TweetObject::RetweetedUserDisplayName ||
		CComBSTR(bstrColumnName) == Twitter::Connection::Metadata::TweetObject::RetweetedUserName)
	{
		CComVar vUserObject;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::RetweetedUserObject, &vUserObject));
		if (vUserObject.vt == VT_UNKNOWN)
		{
			CComQIPtr<IVariantObject> pObj = vUserObject.punkVal;
			ATLASSERT(pObj);
			RETURN_IF_FAILED(OpenUserInfoForm(pObj));
		}
	}

	if (CComBSTR(bstrColumnName) == Twitter::Metadata::Item::TwitterRelativeTime)
	{
		RETURN_IF_FAILED(OpenTwitViewForm(pVariantObject));
	}

	if (CComBSTR(bstrColumnName) == Twitter::Connection::Metadata::TweetObject::Url)
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Object::Value, &bstr));
		if (bstr == L"")
		{
			RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Layout::Metadata::TextColumn::TextFullKey, &bstr));
		}

		if (bstr.Length() && bstr[0] == L'@')
		{
			CString strUserName(bstr);
			strUserName = strUserName.Mid(1);
			CComPtr<IVariantObject> pUserObject;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUserObject));
			RETURN_IF_FAILED(pUserObject->SetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &(CComVar(strUserName))));
			RETURN_IF_FAILED(OpenUserInfoForm(pUserObject));
		}

		if (bstr.Length() && bstr[0] == L'#') 
		{
			CComPtr<IVariantObject> pTextObject;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pTextObject));
			RETURN_IF_FAILED(pTextObject->SetVariantValue(Twitter::Metadata::Object::Text, &CComVar(bstr)));
			RETURN_IF_FAILED(OpenSearchForm(pTextObject));
		}

		strUrl = bstr;
	}

	if (CComBSTR(bstrColumnName) == Twitter::Connection::Metadata::UserObject::Image)
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Metadata::Object::Value, &bstr));
		CString strUrlImage(bstr);
		strUrlImage.Replace(L"_normal", L"");

		CComPtr<IVariantObject> pUrlObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUrlObject));
		RETURN_IF_FAILED(pUrlObject->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &CComVar(strUrlImage)));

		RETURN_IF_FAILED(m_pWindowService->OpenWindow(m_hControlWnd, CLSID_PictureWindow, pUrlObject));
		return S_OK;
	}

	if (CComBSTR(bstrColumnName) == Twitter::Connection::Metadata::TweetObject::Image)
	{
		CComBSTR bstr;
		RETURN_IF_FAILED(pColumnsInfoItem->GetRectStringProp(Twitter::Connection::Metadata::MediaObject::MediaUrl, &bstr));

		CComPtr<IVariantObject> pUrlObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUrlObject));

		CComVar vObjectType;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Type, &vObjectType));
		if (vObjectType.vt == VT_BSTR && CComBSTR(vObjectType.bstrVal) == Twitter::Connection::Metadata::TweetObject::TypeId)
		{
			CComVar vUserName;
			RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Name, &vUserName));
			CComVar vId;
			RETURN_IF_FAILED(pVariantObject->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));
			auto strUrlImage = L"https://twitter.com/" + CString(vUserName.bstrVal) + L"/status/" + CString(vId.bstrVal);
			RETURN_IF_FAILED(pUrlObject->SetVariantValue(Twitter::Metadata::Object::Url, &CComVar(strUrlImage)));
		}

		RETURN_IF_FAILED(pUrlObject->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &CComVar(bstr)));

		CComVar vMediaUrls;
		if (SUCCEEDED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vMediaUrls)))
		{
			RETURN_IF_FAILED(pUrlObject->SetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vMediaUrls));
		}

		RETURN_IF_FAILED(m_pWindowService->OpenWindow(m_hControlWnd, CLSID_PictureWindow, pUrlObject));
		return S_OK;
	}

	if (!strUrl.IsEmpty())
		ShellExecute(NULL, L"open", strUrl, NULL, NULL, SW_SHOW);

	return S_OK;
}
