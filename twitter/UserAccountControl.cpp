#include "stdafx.h"
#include "UserAccountControl.h"
#include "Plugins.h"

STDMETHODIMP CUserAccountControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &dw_mAdviceDownloadService));
	return S_OK;
}

STDMETHODIMP CUserAccountControl::OnShutdown()
{
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

	m_pSkinUserAccountControl->Draw(cdc, &rect, m_pVariantObject);

	EndPaint(&ps);
	return 0;
}

STDMETHODIMP CUserAccountControl::OnActivate()
{
	CComVariant vBannerUrl;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(VAR_TWITTER_USER_BANNER, &vBannerUrl));
	if(vBannerUrl.vt != VT_BSTR)
		return S_OK;

	m_bstrBannerUrl = vBannerUrl.bstrVal;

	CComPtr<IVariantObject> pDownloadObject;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadObject));
	RETURN_IF_FAILED(pDownloadObject->SetVariantValue(VAR_URL, &vBannerUrl));
	RETURN_IF_FAILED(pDownloadObject->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(TYPE_IMAGE_USER_BANNER)));
	RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadObject));
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