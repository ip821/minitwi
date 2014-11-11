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

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return E_FAIL;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return E_FAIL;

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
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