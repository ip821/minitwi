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
		if (status != Gdiplus::Status::Ok)
			return E_FAIL;
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

STDMETHODIMP CImageManagerService::CopyTo(IImageManagerService* pDest)
{
	CHECK_E_POINTER(pDest);
	{
		lock_guard<mutex> mutexSource(m_mutex);
		CImageManagerService* pDestClass = static_cast<CImageManagerService*>(pDest);
		lock_guard<mutex> mutexDest(pDestClass->m_mutex);
		for (auto& it : m_bitmaps)
		{
			pDestClass->m_bitmaps[it.first] = it.second;
		}
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::CopyImageTo(BSTR bstrKey, IImageManagerService* pDest)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(pDest);
	{
		lock_guard<mutex> mutexSource(m_mutex);
		CImageManagerService* pDestClass = static_cast<CImageManagerService*>(pDest);
		lock_guard<mutex> mutexDest(pDestClass->m_mutex);
		pDestClass->m_bitmaps[bstrKey] = m_bitmaps[bstrKey];
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::AddImage(BSTR bstrKey, BSTR bstrFileName)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(bstrFileName);
	{
		lock_guard<mutex> mutex(m_mutex);
		m_bitmaps[bstrKey] = std::make_shared<Gdiplus::Bitmap>(bstrFileName);
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::ContainsImageKey(BSTR bstrKey, BOOL* pbContains)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(pbContains);
	{
		lock_guard<mutex> mutex(m_mutex);
		*pbContains = m_bitmaps.find(CComBSTR(bstrKey)) != m_bitmaps.end();
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::RemoveImage(BSTR bstrKey)
{
	CHECK_E_POINTER(bstrKey);
	{
		lock_guard<mutex> mutex(m_mutex);
		m_bitmaps.erase(bstrKey);
	}
	return S_OK;
}