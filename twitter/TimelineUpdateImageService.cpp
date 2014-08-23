// TimelineUpdateImageService.cpp : Implementation of CTimelineUpdateImageService

#include "stdafx.h"
#include "TimelineUpdateImageService.h"


// CTimelineUpdateImageService

STDMETHODIMP CTimelineUpdateImageService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_DownloadService, &m_pDownloadService));
	RETURN_IF_FAILED(AtlAdvise(m_pDownloadService, pUnk, __uuidof(IDownloadServiceEventSink), &m_dwAdvice));
	return S_OK;
}

STDMETHODIMP CTimelineUpdateImageService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pDownloadService, __uuidof(IDownloadServiceEventSink), m_dwAdvice));
	m_pDownloadService.Release();
	return S_OK;
}

STDMETHODIMP CTimelineUpdateImageService::OnDownloadComplete(BSTR bstrUrl, BSTR bstrFilePath)
{
	return S_OK;
}