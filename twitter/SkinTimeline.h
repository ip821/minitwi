// SkinTimeline.h : Declaration of the CSkinTimeline

#pragma once

#include <gdiplus.h>

#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

using namespace ATL;
using namespace std;

// CSkinTimeline

class ATL_NO_VTABLE CSkinTimeline :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinTimeline, &CLSID_SkinTimeline>,
	public ISkinTimeline
{
public:
	CSkinTimeline();

	DECLARE_REGISTRY_RESOURCEID(IDR_SKINTIMELINE)

	BEGIN_COM_MAP(CSkinTimeline)
		COM_INTERFACE_ENTRY(ISkinTimeline)
	END_COM_MAP()

private:
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IImageManagerService> m_pImageManagerService;
	map<int, shared_ptr<CBitmap>> m_cacheBitmaps;

	struct AnimationItemImageData
	{
		int index = 0;
		int step = 0;
		BYTE alpha = 0;
	};

	struct AnimationItemData
	{
		int step = 0;
		BYTE alpha = 0;
		map<int, AnimationItemImageData> columns;
	};

	const BYTE MAX_ALPHA = 255;
	const BYTE STEPS = 5;
	const BYTE STEP_ALPHA = MAX_ALPHA / STEPS;
	map<UINT, AnimationItemData> m_steps;

	enum class Justify
	{
		None = 0,
		Center,
		Right
	};

	SIZE CSkinTimeline::AddColumn(HDC hdc, IColumnsInfo* pColumnsInfo, CString& strColumnName, CString& strDisplayText, CString& strValue, int x, int y, SIZE size, BOOL bIsUrl, BOOL bWordWrap, LONG ulCustomFixedWidth, Justify justify, BOOL bDisabledSelection, BOOL bDoubleSize);
	void GetValue(IVariantObject* pItemObject, CComBSTR& bstrColumnName, CString& strValue);
	STDMETHOD(DrawTextColumns)(HWND hwndControl, IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis);
	STDMETHOD(DrawImageColumns)(IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis);
public:

	STDMETHOD(DrawItem)(HWND hwndControl, IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis);
	STDMETHOD(MeasureItem)(HWND hwndControl, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnsInfo* pColumnsInfo);

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
	STDMETHOD(SetImageManagerService)(IImageManagerService* pImageManagerService);
	STDMETHOD(GetImageManagerService)(IImageManagerService** ppImageManagerService);

	STDMETHOD(AnimationRegisterItemIndex)(UINT uiIndex, IColumnsInfo* pColumnsInfo, int iColumnIndex);
	STDMETHOD(AnimationGetParams)(UINT* puiMilliseconds);
	STDMETHOD(AnimationGetIndexes)(UINT* puiIndexArray, UINT* puiCount);
	STDMETHOD(AnimationNextFrame)(BOOL* pbContinueAnimation);

};

OBJECT_ENTRY_AUTO(__uuidof(SkinTimeline), CSkinTimeline)
