// SkinDefault.cpp : Implementation of CSkinDefault

#include "stdafx.h"
#include "ThemeDefault.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

// CSkinDefault

#define FONT_NAME L"Tahoma"
#define FONT_SIZE 10

using namespace Gdiplus;
using namespace IP;
using namespace std;

HRESULT CThemeDefault::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTabControl, &m_pSkinTabControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinCommonControl, &m_pSkinCommonControl));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_LayoutManager, &m_pLayoutManager));

	CComPtr<IStream> pStream;
	RETURN_IF_FAILED(HrGetResourceStream(_AtlBaseModule.GetModuleInstance(), IDR_THEMEJSON, L"JSON", &pStream));
	RETURN_IF_FAILED(m_pLayoutManager->LoadThemeFromStream(pStream));
	RETURN_IF_FAILED(m_pLayoutManager->GetColorMap(&m_pThemeColorMap));
	RETURN_IF_FAILED(m_pLayoutManager->GetFontMap(&m_pThemeFontMap));
	RETURN_IF_FAILED(m_pLayoutManager->GetImageManagerService(&m_pImageManagerService));

	CComQIPtr<IThemeSupport> pSkinTabControlThemeSupport = m_pSkinTabControl;
	RETURN_IF_FAILED(pSkinTabControlThemeSupport->SetTheme(this));
	RETURN_IF_FAILED(m_pSkinCommonControl->SetColorMap(m_pThemeColorMap));
	RETURN_IF_FAILED(m_pSkinCommonControl->SetFontMap(m_pThemeFontMap));

	return S_OK;
}

STDMETHODIMP CThemeDefault::GetTimelineSkin(ISkinTimeline** ppSkinTimeline)
{
	CHECK_E_POINTER(ppSkinTimeline);
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTimeline, ppSkinTimeline));
	RETURN_IF_FAILED((*ppSkinTimeline)->SetFontMap(m_pThemeFontMap));
	RETURN_IF_FAILED((*ppSkinTimeline)->SetColorMap(m_pThemeColorMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetTabControlSkin(ISkinTabControl** pSkinTabControl)
{
	CHECK_E_POINTER(pSkinTabControl);
	RETURN_IF_FAILED(m_pSkinTabControl->QueryInterface(pSkinTabControl));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetCommonControlSkin(ISkinCommonControl** pSkinCommonControl)
{
	CHECK_E_POINTER(pSkinCommonControl);
	RETURN_IF_FAILED(m_pSkinCommonControl->QueryInterface(pSkinCommonControl));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetColorMap(IThemeColorMap** ppThemeColorMap)
{
	CHECK_E_POINTER(ppThemeColorMap);
	RETURN_IF_FAILED(m_pThemeColorMap->QueryInterface(ppThemeColorMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetFontMap(IThemeFontMap** ppThemeFontMap)
{
	CHECK_E_POINTER(ppThemeFontMap);
	RETURN_IF_FAILED(m_pThemeFontMap->QueryInterface(ppThemeFontMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetSkinUserAccountControl(ISkinUserAccountControl** ppSkinUserAccountControl)
{
	CHECK_E_POINTER(ppSkinUserAccountControl);
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinUserAccountControl, ppSkinUserAccountControl));
	RETURN_IF_FAILED((*ppSkinUserAccountControl)->SetFontMap(m_pThemeFontMap));
	RETURN_IF_FAILED((*ppSkinUserAccountControl)->SetColorMap(m_pThemeColorMap));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetLayoutManager(ILayoutManager** ppLayoutManager)
{
	return m_pLayoutManager->QueryInterface(ppLayoutManager);
}

STDMETHODIMP CThemeDefault::GetLayout(BSTR bstrLayoutName, IVariantObject** ppVariantObject)
{
	CHECK_E_POINTER(bstrLayoutName);
	CHECK_E_POINTER(ppVariantObject);
	RETURN_IF_FAILED(m_pLayoutManager->GetLayout(bstrLayoutName, ppVariantObject));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetImageManagerService(IImageManagerService** ppImageManagerService)
{
	CHECK_E_POINTER(ppImageManagerService);
	RETURN_IF_FAILED(m_pImageManagerService->QueryInterface(ppImageManagerService));
	return S_OK;
}