#pragma once

using namespace ATL;
using namespace std;

class CCustomTabControl : public CWindowImpl<CCustomTabControl>
{
public:
	CCustomTabControl();
	~CCustomTabControl();


	BEGIN_MSG_MAP(CCustomTabControl)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
	END_MSG_MAP()
private:

public:
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	BOOL SubclassWindow(HWND hWnd);
};

CCustomTabControl::CCustomTabControl()
{
}

CCustomTabControl::~CCustomTabControl()
{
}

BOOL CCustomTabControl::SubclassWindow(HWND hWnd)
{
	auto res = __super::SubclassWindow(hWnd);
	auto style = GetWindowLong(GWL_STYLE);
	style |= TCS_OWNERDRAWFIXED;
	style |= TCS_BUTTONS;
	SetWindowLong(GWL_STYLE, style);
	return res;
}

LRESULT CCustomTabControl::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	return 0;
}

LRESULT CCustomTabControl::OnDrawItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}
