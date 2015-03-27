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

STDMETHODIMP CScrollControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_AnimationService, &m_pAnimationService));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pAnimationService, pUnk));
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pAnimationService, pServiceProvider));
	RETURN_IF_FAILED(HrInitializeWithControl(m_pAnimationService, pUnk))
	RETURN_IF_FAILED(AtlAdvise(m_pAnimationService, pUnk, __uuidof(IAnimationServiceEventSink), &m_dwAdvice));
	return S_OK;
}

STDMETHODIMP CScrollControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pAnimationService, __uuidof(IAnimationServiceEventSink), m_dwAdvice))
	RETURN_IF_FAILED(HrNotifyOnShutdown(m_pAnimationService));
	m_pAnimationService.Release();
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
	__super::Create(hWndParent, 0, L"", WS_CHILD, WS_EX_COMPOSITED);
	if (hWnd)
		*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CScrollControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	return S_OK;
}

LRESULT CScrollControl::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CComQIPtr<IMsgHandler> p = m_pAnimationService;
	if (p)
	{
		LRESULT lResult = 0;
		ASSERT_IF_FAILED(p->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bHandled));
	}
	return 0;
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

STDMETHODIMP CScrollControl::ScrollY(BOOL bFromDownToTop, DWORD distance, DWORD steps, DWORD timerInterval)
{
	CRect rect;
	GetClientRect(&rect);

	if (!distance)
		distance = rect.Height();

	if (!steps)
		steps = STEPS_DEFAULT;

	if (!timerInterval)
		timerInterval = TARGET_INTERVAL;

	m_dwSteps = steps;

	if (bFromDownToTop)
	{
		RETURN_IF_FAILED(m_pAnimationService->SetParams(0, distance, m_dwSteps, timerInterval));
	}
	else
	{
		RETURN_IF_FAILED(m_pAnimationService->SetParams(distance, 0, m_dwSteps, timerInterval));
	}
	m_bFromDownToTop = bFromDownToTop;
	m_bVertical = TRUE;
	RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());
	return S_OK;
}

STDMETHODIMP CScrollControl::ScrollX(BOOL bFromRightToLeft, DWORD distance, DWORD steps, DWORD timerInterval)
{
	CRect rect;
	GetClientRect(&rect);

	if (!distance)
		distance = rect.Height();

	if (!steps)
		steps = STEPS_DEFAULT;

	if (!timerInterval)
		timerInterval = TARGET_INTERVAL;

	m_dwSteps = steps;

	if (bFromRightToLeft)
	{
		RETURN_IF_FAILED(m_pAnimationService->SetParams(0, rect.Width(), m_dwSteps, timerInterval));
	}
	else
	{
		RETURN_IF_FAILED(m_pAnimationService->SetParams(rect.Width(), 0, m_dwSteps, timerInterval));
	}
	m_bFromRightToLeft = bFromRightToLeft;
	m_bVertical = FALSE;
	RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());
	return S_OK;
}

LRESULT CScrollControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

STDMETHODIMP CScrollControl::OnAnimationStep(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep)
{
	if (pAnimationService == m_pAnimationService)
	{
		CRect rectUpdate;
		Invalidate();
		RedrawWindow(0, 0, RDW_UPDATENOW);
		RedrawWindow(rectUpdate, 0, RDW_UPDATENOW | RDW_NOERASE);

		if (dwStep == m_dwSteps)
		{
			m_pCustomTabControl->OnEndScroll();
			return S_OK;
		}
		RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());
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
#ifdef _DEBUG
	using namespace boost;
	CString str;
	str.Format(
		L"CScrollControl::OnPaint - ps.rcPaint = (%s, %s - %s, %s)\n",
		lexical_cast<wstring>(rect.left).c_str(),
		lexical_cast<wstring>(rect.top).c_str(),
		lexical_cast<wstring>(rect.right).c_str(),
		lexical_cast<wstring>(rect.bottom).c_str()
		);
	OutputDebugString(str);
#endif

	CDC cdcBitmap;
	cdcBitmap.CreateCompatibleDC(cdc);
	CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, m_bitmap);

	DWORD dx = 0;
	ASSERT_IF_FAILED(m_pAnimationService->GetCurrentValue(&dx));
	if (m_bVertical)
		cdc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), cdcBitmap, rect.left, dx, SRCCOPY);
	else
		cdc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), cdcBitmap, dx + rect.left, 0, SRCCOPY);

	EndPaint(&ps);
	return 0;
}

STDMETHODIMP CScrollControl::SetTabControl(IScrollControlEventSink* pCustomTabControl)
{
	m_pCustomTabControl = pCustomTabControl;
	return S_OK;
}