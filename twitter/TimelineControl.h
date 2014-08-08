// TimelineControl.h : Declaration of the CTimelineControl

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "CustomListBox.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CTimelineControl

class ATL_NO_VTABLE CTimelineControl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineControl, &CLSID_TimelineControl>,
	public IControl,
	public IContainerControl,
	public IMsgHandler,
	public CAxDialogImpl<CTimelineControl>,
	public CDialogResize<CTimelineControl>,
	public IInitializeWithControlImpl
{
public:
	enum { IDD = IDD_TIMELINECONTROL };

	CTimelineControl()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TIMELINECONTROL)

	BEGIN_COM_MAP(CTimelineControl)
		COM_INTERFACE_ENTRY(IControl)
		COM_INTERFACE_ENTRY(IContainerControl)
		COM_INTERFACE_ENTRY(IMsgHandler)
	END_COM_MAP()
	
	BEGIN_DLGRESIZE_MAP(CTimelineControl)
		DLGRESIZE_CONTROL(IDC_LIST1, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CTimelineControl)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(CDialogResize<CTimelineControl>)
		CHAIN_MSG_MAP(CAxDialogImpl<CTimelineControl>)
	END_MSG_MAP()

private:
	CCustomListBox m_listBox;

public:

	STDMETHOD(GetHWND)(HWND *hWnd);
	STDMETHOD(CreateEx)(HWND hWndParent, HWND *hWnd);
	STDMETHOD(PreTranslateMessage)(MSG *pMsg, BOOL *pbResult);

	STDMETHOD(UpdateControl)(IControl *pControl);

	STDMETHOD(ProcessWindowMessage)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult);

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineControl), CTimelineControl)
