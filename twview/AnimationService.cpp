#include "stdafx.h"
#include "AnimationService.h"

#define TARGET_RESOLUTION 1

STDMETHODIMP CAnimationService::OnInitialized(IServiceProvider *pServiceProvider)
{
	return S_OK;
}

STDMETHODIMP CAnimationService::OnShutdown()
{
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
	auto res = timeGetDevCaps(&m_tc, sizeof(TIMECAPS));
	ATLASSERT(res == MMSYSERR_NOERROR);
	m_wTimerRes = min(max(m_tc.wPeriodMin, TARGET_RESOLUTION), m_tc.wPeriodMax);
	res = timeBeginPeriod(m_wTimerRes);
	ATLASSERT(res == MMSYSERR_NOERROR);

	m_uiTimerId = timeSetEvent(m_dwTimerInternal, m_wTimerRes, CAnimationService::TimerCallback, (DWORD_PTR)this, TIME_ONESHOT);
	ATLASSERT(m_uiTimerId != 0);
	return S_OK;
}

void CALLBACK CAnimationService::TimerCallback(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	auto p = (CAnimationService*)(dwUser);
	timeKillEvent(p->m_uiTimerId);
	timeEndPeriod(p->m_wTimerRes);
	ATLASSERT(p->m_hControlWnd);
	::SendMessage(p->m_hControlWnd, WM_ANIMATION_TIMER, (WPARAM)p->m_uiTimerId, 0);
}

STDMETHODIMP CAnimationService::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult)
{
	if (hWnd == m_hControlWnd && uMsg == WM_ANIMATION_TIMER && wParam == (WPARAM)m_uiTimerId)
	{
		int stepDiff = ((int)m_dwFinish - (int)m_dwStart) / (int)m_dwSteps;
		m_dwValue = (int)m_dwValue + stepDiff;
		m_dwStep++;
		CComPtr<IAnimationService> pThis;
		RETURN_IF_FAILED(QueryInterface(__uuidof(IAnimationService), (LPVOID*)&pThis));
		RETURN_IF_FAILED(Fire_OnAnimationTimer(pThis, m_dwValue, m_dwStep));
	}
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