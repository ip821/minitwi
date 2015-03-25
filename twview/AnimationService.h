#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;
using namespace IP;

class ATL_NO_VTABLE CAnimationService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CAnimationService, &CLSID_AnimationService>,
	public IPluginSupportNotifications,
	public IInitializeWithControlImpl,
	public IMsgHandler,
	public IAnimationService,
	public IConnectionPointContainerImpl<CAnimationService>,
	public IConnectionPointImpl<CAnimationService, &__uuidof(IAnimationServiceEventSink)>
{
public:

	CAnimationService()
	{
		m_tc = { 0 };
	}

	DECLARE_NO_REGISTRY()

	BEGIN_CONNECTION_POINT_MAP(CAnimationService)
		CONNECTION_POINT_ENTRY(__uuidof(IAnimationServiceEventSink))
	END_CONNECTION_POINT_MAP()

	BEGIN_COM_MAP(CAnimationService)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IMsgHandler)
		COM_INTERFACE_ENTRY(IAnimationService)
		COM_INTERFACE_ENTRY(IConnectionPointContainer)
	END_COM_MAP()

private:
	TIMECAPS m_tc;
	UINT m_wTimerRes = 0;
	UINT m_uiTimerId = 0;

	DWORD m_dwValue = 0;
	DWORD m_dwStep = 0;
	DWORD m_dwStart = 0;
	DWORD m_dwFinish = 0;
	DWORD m_dwSteps = 0;
	DWORD m_dwTimerInternal = 0;

	static void CALLBACK CAnimationService::TimerCallback(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);
	HRESULT Fire_OnAnimationTimer(IAnimationService* pAnimationService, DWORD dwValue, DWORD dwStep);

public:
	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *plResult, BOOL *bResult);

	STDMETHOD(SetParams)(DWORD dwStart, DWORD dwFinish, DWORD dwSteps, DWORD dwTimerInternal);
	STDMETHOD(StartAnimationTimer)();
};

OBJECT_ENTRY_AUTO(__uuidof(AnimationService), CAnimationService)