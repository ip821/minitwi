#pragma once

#include "twview_i.h"
#include "..\model-libs\viewmdl\IInitializeWithControlImpl.h"
#include "..\twiconn\Plugins.h"
#include "..\model-libs\asyncsvc\asyncsvc_contract_i.h"
#include "Plugins.h"

using namespace ATL;
using namespace std;
using namespace Gdiplus;
using namespace IP;

class CUserAccountControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUserAccountControl, &CLSID_UserAccountControl>,
	public CWindowImpl<CUserAccountControl>,
	public CDialogResize<CUserAccountControl>,
	public IUserAccountControl,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications,
	public IDownloadServiceEventSink,
	public IThemeSupport,
	public IThreadServiceEventSink,
	public IAnimationServiceEventSink
{
public:
	DECLARE_WND_CLASS(L"UserAccountControl")
	CUserAccountControl()
	{

	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CUserAccountControl)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IControl2)
		COM_INTERFACE_ENTRY(IUserAccountControl)
		COM_INTERFACE_ENTRY(IDownloadServiceEventSink)
		COM_INTERFACE_ENTRY(IThemeSupport)
		COM_INTERFACE_ENTRY(IThreadServiceEventSink)
		COM_INTERFACE_ENTRY(IAnimationServiceEventSink)
	END_COM_MAP()

	BEGIN_MSG_MAP(CUserAccountControl)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPrintClient)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_START_ANIMATION, OnStartAnimation)
		DEFAULT_MESSAGE_HANDLER(OnMessage)
	END_MSG_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();

private:
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IImageManagerService> m_pImageManagerService;
	CComPtr<IThemeColorMap> m_pThemeColorMap;
	CComPtr<IThemeFontMap> m_pThemeFontMap;
	CComPtr<IDownloadService> m_pDownloadService;
	CComPtr<IWindowService> m_pWindowService;
	CComPtr<IThreadService> m_pFollowThreadService;
	CComPtr<IThreadService> m_pFollowStatusThreadService;
	CComPtr<ITheme> m_pTheme;
	CComPtr<ISkinCommonControl> m_pSkinCommonControl;
	CComPtr<ISkinUserAccountControl> m_pSkinUserAccountControl;
	CComPtr<IAnimationService> m_pAnimationServiceBackgroundImage;
	CComPtr<IAnimationService> m_pAnimationServiceUserImage;
	CComPtr<IAnimationService> m_pAnimationUserText;

	const DWORD STEPS = 25;
	const DWORD MAX_ALPHA = 255;

	DWORD m_dwAdviceAnimationServiceBackgroundImage = 0;
	DWORD m_dwAdviceAnimationServiceUserImage = 0;
	DWORD m_dwAdviceAnimationServiceUserText = 0;
	DWORD m_dwAdviceDownloadService = 0;
	DWORD m_dwAdviceFollowService = 0;
	DWORD m_dwAdviceFollowStatusService = 0;

	BOOL m_bAnimationBackgroundImagePending = FALSE;
	BOOL m_bAnimationUserImagePending = FALSE;
	BOOL m_bAnimationUserTextRunning = FALSE;
	BOOL m_bActivated = FALSE;

	CComBSTR m_bstrBannerUrl;
	CRect m_rectUserImage;
	CRect m_rectFollowButton;
	CCursor m_handCursor;
	CCursor m_arrowCursor;
	bool m_bFollowButtonDisabled = FALSE;
	bool m_bFollowing = FALSE;

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPrintClient(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnStartAnimation(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void UpdateRects();

	STDMETHOD(StartAnimationUserImage)();
	STDMETHOD(StartAnimationBackgroundImage)();

	struct TSTARTANIMATIONPARAMS
	{
		BSTR bstrKey = nullptr;
		IStream* pStream = nullptr;
		SkinUserAccountControlAnimationType animationType = SkinUserAccountControlAnimationType::AnimationNone;
	};

public:
	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	METHOD_EMPTY(STDMETHOD(CreateEx2)(HWND hWndParent, RECT rect, HWND* hWnd));
	METHOD_EMPTY(STDMETHOD(GetText)(BSTR* pbstr));
	STDMETHOD(OnActivate)();
	STDMETHOD(OnDeactivate)();
	METHOD_EMPTY(STDMETHOD(OnClose)());

	STDMETHOD(SetVariantObject)(IVariantObject *pVariantObject);

	STDMETHOD(OnInitialized)(IServiceProvider *pServiceProvider);
	STDMETHOD(OnShutdown)();

	STDMETHOD(OnDownloadComplete)(IVariantObject *pResult);

	STDMETHOD(SetTheme)(ITheme* pTheme);

	STDMETHOD(OnStart)(IVariantObject *pResult);
	METHOD_EMPTY(STDMETHOD(OnRun)(IVariantObject *pResult));
	STDMETHOD(OnFinish)(IVariantObject *pResult);

	STDMETHOD(OnAnimationStep)(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep);
};

OBJECT_ENTRY_AUTO(__uuidof(UserAccountControl), CUserAccountControl)