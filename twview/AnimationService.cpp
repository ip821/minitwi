#include "stdafx.h"
#include "AnimationService.h"

#define TARGET_RESOLUTION 1

STDMETHODIMP CAnimationService::OnInitialized(IServiceProvider *pServiceProvider)
{
	ATLASSERT(m_pSettings);
	m_animationTimer.SetHWND(m_hControlWnd);
	return S_OK;
}

STDMETHODIMP CAnimationService::OnShutdown()
{
	m_pSettings.Release();
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	return S_OK;
}

STDMETHODIMP CAnimationService::SetParams(DWORD dwStart, DWORD dwFinish, DWORD dwSteps, DWORD dwTimerInternal)
{
	m_dwStart = dwStart;
	m_dwFinish = dwFinish;
	m_dwSteps = dwSteps;
	m_dwTimerInternal = dwTimerInternal;
	m_dwValue = m_dwStart;
	m_dwStep = 0;
	return S_OK;
}

STDMETHODIMP CAnimationService::StartAnimationTimer()
{
	if (m_bDisableAnimation)
	{
		CComPtr<IAnimationService> pThis;
		RETURN_IF_FAILED(QueryInterface(__uuidof(IAnimationService), (LPVOID*)&pThis));
		RETURN_IF_FAILED(Fire_OnAnimationTimer(pThis, m_dwFinish, m_dwSteps));
	}
	else
	{
		m_animationTimer.StartAnimationTimer(m_dwTimerInternal);
	}
	return S_OK;
}

STDMETHODIMP CAnimationService::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult)
{
	if (hWnd == m_hControlWnd && uMsg == WM_ANIMATION_TIMER && wParam == (WPARAM)&m_animationTimer)
	{
		int stepDiff = ((int)m_dwFinish - (int)m_dwStart) / (int)m_dwSteps;
		m_dwValue = (int)m_dwValue + stepDiff;
		m_dwStep++;
		if (m_dwStep == m_dwSteps && m_dwValue != m_dwFinish)
			m_dwValue = m_dwFinish;
		CComPtr<IAnimationService> pThis;
		RETURN_IF_FAILED(QueryInterface(__uuidof(IAnimationService), (LPVOID*)&pThis));
		RETURN_IF_FAILED(Fire_OnAnimationTimer(pThis, m_dwValue, m_dwStep));
	}
	return S_OK;
}

STDMETHODIMP CAnimationService::GetCurrentValue(DWORD* pdwValue)
{
	CHECK_E_POINTER(pdwValue);
	*pdwValue = m_dwValue;
	return S_OK;
}

HRESULT CAnimationService::Fire_OnAnimationTimer(IAnimationService* pAnimationService, DWORD dwValue, DWORD dwStep)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CAnimationService* pThis = static_cast<CAnimationService*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		IAnimationServiceEventSink * pConnection = static_cast<IAnimationServiceEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnAnimationStep(pAnimationService, dwValue, dwStep);
		}
	}
	return hr;
}

STDMETHODIMP CAnimationService::Load(ISettings* pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	CComPtr<ISettings> pTimelineSettings;
	RETURN_IF_FAILED(pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathTimeline, &pTimelineSettings));
	CComVariant vDisableAnimation;
	RETURN_IF_FAILED(pTimelineSettings->GetVariantValue(Twitter::Metadata::Settings::Timeline::DisableAnimation, &vDisableAnimation));
	m_bDisableAnimation = vDisableAnimation.vt != VT_I4 || (!vDisableAnimation.intVal);
	return S_OK;
}