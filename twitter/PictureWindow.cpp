// PictureWindow.cpp : Implementation of CPictureWindow

#include "stdafx.h"
#include "PictureWindow.h"
#include "Plugins.h"

// CPictureWindow

#define IMAGE_PADDING 25

STDMETHODIMP CPictureWindow::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &m_dwAdviceDownloadService));

	return S_OK;
}

STDMETHODIMP CPictureWindow::OnShutdown()
{
	if (IsWindow())
		DestroyWindow();

	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), m_dwAdviceDownloadService));
	m_pDownloadService.Release();
	return S_OK;
}

HRESULT CPictureWindow::FinalConstruct()
{
	return S_OK;
}

void CPictureWindow::FinalRelease()
{
}

LRESULT CPictureWindow::OnRButtomUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto x = GET_X_LPARAM(lParam);
	auto y = GET_Y_LPARAM(lParam);
	CPoint pt(x, y);
	ClientToScreen(&pt);
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

	HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport);
	m_pPluginSupport->InitializePlugins(PNAMESP_PICTUREWINDOW_CONTROL, PVIEWTYPE_COMMAND);
	m_pPluginSupport->OnInitialized();

	m_popupMenu.CreatePopupMenu();
	HrCoCreateInstance(CLSID_CommandSupport, &m_pCommandSupport);
	m_pCommandSupport->SetMenu(m_popupMenu);
	m_pCommandSupport->InstallCommands(m_pPluginSupport);

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
	CRect rect(0, 0, 100, 100);
	__super::Create(NULL, rect, 0, WS_VISIBLE | WS_BORDER | WS_SYSMENU);
	SetPosition(100, 100, rect);
	return S_OK;
}

STDMETHODIMP CPictureWindow::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);

	CComVariant vMediaUrl;
	RETURN_IF_FAILED(pVariantObject->GetVariantValue(VAR_TWITTER_MEDIAURL, &vMediaUrl));

	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pCommandSupport;
	if (pInitializeWithVariantObject)
	{
		RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(pVariantObject));
	}

	CComPtr<IVariantObject> pDownloadTask;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pDownloadTask));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_URL, &CComVariant(vMediaUrl.bstrVal)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_ID, &CComVariant((INT64)m_hWnd)));
	RETURN_IF_FAILED(pDownloadTask->SetVariantValue(VAR_OBJECT_TYPE, &CComVariant(L"TYPE_IMAGE_PICTURE_WINDOW")));
	RETURN_IF_FAILED(m_pDownloadService->AddDownload(pDownloadTask));
	return S_OK;
}

STDMETHODIMP CPictureWindow::OnDownloadComplete(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);

	CComVariant vType;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_OBJECT_TYPE, &vType));

	if (vType.vt != VT_BSTR || CComBSTR(vType.bstrVal) != CComBSTR(L"TYPE_IMAGE_PICTURE_WINDOW"))
		return S_OK;

	CComVariant vId;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_ID, &vId));

	if (vId.vt != VT_I8 || vId.llVal != (INT64)m_hWnd)
		return S_OK;

	CComVariant vFilePath;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));

	{
		lock_guard<mutex> lock(m_mutex);
		ATLASSERT(!m_pBitmap);
		m_pBitmap = std::make_shared<Gdiplus::Bitmap>(vFilePath.bstrVal);
	}

	CRect rect;
	GetWindowRect(&rect);

	auto width = m_pBitmap->GetWidth();
	auto height = m_pBitmap->GetHeight();

	SetPosition(width, height, rect);
	return S_OK;
}

void CPictureWindow::SetPosition(int width, int height, CRect& rect)
{
	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	rect.left = nScreenWidth / 2 - width / 2;
	rect.top = nScreenHeight / 2 - height / 2;

	rect.right = rect.left + width + IMAGE_PADDING * 2;
	rect.bottom = rect.top + height + IMAGE_PADDING * 2;

	SetWindowPos(NULL, &rect, SWP_NOZORDER);
}

void CPictureWindow::OnFinalMessage(HWND hWnd)
{
	Fire_OnClosed(hWnd);
}

LRESULT CPictureWindow::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PAINTSTRUCT ps;
	CDC cdc(BeginPaint(&ps));

	CRect rect;
	GetClientRect(&rect);

	{
		lock_guard<mutex> lock(m_mutex);

		if (!m_pBitmap)
		{
			CString str = L"Downloading...";
			CSize size;
			GetTextExtentPoint32(cdc, str, str.GetLength(), &size);

			static Gdiplus::Color colorText(Gdiplus::Color::Black);
			cdc.SetTextColor(colorText.ToCOLORREF());

			auto x = (rect.right - rect.left) / 2 - (size.cx / 2);
			auto y = (rect.bottom - rect.top) / 2 - (size.cy / 2);
			cdc.DrawText(str, str.GetLength(), CRect(x, y, x + size.cx, y + size.cy), 0);
		}
		else
		{
			auto width = m_pBitmap->GetWidth();
			auto height = m_pBitmap->GetHeight();
			auto x = (rect.right - rect.left) / 2 - (width / 2);
			auto y = (rect.bottom - rect.top) / 2 - (height / 2);

			CBitmap bitmap;
			m_pBitmap->GetHBITMAP(Gdiplus::Color::White, &bitmap.m_hBitmap);
			CDC cdcBitmap;
			cdcBitmap.CreateCompatibleDC(cdc);
			cdcBitmap.SelectBitmap(bitmap);
			BitBlt(cdc, x, y, width, height, cdcBitmap, 0, 0, SRCCOPY);
		}
	}
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
	return 0;
}