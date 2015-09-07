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
	shared_ptr<Gdiplus::Bitmap> m_pBitmapRetweet;

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

	SIZE CSkinTimeline::AddColumn(HDC hdc, IColumnsInfo* pColumnsInfo, CString& strColumnName, CString& strDisplayText, CString& strValue, int x, int y, SIZE size, BOOL bIsUrl, BOOL bWordWrap, LONG ulCustomFixedWidth, Justify justify, BOOL bDisabledSelection, BOOL bDoubleSize);
	void GetValue(IVariantObject* pItemObject, const CComBSTR& bstrColumnName, CString& strValue);
	STDMETHOD(DrawTextColumns)(IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis);
	STDMETHOD(DrawImageColumns)(IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis);
	STDMETHOD(InitImageFromResource)(int nId, LPCTSTR lpType, shared_ptr<Gdiplus::Bitmap>& pBitmap);
public:

	STDMETHOD(DrawItem)(IColumnsInfo* pColumnsInfo, TDRAWITEMSTRUCTTIMELINE* lpdis);
	STDMETHOD(MeasureItem)(HDC hdc, RECT* pClientRect, IVariantObject* pItemObject, TMEASUREITEMSTRUCT* lpMeasureItemStruct, IColumnsInfo* pColumnsInfo);

	STDMETHOD(SetTheme)(ITheme* pTheme);
	STDMETHOD(SetImageManagerService)(IImageManagerService* pImageManagerService);
	STDMETHOD(GetImageManagerService)(IImageManagerService** ppImageManagerService);

	STDMETHOD(AnimationRegisterItemIndex)(UINT uiIndex, IColumnsInfoItem* pColumnsInfoItem, int iColumnIndex);
	STDMETHOD(AnimationGetParams)(UINT* puiMilliseconds);
	STDMETHOD(AnimationGetIndexes)(UINT* puiIndexArray, UINT* puiCount);
	STDMETHOD(AnimationNextFrame)(BOOL* pbContinueAnimation);

};

OBJECT_ENTRY_AUTO(__uuidof(SkinTimeline), CSkinTimeline)
