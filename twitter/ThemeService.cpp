// SkinService.cpp : Implementation of CSkinService

#include "stdafx.h"
#include "ThemeService.h"
#include "twitter_i.h"
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
	m_pObjectArray.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CThemeService::GetThemes(IObjArray** ppObjectArray)
{
	RETURN_IF_FAILED(m_pObjectArray->QueryInterface(ppObjectArray));
	return S_OK;
}

STDMETHODIMP CThemeService::ApplyTheme(GUID gId)
{
	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	CComQIPtr<ICustomTabControl> pTabbedControl = pContainerControl;
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(pTabbedControl->GetPage(0, &pControl));
	CComQIPtr<ITimelineControl> pTimelineControl = pControl;

	GUID gThemeId = GUID_NULL;
	UINT uiCount = 0;
	RETURN_IF_FAILED(m_pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(m_pObjectArray->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pVariantObject));
		CComVariant vId;
		RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_GUID_ID, &vId));
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

	CComPtr<IImageManagerService> pImageManagerService;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &pImageManagerService));
	RETURN_IF_FAILED(m_pCurrentTheme->SetImageManagerService(pImageManagerService));
	
	CComPtr<ISkinTimeline> pSkinTimeline;
	RETURN_IF_FAILED(m_pCurrentTheme->GetTimelineSkin(&pSkinTimeline));
	RETURN_IF_FAILED(pTimelineControl->SetSkinTimeline(pSkinTimeline));

	CComPtr<ISkinTabControl> pSkinTabControl;
	RETURN_IF_FAILED(m_pCurrentTheme->GetTabControlSkin(&pSkinTabControl));
	RETURN_IF_FAILED(pTabbedControl->SetSkinTabControl(pSkinTabControl));

	return S_OK;
}

STDMETHODIMP CThemeService::ApplyThemeFromSettings()
{
	CComPtr<ISettings> pSettings;
	RETURN_IF_FAILED(m_pSettings->OpenSubSettings(SETTINGS_PATH_THEMES, &pSettings));
	CComVariant vThemeId;
	RETURN_IF_FAILED(pSettings->GetVariantValue(VAR_GUID_ID, &vThemeId));
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