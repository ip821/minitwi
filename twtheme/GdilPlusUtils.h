#pragma once

#include <memory>
#include <gdiplus.h>
#include <atlapp.h>
#include <atlmisc.h>

using namespace std;
using namespace Gdiplus;

static void ResizeDownImage(std::shared_ptr<Bitmap>& bmp, UINT height)
{
	std::shared_ptr<Bitmap> originalBitmap = bmp;
	UINT bmpHeight = originalBitmap->GetHeight();
	UINT bmpWidth = originalBitmap->GetWidth();
	double ratio = ((double)bmpWidth) / ((double)bmpHeight);
	if (bmpHeight <= height)
	{
		return;
	}

	bmp.reset();
	auto width = static_cast<INT>(height * ratio);
	bmp = std::make_shared<Bitmap>(width, height, originalBitmap->GetPixelFormat());
	Graphics graphics(bmp.get());
	graphics.DrawImage(originalBitmap.get(), 0, 0, width, height);
}

static HRESULT GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;
	UINT  size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	if (GetImageEncodersSize(&num, &size) != Status::Ok)
		return E_FAIL;

	if (size == 0)
		return E_FAIL;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return E_FAIL;

	if (GetImageEncoders(num, size, pImageCodecInfo) != Status::Ok)
		return E_FAIL;

	for (UINT j = 0; j < num; ++j)
	{
#pragma warning(suppress: 6385)
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return S_OK;
		}
	}

	free(pImageCodecInfo);
	return E_FAIL;
}

static void DrawRoundedRect(CDCHandle& cdc, CRect rectText, bool strictRect = false, COLORREF colorRefBrush = 0)
{
	const int diff_x = 10;
	const int diff_y = 0; // diff_x / 2;

	CRgn rgn;
	if (strictRect)
		rgn.CreateRoundRectRgn(rectText.left, rectText.top, rectText.right, rectText.bottom, 10, 10);
	else
		rgn.CreateRoundRectRgn(rectText.left - diff_x, rectText.top - diff_y, rectText.right + diff_x, rectText.bottom + diff_y, 10, 10);

	Gdiplus::Region r(rgn);
	Gdiplus::Graphics g(cdc);
	Color colorBrush(0x882F4F4F);
	if (colorRefBrush)
		colorBrush.SetFromCOLORREF(colorRefBrush);
	Gdiplus::SolidBrush brush(colorBrush);
	g.FillRegion(&brush, &r);
}

class CDCSelectBitmapManualScope
{
private:
	CDCHandle m_cdc;
	CBitmapHandle m_newBitmap;
	CBitmapHandle m_oldBitmap;
public:
	CDCSelectBitmapManualScope()
	{
	}

	void SelectBitmap(HDC hdc, HBITMAP hBitmap)
	{
		m_cdc = hdc;
		m_newBitmap = hBitmap;
		m_oldBitmap = m_cdc.SelectBitmap(m_newBitmap);
	}

	virtual ~CDCSelectBitmapManualScope()
	{
		if (!m_oldBitmap.IsNull())
			m_cdc.SelectBitmap(m_oldBitmap);
	}
};

class CDCSelectBitmapScope : CDCSelectBitmapManualScope
{
public:
	CDCSelectBitmapScope(HDC hdc, HBITMAP hBitmap)
	{
		SelectBitmap(hdc, hBitmap);
	}
};

class CDCSelectFontScope
{
private:
	CDCHandle m_cdc;
	CFontHandle m_oldFont;
public:
	CDCSelectFontScope(HDC hdc, HFONT hFont)
	{
		m_cdc = hdc;
		m_oldFont = m_cdc.SelectFont(hFont);
	}

	virtual ~CDCSelectFontScope()
	{
		m_cdc.SelectFont(m_oldFont);
	}
};

class CDoubleBufferScope
{
	HDC m_hdc;
	CRect m_rect;
	CDC m_cdc;
	CBitmap m_bitmap;
	shared_ptr<CDCSelectBitmapScope> m_scope;

public:
	CDoubleBufferScope(HDC hdc, CRect& rect)
	{
		m_hdc = hdc;
		m_rect = rect;
		m_cdc.CreateCompatibleDC(hdc);
		m_bitmap.CreateCompatibleBitmap(hdc, rect.Width(), rect.Height());
		m_scope = make_shared<CDCSelectBitmapScope>(m_cdc, m_bitmap);
	}

	HDC GetHDC()
	{
		return m_cdc;
	}

	~CDoubleBufferScope()
	{
		BitBlt(m_hdc, m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(), m_cdc, 0, 0, SRCCOPY);
	}
};