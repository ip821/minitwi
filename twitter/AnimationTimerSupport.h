#pragma once

#include "Plugins.h"

#define TARGET_RESOLUTION 1

template<class T>
class CAnimationTimerSupport
{
private:
	TIMECAPS m_tc;
	UINT m_wTimerRes = 0;
	UINT m_uiTimerId = 0;

public:
	CAnimationTimerSupport()
	{
		timeGetDevCaps(&m_tc, sizeof(TIMECAPS));
		m_wTimerRes = min(max(m_tc.wPeriodMin, TARGET_RESOLUTION), m_tc.wPeriodMax);
		timeBeginPeriod(m_wTimerRes);
	}

	~CAnimationTimerSupport()
	{
		timeEndPeriod(m_wTimerRes);
	}

	static void CALLBACK TimerCallback(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
	{
		auto p = (T*)(dwUser);
		timeKillEvent(p->m_uiTimerId);
		::SendMessage(p->m_hWnd, WM_ANIMATION_TIMER, 0, 0);
	}

	void StopAnimationTimer()
	{
		timeKillEvent(m_uiTimerId);
	}

	void StartAnimationTimer(/*HWND hWnd, */UINT uiInterval)
	{
		auto p = static_cast<T*>(this);
		m_uiTimerId = timeSetEvent(uiInterval, m_wTimerRes, TimerCallback, (DWORD_PTR)p, TIME_ONESHOT);
	}
};