// FormsService.cpp : Implementation of CFormsService

#include "stdafx.h"
#include "FormsService.h"


// CFormsService

STDMETHODIMP CFormsService::OnInitializing(IServiceProvider* pServiceProvider)
{
	CComQIPtr<IMainWindow> pWindow = m_pControl;
	RETURN_IF_FAILED(pWindow->SetFlags(MainWindowFlags::MainWindowNone));
	return S_OK;
}

STDMETHODIMP CFormsService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CComQIPtr<IMainWindow> pWindow = m_pControl;

	CComPtr<IUnknown> pUnk2;
	RETURN_IF_FAILED(GetPluginManager()->CreatePluginInstance(PNAMESP_HOSTFORM, PVIEWTYPE_CONTAINERWINDOW, CONTROL_TABCONTAINER, &pUnk2));
	CComQIPtr<IContainerControl> pContainerWindow = pUnk2;

	RETURN_IF_FAILED(pWindow->SetContainerControl(pContainerWindow));

	CComPtr<IFormManager> pFormManager;
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));

	CComPtr<IObjectArray> pObjectArray;
	RETURN_IF_FAILED(GetPluginManager()->GetPluginInfoCollection(PNAMESP_HOSTFORM, PVIEWTYPE_INPLACE_CONTROL, &pObjectArray));

	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IPluginInfo> pPluginInfo;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, __uuidof(IPluginInfo), (LPVOID*)&pPluginInfo));
		GUID gId = GUID_NULL;
		RETURN_IF_FAILED(pPluginInfo->GetId(&gId));
		CComPtr<IControl> pControl;
		RETURN_IF_FAILED(pFormManager->OpenForm(gId, &pControl));
	}

	return S_OK;
}

STDMETHODIMP CFormsService::OnShutdown()
{
	return S_OK;
}
