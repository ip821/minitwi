// ImageManagerService.cpp : Implementation of CImageManagerService

#include "stdafx.h"
#include "ImageManagerService.h"


// CImageManagerService

STDMETHODIMP CImageManagerService::CreateImageBitmap(BSTR bstrKey, HBITMAP* phBitmap)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(phBitmap);

	{
		lock_guard<mutex> mutex(m_mutex);
		auto it = m_bitmaps.find(CComBSTR(bstrKey));
		if (it == m_bitmaps.end())
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

		auto status = it->second->GetHBITMAP(Gdiplus::Color::Transparent, phBitmap);
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::GetImageInfo(BSTR bstrKey, TBITMAP* ptBitmap)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(ptBitmap);

	{
		lock_guard<mutex> mutex(m_mutex);
		auto it = m_bitmaps.find(CComBSTR(bstrKey));
		if (it == m_bitmaps.end())
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

		ptBitmap->Width = it->second->GetWidth();
		ptBitmap->Height = it->second->GetHeight();
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

STDMETHODIMP CImageManagerService::ContainsImageKey(BSTR bstrKey, BOOL* pbContains)
{
	{
		lock_guard<mutex> mutex(m_mutex);
		*pbContains = m_bitmaps.find(bstrKey) != m_bitmaps.end();
	}
	return S_OK;
}