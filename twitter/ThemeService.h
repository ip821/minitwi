// SkinService.h : Declaration of the CSkinService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "..\ObjMdl\GUIDComparer.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace IP;

// CSkinService

class ATL_NO_VTABLE CThemeService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThemeService, &CLSID_ThemeService>,
	public IThemeService,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public IInitializeWithSettings
{
public:
	CThemeService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SKINSERVICE)

	BEGIN_COM_MAP(CThemeService)
		COM_INTERFACE_ENTRY(IThemeService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IInitializeWithSettings)
	END_COM_MAP()

private:
	std::map<GUID, CAdapt<CComPtr<IThemeColorMap> >, GUIDComparer> m_colorMaps;
	CComPtr<IObjArray> m_pObjectArray;
	CComPtr<ITheme> m_pCurrentTheme;
	CComPtr<ISettings> m_pSettings;
	CComPtr<IServiceProvider> m_pServiceProvider;

public:

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(Load)(ISettings* pSettings);

	METHOD_EMPTY(STDMETHOD(AddColorMap)(GUID gThemeId, IThemeColorMap* pThemeColorMap));
	METHOD_EMPTY(STDMETHOD(RemoveColorMap)(GUID gThemeId));

	METHOD_EMPTY(STDMETHOD(AddTheme)(GUID gThemeId, ITheme* pTheme));
	METHOD_EMPTY(STDMETHOD(RemoveTheme)(GUID gThemeId));

	STDMETHOD(GetThemes)(IObjArray** ppObjectArray);
	STDMETHOD(ApplyTheme)(GUID gId);
	STDMETHOD(ApplyThemeFromSettings)();
	STDMETHOD(GetCurrentTheme)(ITheme** ppTheme);
};

OBJECT_ENTRY_AUTO(__uuidof(ThemeService), CThemeService)
