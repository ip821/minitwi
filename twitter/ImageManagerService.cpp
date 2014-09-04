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
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(bstrFileName);
	{
		lock_guard<mutex> mutex(m_mutex);
		ATLASSERT(m_bitmapRefs.find(bstrKey) == m_bitmapRefs.end());
		m_bitmaps[bstrKey] = std::make_shared<Gdiplus::Bitmap>(bstrFileName);
		m_bitmapRefs[bstrKey] = 1;
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::ContainsImageKey(BSTR bstrKey, BOOL* pbContains)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(pbContains);
	{
		lock_guard<mutex> mutex(m_mutex);
		*pbContains = m_bitmaps.find(bstrKey) != m_bitmaps.end();
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::AddImageRef(BSTR bstrKey)
{
	CHECK_E_POINTER(bstrKey);
	{
		lock_guard<mutex> mutex(m_mutex);
		m_bitmapRefs[bstrKey]++;
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::RemoveImageRef(BSTR bstrKey)
{
	CHECK_E_POINTER(bstrKey);
	{
		lock_guard<mutex> mutex(m_mutex);
		m_bitmapRefs[bstrKey]--;
		if (m_bitmapRefs[bstrKey] <= 0)
		{
			m_bitmapRefs.erase(bstrKey);
			m_bitmaps.erase(bstrKey);
		}
	}
	return S_OK;
}