// WindowService.cpp : Implementation of CWindowService

#include "stdafx.h"
#include "WindowService.h"


// CWindowService

STDMETHODIMP CWindowService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	return S_OK;
}

STDMETHODIMP CWindowService::OnShutdown()
{
	return S_OK;
}

STDMETHODIMP CWindowService::OnClosed(HWND hWndSelf)
{
	auto it = m_windows.find(hWndSelf);
	ATLASSERT(it != m_windows.end());
	RETURN_IF_FAILED(AtlUnadvise(it->second.m_pWindow, __uuidof(IWindowEventSink), it->second.m_dwAdvice));
	CComQIPtr<IPluginSupportNotifications> pInit = it->second.m_pWindow;
	if (pInit)
	{
		RETURN_IF_FAILED(pInit->OnShutdown());
	}
	m_windows.erase(it);
	return S_OK;
}

STDMETHODIMP CWindowService::OpenWindow(HWND hWndParent, REFCLSID clsid, IVariantObject* pVariantObject)
{
	CComPtr<IWindow> pWindow;
	RETURN_IF_FAILED(HrCoCreateInstance(clsid, &pWindow));
	RETURN_IF_FAILED(pWindow->Show(hWndParent));
	HWND hWnd = 0;
	RETURN_IF_FAILED(pWindow->GetHWND(&hWnd));
	WindowData windowData;
	windowData.m_pWindow = pWindow;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(AtlAdvise(pWindow, pUnk, __uuidof(IWindowEventSink), &windowData.m_dwAdvice));

	m_windows[hWnd] = windowData;

	{
		CComQIPtr<IInitializeWithControl> pInit = pWindow;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->SetControl(m_pControl));
		}
	}

	{
		CComQIPtr<IPluginSupportNotifications> pInit = pWindow;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->OnInitialized(m_pServiceProvider));
		}
	}

	{
		CComQIPtr<IInitializeWithVariantObject> pInit = pWindow;
		if (pInit)
		{
			RETURN_IF_FAILED(pInit->SetVariantObject(pVariantObject));
		}
	}
	return S_OK;
}