// PictureWindow.cpp : Implementation of CPictureWindow

#include "stdafx.h"
#include "PictureWindow.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "..\twtheme\GdilPlusUtils.h"

// CPictureWindow

#define IMAGE_PADDING 25

#define TARGET_INTERVAL 15

CPictureWindow::CPictureWindow()
{
}

CPictureWindow::~CPictureWindow()
{
	if (IsWindow())
		DestroyWindow();
}

STDMETHODIMP CPictureWindow::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	CComPtr<ISettings> pSubSettings;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(PICTURE_WINDOW_SETTINGS_PATH, &pSubSettings));
	CComVariant vDisableMonitorSnap;
	RETURN_IF_FAILED(pSubSettings->GetVariantValue(PICTURE_WINDOW_SETTINGS_DISABLE_MONITOR_SNAP_KEY, &vDisableMonitorSnap));
	m_bDisableMonitorSnap = vDisableMonitorSnap.vt == VT_I4 && vDisableMonitorSnap.intVal > 0;
	return S_OK;
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

	m_pServiceProvider = m_pPluginSupport;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));

	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_AnimationService, &m_pAnimationService));
	RETURN_IF_FAILED(AtlAdvise(m_pAnimationService, pUnk, __uuidof(IAnimationServiceEventSink), &m_dwAdviceAnimationService));

	RETURN_IF_FAILED(HrInitializeWithControl(m_pPluginSupport, pUnk));
	RETURN_IF_FAILED(m_pPluginSupport->OnInitialized());

	m_popupMenu.CreatePopupMenu();
	HrCoCreateInstance(CLSID_CommandSupport, &m_pCommandSupport);
	m_pCommandSupport->SetMenu(m_popupMenu);
	m_pCommandSupport->InstallCommands(m_pPluginSupport);

	return S_OK;
}

STDMETHODIMP CPictureWindow::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pAnimationService, __uuidof(IAnimationServiceEventSink), m_dwAdviceAnimationService));
	m_pAnimationService.Release();
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

LRESULT CPictureWindow::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pPluginSupport)
	{
		CComQIPtr<IMsgHandler> pMsgHandler = m_pPluginSupport;
		if (pMsgHandler)
		{
			LRESULT lResult = 0;
			BOOL bResult = FALSE;
			ASSERT_IF_FAILED(pMsgHandler->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult));
			if (bResult)
				return bResult;
		}
	}

	return 0;
}

STDMETHODIMP CPictureWindow::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(bResult);
	RETURN_IF_FAILED(m_pCommandSupport->PreTranslateMessage(m_hWnd, pMsg, bResult));
	return S_OK;
}

void CPictureWindow::MoveToPicture(BOOL bForward)
{
	int currentBitmapIndex = 0;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		if (m_currentBitmapIndex == -1)
			return;
		if (m_bitmapsUrls.size() == 1)
			return;

		if (bForward)
			++m_currentBitmapIndex;
		else
			--m_currentBitmapIndex;

		if ((size_t)m_currentBitmapIndex == m_bitmapsUrls.size())
			m_currentBitmapIndex = 0;

		if (m_currentBitmapIndex < 0)
			m_currentBitmapIndex = m_bitmapsUrls.size() - 1;

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

		ASSERT_IF_FAILED(StartNextDownload(m_currentBitmapIndex));
		m_currentBitmapIndex = -1;
	}

	Invalidate();
}

LRESULT CPictureWindow::OnLButtomUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MoveToPicture(TRUE);
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

STDMETHODIMP CPictureWindow::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CRect rect(0, 0, 100, 100);
	CalcRect(rect.right, rect.bottom, rect);
	__super::Create(NULL, rect, 0, WS_VISIBLE | WS_BORDER | WS_SYSMENU, WS_EX_COMPOSITED);
	if (hWnd)
		*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CPictureWindow::Show(HWND hWndParent)
{
	m_hWndParent = hWndParent;
	RETURN_IF_FAILED(CreateEx(nullptr, nullptr));
	return S_OK;
}

STDMETHODIMP CPictureWindow::InitCommandSupport(int index)
{
	ATLASSERT(index >= 0);

	CComPtr<IVariantObject> pVariantObject;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &CComVariant(m_bitmapsUrls[index])));

	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pCommandSupport;
	ATLASSERT(pInitializeWithVariantObject);
	RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(pVariantObject));
	return S_OK;
}

STDMETHODIMP CPictureWindow::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);

	CComVariant vMediaUrl;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &vMediaUrl));

	int currentBitmapIndex = 0;
	CComVariant vMediaUrls;
	if (SUCCEEDED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vMediaUrls)) && vMediaUrls.vt == VT_UNKNOWN)
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
			RETURN_IF_FAILED(pMediaUrlObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &vMediaUrlForObject));
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
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::Object::Url, &CComVariant(str)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Id, &CComVariant((INT64)m_hWnd)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_INDEX, &CComVariant(index)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVariant(Twitter::Metadata::Types::ImagePictureWindow)));
	RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));

	return S_OK;
}

STDMETHODIMP CPictureWindow::OnAnimationStep(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep)
{
	Invalidate();

	if (dwStep == STEPS)
	{
		return S_OK;
	}

	RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());
	return 0;
}

STDMETHODIMP CPictureWindow::OnDownloadComplete(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CComVariant vType;
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Type, &vType));

	if (vType.vt != VT_BSTR || CComBSTR(vType.bstrVal) != Twitter::Metadata::Types::ImagePictureWindow)
		return S_OK;

	CComVariant vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		CComVariant vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResultDescription, &vDesc));
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
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));

	if (vId.vt != VT_I8 || vId.llVal != (INT64)m_hWnd)
		return S_OK;

	CComVariant vIndex;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Item::VAR_ITEM_INDEX, &vIndex));

	CComVariant vStream;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::File::StreamObject, &vStream));
	CComQIPtr<IStream> pStream = vStream.punkVal;

	int currentBitmapIndex = 0;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_currentBitmapIndex = vIndex.intVal;
		currentBitmapIndex = m_currentBitmapIndex;
	}

	auto pBitmap = make_shared<Bitmap>(pStream);
	auto hMonitor = GetHMonitor();

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
		ResizeToCurrentBitmap();
	}

	RETURN_IF_FAILED(m_pAnimationService->SetParams(0, 255, STEPS, TARGET_INTERVAL));
	RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());
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

HMONITOR CPictureWindow::GetHMonitor()
{
	if (m_bDisableMonitorSnap)
	{
		const POINT ptZero = { 0, 0 };
		return MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
	}

	if (m_bInitialMonitorDetection)
	{
		m_bInitialMonitorDetection = FALSE;
		return MonitorFromWindow(m_hWndParent, MONITOR_DEFAULTTONULL);
	}

	return MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONULL);
}

void CPictureWindow::CalcRect(int width, int height, CRect& rect)
{
	auto hMonitor = GetHMonitor();

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
		ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwBrushColor));
		ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::PictureWindowText, &dwTextColor));
	}

	static HFONT font = 0;
	if (!font)
	{
		CComPtr<IThemeFontMap> pThemeFontMap;
		ASSERT_IF_FAILED(m_pTheme->GetFontMap(&pThemeFontMap));
		ASSERT_IF_FAILED(pThemeFontMap->GetFont(Twitter::Metadata::Drawing::PictureWindowText, &font));
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
		DrawRoundedRect(CDCHandle(cdc), rectText, false);
		cdc.DrawTextEx(str, str.Length(), &rectText, DT_WORDBREAK | DT_CENTER, NULL);
	}
	else
	{
		DWORD dwAlpha = 0;
		ASSERT_IF_FAILED(m_pAnimationService->GetCurrentValue(&dwAlpha));
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
		bf.SourceConstantAlpha = (BYTE)dwAlpha;
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
		DrawRoundedRect(CDCHandle(cdc), rectText, false);
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
	else if (wParam == VK_RETURN || wParam == VK_RIGHT)
		MoveToPicture(TRUE);
	else if (wParam == VK_LEFT)
		MoveToPicture(FALSE);
	return 0;
}

STDMETHODIMP CPictureWindow::SetTheme(ITheme* pTheme)
{
	CHECK_E_POINTER(pTheme);
	m_pTheme = pTheme;
	return S_OK;
}