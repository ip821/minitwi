// TimelineControl.cpp : Implementation of CTimelineControl

#include "stdafx.h"
#include "TimelineControl.h"
#include "Plugins.h"

// CTimelineControl

HRESULT CTimelineControl::FinalConstruct()
{
	return S_OK;
}

void CTimelineControl::FinalRelease()
{
	if (m_hWnd)
		DestroyWindow();
}

STDMETHODIMP CTimelineControl::OnInitialized(IServiceProvider* pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;

	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	RETURN_IF_FAILED(AtlAdvise(m_pCommandSupport, pUnk, __uuidof(ICommandSupportEventSink), &m_dwAdviceCommandSupport));
	RETURN_IF_FAILED(IInitializeWithControlImpl::OnShutdown());
	return S_OK;
}

STDMETHODIMP CTimelineControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pCommandSupport, __uuidof(ICommandSupportEventSink), m_dwAdviceCommandSupport));
	m_pServiceProvider.Release();
	//exit(0); //skip OnShutdown calls, thread joins etc.
	return S_OK;
}

STDMETHODIMP CTimelineControl::GetText(BSTR* pbstr)
{
	*pbstr = CComBSTR(L"Home").Detach();
	return S_OK;
}

STDMETHODIMP CTimelineControl::GetHWND(HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CTimelineControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CHECK_E_POINTER(hWnd);
	m_hWnd = __super::Create(hWndParent);
	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CTimelineControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(pbResult);
	RETURN_IF_FAILED(m_pCommandSupport->PreTranslateMessage(m_hWnd, pMsg, pbResult));
	return S_OK;
}

STDMETHODIMP CTimelineControl::OnBeforeCommandInvoke(REFGUID guidCommand, ICommand* pCommand)
{
	CComQIPtr<IInitializeWithVariantObject> pInit = pCommand;
	if (pInit)
	{
		CComPtr<IVariantObject> pVariantObject;
		auto selectedIndex = m_listBox.GetCurSel();
		CComPtr<IObjArray> pObjArray;
		RETURN_IF_FAILED(m_listBox.GetItems(&pObjArray));
		pObjArray->GetAt(selectedIndex, __uuidof(IVariantObject), (LPVOID*)&pVariantObject);
		RETURN_IF_FAILED(pInit->SetVariantObject(pVariantObject));
	}

	return S_OK;
}

STDMETHODIMP CTimelineControl::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* plResult, BOOL* bResult)
{
	CHECK_E_POINTER(bResult);
	*bResult = ProcessWindowMessage(hWnd, uMsg, wParam, lParam, *plResult);
	return S_OK;
}

LRESULT CTimelineControl::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	DlgResize_Init(false);
	auto dwExStyle = GetWindowLong(GWL_EXSTYLE);
	dwExStyle |= WS_EX_COMPOSITED;
	SetWindowLong(GWL_EXSTYLE, dwExStyle);
	m_listBox.SubclassWindow(GetDlgItem(IDC_LIST1));
	HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport);
	m_pPluginSupport->InitializePlugins(PNAMESP_TIMELINE_CONTROL, PVIEWTYPE_COMMAND);
	CComQIPtr<IInitializeWithControl> pInit = m_pPluginSupport;
	if (pInit)
	{
		pInit->SetControl(m_pControl);
	}

	m_pPluginSupport->OnInitialized();

	m_popupMenu.CreatePopupMenu();
	HrCoCreateInstance(CLSID_CommandSupport, &m_pCommandSupport);
	m_pCommandSupport->SetMenu(m_popupMenu);
	m_pCommandSupport->InstallCommands(m_pPluginSupport);
	bHandled = FALSE;
	return 0;
}

STDMETHODIMP CTimelineControl::BeginUpdate()
{
	m_listBox.BeginUpdate();
	return S_OK;
}

STDMETHODIMP CTimelineControl::EndUpdate()
{
	m_listBox.EndUpdate();
	return S_OK;
}

STDMETHODIMP CTimelineControl::OnItemsUpdated()
{
	m_listBox.OnItemsUpdated();
	return S_OK;
}

STDMETHODIMP CTimelineControl::Clear()
{
	m_listBox.Clear();
	return S_OK;
}

STDMETHODIMP CTimelineControl::IsEmpty(BOOL* pbIsEmpty)
{
	CHECK_E_POINTER(pbIsEmpty);
	m_listBox.IsEmpty(pbIsEmpty);
	return S_OK;
}

STDMETHODIMP CTimelineControl::GetItems(IObjArray** ppObjectArray)
{
	RETURN_IF_FAILED(m_listBox.GetItems(ppObjectArray));
	return S_OK;
}

STDMETHODIMP CTimelineControl::InsertItems(IObjArray* pObjectArray, UINT uiStartIndex)
{
	UINT uiCount = 0;
	RETURN_IF_FAILED(pObjectArray->GetCount(&uiCount));
	for (size_t i = 0; i < uiCount; i++)
	{
		CComPtr<IVariantObject> pVariantObject;
		RETURN_IF_FAILED(pObjectArray->GetAt(i, IID_IVariantObject, (LPVOID*)&pVariantObject));

		m_listBox.InsertItem(pVariantObject, uiStartIndex + i);
	}
	return S_OK;
}

STDMETHODIMP CTimelineControl::InvalidateItems(IVariantObject** pItemArray, UINT uiCountArray)
{
	m_listBox.InvalidateItems(pItemArray, uiCountArray);
	return S_OK;
}

STDMETHODIMP CTimelineControl::UpdateControl(IControl *pControl)
{
	return S_OK;
}

STDMETHODIMP CTimelineControl::SetSkinTimeline(ISkinTimeline* pSkinTimeline)
{
	m_listBox.SetSkinTimeline(pSkinTimeline);
	return S_OK;
}

STDMETHODIMP CTimelineControl::RefreshItem(UINT uiIndex)
{
	m_listBox.RefreshItem(uiIndex);
	return S_OK;
}

LRESULT CTimelineControl::OnColumnClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	NMCOLUMNCLICK* pNm = reinterpret_cast<NMCOLUMNCLICK*>(pnmh);
	if (pNm->dwCurrentColumn == CCustomListBox::INVALID_COLUMN_INDEX)
		return 0;

	BOOL bIsUrl = FALSE;
	pNm->pColumnRects->GetRectBoolProp(pNm->dwCurrentColumn, VAR_IS_URL, &bIsUrl);

	if (bIsUrl)
	{
		CComBSTR bstrColumnName;
		pNm->pColumnRects->GetRectStringProp(pNm->dwCurrentColumn, VAR_COLUMN_NAME, &bstrColumnName);
		RETURN_IF_FAILED(Fire_OnColumnClick(bstrColumnName, pNm->dwCurrentColumn, pNm->pColumnRects, pNm->pVariantObject));
	}

	return 0;
}

LRESULT CTimelineControl::OnColumnRClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMCOLUMNCLICK* pNm = reinterpret_cast<NMCOLUMNCLICK*>(pnmh);

	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pCommandSupport;
	if (pInitializeWithVariantObject)
	{
		RETURN_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(pNm->pVariantObject));
	}

	if (pNm->dwCurrentColumn != CCustomListBox::INVALID_COLUMN_INDEX)
	{
		CComQIPtr<IInitializeWithColumnName> pInitializeWithColumnName = m_pCommandSupport;
		if (pInitializeWithColumnName)
		{
			CComBSTR bstrColumnName;
			pNm->pColumnRects->GetRectStringProp(pNm->dwCurrentColumn, VAR_COLUMN_NAME, &bstrColumnName);
			RETURN_IF_FAILED(pInitializeWithColumnName->SetColumnName(bstrColumnName));
		}
	}

	CComQIPtr<IIdleHandler> pIdleHandler = m_pCommandSupport;
	ATLASSERT(pIdleHandler);
	if (pIdleHandler)
	{
		BOOL bResult = FALSE;
		RETURN_IF_FAILED(pIdleHandler->OnIdle(&bResult));
	}

	m_popupMenu.TrackPopupMenu(0, pNm->x, pNm->y, m_hWnd);
	return 0;
}

LRESULT CTimelineControl::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bResult = FALSE;
	LRESULT lResult = 0;
	RETURN_IF_FAILED(m_pCommandSupport->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult));
	bHandled = bResult;
	return lResult;
}

LRESULT CTimelineControl::OnItemRemove(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMITEMREMOVED* pNm = reinterpret_cast<NMITEMREMOVED*>(pnmh);
	RETURN_IF_FAILED(Fire_OnItemRemoved(pNm->pVariantObject));
	return 0;
}

STDMETHODIMP CTimelineControl::GetTopVisibleItemIndex(UINT* puiIndex)
{
	CHECK_E_POINTER(puiIndex);
	*puiIndex = m_listBox.GetTopIndex();
	return S_OK;
}

STDMETHODIMP CTimelineControl::GetItemsCount(UINT* puiCount)
{
	CHECK_E_POINTER(puiCount);
	*puiCount = m_listBox.GetCount();
	return S_OK;
}

STDMETHODIMP CTimelineControl::RemoveItemByIndex(UINT uiIndex)
{
	m_listBox.RemoveItemByIndex(uiIndex);
	return S_OK;
}

HRESULT CTimelineControl::Fire_OnItemRemoved(IVariantObject *pItemObject)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CTimelineControl* pThis = static_cast<CTimelineControl*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITimelineControlEventSink * pConnection = static_cast<ITimelineControlEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnItemRemoved(pItemObject);
		}
	}
	return hr;
}

HRESULT CTimelineControl::Fire_OnColumnClick(BSTR bstrColumnName, DWORD dwColumnIndex, IColumnRects* pColumnRects, IVariantObject* pVariantObject)
{
	CComPtr<IUnknown> pUnk;
	RETURN_IF_FAILED(this->QueryInterface(__uuidof(IUnknown), (LPVOID*)&pUnk));
	HRESULT hr = S_OK;
	CTimelineControl* pThis = static_cast<CTimelineControl*>(this);
	int cConnections = m_vec.GetSize();

	for (int iConnection = 0; iConnection < cConnections; iConnection++)
	{
		pThis->Lock();
		CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
		pThis->Unlock();

		ITimelineControlEventSink * pConnection = static_cast<ITimelineControlEventSink*>(punkConnection.p);

		if (pConnection)
		{
			hr = pConnection->OnColumnClick(bstrColumnName, dwColumnIndex, pColumnRects, pVariantObject);
		}
	}
	return hr;
}

STDMETHODIMP CTimelineControl::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	CComPtr<ISettings> pTimelineSettings;
	RETURN_IF_FAILED(m_pSettings->OpenSubSettings(VAR_SETTINGS_TIMELINE_PATH, &pTimelineSettings));
	CComVariant vDisableAnimation;
	RETURN_IF_FAILED(pTimelineSettings->GetVariantValue(VAR_SETTINGS_TIMELINE_DISABLE_ANIMATION, &vDisableAnimation));
	m_listBox.EnableAnimation(vDisableAnimation.vt != VT_I4 || (!vDisableAnimation.intVal));
	return S_OK;
}
