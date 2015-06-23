// PictureWindowCopyCommand.cpp : Implementation of CPictureWindowCopyCommand

#include "stdafx.h"
#include "PictureWindowCopyCommand.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

// CPictureWindowCopyCommand

STDMETHODIMP CPictureWindowCopyCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CHECK_E_POINTER(bstrText);
	if (guidCommand == COMMAND_COPY_IMAGE_URL)
		*bstrText = CComBSTR(L"Copy link\tCtrl+C").Detach();
	return S_OK;
}

STDMETHODIMP CPictureWindowCopyCommand::InstallMenu(IMenu* pMenu)
{
	CHECK_E_POINTER(pMenu);
	RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_COPY_IMAGE_URL, this));
	return S_OK;
}

STDMETHODIMP CPictureWindowCopyCommand::Invoke(REFGUID guidCommand)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CComPtr<IVariantObject> pTempObject = m_pVariantObject;
	m_pVariantObject.Release();

	if (OpenClipboard(NULL))
	{
		if (EmptyClipboard())
		{
			std::wstring str;

			CComVar vUrl;
			RETURN_IF_FAILED(pTempObject->GetVariantValue(Twitter::Metadata::Object::Url, &vUrl));

			if (vUrl.vt == VT_BSTR)
			{
				str = vUrl.bstrVal;
			}

			if (str.empty())
			{
				CComVar vMediaUrl;
				RETURN_IF_FAILED(pTempObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &vMediaUrl));
				str = vMediaUrl.bstrVal;
			}

			size_t nLength = _tcslen(str.c_str());
			size_t nByteOfBuffer = (nLength + 1) * sizeof(TCHAR);
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, nByteOfBuffer);
			if (NULL == hGlobal)
			{
				CloseClipboard();
				return S_OK;
			}

			LPTSTR pBuf = (LPTSTR)GlobalLock(hGlobal);
			if (NULL == pBuf)
			{
				GlobalFree(hGlobal);
				hGlobal = NULL;
				return HRESULT_FROM_WIN32(GetLastError());
			}

			_tcscpy_s(pBuf, nLength + 1, str.c_str());
			SetClipboardData(CF_UNICODETEXT, hGlobal);
			GlobalUnlock(hGlobal);
			CloseClipboard();
		}
	}
	return S_OK;
}

STDMETHODIMP CPictureWindowCopyCommand::SetVariantObject(IVariantObject* pVariantObject)
{
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CPictureWindowCopyCommand::GetAccelerator(REFGUID guidCommand, TACCEL *pAccel)
{
	CHECK_E_POINTER(pAccel);
	if (guidCommand == COMMAND_COPY_IMAGE_URL)
		*pAccel = { FVIRTKEY | FCONTROL, 0x43, 0 }; //CTRL-C
	return S_OK;
}