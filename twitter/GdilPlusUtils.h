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
