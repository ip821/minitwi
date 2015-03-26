#pragma once

#include "Plugins.h"

#define TARGET_RESOLUTION 1

template<class T>
class CAnimationTimerSupport
{
private:
	TIMECAPS m_tc;
	UINT m_wTimerRes = 0;
	HWND m_hWnd = 0;
	UINT m_uiTimerId = 0;

public:

	CAnimationTimerSupport()
	{
		m_tc = { 0 };
	}

	~CAnimationTimerSupport()
	{
	}

	void SetHWND(HWND hWnd)
	{
		m_hWnd = hWnd;
	}

	static void CALLBACK TimerCallback(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
	{
		auto p = (CAnimationTimerSupport*)(dwUser);
		timeKillEvent(p->m_uiTimerId);
		timeEndPeriod(p->m_wTimerRes);
		::SendMessage(p->m_hWnd, WM_ANIMATION_TIMER, (WPARAM)p, 0);
	}

	void StopAnimationTimer()
	{
		auto res = timeKillEvent(m_uiTimerId);
		ATLASSERT(res == MMSYSERR_NOERROR);
		res = timeEndPeriod(m_wTimerRes);
		ATLASSERT(res == MMSYSERR_NOERROR); CMenuItemInfo; MENUITEMINFO
	}

	void StartAnimationTimer(/*HWND hWnd, */UINT uiInterval)
	{
		auto res = timeGetDevCaps(&m_tc, sizeof(TIMECAPS));
		ATLASSERT(res == MMSYSERR_NOERROR);
		m_wTimerRes = min(max(m_tc.wPeriodMin, TARGET_RESOLUTION), m_tc.wPeriodMax);
		res = timeBeginPeriod(m_wTimerRes);
		ATLASSERT(res == MMSYSERR_NOERROR);

		m_uiTimerId = timeSetEvent(uiInterval, m_wTimerRes, TimerCallback, (DWORD_PTR)this, TIME_ONESHOT);
		ATLASSERT(m_uiTimerId != 0);
	}
};