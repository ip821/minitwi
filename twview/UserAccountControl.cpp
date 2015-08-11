#include "stdafx.h"
#include "UserAccountControl.h"
#include "Plugins.h"

#define OFFSET_X 10
#define OFFSET_Y 20
#define TARGET_INTERVAL 15

HRESULT CUserAccountControl::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfo, &m_pColumnsInfo));
	return S_OK;
}

void CUserAccountControl::FinalRelease()
{
	m_pColumnsInfo.Release();
	if (m_hWnd)
		DestroyWindow();
}

STDMETHODIMP CUserAccountControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_WindowService, &m_pWindowService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FOLLOW_THREAD, &m_pFollowThreadService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FOLLOW_STATUS_THREAD, &m_pFollowStatusThreadService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_AnimationService, &m_pAnimationService));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &dw_mAdviceDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pFollowThreadService, pUnk, __uuidof(IThreadServiceEventSink), &dw_mAdviceFollowService));
	RETURN_IF_FAILED(AtlAdvise(m_pFollowStatusThreadService, pUnk, __uuidof(IThreadServiceEventSink), &dw_mAdviceFollowStatusService));
	RETURN_IF_FAILED(AtlAdvise(m_pAnimationService, pUnk, __uuidof(IAnimationServiceEventSink), &dw_mAdviceAnimationService));

	m_handCursor.LoadSysCursor(IDC_HAND);
	m_arrowCursor.LoadSysCursor(IDC_ARROW);

	return S_OK;
}

STDMETHODIMP CUserAccountControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pAnimationService, __uuidof(IAnimationServiceEventSink), dw_mAdviceAnimationService));
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), dw_mAdviceDownloadService));
	RETURN_IF_FAILED(AtlUnadvise(m_pFollowThreadService, __uuidof(IThreadServiceEventSink), dw_mAdviceFollowService));
	RETURN_IF_FAILED(AtlUnadvise(m_pFollowStatusThreadService, __uuidof(IThreadServiceEventSink), dw_mAdviceFollowStatusService));

	m_pAnimationService.Release();
	m_pTheme.Release();
	m_pSkinCommonControl.Release();
	m_pSkinUserAccountControl.Release();
	m_pVariantObject.Release();
	m_pImageManagerService.Release();
	m_pThemeColorMap.Release();
	m_pThemeFontMap.Release();
	m_pDownloadService.Release();
	m_pWindowService.Release();
	m_pFollowThreadService.Release();
	m_pFollowStatusThreadService.Release();

	return S_OK;
}

STDMETHODIMP CUserAccountControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CUserAccountControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = Create(hWndParent);
	return S_OK;
}

STDMETHODIMP CUserAccountControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return S_OK;
}

LRESULT CUserAccountControl::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CComQIPtr<IMsgHandler> p = m_pAnimationService;
	if (p)
	{
		LRESULT lRes = 0;
		BOOL bH = FALSE;
		ASSERT_IF_FAILED(p->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lRes, &bH));
	}
	return S_OK;
}

STDMETHODIMP CUserAccountControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	UpdateRects();
	return S_OK;
}

STDMETHODIMP CUserAccountControl::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	RETURN_IF_FAILED(m_pTheme->GetSkinUserAccountControl(&m_pSkinUserAccountControl));
	RETURN_IF_FAILED(m_pTheme->GetCommonControlSkin(&m_pSkinCommonControl));
	RETURN_IF_FAILED(m_pSkinUserAccountControl->SetImageManagerService(m_pImageManagerService));
	UpdateRects();
	return S_OK;
}

LRESULT CUserAccountControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRect rect;
	GetClientRect(&rect);
	if (m_pVariantObject)
		m_pSkinUserAccountControl->EraseBackground((HDC)wParam, &rect, m_pVariantObject);

	return 0;
}

LRESULT CUserAccountControl::OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRect rect;
	GetClientRect(&rect);
	m_pSkinUserAccountControl->Draw((HDC)wParam, &rect, m_pVariantObject, m_pColumnsInfo);
	return 0;
}

LRESULT CUserAccountControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pSkinUserAccountControl)
		return 0;

	PAINTSTRUCT ps = { 0 };
	BeginPaint(&ps);

	CRect rect;
	GetClientRect(&rect);
	CDCHandle cdc(ps.hdc);

	ASSERT_IF_FAILED(m_pSkinUserAccountControl->Draw(cdc, &rect, m_pVariantObject, m_pColumnsInfo));
	EndPaint(&ps);
	return 0;
}

LRESULT CUserAccountControl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UpdateRects();
	return 0;
}

void CUserAccountControl::UpdateRects()
{
	m_rectUserImage.SetRectEmpty();
	m_rectFollowButton.SetRectEmpty();
	m_pColumnsInfo->Clear();

	if (!m_pSkinUserAccountControl || !m_pVariantObject)
		return;

	CRect rect;
	GetClientRect(&rect);
	ASSERT_IF_FAILED(m_pSkinUserAccountControl->Measure(m_hWnd, &rect, m_pColumnsInfo, m_pVariantObject));

	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		ASSERT_IF_FAILED(m_pColumnsInfo->FindItemByName(Twitter::Connection::Metadata::UserObject::Image, &pColumnsInfoItem));
		ASSERT_IF_FAILED(pColumnsInfoItem->GetRect(&m_rectUserImage));
	}

	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem;
		ASSERT_IF_FAILED(m_pColumnsInfo->FindItemByName(Twitter::Metadata::Item::VAR_ITEM_FOLLOW_BUTTON, &pColumnsInfoItem));
		ASSERT_IF_FAILED(pColumnsInfoItem->GetRect(&m_rectFollowButton));
	}

	ASSERT_IF_FAILED(UpdateColumnInfo());
}

STDMETHODIMP CUserAccountControl::OnActivate()
{
	UpdateColumnInfo();
	RETURN_IF_FAILED(m_pFollowStatusThreadService->Run());

	CComVar vBannerUrl;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Banner, &vBannerUrl));
	if (vBannerUrl.vt == VT_BSTR)
	{
		m_bstrBannerUrl = vBannerUrl.bstrVal;

		CComPtr<IVariantObject> pDownloadObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadObject));
		RETURN_IF_FAILED(pDownloadObject->SetVariantValue(Twitter::Metadata::Object::Url, &vBannerUrl));
		RETURN_IF_FAILED(pDownloadObject->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVar(Twitter::Metadata::Types::ImageUserBanner)));
		RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadObject));
	}

	CComVar vUserImage;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Image, &vUserImage));
	if (vUserImage.vt == VT_BSTR)
	{
		BOOL bContains = FALSE;
		RETURN_IF_FAILED(m_pImageManagerService->ContainsImageKey(vUserImage.bstrVal, &bContains));
		if (!bContains)
		{
			CComPtr<IVariantObject> pDownloadObjectUserImage;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadObjectUserImage));
			RETURN_IF_FAILED(pDownloadObjectUserImage->SetVariantValue(Twitter::Metadata::Object::Url, &vUserImage));
			RETURN_IF_FAILED(pDownloadObjectUserImage->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVar(Twitter::Metadata::Types::ImageUserImage)));
			RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadObjectUserImage));
		}
	}
	return S_OK;
}

STDMETHODIMP CUserAccountControl::OnDeactivate()
{
	return S_OK;
}

STDMETHODIMP CUserAccountControl::OnDownloadComplete(IVariantObject *pResult)
{
	CComVar vType;
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Type, &vType));
	CComVar vUrl;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Object::Url, &vUrl));
	CComVar vStream;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::File::StreamObject, &vStream));
	CComQIPtr<IStream> pStream = vStream.punkVal;

	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	if (vType.vt == VT_BSTR && CComBSTR(vType.bstrVal) == Twitter::Metadata::Types::ImageUserImage && vUrl.vt == VT_BSTR)
	{
		BOOL bContains = FALSE;
		RETURN_IF_FAILED(m_pImageManagerService->ContainsImageKey(vUrl.bstrVal, &bContains));
		if (!bContains)
		{
			RETURN_IF_FAILED(m_pImageManagerService->AddImageFromStream(vUrl.bstrVal, pStream));
		}
		Invalidate();
	}

	if (vType.vt == VT_BSTR && CComBSTR(vType.bstrVal) == Twitter::Metadata::Types::ImageUserBanner && vUrl.vt == VT_BSTR && m_bstrBannerUrl == vUrl.bstrVal)
	{
		BOOL bContains = FALSE;
		RETURN_IF_FAILED(m_pImageManagerService->ContainsImageKey(vUrl.bstrVal, &bContains));
		if (!bContains)
		{
			RETURN_IF_FAILED(m_pImageManagerService->AddImageFromStream(vUrl.bstrVal, pStream));
		}
		StartAnimation();
	}
	return S_OK;
}

void CUserAccountControl::StartAnimation()
{
	ASSERT_IF_FAILED(m_pAnimationService->SetParams(0, 255, STEPS, TARGET_INTERVAL));
	ASSERT_IF_FAILED(m_pAnimationService->StartAnimationTimer());
}

STDMETHODIMP CUserAccountControl::OnAnimationStep(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep)
{
	ASSERT_IF_FAILED(m_pSkinUserAccountControl->AnimationSetValue(dwValue));
	Invalidate();
	if (dwStep != STEPS)
	{
		ASSERT_IF_FAILED(m_pAnimationService->StartAnimationTimer());
	}
	return 0;
}

LRESULT CUserAccountControl::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	if (m_rectUserImage.PtInRect(CPoint(x, y)))
		SetCursor(m_handCursor);
	else
		SetCursor(m_arrowCursor);
	return 0;
}

LRESULT CUserAccountControl::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pVariantObject)
		return 0;

	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);

	if (m_rectUserImage.PtInRect(CPoint(x, y)))
	{
		CComVar vUserImage;
		ASSERT_IF_FAILED(m_pVariantObject->GetVariantValue(Twitter::Connection::Metadata::UserObject::Image, &vUserImage));
		if (vUserImage.vt != VT_BSTR)
			return 0;
		CString strUrl(vUserImage.bstrVal);
		strUrl.Replace(L"_normal", L"");

		CComPtr<IVariantObject> pUrlObject;
		ASSERT_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUrlObject));
		ASSERT_IF_FAILED(pUrlObject->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &CComVar(strUrl)));
		ASSERT_IF_FAILED(m_pWindowService->OpenWindow(m_hControlWnd, CLSID_PictureWindow, pUrlObject));
	}
	else if (m_rectFollowButton.PtInRect(CPoint(x, y)) && !m_bFollowButtonDisabled)
	{
		RETURN_IF_FAILED(m_pFollowThreadService->Run());
	}
	return 0;
}

STDMETHODIMP CUserAccountControl::OnStart(IVariantObject *pResult)
{
	m_bFollowButtonDisabled = TRUE;
	UpdateColumnInfo();
	Invalidate();
	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Item::VAR_IS_FOLLOWING, &CComVar(m_bFollowing)));
	return S_OK;
}

STDMETHODIMP CUserAccountControl::OnFinish(IVariantObject *pResult)
{
	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));

	if (FAILED(vHr.intVal))
	{
		return S_OK;
	}

	CComVar vFollowing;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Item::VAR_IS_FOLLOWING, &vFollowing));
	m_bFollowing = vFollowing.vt == VT_BOOL && vFollowing.boolVal;
	m_bFollowButtonDisabled = FALSE;
	UpdateColumnInfo();
	Invalidate();
	return S_OK;
}

STDMETHODIMP CUserAccountControl::UpdateColumnInfo()
{
	CComPtr<IColumnsInfoItem> pColumnsInfoItem;
	ASSERT_IF_FAILED(m_pColumnsInfo->FindItemByName(Twitter::Metadata::Item::VAR_ITEM_FOLLOW_BUTTON, &pColumnsInfoItem));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Twitter::Metadata::Item::TwitterItemFollowButtonRectDisabled, m_bFollowButtonDisabled));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectBoolProp(Layout::Metadata::Element::Selected, m_bFollowing));
	RETURN_IF_FAILED(pColumnsInfoItem->SetRectStringProp(Layout::Metadata::TextColumn::Text, m_bFollowing ? L"Following" : L"  Follow  "));
	return S_OK;
}
