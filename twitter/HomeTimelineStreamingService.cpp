// HomeTimelineStreamingService.cpp : Implementation of CHomeTimelineStreamingService

#include "stdafx.h"
#include "HomeTimelineStreamingService.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"
#include "twconn_contract_i.h"
#include "UpdateScope.h"

// CHomeTimelineStreamingService

STDMETHODIMP CHomeTimelineStreamingService::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingService::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_STREAMING_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdviceThreadService));

	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_QUEUE, &m_pTimelineQueueService));

	return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdviceThreadService));
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pSettings.Release();
		m_pTimelineQueueService.Release();
		m_pServiceProvider.Release();
	}

	m_pThreadService.Release();

	return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingService::OnStart(IVariantObject* pResult)
{
	return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingService::OnRun(IVariantObject* pResultObj)
{
	CHECK_E_POINTER(pResultObj);

	CComPtr<IVariantObject> pResult = pResultObj;
	CComPtr<ISettings> pSettings;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		pSettings = m_pSettings;
	}

	CComPtr<ISettings> pSettingsTwitter;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));

	CComBSTR bstrKey;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterKey, &bstrKey));

	CComBSTR bstrSecret;
	RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterSecret, &bstrSecret));

	CComPtr<ITwitterStreamingConnection> pTwitterStreamingConnection;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterStreamingConnection, &pTwitterStreamingConnection));

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pTwitterStreamingConnection = pTwitterStreamingConnection;
	}

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));

	DWORD dwAdvice = 0;
	RETURN_IF_FAILED(AtlAdvise(pTwitterStreamingConnection, pUnk, __uuidof(ITwitterStreamingConnectionEventSink), &dwAdvice));
	RETURN_IF_FAILED(pTwitterStreamingConnection->StartStream(bstrKey, bstrSecret));
	RETURN_IF_FAILED(AtlUnadvise(pTwitterStreamingConnection, __uuidof(ITwitterStreamingConnectionEventSink), dwAdvice));

	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		m_pTwitterStreamingConnection.Release();
	}

	return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingService::OnFinish(IVariantObject* pResult)
{
	CHECK_E_POINTER(pResult);
	return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingService::OnMessages(IObjArray *pObjectArray)
{
	CComPtr<ITimelineQueueService> pTimelineQueueService;
	CComPtr<IServiceProvider> pServiceProvider;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		pTimelineQueueService = m_pTimelineQueueService;
		pServiceProvider = m_pServiceProvider;
	}

	if (!pTimelineQueueService || !pServiceProvider)
		return S_OK;

	CComPtr<IVariantObject> pResult;
	RETURN_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pResult));
	RETURN_IF_FAILED(pResult->SetVariantValue(Twitter::Metadata::Object::Result, &CComVar(pObjectArray)));
	RETURN_IF_FAILED(pTimelineQueueService->AddToQueue(pResult));

	//temp, replace with timer
	CComPtr<IThreadService> pThreadService;
	RETURN_IF_FAILED(pServiceProvider->QueryService(SERVICE_TIMELINE_THREAD, &pThreadService));
	if (!pThreadService)
		return S_OK;
	RETURN_IF_FAILED(pThreadService->Run());

	return S_OK;
}

STDMETHODIMP CHomeTimelineStreamingService::Stop()
{
	CComPtr<ITwitterStreamingConnection> pTwitterStreamingConnection;
	{
		boost::lock_guard<boost::mutex> lock(m_mutex);
		pTwitterStreamingConnection = m_pTwitterStreamingConnection;
	}
	if (pTwitterStreamingConnection)
	{
		RETURN_IF_FAILED(pTwitterStreamingConnection->StopStream());
	}
	return S_OK;
}
