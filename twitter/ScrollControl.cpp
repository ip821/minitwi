#include "stdafx.h"
#include "ScrollControl.h"
#include "CustomTabControl.h"

void CScrollControl::SetBitmap(HBITMAP hBitmap)
{
	m_bitmap = hBitmap;

	CRect rect;
	GetClientRect(&rect);

	if (!m_pD2D1HwndRenderTarget)
	{
		CComPtr<ID2D1Factory> pD2D1Factory;
		ASSERT_IF_FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2D1Factory));
		ASSERT_IF_FAILED(pD2D1Factory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, D2D1::SizeU(rect.Width(), rect.Height())),
			&m_pD2D1HwndRenderTarget
			));
	}

	if (m_pD2D1Bitmap)
		m_pD2D1Bitmap.Release();

	CComPtr<IWICImagingFactory> pWICImagingFactory;
	ASSERT_IF_FAILED(pWICImagingFactory.CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER));

	CComPtr<IWICBitmap> pWICBitmap;
	ASSERT_IF_FAILED(pWICImagingFactory->CreateBitmapFromHBITMAP(m_bitmap.m_hBitmap, NULL, WICBitmapIgnoreAlpha, &pWICBitmap));

	CComPtr<IWICFormatConverter> pWICFormatConverter;
	ASSERT_IF_FAILED(pWICImagingFactory->CreateFormatConverter(&pWICFormatConverter));
	ASSERT_IF_FAILED(pWICFormatConverter->Initialize(pWICBitmap, GUID_WICPixelFormat32bppBGR, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeMedianCut));

	CComPtr<IWICBitmapSource> pWICBitmapConverted;
	WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, pWICBitmap, &pWICBitmapConverted);
	ASSERT_IF_FAILED(m_pD2D1HwndRenderTarget->CreateBitmapFromWicBitmap(pWICBitmapConverted, &m_pD2D1Bitmap));

	Invalidate();
}

void CScrollControl::Scroll(BOOL bFromRightToLeft)
{
	CRect rect;
	GetClientRect(&rect);

	m_scrollAmount = rect.Width() / STEPS;
	m_step = 0;

	if (bFromRightToLeft)
	{
		m_dx = 0;
	}
	else
	{
		m_dx = rect.Width();
		m_scrollAmount = -m_scrollAmount;
	}
	m_bFromRightToLeft = bFromRightToLeft;
	SetTimer(1, 50);
}

LRESULT CScrollControl::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_dx += m_scrollAmount;
	m_step++;
	if (m_step == STEPS)
	{
		KillTimer(1);
		m_pCustomTabControl->OnEndScroll();
		return 0;
	}
	//CRect rectUpdate;
	//ScrollWindowEx(m_scrollAmount * STEPS, 0, MAKELONG(100, SW_SMOOTHSCROLL), 0, 0, 0, &rectUpdate);
	////ScrollWindowEx(m_scrollAmount, 0, SW_INVALIDATE, 0, 0, 0, &rectUpdate);
	Invalidate();
	return 0;
}

LRESULT CScrollControl::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CScrollControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_bitmap.IsNull())
		return 0;
	PAINTSTRUCT ps = { 0 };
	CDCHandle cdc(BeginPaint(&ps));

	CRect rect;
	GetClientRect(&rect);

	D2D1_RECT_F rectF;
	rectF.left = rect.left;
	rectF.right = rect.right;
	rectF.top = rect.top;
	rectF.bottom = rect.bottom;

	D2D1_RECT_F rcDraw;
	rcDraw.left = m_dx;
	rcDraw.bottom = rect.bottom;
	rcDraw.right = rect.right;
	rcDraw.top = rect.top;

	m_pD2D1HwndRenderTarget->BeginDraw();
	m_pD2D1HwndRenderTarget->DrawBitmap(m_pD2D1Bitmap, rectF, 0.f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, rcDraw);
	ASSERT_IF_FAILED(m_pD2D1HwndRenderTarget->EndDraw());

	//CRect rect;
	//GetClientRect(&rect);

	//CDC cdcBitmap;
	//cdcBitmap.CreateCompatibleDC(cdc);
	//cdcBitmap.SelectBitmap(m_bitmap);

	//BitBlt(cdc, rect.left, rect.top, rect.Width(), rect.Height(), cdcBitmap, m_dx, 0, SRCCOPY);

	EndPaint(&ps);
	return 0;
}

void CScrollControl::SetTabWindow(CCustomTabControl* pCustomTabControl)
{
	m_pCustomTabControl = pCustomTabControl;
}