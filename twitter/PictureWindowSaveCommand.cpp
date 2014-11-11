#include "stdafx.h"
#include "PictureWindowSaveCommand.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

// CPictureWindowSaveCommand

STDMETHODIMP CPictureWindowSaveCommand::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	return S_OK;
}

STDMETHODIMP CPictureWindowSaveCommand::OnShutdown()
{
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CPictureWindowSaveCommand::GetCommandText(REFGUID guidCommand, BSTR* bstrText)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CHECK_E_POINTER(bstrText);
	if (guidCommand == COMMAND_SAVE_IMAGE_URL)
		*bstrText = CComBSTR(L"Save...\tCtrl+S").Detach();
	return S_OK;
}

STDMETHODIMP CPictureWindowSaveCommand::InstallMenu(IMenu* pMenu)
{
	CHECK_E_POINTER(pMenu);
	RETURN_IF_FAILED(pMenu->AddMenuCommand(GUID_NULL, COMMAND_SAVE_IMAGE_URL, this));
	return S_OK;
}

STDMETHODIMP CPictureWindowSaveCommand::Invoke(REFGUID guidCommand)
{
	UNREFERENCED_PARAMETER(guidCommand);
	CComPtr<IVariantObject> pTempObject = m_pVariantObject;
	m_pVariantObject.Release();

	CComPtr<IImageManagerService> pImageManagerService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &pImageManagerService));

	CComVariant vMediaUrl;
	RETURN_IF_FAILED(pTempObject->GetVariantValue(VAR_TWITTER_MEDIAURL, &vMediaUrl));

	if (vMediaUrl.vt != VT_BSTR)
	{
		return S_OK;
	}

	CString strFileName(vMediaUrl.bstrVal);
	CFileDialog dlg(FALSE, NULL, PathFindFileName(strFileName));
	dlg.m_ofn.Flags |= OFN_OVERWRITEPROMPT;

	if (dlg.DoModal() != IDOK)
		return S_OK;
	
	RETURN_IF_FAILED(pImageManagerService->SaveImage(vMediaUrl.bstrVal, CComBSTR(dlg.m_ofn.lpstrFile)));

	return S_OK;
}

STDMETHODIMP CPictureWindowSaveCommand::SetVariantObject(IVariantObject* pVariantObject)
{
	m_pVariantObject = pVariantObject;
	return S_OK;
}

STDMETHODIMP CPictureWindowSaveCommand::GetAccelerator(REFGUID guidCommand, TACCEL *pAccel)
{
	CHECK_E_POINTER(pAccel);
	if (guidCommand == COMMAND_SAVE_IMAGE_URL)
		*pAccel = { FVIRTKEY | FCONTROL, 0x53, 0 }; //CTRL-S
	return S_OK;
}

STDMETHODIMP CPictureWindowSaveCommand::GetEnabled(REFGUID guidCommand, BOOL *pbEnabled)
{
	CHECK_E_POINTER(pbEnabled);
	
	if (guidCommand != COMMAND_SAVE_IMAGE_URL)
		return S_OK;

	if (!m_pServiceProvider || !m_pVariantObject)
	{
		*pbEnabled = FALSE;
		return S_OK;
	}

	CComPtr<IImageManagerService> pImageManagerService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &pImageManagerService));

	CComVariant vMediaUrl;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(VAR_TWITTER_MEDIAURL, &vMediaUrl));

	if (vMediaUrl.vt != VT_BSTR)
	{
		*pbEnabled = FALSE;
		return S_OK;
	}

	RETURN_IF_FAILED(pImageManagerService->ContainsImageKey(vMediaUrl.bstrVal, pbEnabled));

	return S_OK;
}
