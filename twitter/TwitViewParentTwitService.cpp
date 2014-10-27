#include "stdafx.h"
#include "TwitViewParentTwitService.h"
#include "Plugins.h"

STDMETHODIMP CTwitViewParentTwitService::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(SERVICE_TWITVIEWPARENTTWITSERVICE_THREAD, &m_pThreadService));
	RETURN_IF_FAILED(AtlAdvise(m_pThreadService, pUnk, __uuidof(IThreadServiceEventSink), &m_dwAdvice));
	return S_OK;
}

STDMETHODIMP CTwitViewParentTwitService::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pThreadService, __uuidof(IThreadServiceEventSink), m_dwAdvice));
	{
		lock_guard<mutex> lock(m_mutex);
		m_pSettings.Release();
	}
	m_pThreadService.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CTwitViewParentTwitService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	{
		lock_guard<mutex> lock(m_mutex);
		m_pSettings = pSettings;
	}
	return S_OK;
}

STDMETHODIMP CTwitViewParentTwitService::OnStart(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);
	return S_OK;
}

STDMETHODIMP CTwitViewParentTwitService::OnRun(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);
	CComPtr<ISettings> pSettings;
	{
		lock_guard<mutex> lock(m_mutex);
		pSettings = m_pSettings;
	}
	return S_OK;
}

STDMETHODIMP CTwitViewParentTwitService::OnFinish(IVariantObject *pResult)
{
	CHECK_E_POINTER(pResult);
	return S_OK;
}
