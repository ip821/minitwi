#pragma once

#include <memory>
#include <gdiplus.h>

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