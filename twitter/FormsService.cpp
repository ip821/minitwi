// FormsService.cpp : Implementation of CFormsService

#include "stdafx.h"
#include "FormsService.h"


// CFormsService

STDMETHODIMP CFormsService::Load(ISettings* pSettings)
{
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CFormsService::OnInitializing(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	CComQIPtr<IMainWindow> pWindow = m_pControl;
	RETURN_IF_FAILED(pWindow->SetFlags(MainWindowFlags::MainWindowNone));

	CComPtr<IUnknown> pUnk2;
	RETURN_IF_FAILED(GetPluginManager()->CreatePluginInstance(PNAMESP_HOSTFORM, PVIEWTYPE_CONTAINERWINDOW, CONTROL_TABCONTAINER, &pUnk2));
	CComQIPtr<IContainerControl> pContainerWindow = pUnk2;

	RETURN_IF_FAILED(pWindow->SetContainerControl(pContainerWindow));

	CComQIPtr<IControl> pControl = pContainerWindow;
	HWND hwnd = 0;
	RETURN_IF_FAILED(pControl->GetHWND(&hwnd));

	LONG lStyle = GetWindowLong(hwnd, GWL_STYLE);
	lStyle &= ~(WS_BORDER);
	SetWindowLong(hwnd, GWL_STYLE, lStyle);

	InvalidateRect(hwnd, NULL, TRUE);

	return S_OK;
}

STDMETHODIMP CFormsService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	InvalidateRect(m_hControlWnd, NULL, TRUE);
	m_pServiceProvider = pServiceProvider;
	CComQIPtr<IMainWindow> pWindow = m_pControl;

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &m_pFormManager));

	CComPtr<IObjArray> pObjectArray;
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
		RETURN_IF_FAILED(m_pFormManager->OpenForm(gId, &pControl));
	}

	RETURN_IF_FAILED(m_pFormManager->ActivateForm(CLSID_HomeTimeLineControl));

	return S_OK;
}

STDMETHODIMP CFormsService::OnShutdown()
{
	m_pFormManager.Release();
	m_pSettings.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CFormsService::Close(IControl *pControl)
{
	CComQIPtr<IPluginSupportNotifications> pPluginSupportNotifications = pControl;
	if (pPluginSupportNotifications)
	{
		RETURN_IF_FAILED(pPluginSupportNotifications->OnShutdown());
	}
	RETURN_IF_FAILED(m_pFormManager->CloseForm(pControl));
	return S_OK;
}

STDMETHODIMP CFormsService::OpenForm(GUID gId, IVariantObject* pVariantObject)
{
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(m_pFormManager->OpenForm(gId, &pControl));

	CComPtr<IThemeService> pThemeService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ThemeService, &pThemeService));
	CComPtr<ITheme> pTheme;
	RETURN_IF_FAILED(pThemeService->GetCurrentTheme(&pTheme));
	CComQIPtr<IThemeSupport> pThemeSupport = pControl;
	if (pThemeSupport)
	{
		RETURN_IF_FAILED(pThemeSupport->SetTheme(pTheme));
	}

	RETURN_IF_FAILED(HrInitializeWithControl(pControl, m_pControl));
	RETURN_IF_FAILED(CopyImages(pControl));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(pControl, pVariantObject));
	RETURN_IF_FAILED(m_pFormManager->ActivateForm2(pControl));

	return S_OK;
}

STDMETHODIMP CFormsService::CopyImages(IControl* pControl)
{
	CComPtr<IControl> pCurrentControl;
	RETURN_IF_FAILED(m_pFormManager->FindForm(CLSID_HomeTimeLineControl, &pCurrentControl));
	ATLASSERT(pCurrentControl);
	CComQIPtr<IServiceProviderSupport> pCurrentControlServiceProviderSupport = pCurrentControl;
	ATLASSERT(pCurrentControlServiceProviderSupport);
	CComPtr<IServiceProvider> pCurrentControlServiceProvider;
	RETURN_IF_FAILED(pCurrentControlServiceProviderSupport->GetServiceProvider(&pCurrentControlServiceProvider));
	CComPtr<IImageManagerService> pCurrentControlImageService;
	RETURN_IF_FAILED(pCurrentControlServiceProvider->QueryService(CLSID_ImageManagerService, &pCurrentControlImageService));

	CComQIPtr<IServiceProviderSupport> pUserInfoServiceProviderSupport = pControl;
	ATLASSERT(pUserInfoServiceProviderSupport);
	CComPtr<IServiceProvider> pUserInfoServiceProvider;
	RETURN_IF_FAILED(pUserInfoServiceProviderSupport->GetServiceProvider(&pUserInfoServiceProvider));
	CComPtr<IImageManagerService> pUserInfoImageManagerService;
	RETURN_IF_FAILED(pUserInfoServiceProvider->QueryService(CLSID_ImageManagerService, &pUserInfoImageManagerService));

	RETURN_IF_FAILED(pCurrentControlImageService->CopyTo(pUserInfoImageManagerService));

	return S_OK;
}
