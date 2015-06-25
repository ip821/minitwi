#include "stdafx.h"
#include "ImageViewControl.h"
#include "..\twtheme\GdilPlusUtils.h"

#define TARGET_INTERVAL 15

CImageViewControl::CImageViewControl()
{

}

CImageViewControl::~CImageViewControl()
{
	if (IsWindow())
		DestroyWindow();
}

STDMETHODIMP CImageViewControl::GetHWND(HWND* phWnd)
{
	CHECK_E_POINTER(phWnd);
	*phWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CImageViewControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	m_hWnd = __super::Create(hWndParent, 0, 0, WS_VISIBLE | WS_CHILD);
	if (hWnd)
		*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CImageViewControl::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	return S_OK;
}

LRESULT CImageViewControl::OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return GetParent().PostMessage(uMsg, wParam, lParam);
}

STDMETHODIMP CImageViewControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;

	CComVar vUrl;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(Twitter::Metadata::Object::Url, &vUrl));
	ATLASSERT(vUrl.vt == VT_BSTR);
	m_bstrUrl = vUrl.bstrVal;

	return S_OK;
}

STDMETHODIMP CImageViewControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_AnimationService, &m_pAnimationService));
	RETURN_IF_FAILED(AtlAdvise(m_pAnimationService, pUnk, __uuidof(IAnimationServiceEventSink), &m_dwAdviceAnimationService));

	RETURN_IF_FAILED(m_pAnimationService->SetParams(0, 255, STEPS, TARGET_INTERVAL));
	RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());

	return S_OK;
}
STDMETHODIMP CImageViewControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pAnimationService, __uuidof(IAnimationServiceEventSink), m_dwAdviceAnimationService));
	IInitializeWithControlImpl::OnShutdown();
	m_pAnimationService.Release();
	m_pVariantObject.Release();
	m_pImageManagerService.Release();
	m_pServiceProvider.Release();
	m_pTheme.Release();
	return S_OK;
}

STDMETHODIMP CImageViewControl::OnAnimationStep(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep)
{
	Invalidate();

	if (dwStep == STEPS)
	{
		return S_OK;
	}

	RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());
	return 0;
}

LRESULT CImageViewControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CImageViewControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	CDCHandle cdcReal(BeginPaint(&ps));

	CRect rect;
	GetClientRect(&rect);

	CBitmap bufferBitmap;
	bufferBitmap.CreateCompatibleBitmap(cdcReal, rect.Width(), rect.Height());

	CDC cdc;
	cdc.CreateCompatibleDC(cdcReal);
	CDCSelectBitmapScope cdcSelectBitmapScopeBufferBitmap(cdc, bufferBitmap);

	static DWORD dwBrushColor = 0;
	static DWORD dwTextColor = 0;
	if (!dwBrushColor)
	{
		CComPtr<IThemeColorMap> pThemeColorMap;
		ASSERT_IF_FAILED(m_pTheme->GetColorMap(&pThemeColorMap));
		ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwBrushColor));
		ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::PictureWindowText, &dwTextColor));
	}

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwTextColor);

	cdc.FillSolidRect(&rect, dwBrushColor);

	DWORD dwAlpha = 0;
	ASSERT_IF_FAILED(m_pAnimationService->GetCurrentValue(&dwAlpha));
	TBITMAP tBitmap = { 0 };
	ASSERT_IF_FAILED(m_pImageManagerService->GetImageInfo(m_bstrUrl, &tBitmap));
	auto width = tBitmap.Width;
	auto height = tBitmap.Height;
	auto x = (rect.right - rect.left) / 2 - (width / 2);
	auto y = (rect.bottom - rect.top) / 2 - (height / 2);

	CBitmap bitmap;
	ASSERT_IF_FAILED(m_pImageManagerService->CreateImageBitmap(m_bstrUrl, &bitmap.m_hBitmap));
	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(cdc);
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);
	BLENDFUNCTION bf = { 0 };
	bf.BlendOp = AC_SRC_OVER;
	bf.SourceConstantAlpha = (BYTE)dwAlpha;
	cdc.AlphaBlend(x, y, width, height, cdcBitmap, 0, 0, width, height, bf);

	cdcReal.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), cdc, 0, 0, SRCCOPY);

	EndPaint(&ps);
	return 0;
}

STDMETHODIMP CImageViewControl::SetTheme(ITheme *pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	return S_OK;
}