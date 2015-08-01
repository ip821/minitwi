// PictureWindow.cpp : Implementation of CPictureWindow

#include "stdafx.h"
#include "PictureWindow.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "..\model-libs\layout\GdilPlusUtils.h"

// CPictureWindow

#define IMAGE_PADDING 25
#define BOTTOM_PANEL_HEIGHT 50
#define MIN_VIDEO_HEIGHT 300

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
	m_pSettings = pSettings;
	CComPtr<ISettings> pSubSettings;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(PICTURE_WINDOW_SETTINGS_PATH, &pSubSettings));
	CComVar vDisableMonitorSnap;
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

	RETURN_IF_FAILED(HrInitializeWithControl(m_pPluginSupport, pUnk));
	RETURN_IF_FAILED(HrInitializeWithSettings(m_pPluginSupport, m_pSettings));
	RETURN_IF_FAILED(m_pPluginSupport->OnInitialized());

	m_popupMenu.CreatePopupMenu();
	HrCoCreateInstance(CLSID_CommandSupport, &m_pCommandSupport);
	m_pCommandSupport->SetMenu(m_popupMenu);
	m_pCommandSupport->InstallCommands(m_pPluginSupport);

	return S_OK;
}

STDMETHODIMP CPictureWindow::OnShutdown()
{
	RETURN_IF_FAILED(ShutdownViewControl());

	for (auto& it : m_videoFilePaths)
	{
		DeleteFile(it);
	}

	m_pVariantObject.Release();
	m_pSettings.Release();
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
	m_pVariantObject.Release();
	m_pLayout.Release();
	m_pLayoutManager.Release();
	m_pColumnsInfo.Release();
	return S_OK;
}

LRESULT CPictureWindow::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ASSERT_IF_FAILED(m_pLayoutManager->EraseBackground((HDC)wParam, m_pColumnsInfo));
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

		ASSERT_IF_FAILED(ShutdownViewControl());

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

	if (bContains)
	{
		ResizeToCurrentBitmap();
		ASSERT_IF_FAILED(LoadViewControl());
		return;
	}

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);

		ASSERT_IF_FAILED(StartNextDownload(m_currentBitmapIndex));
		m_currentBitmapIndex = -1;
	}

	Invalidate();
}

STDMETHODIMP CPictureWindow::ShutdownViewControl()
{
	if (m_pViewControl)
	{
		if (::IsWindow(m_hWnd))
		{
			RETURN_IF_FAILED(RebuildLayout());
		}

		RETURN_IF_FAILED(HrNotifyOnShutdown(m_pViewControl));
		m_pViewControl.Release();
	}
	m_hWndViewControl = 0;
	return S_OK;
}

STDMETHODIMP CPictureWindow::LoadViewControl()
{
	ATLASSERT(!m_pViewControl);

	RETURN_IF_FAILED(RebuildLayout());

	CComPtr<IVariantObject> pVariantObject;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	if (!m_videoUrls.size() || m_videoUrls[m_currentBitmapIndex] == L"")
	{
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ImageViewControl, &m_pViewControl));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::Object::Url, &CComVar(m_bitmapsUrls[m_currentBitmapIndex])));
	}
	else
	{
		RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VideoViewControl, &m_pViewControl));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::File::Path, &CComVar(m_videoFilePaths[m_currentBitmapIndex])));
	}

	CComQIPtr<IThemeSupport> pThemeSupport = m_pViewControl;
	if (pThemeSupport)
	{
		RETURN_IF_FAILED(pThemeSupport->SetTheme(m_pTheme));
	}
	RETURN_IF_FAILED(HrInitializeWithControl(m_pViewControl, pUnk));
	RETURN_IF_FAILED(HrInitializeWithSettings(m_pViewControl, m_pSettings));
	RETURN_IF_FAILED(HrInitializeWithVariantObject(m_pViewControl, pVariantObject));
	RETURN_IF_FAILED(m_pViewControl->CreateEx(m_hWnd, &m_hWndViewControl));
	AdjustSize();
	RETURN_IF_FAILED(HrNotifyOnInitialized(m_pViewControl, m_pServiceProvider));
	Invalidate();
	return S_OK;
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
	__super::Create(NULL, rect, 0, WS_VISIBLE | WS_BORDER | WS_SYSMENU, WS_EX_CONTROLPARENT);
	if (hWnd)
		*hWnd = m_hWnd;
	ResizeWindow(rect.Width(), rect.Height());
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
	RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &CComVar(m_bitmapsUrls[index])));

	if (m_videoUrls.size() && m_videoUrls[index] != L"")
	{
		CComVar vTwitUrl;
		RETURN_IF_FAILED(m_pVariantObject->GetVariantValue(Twitter::Metadata::Object::Url, &vTwitUrl));
		RETURN_IF_FAILED(pVariantObject->SetVariantValue(Twitter::Metadata::Object::Url, &vTwitUrl));
	}

	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pCommandSupport;
	ATLASSERT(pInitializeWithVariantObject);
	RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(pVariantObject));
	return S_OK;
}

STDMETHODIMP CPictureWindow::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);

	m_pVariantObject = pVariantObject;

	CComVar vMediaUrl;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &vMediaUrl));

	int currentBitmapIndex = 0;
	CComVar vMediaUrls;
	if (SUCCEEDED(pVariantObject->GetVariantValue(Twitter::Connection::Metadata::TweetObject::MediaUrls, &vMediaUrls)) && vMediaUrls.vt == VT_UNKNOWN)
	{
		CComQIPtr<IObjArray> pMediaUrls = vMediaUrls.punkVal;
		UINT uiCount = 0;
		RETURN_IF_FAILED(pMediaUrls->GetCount(&uiCount));
		m_bitmapsUrls.resize(uiCount);
#ifndef __WINXP__
		m_videoUrls.resize(uiCount);
#endif
		m_videoFilePaths.resize(uiCount);
		m_sizes.resize(uiCount);
		for (size_t i = 0; i < uiCount; ++i)
		{
			CComPtr<IVariantObject> pMediaUrlObject;
			RETURN_IF_FAILED(pMediaUrls->GetAt(i, __uuidof(IVariantObject), (LPVOID*)&pMediaUrlObject));
			CComVar vMediaUrlForObject;
			RETURN_IF_FAILED(pMediaUrlObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaUrl, &vMediaUrlForObject));
			m_bitmapsUrls[i] = vMediaUrlForObject.bstrVal;

#ifndef __WINXP__
			CComVar vVideoUrlForObject;
			RETURN_IF_FAILED(pMediaUrlObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaVideoUrl, &vVideoUrlForObject));
			m_videoUrls[i] = vVideoUrlForObject.bstrVal;
#endif
			CComVar vHeight;
			RETURN_IF_FAILED(pMediaUrlObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaHeight, &vHeight));
			CComVar vWidth;
			RETURN_IF_FAILED(pMediaUrlObject->GetVariantValue(Twitter::Connection::Metadata::MediaObject::MediaWidth, &vWidth));
			ATLASSERT(vHeight.vt == VT_I4 && vWidth.vt == VT_I4);

#ifndef __WINXP__
			if (m_videoUrls[i] != L"" && vHeight.intVal < MIN_VIDEO_HEIGHT)
			{
				float ratio = (float)MIN_VIDEO_HEIGHT / vHeight.intVal;
				vHeight.intVal = (int)(vHeight.intVal * ratio);
				vWidth.intVal = (int)(vWidth.intVal * ratio);
			}
#endif
			m_sizes[i] = CSize(vWidth.intVal, vHeight.intVal);

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

	auto strImageUrl = m_bitmapsUrls[index];
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::Object::Url, &CComVar(strImageUrl)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Id, &CComVar((INT64)m_hWnd)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::Item::VAR_ITEM_INDEX, &CComVar(index)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVar(Twitter::Metadata::Types::ImagePictureWindow)));

	if (m_videoUrls.size())
	{
		auto strVideoUrl = m_videoUrls[index];
		if (strVideoUrl != L"")
		{
			auto lpszExt = PathFindExtension(strVideoUrl);
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::Object::Url, &CComVar(strVideoUrl)));
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(ObjectModel::Metadata::Object::Type, &CComVar(Twitter::Metadata::Types::VideoPictureWindow)));
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::File::Extension, &CComVar(lpszExt)));
			RETURN_IF_FAILED(pDownloadTask->SetVariantValue(Twitter::Metadata::File::KeepFileFlag, &CComVar(true)));
		}
	}

	RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));

	return S_OK;
}

STDMETHODIMP CPictureWindow::OnDownloadComplete(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CComVar vType;
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Type, &vType));

	if (
		vType.vt != VT_BSTR
		||
			(
				CComBSTR(vType.bstrVal) != Twitter::Metadata::Types::ImagePictureWindow
				&&
				CComBSTR(vType.bstrVal) != Twitter::Metadata::Types::VideoPictureWindow
				)
		)
		return S_OK;

	CComVar vHr;
	RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResult, &vHr));
	if (FAILED(vHr.intVal))
	{
		CComVar vDesc;
		RETURN_IF_FAILED(pResult->GetVariantValue(AsyncServices::Metadata::Thread::HResultDescription, &vDesc));
		CComBSTR bstrMsg = L"Unknown error";
		if (vDesc.vt == VT_BSTR)
			bstrMsg = vDesc.bstrVal;

		m_strLastErrorMsg = bstrMsg;
		ResizeWindow(200, 200);
		Invalidate();
		return S_OK;
	}

	CComVar vId;
	RETURN_IF_FAILED(pResult->GetVariantValue(ObjectModel::Metadata::Object::Id, &vId));

	if (vId.vt != VT_I8 || vId.llVal != (INT64)m_hWnd)
		return S_OK;

	CComVar vIndex;
	RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::Item::VAR_ITEM_INDEX, &vIndex));

	int currentBitmapIndex = 0;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_currentBitmapIndex = vIndex.intVal;
		currentBitmapIndex = m_currentBitmapIndex;
	}

	if (CComBSTR(vType.bstrVal) == Twitter::Metadata::Types::ImagePictureWindow)
	{
		CComVar vStream;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::File::StreamObject, &vStream));
		CComQIPtr<IStream> pStream = vStream.punkVal;

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
	}
	else if (CComBSTR(vType.bstrVal) == Twitter::Metadata::Types::VideoPictureWindow)
	{
		CComVar vFilePath;
		RETURN_IF_FAILED(pResult->GetVariantValue(Twitter::Metadata::File::Path, &vFilePath));
		ATLASSERT(vFilePath.vt == VT_BSTR);
		m_videoFilePaths[currentBitmapIndex] = vFilePath.bstrVal;
	}

	ResizeToCurrentBitmap();
	SendMessage(WM_UPDATE_VIEW_CONTROL);

	return S_OK;
}

LRESULT CPictureWindow::OnUpdateViewControl(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ASSERT_IF_FAILED(LoadViewControl());
	return 0;
}

void CPictureWindow::AdjustSize()
{
	if (m_hWndViewControl)
	{
		CRect clientRect;
		GetClientRect(&clientRect);
		if (m_bitmapsUrls.size() > 1)
			clientRect.bottom -= BOTTOM_PANEL_HEIGHT;
		::SetWindowPos(m_hWndViewControl, 0, 0, 0, clientRect.Width(), clientRect.Height(), 0);
	}
}

void CPictureWindow::ResizeWindow(UINT uiWidth, UINT uiHeight)
{
	CRect rect;
	GetWindowRect(&rect);

	CalcRect(uiWidth, uiHeight, rect);
	if (m_bitmapsUrls.size() > 1)
	{
		rect.bottom += BOTTOM_PANEL_HEIGHT;
	}
	SetWindowPos(NULL, &rect, SWP_NOZORDER);
	AdjustSize();

	ASSERT_IF_FAILED(RebuildLayout());
}

STDMETHODIMP CPictureWindow::RebuildLayout()
{
	RETURN_IF_FAILED(m_pColumnsInfo->Clear());

	CComPtr<IVariantObject> pLayoutItemCenterItem;
	RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayout, L"CenterString", &pLayoutItemCenterItem));
	RETURN_IF_FAILED(HrLayoutSetVariantValueRecursive(pLayoutItemCenterItem, Layout::Metadata::Element::Visible, &CComVar(m_pViewControl == nullptr)));

	CComPtr<IVariantObject> pLayoutItemImageNumber;
	RETURN_IF_FAILED(HrLayoutFindItemByName(m_pLayout, L"SelectedImageString", &pLayoutItemImageNumber));
	RETURN_IF_FAILED(HrLayoutSetVariantValueRecursive(pLayoutItemImageNumber, Layout::Metadata::Element::Visible, &CComVar(false)));

	int currentBitmapIndex = m_currentBitmapIndex;
	//{
	//	boost::lock_guard<boost::mutex> lock(m_mutex);
	//	currentBitmapIndex = m_currentBitmapIndex;
	//}

	if (m_bitmapsUrls.size() > 1 && m_currentBitmapIndex >= 0)
	{
		CString str;
		str.Format(L"%u / %u", (UINT)currentBitmapIndex + 1, m_bitmapsUrls.size());

		RETURN_IF_FAILED(pLayoutItemImageNumber->SetVariantValue(Layout::Metadata::TextColumn::Text, &CComVar(str)));
		RETURN_IF_FAILED(HrLayoutSetVariantValueRecursive(pLayoutItemImageNumber, Layout::Metadata::Element::Visible, &CComVar(true)));
	}

	CRect rect;
	GetClientRect(&rect);
	CClientDC cdc(m_hWnd);
	RETURN_IF_FAILED(m_pLayoutManager->BuildLayout(cdc, &rect, m_pLayout, nullptr, m_pImageManagerService, m_pColumnsInfo));

	return S_OK;
}

void CPictureWindow::ResizeToCurrentBitmap()
{
	TBITMAP tBitmap = { 0 };
	UINT width = 0;
	UINT height = 0;
	if (m_pImageManagerService->GetImageInfo(m_bitmapsUrls[m_currentBitmapIndex], &tBitmap) == S_OK)
	{
		width = tBitmap.Width;
		height = tBitmap.Height;
	}
	else
	{
		if (!m_sizes.size())
			return;

		width = m_sizes[m_currentBitmapIndex].cx;
		height = m_sizes[m_currentBitmapIndex].cy;
	}
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
	BeginPaint(&ps);
	ASSERT_IF_FAILED(m_pLayoutManager->PaintLayout(ps.hdc, m_pImageManagerService, m_pColumnsInfo));
	EndPaint(&ps);

	return 0;
	//PAINTSTRUCT ps = { 0 };
	//BeginPaint(&ps);
	//CDCHandle cdcReal(ps.hdc);

	//CRect rect;
	//GetClientRect(&rect);

	//int currentBitmapIndex = 0;
	//{
	//	boost::lock_guard<boost::mutex> lock(m_mutex);
	//	currentBitmapIndex = m_currentBitmapIndex;
	//}

	//static DWORD dwBrushColor = 0;
	//static DWORD dwTextColor = 0;
	//if (!dwBrushColor)
	//{
	//	CComPtr<IThemeColorMap> pThemeColorMap;
	//	ASSERT_IF_FAILED(m_pTheme->GetColorMap(&pThemeColorMap));
	//	ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::BrushBackground, &dwBrushColor));
	//	ASSERT_IF_FAILED(pThemeColorMap->GetColor(Twitter::Metadata::Drawing::PictureWindowText, &dwTextColor));
	//}

	//static HFONT font = 0;
	//if (!font)
	//{
	//	CComPtr<IThemeFontMap> pThemeFontMap;
	//	ASSERT_IF_FAILED(m_pTheme->GetFontMap(&pThemeFontMap));
	//	ASSERT_IF_FAILED(pThemeFontMap->GetFont(Twitter::Metadata::Drawing::PictureWindowText, &font));
	//}

	//CDCSelectFontScope cdcSelectFontScope(cdcReal, font);

	//cdcReal.SetBkMode(TRANSPARENT);
	//cdcReal.SetTextColor(dwTextColor);

	//cdcReal.FillSolidRect(&rect, dwBrushColor);

	//if (!m_pViewControl)
	//{
	//	CComBSTR str = L"Downloading...";
	//	if (!m_strLastErrorMsg.IsEmpty())
	//		str = m_strLastErrorMsg;

	//	CSize size;
	//	CRect rect1 = rect;
	//	cdcReal.DrawTextEx(str, str.Length(), &rect1, DT_WORDBREAK | DT_CENTER | DT_CALCRECT, NULL);
	//	size.cx = rect1.Width();
	//	size.cy = rect1.Height();

	//	auto x = (rect.right - rect.left) / 2 - (size.cx / 2);
	//	auto y = (rect.bottom - rect.top) / 2 - (size.cy / 2);

	//	CRect rectText(x, y, x + size.cx, y + size.cy);
	//	DrawRoundedRect(cdcReal, rectText, false);
	//	cdcReal.DrawTextEx(str, str.Length(), &rectText, DT_WORDBREAK | DT_CENTER, NULL);
	//}

	//if (m_bitmapsUrls.size() > 1 && m_currentBitmapIndex >= 0)
	//{
	//	CString str;
	//	str.Format(L"%u / %u", (UINT)currentBitmapIndex + 1, m_bitmapsUrls.size());
	//	CSize sz;
	//	cdcReal.GetTextExtent(str, str.GetLength(), &sz);

	//	auto x = rect.Width() / 2;
	//	auto y = rect.bottom - BOTTOM_PANEL_HEIGHT / 2 - sz.cy / 2;

	//	CRect rectText(x, y, x + sz.cx, y + sz.cy);
	//	DrawRoundedRect(cdcReal, rectText, false);
	//	cdcReal.DrawText(str, str.GetLength(), rectText, 0);
	//}

	//EndPaint(&ps);
	//return 0;
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
	RETURN_IF_FAILED(m_pTheme->GetLayout(L"PictureWindowLayout", &m_pLayout));
	RETURN_IF_FAILED(m_pTheme->GetLayoutManager(&m_pLayoutManager));
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_ColumnsInfo, &m_pColumnsInfo));
	return S_OK;
}