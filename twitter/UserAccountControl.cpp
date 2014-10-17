#include "stdafx.h"
#include "UserAccountControl.h"
#include "Plugins.h"

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

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &dw_mAdviceDownloadService));

	m_handCursor.LoadSysCursor(IDC_HAND);
	m_arrowCursor.LoadSysCursor(IDC_ARROW);

	return S_OK;
}

STDMETHODIMP CUserAccountControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), dw_mAdviceDownloadService));

	m_pSkinUserAccountControl.Release();
	m_pVariantObject.Release();
	m_pImageManagerService.Release();
	m_pThemeColorMap.Release();
	m_pThemeFontMap.Release();
	m_pDownloadService.Release();
	m_pWindowService.Release();

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

STDMETHODIMP CUserAccountControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CUserAccountControl::SetSkinUserAccountControl(ISkinUserAccountControl* pSkinUserAccountControl)
{
	CHECK_E_POINTER(pSkinUserAccountControl);
	m_pSkinUserAccountControl = pSkinUserAccountControl;
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
	m_pSkinUserAccountControl->Draw((HDC)wParam, &rect, m_pVariantObject);
	return 0;
}

LRESULT CUserAccountControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	BeginPaint(&ps);

	CRect rect = ps.rcPaint;
	CDC cdc(ps.hdc);

	ASSERT_IF_FAILED(m_pSkinUserAccountControl->Draw(cdc, &rect, m_pVariantObject));
	ASSERT_IF_FAILED(m_pSkinUserAccountControl->GetColumnRect(VAR_TWITTER_USER_IMAGE, &m_rectUserImage));

	EndPaint(&ps);
	return 0;
}

STDMETHODIMP CUserAccountControl::OnActivate()
{
	CComVariant vBannerUrl;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_BANNER, &vBannerUrl));
	if (vBannerUrl.vt == VT_BSTR)
	{
		m_bstrBannerUrl = vBannerUrl.bstrVal;

		CComPtr<IVariantObject> pDownloadObject;
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadObject));
		RETURN_IF_FAILED(pDownloadObject->SetVariantValue(VAR_URL, &vBannerUrl));
		RETURN_IF_FAILED(pDownloadObject->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(TYPE_IMAGE_USER_BANNER)));
		RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadObject));
	}

	CComVariant vUserImage;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_IMAGE, &vUserImage));
	if (vUserImage.vt == VT_BSTR)
	{
		BOOL bContains = FALSE;
		RETURN_IF_FAILED(m_pImageManagerService->ContainsImageKey(vUserImage.bstrVal, &bContains));
		if (!bContains)
		{
			CComPtr<IVariantObject> pDownloadObjectUserImage;
			RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadObjectUserImage));
			RETURN_IF_FAILED(pDownloadObjectUserImage->SetVariantValue(VAR_URL, &vUserImage));
			RETURN_IF_FAILED(pDownloadObjectUserImage->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(TYPE_IMAGE_USER_IMAGE)));
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
	CComVariant vType;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_OBJECT_TYPE, &vType));
	CComVariant vUrl;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_URL, &vUrl));
	CComVariant vFilePath;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));

	if (vType.vt == VT_BSTR && CComBSTR(vType.bstrVal) == CComBSTR(TYPE_IMAGE_USER_IMAGE) && vUrl.vt == VT_BSTR && vFilePath.vt == VT_BSTR)
	{
		BOOL bContains = FALSE;
		RETURN_IF_FAILED(m_pImageManagerService->ContainsImageKey(vUrl.bstrVal, &bContains));
		if (!bContains)
		{
			RETURN_IF_FAILED(m_pImageManagerService->AddImage(vUrl.bstrVal, vFilePath.bstrVal));
		}
		Invalidate();
	}

	if (vType.vt == VT_BSTR && CComBSTR(vType.bstrVal) == CComBSTR(TYPE_IMAGE_USER_BANNER) && vUrl.vt == VT_BSTR && m_bstrBannerUrl == vUrl.bstrVal && vFilePath.vt == VT_BSTR)
	{
		RETURN_IF_FAILED(m_pSkinUserAccountControl->AnimationStart());
		BOOL bContains = FALSE;
		RETURN_IF_FAILED(m_pImageManagerService->ContainsImageKey(vUrl.bstrVal, &bContains));
		if (!bContains)
		{
			RETURN_IF_FAILED(m_pImageManagerService->AddImage(vUrl.bstrVal, vFilePath.bstrVal));
		}
		StartAnimation();
	}
	return S_OK;
}

void CUserAccountControl::StartAnimation()
{
	UINT uiInterval = 0;
	ASSERT_IF_FAILED(m_pSkinUserAccountControl->AnimationGetParams(&uiInterval));
	StartAnimationTimer(uiInterval);
}

LRESULT CUserAccountControl::OnAnimationTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bContinueAnimation = FALSE;
	ASSERT_IF_FAILED(m_pSkinUserAccountControl->AnimationNextFrame(&bContinueAnimation));
	Invalidate();
	if (bContinueAnimation)
	{
		StartAnimation();
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

	if (!m_rectUserImage.PtInRect(CPoint(x, y)))
		return 0;

	CComVariant vUserImage;
	ASSERT_IF_FAILED(m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_IMAGE, &vUserImage));
	if (vUserImage.vt != VT_BSTR)
		return 0;
	CString strUrl(vUserImage.bstrVal);
	strUrl.Replace(L"_normal", L"");

	CComPtr<IVariantObject> pUrlObject;
	ASSERT_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pUrlObject));
	ASSERT_IF_FAILED(pUrlObject->SetVariantValue(VAR_TWITTER_MEDIAURL, &CComVariant(strUrl)));
	ASSERT_IF_FAILED(m_pWindowService->OpenWindow(m_hControlWnd, CLSID_PictureWindow, pUrlObject));
	return 0;
}
