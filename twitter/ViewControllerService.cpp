// ViewControllerService.cpp : Implementation of CViewControllerService

#include "stdafx.h"
#include "ViewControllerService.h"


// CViewControllerService

STDMETHODIMP CViewControllerService::OnInitialized(IServiceProvider *pServiceProvider)
{
	RETURN_IF_FAILED(pServiceProvider->QueryService(CLSID_TimerService, &m_pTimerService));
	RETURN_IF_FAILED(m_pTimerService->StartTimer(1 * 1000 * 60));
	return S_OK;
}

STDMETHODIMP CViewControllerService::OnShutdown()
{
	RETURN_IF_FAILED(m_pTimerService->StopTimer());
	m_pTimerService.Release();
	return S_OK;
}
