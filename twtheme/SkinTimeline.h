// SkinTimeline.h : Declaration of the CSkinTimeline

#pragma once

#include <gdiplus.h>

#include "resource.h"       // main symbols
#include "twtheme_i.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

using namespace ATL;
using namespace std;
using namespace IP;

// CSkinTimeline

class ATL_NO_VTABLE CSkinTimeline :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinTimeline, &CLSID_SkinTimeline>,
	public ISkinTimeline
{
public:
	CSkinTimeline();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSkinTimeline)
		COM_INTERFACE_ENTRY(ISkinTimeline)
		COM_INTERFACE_ENTRY(IThemeSupport)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

private:
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IImageManagerService> m_pImageManagerService;

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
	CComPtr<ILayoutManager> m_pLayoutManager;

	enum class Justify
	{
		None = 0,
		Center,
		Right
	};

	void GetValue(IVariantObject* pItemObject, const CComBSTR& bstrColumnName, CString& strValue);

public:

	STDMETHOD(DrawItem)(IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis);
	STDMETHOD(MeasureItem)(HDC hdc, RECT* pClientRect, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnsInfo* pColumnsInfo);

	STDMETHOD(SetTheme)(ITheme* pTheme);
	STDMETHOD(SetImageManagerService)(IImageManagerService* pImageManagerService);
	STDMETHOD(GetImageManagerService)(IImageManagerService** ppImageManagerService);

	STDMETHOD(AnimationRegisterItemIndex)(UINT uiIndex, IColumnsInfoItem* pColumnsInfoItem, int iColumnIndex);
	STDMETHOD(AnimationGetParams)(UINT* puiMilliseconds);
	STDMETHOD(AnimationGetIndexes)(UINT* puiIndexArray, UINT* puiCount);
	STDMETHOD(AnimationNextFrame)(IColumnsInfo** ppColumnsInfoArray, UINT uiCount, BOOL* pbContinueAnimation);

};

OBJECT_ENTRY_AUTO(__uuidof(SkinTimeline), CSkinTimeline)
