// PictureWindow.cpp : Implementation of CPictureWindow

#include "stdafx.h"
#include "PictureWindow.h"
#include "Plugins.h"
#include "SkinUserAccountControl.h"
#include "..\twiconn\Plugins.h"
#include "GdilPlusUtils.h"

// CPictureWindow

#define IMAGE_PADDING 25

#define TARGET_RESOLUTION 1
#define TARGET_INTERVAL 15

CPictureWindow::CPictureWindow()
{
}

CPictureWindow::~CPictureWindow()
{
	if (IsWindow())
		DestroyWindow();
}

STDMETHODIMP CPictureWindow::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &m_dwAdviceDownloadService));

	HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport);
	m_pPluginSupport->InitializePlugins(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_WINDOW_SERVICE);
	m_pPluginSupport->InitializePlugins(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_COMMAND);
	m_pPluginSupport->OnInitialized();

	m_popupMenu.CreatePopupMenu();
	HrCoCreateInstance(CLSID_CommandSupport, &m_pCommandSupport);
	m_pCommandSupport->SetMenu(m_popupMenu);
	m_pCommandSupport->InstallCommands(m_pPluginSupport);

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	ATLASSERT(pMainWindow);
	RETURN_IF_FAILED(pMainWindow->GetMessageLoop(&m_pMessageLoop));
	RETURN_IF_FAILED(m_pMessageLoop->AddMessageFilter(this));

	m_pServiceProvider = m_pPluginSupport;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));

	return S_OK;
}

STDMETHODIMP CPictureWindow::OnShutdown()
{
	RETURN_IF_FAILED(m_pMessageLoop->RemoveMessageFilter(this));
	m_pMessageLoop.Release();
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), m_dwAdviceDownloadService));
	m_pDownloadService.Release();
	RETURN_IF_FAILED(m_pCommandSupport->UninstallAll());
	m_pCommandSupport.Release();
	RETURN_IF_FAILED(m_pPluginSupport->OnShutdown());
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	m_pPluginSupport.Release();
	m_pImageManagerService.Release();
	m_pServiceProvider.Release();
	m_pTheme.Release();
	return S_OK;
}

LRESULT CPictureWindow::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}

STDMETHODIMP CPictureWindow::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(bResult);
	RETURN_IF_FAILED(m_pCommandSupport->PreTranslateMessage(m_hWnd, pMsg, bResult));
	return S_OK;
}

void CPictureWindow::MoveToNextPicture()
{
	int currentBitmapIndex = 0;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		if (m_currentBitmapIndex == -1)
			return;
		if (m_bitmapsUrls.size() == 1)
			return;

		++m_currentBitmapIndex;

		if ((size_t)m_currentBitmapIndex == m_bitmapsUrls.size())
			m_currentBitmapIndex = 0;

		ASSERT_IF_FAILED(InitCommandSupport(m_currentBitmapIndex));

		currentBitmapIndex = m_currentBitmapIndex;
	}

	BOOL bContains = FALSE;
	ASSERT_IF_FAILED(m_pImageManagerService->ContainsImageKey(m_bitmapsUrls[currentBitmapIndex], &bContains));

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		if (bContains)
		{
			ResizeToCurrentBitmap();
			Invalidate();
			return;
		}

		ASSERT_IF_FAILED(ResetAnimation());

		ASSERT_IF_FAILED(StartNextDownload(m_currentBitmapIndex));
		m_currentBitmapIndex = -1;
	}

	Invalidate();
}

LRESULT CPictureWindow::OnLButtomUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MoveToNextPicture();
	return 0;
}

LRESULT CPictureWindow::OnRButtomUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	ClientToScreen(&pt);

	CComQIPtr<IIdleHandler> pIdleHandler = m_pCommandSupport;
	if (pIdleHandler)
	{
		ASSERT_IF_FAILED(pIdleHandler->OnIdle(NULL));
	}
	m_popupMenu.TrackPopupMenu(0, pt.x, pt.y, m_hWnd);
	return 0;
}

LRESULT CPictureWindow::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bResult = FALSE;
	LRESULT lResult = 0;
	RETURN_IF_FAILED(m_pCommandSupport->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult));
	bHandled = bResult;
	return lResult;
}

LRESULT CPictureWindow::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_icon.LoadIcon(IDR_TWITTER);

	SetClassLong(m_hWnd, GCL_HICONSM, (LONG)m_icon.m_hIcon);
	SetClassLong(m_hWnd, GCL_HICON, (LONG)m_icon.m_hIcon);

	return 0;
}

STDMETHODIMP CPictureWindow::GetHWND(HWND* phWnd)
{
	CHECK_E_POINTER(phWnd);
	*phWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CPictureWindow::Show(HWND hWndParent)
{
	m_hWndParent = hWndParent;
	CRect rect(0, 0, 100, 100);
	CalcRect(rect.right, rect.bottom, rect);
	__super::Create(NULL, rect, 0, WS_VISIBLE | WS_BORDER | WS_SYSMENU, WS_EX_COMPOSITED);
	return S_OK;
}

STDMETHODIMP CPictureWindow::InitCommandSupport(int index)
{
	ATLASSERT(index >= 0);

	CComPtr<IVariantObject> pVariantObject;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(VAR_TWITTER_MEDIAURL, &CComVariant(m_bitmapsUrls[index])));

	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pCommandSupport;
	ATLASSERT(pInitializeWithVariantObject);
	RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(pVariantObject));
	return S_OK;
}

STDMETHODIMP CPictureWindow::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);

	CComVariant vMediaUrl;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_MEDIAURL, &vMediaUrl));

	int currentBitmapIndex = 0;
	CComVariant vMediaUrls;
	if (SUCCEEDED(pVariantObject->GetVariantValue(VAR_TWITTER_MEDIAURLS, &vMediaUrls)) && vMediaUrls.vt == VT_UNKNOWN)
	{
		CComQIPtr<IObjArray> pMediaUrls = vMediaUrls.punkVal;
		UINT uiCount = 0;
		RETURN_IF_FAILED(pMediaUrls->GetCount(&uiCount));
		m_bitmapsUrls.resize(uiCount);
		for (size_t i = 0; i < uiCount; ++i)
		{
			CComPtr<IVariantObject> pMediaUrlObject;
			RETURN_IF_FAILED(pMediaUrls->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaUrlObject));
			CComVariant vMediaUrlForObject;
			RETURN_IF_FAILED(pMediaUrlObject->GetVariantValue(VAR_TWITTER_MEDIAURL, &vMediaUrlForObject));
			m_bitmapsUrls[i] = vMediaUrlForObject.bstrVal;

			if (CComBSTR(vMediaUrlForObject.bstrVal) == CComBSTR(vMediaUrl.bstrVal))
				currentBitmapIndex = i;
		}
	}
	else
	{
		m_bitmapsUrls.push_back(vMediaUrl.bstrVal);
		currentBitmapIndex = 0;
	}

	RETURN_IF_FAILED(InitCommandSupport(currentBitmapIndex));
	RETURN_IF_FAILED(StartNextDownload(currentBitmapIndex));
	return S_OK;
}

STDMETHODIMP CPictureWindow::StartNextDownload(int index)
{
	m_strLastErrorMsg = L"";
	CComPtr<IVariantObject> pDownloadTask;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadTask));
	auto str = m_bitmapsUrls[index];
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_URL, &CComVariant(str)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_ID, &CComVariant((INT64)m_hWnd)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_ITEM_INDEX, &CComVariant(index)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(TYPE_IMAGE_PICTURE_WINDOW)));
	RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));

	return S_OK;
}

LRESULT CPictureWindow::OnAnimationTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	boost::lock_guard<boost::mutex> lock(m_mutex);
	m_alpha += m_alphaAmount;
	m_step++;
	Invalidate();

	if (m_step == STEPS)
	{
		m_alpha = 255;
		return 0;
	}
	StartAnimationTimer(TARGET_INTERVAL);
	return 0;
}

STDMETHODIMP CPictureWindow::ResetAnimation()
{
	m_alpha = 0;
	m_alphaAmount = (255 / STEPS);
	m_step = 0;
	return S_OK;
}

STDMETHODIMP CPictureWindow::OnDownloadComplete(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CComVariant vType;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_OBJECT_TYPE, &vType));

	if (vType.vt != VT_BSTR || CComBSTR(vType.bstrVal) != CComBSTR(TYPE_IMAGE_PICTURE_WINDOW))
		return S_OK;

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT, &vHr));
	if (FAILED(vHr.intVal))
	{
		CComVariant vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(KEY_HRESULT_DESCRIPTION, &vDesc));
		CComBSTR bstrMsg = L"Unknown error";
		if (vDesc.vt == VT_BSTR)
			bstrMsg = vDesc.bstrVal;

		m_strLastErrorMsg = bstrMsg;
		{
			boost::lock_guard<boost::mutex> lock(m_mutex);
			ResizeWindow(200, 200);
		}
		Invalidate();
		return S_OK;
	}

	CComVariant vId;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_ID, &vId));

	if (vId.vt != VT_I8 || vId.llVal != (INT64)m_hWnd)
		return S_OK;

	CComVariant vIndex;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_ITEM_INDEX, &vIndex));

	CComVariant vFilePath;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));

	int currentBitmapIndex = 0;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_currentBitmapIndex = vIndex.intVal;
		currentBitmapIndex = m_currentBitmapIndex;
	}

	auto pBitmap = make_shared<Bitmap>(vFilePath.bstrVal);
	auto hMonitor = MonitorFromWindow(m_hWndParent, MONITOR_DEFAULTTONULL);

	if (hMonitor)
	{
		MONITORINFO mi = { 0 };
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &mi);
		CRect rectMonitor(mi.rcWork);
		ResizeDownImage(pBitmap, rectMonitor.Height() - 100);
	}

	CBitmap bmp;
	pBitmap->GetHBITMAP(Color::Transparent, &bmp.m_hBitmap);
	ASSERT_IF_FAILED(m_pImageManagerService->AddImageFromHBITMAP(m_bitmapsUrls[currentBitmapIndex], bmp));

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		RETURN_IF_FAILED(ResetAnimation());
		ResizeToCurrentBitmap();
	}

	StartAnimationTimer(TARGET_INTERVAL);
	return S_OK;
}

void CPictureWindow::ResizeWindow(UINT uiWidth, UINT uiHeight)
{
	CRect rect;
	GetWindowRect(&rect);

	CalcRect(uiWidth, uiHeight, rect);
	SetWindowPos(NULL, &rect, SWP_NOZORDER);
}

void CPictureWindow::ResizeToCurrentBitmap()
{
	TBITMAP tBitmap = { 0 };
	ASSERT_IF_FAILED(m_pImageManagerService->GetImageInfo(m_bitmapsUrls[m_currentBitmapIndex], &tBitmap));
	auto width = tBitmap.Width;
	auto height = tBitmap.Height;
	ResizeWindow(width, height);
}

void CPictureWindow::CalcRect(int width, int height, CRect& rect)
{
	auto hMonitor = MonitorFromWindow(m_hWndParent, MONITOR_DEFAULTTONULL);

	if (hMonitor)
	{
		MONITORINFO mi = { 0 };
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &mi);

		rect = mi.rcWork;
		rect.left += (mi.rcWork.right - mi.rcWork.left) / 2 - width / 2;
		rect.top += (mi.rcWork.bottom - mi.rcWork.top) / 2 - height / 2;
	}
	else
	{
		int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

		rect.left = nScreenWidth / 2 - width / 2;
		rect.top = nScreenHeight / 2 - height / 2;
	}

	rect.right = rect.left + width + IMAGE_PADDING * 2;
	rect.bottom = rect.top + height + IMAGE_PADDING * 2;
}

void CPictureWindow::OnFinalMessage(HWND hWnd)
{
	Fire_OnClosed(hWnd);
}

LRESULT CPictureWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps = { 0 };
	CDCHandle cdcReal(BeginPaint(&ps));

	CRect rect;
	GetClientRect(&rect);

	CBitmap bufferBitmap;
	bufferBitmap.CreateCompatibleBitmap(cdcReal, rect.Width(), rect.Height());

	CDC cdc;
	cdc.CreateCompatibleDC(cdcReal);
	CDCSelectBitmapScope cdcSelectBitmapScopeBufferBitmap(cdc, bufferBitmap);

	int currentBitmapIndex = 0;
	BYTE alpha = m_alpha;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		currentBitmapIndex = m_currentBitmapIndex;
	}

	static DWORD dwBrushColor = 0;
	static DWORD dwTextColor = 0;
	if (!dwBrushColor)
	{
		CComPtr<IThemeColorMap> pThemeColorMap;
		ASSERT_IF_FAILED(m_pTheme->GetColorMap(&pThemeColorMap));
		ASSERT_IF_FAILED(pThemeColorMap->GetColor(VAR_BRUSH_BACKGROUND, &dwBrushColor));
		ASSERT_IF_FAILED(pThemeColorMap->GetColor(VAR_PICTURE_WINDOW_TEXT, &dwTextColor));
	}

	static HFONT font = 0;
	if (!font)
	{
		CComPtr<IThemeFontMap> pThemeFontMap;
		ASSERT_IF_FAILED(m_pTheme->GetFontMap(&pThemeFontMap));
		ASSERT_IF_FAILED(pThemeFontMap->GetFont(VAR_PICTURE_WINDOW_TEXT, &font));
	}
	CDCSelectFontScope cdcSelectFontScope(cdc, font);

	cdc.SetBkMode(TRANSPARENT);
	cdc.SetTextColor(dwTextColor);

	cdc.FillSolidRect(&rect, dwBrushColor);

	if (currentBitmapIndex == -1)
	{
		CComBSTR str = L"Downloading...";
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
		CSkinUserAccountControl::DrawRoundedRect(CDCHandle(cdc), rectText, false);
		cdc.DrawTextEx(str, str.Length(), &rectText, DT_WORDBREAK | DT_CENTER, NULL);
	}
	else
	{
		TBITMAP tBitmap = { 0 };
		ASSERT_IF_FAILED(m_pImageManagerService->GetImageInfo(m_bitmapsUrls[currentBitmapIndex], &tBitmap));
		auto width = tBitmap.Width;
		auto height = tBitmap.Height;
		auto x = (rect.right - rect.left) / 2 - (width / 2);
		auto y = (rect.bottom - rect.top) / 2 - (height / 2);

		CBitmap bitmap;
		ASSERT_IF_FAILED(m_pImageManagerService->CreateImageBitmap(m_bitmapsUrls[currentBitmapIndex], &bitmap.m_hBitmap));
		CDC cdcBitmap;
		cdcBitmap.CreateCompatibleDC(cdc);
		CDCSelectBitmapScope cdcSelectBitmapScope(cdcBitmap, bitmap);
		BLENDFUNCTION bf = { 0 };
		bf.BlendOp = AC_SRC_OVER;
		bf.SourceConstantAlpha = alpha;
		cdc.AlphaBlend(x, y, width, height, cdcBitmap, 0, 0, width, height, bf);
	}

	if (m_bitmapsUrls.size() > 1 && m_currentBitmapIndex >= 0)
	{
		CString str;
		str.Format(L"%u / %u", (UINT)currentBitmapIndex + 1, m_bitmapsUrls.size());
		CSize sz;
		cdc.GetTextExtent(str, str.GetLength(), &sz);

		auto x = rect.Width() / 2;
		auto y = rect.bottom - sz.cy - IMAGE_PADDING * 2;

		CRect rectText(x, y, x + sz.cx, y + sz.cy);
		CSkinUserAccountControl::DrawRoundedRect(CDCHandle(cdc), rectText, false);
		cdc.DrawText(str, str.GetLength(), rectText, 0);
	}

	cdcReal.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), cdc, 0, 0, SRCCOPY);

	EndPaint(&ps);
	return 0;
}

HRESULT CPictureWindow::Fire_OnClosed(HWND hWnd)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CPictureWindow* pThis = static_cast<CPictureWindow*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IWindowEventSink * pConnection = static_cast<IWindowEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnClosed(hWnd);
		}
	}
	return hr;
}

LRESULT CPictureWindow::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam == VK_ESCAPE)
		PostMessage(WM_CLOSE, 0, 0);
	else if (wParam == VK_RETURN)
		MoveToNextPicture();
	return 0;
}

STDMETHODIMP CPictureWindow::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	return S_OK;
}