#include "stdafx.h"
#include "UserAccountControl.h"
#include "Plugins.h"
#include "Metadata.h"

#define OFFSET_X 10
#define OFFSET_Y 20
#define TARGET_INTERVAL 15

HRESULT CUserAccountControl::FinalConstruct()
{
	return S_OK;
}

void CUserAccountControl::FinalRelease()
{
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
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_ANIMATION_BACKGROUND, &m_pAnimationServiceBackgroundImage));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_ANIMATION_USER, &m_pAnimationServiceUserImage));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &dw_mAdviceDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pFollowThreadService, pUnk, __uuidof(IThreadServiceEventSink), &dw_mAdviceFollowService));
	RETURN_IF_FAILED(AtlAdvise(m_pFollowStatusThreadService, pUnk, __uuidof(IThreadServiceEventSink), &dw_mAdviceFollowStatusService));
	RETURN_IF_FAILED(AtlAdvise(m_pAnimationServiceBackgroundImage, pUnk, __uuidof(IAnimationServiceEventSink), &dw_mAdviceAnimationServiceBackgroundImage));
	RETURN_IF_FAILED(AtlAdvise(m_pAnimationServiceUserImage, pUnk, __uuidof(IAnimationServiceEventSink), &dw_mAdviceAnimationServiceUserImage));

	m_handCursor.LoadSysCursor(IDC_HAND);
	m_arrowCursor.LoadSysCursor(IDC_ARROW);

	return S_OK;
}

STDMETHODIMP CUserAccountControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pAnimationServiceUserImage, __uuidof(IAnimationServiceEventSink), dw_mAdviceAnimationServiceUserImage));
	RETURN_IF_FAILED(AtlUnadvise(m_pAnimationServiceBackgroundImage, __uuidof(IAnimationServiceEventSink), dw_mAdviceAnimationServiceBackgroundImage));
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), dw_mAdviceDownloadService));
	RETURN_IF_FAILED(AtlUnadvise(m_pFollowThreadService, __uuidof(IThreadServiceEventSink), dw_mAdviceFollowService));
	RETURN_IF_FAILED(AtlUnadvise(m_pFollowStatusThreadService, __uuidof(IThreadServiceEventSink), dw_mAdviceFollowStatusService));

	m_pAnimationServiceUserImage.Release();
	m_pAnimationServiceBackgroundImage.Release();
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
	{
		CComQIPtr<IMsgHandler> p = m_pAnimationServiceBackgroundImage;
		if (p)
		{
			LRESULT lRes = 0;
			BOOL bH = FALSE;
			ASSERT_IF_FAILED(p->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lRes, &bH));
		}
	}

	{
		CComQIPtr<IMsgHandler> p = m_pAnimationServiceUserImage;
		if (p)
		{
			LRESULT lRes = 0;
			BOOL bH = FALSE;
			ASSERT_IF_FAILED(p->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lRes, &bH));
		}
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
	RETURN_IF_FAILED(m_pTheme->GetCommonControlSkin(&m_pSkinCommonControl));
	RETURN_IF_FAILED(m_pTheme->GetSkinUserAccountControl(&m_pSkinUserAccountControl));
	RETURN_IF_FAILED(m_pSkinUserAccountControl->SetImageManagerService(m_pImageManagerService));
	UpdateRects();
	return S_OK;
}

LRESULT CUserAccountControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CUserAccountControl::OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CRect rect;
	GetClientRect(&rect);
	RETURN_IF_FAILED(m_pSkinUserAccountControl->EraseBackground((HDC)wParam));
	RETURN_IF_FAILED(m_pSkinUserAccountControl->Draw((HDC)wParam));
	return 0;
}

LRESULT CUserAccountControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	BeginPaint(&ps);

	CRect rect;
	GetClientRect(&rect);

	RETURN_IF_FAILED(m_pSkinUserAccountControl->EraseBackground(ps.hdc));
	RETURN_IF_FAILED(m_pSkinUserAccountControl->Draw(ps.hdc));

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

	if (!m_pVariantObject)
		return;

	CRect rect;
	GetClientRect(&rect);
	CClientDC cdc(m_hWnd);
	ASSERT_IF_FAILED(m_pSkinUserAccountControl->Measure(cdc, &rect, m_pVariantObject, m_bFollowing, m_bFollowButtonDisabled));
	ASSERT_IF_FAILED(m_pSkinUserAccountControl->GetRects(&m_rectUserImage, &m_rectFollowButton));
}

STDMETHODIMP CUserAccountControl::OnActivate()
{
	UpdateRects();
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
		TSTARTANIMATIONPARAMS params = { 0 };
		params.animationType = SkinUserAccountControlAnimationType::UserImage;
		params.bstrKey = vUrl.bstrVal;
		params.pStream = pStream;
		SendMessage(WM_START_ANIMATION, (WPARAM)&params, 0);
	}

	if (vType.vt == VT_BSTR && CComBSTR(vType.bstrVal) == Twitter::Metadata::Types::ImageUserBanner && vUrl.vt == VT_BSTR && m_bstrBannerUrl == vUrl.bstrVal)
	{
		TSTARTANIMATIONPARAMS params = { 0 };
		params.animationType = SkinUserAccountControlAnimationType::BackgroundImage;
		params.bstrKey = vUrl.bstrVal;
		params.pStream = pStream;
		SendMessage(WM_START_ANIMATION, (WPARAM)&params, 0);
	}
	return S_OK;
}

LRESULT CUserAccountControl::OnStartAnimation(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	TSTARTANIMATIONPARAMS* pParams = (TSTARTANIMATIONPARAMS*)wParam;
	ASSERT_IF_FAILED(m_pSkinUserAccountControl->StartAnimation(pParams->animationType));

	BOOL bContains = FALSE;
	RETURN_IF_FAILED(m_pImageManagerService->ContainsImageKey(pParams->bstrKey, &bContains));
	if (!bContains)
	{
		RETURN_IF_FAILED(m_pImageManagerService->AddImageFromStream(pParams->bstrKey, pParams->pStream));
	}

	UpdateRects();

	if (pParams->animationType == SkinUserAccountControlAnimationType::BackgroundImage)
	{
		ASSERT_IF_FAILED(m_pAnimationServiceBackgroundImage->SetParams(0, 255, STEPS, TARGET_INTERVAL));
		ASSERT_IF_FAILED(m_pAnimationServiceBackgroundImage->StartAnimationTimer());
	}
	else if (pParams->animationType == SkinUserAccountControlAnimationType::UserImage)
	{
		ASSERT_IF_FAILED(m_pAnimationServiceUserImage->SetParams(0, 255, STEPS, TARGET_INTERVAL));
		ASSERT_IF_FAILED(m_pAnimationServiceUserImage->StartAnimationTimer());
	}
	return 0;
}

STDMETHODIMP CUserAccountControl::OnAnimationStep(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep)
{
	if (pAnimationService == m_pAnimationServiceBackgroundImage)
	{
		ASSERT_IF_FAILED(m_pSkinUserAccountControl->AnimationSetValue(SkinUserAccountControlAnimationType::BackgroundImage, dwValue));
	}
	else if (pAnimationService == m_pAnimationServiceUserImage)
	{
		ASSERT_IF_FAILED(m_pSkinUserAccountControl->AnimationSetValue(SkinUserAccountControlAnimationType::UserImage, dwValue));
	}
	UpdateRects();
	Invalidate();

	if (dwStep != STEPS)
	{
		if (pAnimationService == m_pAnimationServiceBackgroundImage)
		{
			ASSERT_IF_FAILED(m_pAnimationServiceBackgroundImage->StartAnimationTimer());
		}
		else if (pAnimationService == m_pAnimationServiceUserImage)
		{
			ASSERT_IF_FAILED(m_pAnimationServiceUserImage->StartAnimationTimer());
		}
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
	UpdateRects();
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
	UpdateRects();
	Invalidate();
	return S_OK;
}
