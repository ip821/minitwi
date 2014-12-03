// SkinCommonControl.h : Declaration of the CSkinCommonControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include <gdiplus.h>

using namespace ATL;
using namespace std;
using namespace Gdiplus;

// CSkinCommonControl

class ATL_NO_VTABLE CSkinCommonControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSkinCommonControl, &CLSID_SkinCommonControl>,
	public ISkinCommonControl
{
public:
	CSkinCommonControl();

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CSkinCommonControl)
		COM_INTERFACE_ENTRY(ISkinCommonControl)
	END_COM_MAP()

	BEGIN_MSG_MAP(CSkinCommonControl)
	END_MSG_MAP()

private:
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;

	map<HWND, int> m_refs;

	enum class ControlType
	{
		Unknown = 0,
		Static = 1,
		Button = 2
	};

	struct WindowDescriptor
	{
		HWND hWnd;
		LONG_PTR pWndProc;
		ControlType controlType;
		bool isParent;
	};

	static CBrush m_bkColor;
	static map<HWND, WindowDescriptor> m_procs;

	static LRESULT WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	static void DrawButton(HWND hWndParent, LPDRAWITEMSTRUCT lpdis);
public:

	STDMETHOD(SetColorMap)(IThemeColorMap* pThemeColorMap);
	STDMETHOD(SetFontMap)(IThemeFontMap* pThemeFontMap);
	STDMETHOD(RegisterStaticControl)(HWND hWnd);
	STDMETHOD(UnregisterStaticControl)(HWND hWnd);
	STDMETHOD(RegisterButtonControl)(HWND hWnd);
	STDMETHOD(UnregisterButtonControl)(HWND hWnd);
};

OBJECT_ENTRY_AUTO(__uuidof(SkinCommonControl), CSkinCommonControl)
