#include "stdafx.h"
#include "ScrollControl.h"
#include "CustomTabControl.h"
#include "Mmsystem.h"
#include "..\twtheme\GdilPlusUtils.h"
#pragma comment(lib, "Winmm.lib")

#define TARGET_RESOLUTION 1
#define TARGET_INTERVAL 12

CScrollControl::CScrollControl()
{
}

CScrollControl::~CScrollControl()
{
	DestroyWindow();
}

STDMETHODIMP CScrollControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_AnimationManagerService, &m_pAnimationManagerService));
	return S_OK;
}

STDMETHODIMP CScrollControl::OnShutdown()
{
	m_pAnimationManagerService.Release();
	m_pCustomTabControl.Release();
	return S_OK;
}

STDMETHODIMP CScrollControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CScrollControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	__super::Create(hWndParent, 0, L"", WS_CHILD);
	if (hWnd)
		*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CScrollControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return S_OK;
}

STDMETHODIMP CScrollControl::ShowWindow(DWORD dwCommand)
{
	CWindowImpl<CScrollControl>::ShowWindow(dwCommand);
	return S_OK;
}

STDMETHODIMP CScrollControl::SetBitmap(HBITMAP hBitmap)
{
	m_bitmap = hBitmap;
	Invalidate();
	RedrawWindow(0, 0, RDW_UPDATENOW);
	return S_OK;
}

STDMETHODIMP CScrollControl::Scroll(BOOL bFromRightToLeft)
{
	CRect rect;
	GetClientRect(&rect);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pAnimationManagerService->CreateAnimation(CLSID_AccelerateDecelerateAnimation, &m_pAnimation));
	RETURN_IF_FAILED(AtlAdvise(m_pAnimation, pUnk, __uuidof(IAnimationEventSink), &m_dwAdviceAnimation));
	if (bFromRightToLeft)
	{
		RETURN_IF_FAILED(m_pAnimation->SetParams(0, rect.Width(), 0.5));
	}
	else
	{
		RETURN_IF_FAILED(m_pAnimation->SetParams(rect.Width(), 0, 0.5));
	}
	RETURN_IF_FAILED(m_pAnimation->Run());

	return S_OK;
}

LRESULT CScrollControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

STDMETHODIMP CScrollControl::OnAnimation()
{
	int iValue = 0;
	RETURN_IF_FAILED(m_pAnimation->GetCurrentIntValue(&iValue));

	m_dx = iValue;
	CRect rectUpdate;
	Invalidate();
	RedrawWindow(0, 0, RDW_UPDATENOW);
	RedrawWindow(rectUpdate, 0, RDW_UPDATENOW | RDW_NOERASE);

	BOOL bComplete = FALSE;
	RETURN_IF_FAILED(m_pAnimation->IsAnimationComplete(&bComplete));
	if (bComplete)
	{
		RETURN_IF_FAILED(AtlUnadvise(m_pAnimation, __uuidof(IAnimationEventSink), m_dwAdviceAnimation));
		m_pAnimation.Release();
		m_dwAdviceAnimation = 0;
		m_pCustomTabControl->OnEndScroll();
		return 0;
	}
	return S_OK;
}

LRESULT CScrollControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bitmap.IsNull())
		return 0;

	PAINTSTRUCT ps = { 0 };
	CDCHandle cdc(BeginPaint(&ps));

	CRect rect = ps.rcPaint;
	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(cdc);
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, m_bitmap);

	cdc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), cdcBitmap, m_dx + rect.left, 0, SRCCOPY);

	EndPaint(&ps);
	return 0;
}

STDMETHODIMP CScrollControl::SetTabControl(ICustomTabControlInternal* pCustomTabControl)
{
	m_pCustomTabControl = pCustomTabControl;
	return S_OK;
}