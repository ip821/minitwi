#include "stdafx.h"
#include "VideoViewControl.h"
#include "..\twtheme\GdilPlusUtils.h"

#define TARGET_INTERVAL 15

CVideoViewControl::CVideoViewControl()
{

}

CVideoViewControl::~CVideoViewControl()
{
	if (IsWindow())
		DestroyWindow();
}

STDMETHODIMP CVideoViewControl::GetHWND(HWND* phWnd)
{
	CHECK_E_POINTER(phWnd);
	*phWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CVideoViewControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	m_hWnd = __super::Create(hWndParent, 0, 0, WS_VISIBLE | WS_CHILD);
	if (hWnd)
		*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CVideoViewControl::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	return S_OK;
}

LRESULT CVideoViewControl::OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_LBUTTONUP && !m_bPlaying)
	{
		m_bPlaying = TRUE;
		::SendMessage(m_hWndPlayer, WM_PLAYER_PLAY, 0, 0);
		return 0;
	}
	return GetParent().PostMessage(uMsg, wParam, lParam);
}

STDMETHODIMP CVideoViewControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;

	CComVariant vPath;
	RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(Twitter::Metadata::File::Path, &vPath));
	ATLASSERT(vPath.vt == VT_BSTR);
	m_bstrPath = vPath.bstrVal;

	return S_OK;
}

STDMETHODIMP CVideoViewControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	auto wstrParams = std::to_wstring((long long)m_hWnd);
	CString strCmdLine(L"minitwivp.exe");
	strCmdLine += CString(L" ") + wstrParams.c_str();
	TCHAR lpszCmdLine[100];
	wcscpy(lpszCmdLine, strCmdLine);
	auto res = CreateProcess(
		NULL,
		lpszCmdLine,
		NULL,
		NULL,
		FALSE,
		NULL,
		NULL,
		NULL,
		&si,
		&pi
		);
	res;
	m_hProcess = pi.hProcess;

	return S_OK;
}

LRESULT CVideoViewControl::OnPlayerFinished(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bPlaying = FALSE;
	return 0;
}

LRESULT CVideoViewControl::OnPlayerStarted(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_bPlayerStarted = TRUE;
	m_bPlaying = TRUE;

	m_hWndPlayer = (HWND)lParam;

	TCHAR path[MAX_PATH] = { 0 };
	wcscpy(path, m_bstrPath);
	COPYDATASTRUCT cds = { 0 };
	cds.cbData = MAX_PATH * sizeof(TCHAR);
	cds.dwData = (ULONG_PTR)&path[0];
	cds.lpData = &path[0];
	::SendMessage(m_hWndPlayer, WM_COPYDATA, (WPARAM)m_hWnd, (LPARAM)&cds);

	return 0;
}

STDMETHODIMP CVideoViewControl::OnShutdown()
{
	::SendMessage(m_hWndPlayer, WM_PLAYER_CLOSE, 0, 0);
	::WaitForSingleObject(m_hProcess, 5 * 1000); //5 seconds

	IInitializeWithControlImpl::OnShutdown();
	m_pVariantObject.Release();
	m_pServiceProvider.Release();
	m_pTheme.Release();
	return S_OK;
}

LRESULT CVideoViewControl::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

LRESULT CVideoViewControl::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	CDCHandle cdc(BeginPaint(&ps));

	if (m_bPlayerStarted)
	{
		::SendMessage(m_hWndPlayer, WM_PLAYER_UPDATE, 0, 0);
	}
	else
	{
		CRect rect;
		GetClientRect(&rect);

		static DWORD dwBrushColor = 0;
		static DWORD dwTextColor = 0;
		if (!dwBrushColor)
		{
			CComPtr<IThemeColorMap> pThemeColorMap;
			ASSERT_IF_FAILED(m_pTheme->GetColorMap(&pThemeColorMap));
			ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwBrushColor));
			ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::PictureWindowText, &dwTextColor));
		}

		cdc.SetBkMode(TRANSPARENT);
		cdc.SetTextColor(dwTextColor);

		cdc.FillSolidRect(&rect, dwBrushColor);

		CComBSTR str = L"Launching video player...";
		if (!m_strLastErrorMsg.IsEmpty())
			str = m_strLastErrorMsg;

		CSize size;
		CRect rect1 = rect;
		cdc.DrawTextEx(str, str.Length(), &rect1, DT_WORDBREAK | DT_CENTER | DT_CALCRECT, NULL);
		size.cx = rect1.Width();
		size.cy = rect1.Height();

		auto x = (rect.right - rect.left) / 2 - (size.cx / 2);
		auto y = (rect.bottom - rect.top) / 2 - (size.cy / 2);

		CRect rectText(x, y, x + size.cx, y + size.cy);
		DrawRoundedRect(cdc, rectText, false);
		cdc.DrawTextEx(str, str.Length(), &rectText, DT_WORDBREAK | DT_CENTER, NULL);
	}
	EndPaint(&ps);
	return 0;
}

STDMETHODIMP CVideoViewControl::SetTheme(ITheme *pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	return S_OK;
}