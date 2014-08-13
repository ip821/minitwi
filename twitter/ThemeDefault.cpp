// SkinDefault.cpp : Implementation of CSkinDefault

#include "stdafx.h"
#include "ThemeDefault.h"


// CSkinDefault

HRESULT CThemeDefault::FinalConstruct()
{
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_SkinTimeline, &m_pSkinTimeline));
	return S_OK;
}

STDMETHODIMP CThemeDefault::GetTimelineSkin(ISkinTimeline** ppSkinTimeline)
{
	CHECK_E_POINTER(ppSkinTimeline);
	RETURN_IF_FAILED(m_pSkinTimeline->QueryInterface(ppSkinTimeline));
	return S_OK;
}

STDMETHODIMP CThemeDefault::SetColorMap(IThemeColorMap* pThemeColorMap)
{
	CHECK_E_POINTER(pThemeColorMap);
	m_pThemeColorMap = pThemeColorMap;
	RETURN_IF_FAILED(m_pSkinTimeline->SetColorMap(m_pThemeColorMap));
	return S_OK;
}