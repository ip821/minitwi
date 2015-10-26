// SkinService.cpp : Implementation of CSkinService

#include "stdafx.h"
#include "ThemeService.h"
#include "twmdl_i.h"
#include "Plugins.h"

// CThemeService

STDMETHODIMP CThemeService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	return S_OK;
}

STDMETHODIMP CThemeService::OnInitialized(IServiceProvider *pServiceProvider)
{
	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(GetPluginManager()->GetPluginInfoCollection(PNAMESP_HOSTFORM, PTYPE_THEME, &m_pObjectArray));
	return S_OK;
}

STDMETHODIMP CThemeService::OnShutdown()
{
	m_colorMaps.clear();
	m_pObjectArray.Release();
	m_pServiceProvider.Release();
	m_pCurrentTheme.Release();
	m_pSettings.Release();
	return S_OK;
}

STDMETHODIMP CThemeService::GetThemes(IObjArray** ppObjectArray)
{
	RETURN_IF_FAILED(m_pObjectArray->QueryInterface(ppObjectArray));
	return S_OK;
}

STDMETHODIMP CThemeService::ApplyTheme(GUID gId)
{
	GUID gThemeId = GUID_NULL;
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(m_pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
		CComVar vId;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(ObjectModel::Metadata::Plugins::Object::Id, &vId));
		if (vId.vt == VT_BSTR)
		{
			GUID gFoundId = GUID_NULL;
			RETURN_IF_FAILED(CLSIDFromString(vId.bstrVal, &gFoundId));
			if (gId == gFoundId)
			{
				gThemeId = gFoundId;
				break;
			}
		}
	}

	if (gThemeId != GUID_NULL)
	{
		m_pCurrentTheme.Release();
		CComPtr<IUnknown> pThemeUnk;
		RETURN_IF_FAILED(GetPluginManager()->CreatePluginInstance(PNAMESP_HOSTFORM, PTYPE_THEME, gThemeId, &pThemeUnk));
		RETURN_IF_FAILED(pThemeUnk->QueryInterface(&m_pCurrentTheme));
	}

	CComPtr<IFormManager> pFormManager;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_FORM_MANAGER, &pFormManager));

	{
		CComPtr<IControl> pControlHome;
		RETURN_IF_FAILED(pFormManager->FindForm(CLSID_HomeTimeLineControl, &pControlHome));
		CComQIPtr<IThemeSupport> pThemeSupport = pControlHome;
		ATLASSERT(pThemeSupport);
		if (pThemeSupport)
		{
			RETURN_IF_FAILED(pThemeSupport->SetTheme(m_pCurrentTheme));
		}
	}

	{
		CComPtr<IControl> pControlLists;
		RETURN_IF_FAILED(pFormManager->FindForm(CLSID_ListsControl, &pControlLists));
		CComQIPtr<IThemeSupport> pThemeSupport = pControlLists;
		ATLASSERT(pThemeSupport);
		if (pThemeSupport)
		{
			RETURN_IF_FAILED(pThemeSupport->SetTheme(m_pCurrentTheme));
		}
	}

	{
		CComPtr<IControl> pControlSearch;
		RETURN_IF_FAILED(pFormManager->FindForm(CLSID_SearchControl, &pControlSearch));
		CComQIPtr<IThemeSupport> pThemeSupport = pControlSearch;
		ATLASSERT(pThemeSupport);
		if (pThemeSupport)
		{
			RETURN_IF_FAILED(pThemeSupport->SetTheme(m_pCurrentTheme));
		}
	}

	{
		CComPtr<IControl> pControlSettings;
		RETURN_IF_FAILED(pFormManager->FindForm(CLSID_SettingsControl, &pControlSettings));
		CComQIPtr<IThemeSupport> pThemeSupport = pControlSettings;
		ATLASSERT(pThemeSupport);
		if (pThemeSupport)
		{
			RETURN_IF_FAILED(pThemeSupport->SetTheme(m_pCurrentTheme));
		}
	}

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	ATLASSERT(pMainWindow);
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	CComQIPtr<ICustomTabControl> pTabbedControl = pContainerControl;
	ATLASSERT(pTabbedControl);

	CComPtr<ISkinTabControl> pSkinTabControl;
	RETURN_IF_FAILED(m_pCurrentTheme->GetTabControlSkin(&pSkinTabControl));
	RETURN_IF_FAILED(pTabbedControl->SetSkinTabControl(pSkinTabControl));


	CComPtr<IViewControllerService> pViewControllerService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ViewControllerService, &pViewControllerService));
	RETURN_IF_FAILED(pViewControllerService->SetTheme(m_pCurrentTheme));
	return S_OK;
}

STDMETHODIMP CThemeService::ApplyThemeFromSettings()
{
	CComPtr<ISettings> pSettings;
	RETURN_IF_FAILED(m_pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathThemes, &pSettings));
	CComVar vThemeId;
	RETURN_IF_FAILED(pSettings->GetVariantValue(ObjectModel::Metadata::Plugins::Object::Id, &vThemeId));
	if (vThemeId.vt == VT_BSTR)
	{
		GUID gId = GUID_NULL;
		RETURN_IF_FAILED(CLSIDFromString(vThemeId.bstrVal, &gId));
		RETURN_IF_FAILED(ApplyTheme(gId));
	}
	else
	{
		RETURN_IF_FAILED(ApplyTheme(THEME_DEFAULT));
	}
	return S_OK;
}

STDMETHODIMP CThemeService::GetCurrentTheme(ITheme** ppTheme)
{
	CHECK_E_POINTER(ppTheme);
	if (!m_pCurrentTheme)
		return E_PENDING;
	RETURN_IF_FAILED(m_pCurrentTheme->QueryInterface(ppTheme));
	return S_OK;
}