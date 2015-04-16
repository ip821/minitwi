// ImageManagerService.cpp : Implementation of CImageManagerService

#include "stdafx.h"
#include "ImageManagerService.h"
#include "..\twtheme\GdilPlusUtils.h"
#include "..\model-libs\json\src\base64.h"

// CImageManagerService

STDMETHODIMP CImageManagerService::CreateImageBitmap(BSTR bstrKey, HBITMAP* phBitmap)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(phBitmap);

	{
		boost::lock_guard<boost::mutex> mutex(m_mutex);
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
		boost::lock_guard<boost::mutex> mutex(m_mutex);
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
	ATLASSERT(pDest != this);
	{
		boost::lock_guard<boost::mutex> mutex(m_mutex);
		CImageManagerService* pDestClass = static_cast<CImageManagerService*>(pDest);
		boost::lock_guard<boost::mutex> mutexDest(pDestClass->m_mutex);
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
		boost::lock_guard<boost::mutex> mutexSource(m_mutex);
		CImageManagerService* pDestClass = static_cast<CImageManagerService*>(pDest);
		boost::lock_guard<boost::mutex> mutexDest(pDestClass->m_mutex);
		pDestClass->m_bitmaps[bstrKey] = m_bitmaps[bstrKey];
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::AddImageFromHBITMAP(BSTR bstrKey, HBITMAP hBitmap)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(hBitmap);
	{
		boost::lock_guard<boost::mutex> mutex(m_mutex);
		m_bitmaps[bstrKey] = std::make_shared<Gdiplus::Bitmap>(hBitmap, (HPALETTE)0);
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::AddImageFromStream(BSTR bstrKey, IStream* pStream)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(pStream);
	{
		boost::lock_guard<boost::mutex> mutex(m_mutex);
		auto pBitmap = std::make_shared<Gdiplus::Bitmap>(pStream);
		m_bitmaps[bstrKey] = pBitmap;
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::AddImageFromFile(BSTR bstrKey, BSTR bstrFileName)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(bstrFileName);
	{
		boost::lock_guard<boost::mutex> mutex(m_mutex);
		m_bitmaps[bstrKey] = std::make_shared<Gdiplus::Bitmap>(bstrFileName);
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::ContainsImageKey(BSTR bstrKey, BOOL* pbContains)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(pbContains);
	{
		boost::lock_guard<boost::mutex> mutex(m_mutex);
		*pbContains = m_bitmaps.find(CComBSTR(bstrKey)) != m_bitmaps.end();
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::RemoveImage(BSTR bstrKey)
{
	CHECK_E_POINTER(bstrKey);
	{
		boost::lock_guard<boost::mutex> mutex(m_mutex);
		m_bitmaps.erase(bstrKey);
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::SaveImage(BSTR bstrKey, BSTR bstrFilePath)
{
	CHECK_E_POINTER(bstrKey);
	CHECK_E_POINTER(bstrFilePath);
	{
		boost::lock_guard<boost::mutex> mutex(m_mutex);
		auto it = m_bitmaps.find(CComBSTR(bstrKey));
		if (it == m_bitmaps.end())
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
		CString strExt = PathFindExtension(bstrFilePath);
		strExt.Replace(L".", L"");
		strExt.Replace(L"jpg", L"jpeg");
		auto strMimeType = CString(L"image/") + strExt;
		CLSID clsid = GUID_NULL;
		RETURN_IF_FAILED(GetEncoderClsid(strMimeType, &clsid));
		auto status = it->second->Save(bstrFilePath, &clsid, NULL);
		if (status != Ok)
			return E_FAIL;
	}
	return S_OK;
}

STDMETHODIMP CImageManagerService::Initialize(IObjCollection* pObjectCollection)
{
	CHECK_E_POINTER(pObjectCollection);
	UINT_PTR uiCount = 0;
	RETURN_IF_FAILED(pObjectCollection->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pImageObject;
		RETURN_IF_FAILED(pObjectCollection->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pImageObject));

		CComVariant vName;
		CComVariant vType;
		CComVariant vImageDataBase64;

		RETURN_IF_FAILED(pImageObject->GetVariantValue(L"name", &vName));
		RETURN_IF_FAILED(pImageObject->GetVariantValue(L"type", &vType));
		RETURN_IF_FAILED(pImageObject->GetVariantValue(L"data", &vImageDataBase64));

		ATLASSERT(vName.vt == VT_BSTR && vType.vt == VT_BSTR && vImageDataBase64.vt == VT_BSTR);

		USES_CONVERSION;
		auto strData = string(CW2A(vImageDataBase64.bstrVal));
		auto strDataDecoded = base64_decode(strData);
		auto lpszDataDecoded = strDataDecoded.c_str();
		CComPtr<IStream> pStream;
		pStream.p = SHCreateMemStream((BYTE*)lpszDataDecoded, strDataDecoded.size());
		RETURN_IF_FAILED(AddImageFromStream(vName.bstrVal, pStream));
	}
	return S_OK;
}