// ImageManagerService.cpp : Implementation of CImageManagerService

#include "stdafx.h"
#include "ImageManagerService.h"


// CImageManagerService

STDMETHODIMP CImageManagerService::GetImage(BSTR bstrKey, TBITMAP* phBitmap)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(phBitmap);

	{
		lock_guard<mutex> mutex(m_mutex);
		auto it = m_bitmaps.find(CComBSTR(bstrKey));
		if (it == m_bitmaps.end())
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

		auto status = it->second->GetHBITMAP(Gdiplus::Color::Transparent, &phBitmap->hBitmap);
		phBitmap->Width = it->second->GetWidth();
		phBitmap->Height = it->second->GetHeight();
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::SetImage(BSTR bstrKey, BSTR bstrFileName)
{
	{
		lock_guard<mutex> mutex(m_mutex);
		m_bitmaps[CComBSTR(bstrKey)] = std::make_shared<Gdiplus::Bitmap>(bstrFileName);
	}
	return S_OK;
}
