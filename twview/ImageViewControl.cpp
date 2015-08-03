#include "stdafx.h"
#include "ImageViewControl.h"
#include "..\model-libs\layout\GdilPlusUtils.h"

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

	const DWORD dwValueStart = 0;

	CComPtr<IVariantObject> pImageElement;
	RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayout, L"ImageViewLayoutImageColumn", &pImageElement));
	RETURN_IF_FAILED(pImageElement->SetVariantValue(Layout::Metadata::ImageColumn::ImageKey, &CComVar(m_bstrUrl)));
	RETURN_IF_FAILED(pImageElement->SetVariantValue(Layout::Metadata::ImageColumn::Alpha, &CComVar(dwValueStart)));

	CRect rect;
	GetClientRect(&rect);
	CClientDC cdc(m_hWnd);

	RETURN_IF_FAILED(m_pLayoutManager->BuildLayout(cdc, &rect, m_pLayout, nullptr, m_pImageManagerService, m_pColumnsInfo));
	RETURN_IF_FAILED(m_pAnimationService->SetParams(dwValueStart, 255, STEPS, TARGET_INTERVAL));
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
	m_pLayout.Release();
	m_pLayoutManager.Release();
	m_pColumnsInfo.Release();
	return S_OK;
}

STDMETHODIMP CImageViewControl::OnAnimationStep(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep)
{
	CComPtr<IColumnsInfoItem> pImageColumnsInfoItem;
	RETURN_IF_FAILED(m_pColumnsInfo->FindItemByName(L"ImageViewLayoutImageColumn", &pImageColumnsInfoItem));
	RETURN_IF_FAILED(pImageColumnsInfoItem->SetVariantValue(Layout::Metadata::ImageColumn::Alpha, &CComVar(dwValue)));

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

	cdc.SetBkMode(TRANSPARENT);
	ASSERT_IF_FAILED(m_pLayoutManager->EraseBackground(cdc, m_pColumnsInfo));
	ASSERT_IF_FAILED(m_pLayoutManager->PaintLayout(cdc, m_pImageManagerService, m_pColumnsInfo));
	
	cdcReal.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), cdc, 0, 0, SRCCOPY);

	EndPaint(&ps);
	return 0;
}

STDMETHODIMP CImageViewControl::SetTheme(ITheme *pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	RETURN_IF_FAILED(m_pTheme->GetLayout(L"ImageViewLayout", &m_pLayout));
	RETURN_IF_FAILED(m_pTheme->GetLayoutManager(&m_pLayoutManager));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfo, &m_pColumnsInfo));
	return S_OK;
}