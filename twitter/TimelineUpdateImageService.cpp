// TimelineUpdateImageService.cpp : Implementation of CTimelineUpdateImageService

#include "stdafx.h"
#include "TimelineUpdateImageService.h"
#include "Plugins.h"

// CTimelineUpdateImageService

STDMETHODIMP CTimelineUpdateImageService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &m_dwAdvice));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));

	CComQIPtr<IMainWindow> pMainWindow = m_pControl;
	CComPtr<IContainerControl> pContainerControl;
	RETURN_IF_FAILED(pMainWindow->GetContainerControl(&pContainerControl));
	CComQIPtr<ITabbedControl> pTabbedControl = pContainerControl;
	CComPtr<IControl> pControl;
	RETURN_IF_FAILED(pTabbedControl->GetPage(0, &pControl));
	m_pTimelineControl = pControl;

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_UPDATEIMAGES_TIMER, &m_pTimerService));
	RETURN_IF_FAILED(AtlAdvise(m_pTimerService, pUnk, __uuidof(ITimerServiceEventSink), &m_dwAdviceTimer));
	m_pTimerService->StartTimer(300);

	return S_OK;
}

STDMETHODIMP CTimelineUpdateImageService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), m_dwAdvice));
	RETURN_IF_FAILED(AtlUnadvise(m_pTimerService, __uuidof(ITimerServiceEventSink), m_dwAdviceTimer));
	m_pTimerService.Release();
	m_pDownloadService.Release();
	m_pImageManagerService.Release();
	return S_OK;
}

STDMETHODIMP CTimelineUpdateImageService::OnTimer()
{
	hash_set<wstring> ids;
	{
		lock_guard<mutex> lock(m_mutex);
		ids = hash_set<wstring>(m_idsToUpdate);
		m_idsToUpdate.clear();
	}

	if (ids.size())
	{
		RETURN_IF_FAILED(m_pTimelineControl->Invalidate());
	}

	return S_OK;
}

STDMETHODIMP CTimelineUpdateImageService::OnDownloadComplete(IVariantObject *pResult)
{
	CComVariant vId;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_ID, &vId));
	CComVariant vUrl;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_URL, &vUrl));
	CComVariant vFilePath;
	RETURN_IF_FAILED(pResult->GetVariantValue(VAR_FILEPATH, &vFilePath));
	
	RETURN_IF_FAILED(m_pImageManagerService->SetImage(vUrl.bstrVal, vFilePath.bstrVal));

	{
		lock_guard<mutex> lock(m_mutex);
		m_idsToUpdate.insert(vId.bstrVal);
	}

	return S_OK;
}