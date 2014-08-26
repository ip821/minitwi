// ImageManagerService.h : Declaration of the CImageManagerService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;
using namespace std;

// CImageManagerService

class ATL_NO_VTABLE CImageManagerService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CImageManagerService, &CLSID_ImageManagerService>,
	public IImageManagerService
{
public:
	CImageManagerService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_IMAGEMANAGERSERVICE)

	BEGIN_COM_MAP(CImageManagerService)
		COM_INTERFACE_ENTRY(IImageManagerService)
	END_COM_MAP()

private:
	std::map<CComBSTR, std::shared_ptr<Gdiplus::Bitmap>> m_bitmaps;
	mutex m_mutex;
public:

	STDMETHOD(GetImageInfo)(BSTR bstrKey, TBITMAP* phBitmap);
	STDMETHOD(CreateImageBitmap)(BSTR bstrKey, HBITMAP* ptBitmap);
	STDMETHOD(SetImage)(BSTR bstrKey, BSTR bstrFileName);
	STDMETHOD(ContainsImageKey)(BSTR bstrKey, BOOL* pbContains);
	STDMETHOD(RemoveImage)(BSTR bstrKey);
};

OBJECT_ENTRY_AUTO(__uuidof(ImageManagerService), CImageManagerService)
