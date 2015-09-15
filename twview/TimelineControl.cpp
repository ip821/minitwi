// TimelineControl.cpp : Implementation of CTimelineControl

#include "stdafx.h"
#include "TimelineControl.h"
#include "Plugins.h"
#include "..\twiconn\Plugins.h"

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
	RETURN_IF_FAILED(m_pPluginSupport->OnShutdown());
	RETURN_IF_FAILED(m_pCommandSupport->UninstallAll());
	m_pCommandSupport.Release();
	m_pPluginSupport.Release();
	m_pSettings.Release();
	m_listBox.SetSkinTimeline(NULL);
	return S_OK;
}

STDMETHODIMP CTimelineControl::ScrollToItem(UINT uiIndex)
{
	m_listBox.SetTopIndex(uiIndex);
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
	m_listBox.Create(m_hWnd);
	AdjustSizes();
	return S_OK;
}

STDMETHODIMP CTimelineControl::PreTranslateMessage(MSG *pMsg, BOOL *pbResult)
{
	CHECK_E_POINTER(pMsg);
	CHECK_E_POINTER(pbResult);
	RETURN_IF_FAILED(m_pCommandSupport->PreTranslateMessage(m_hWnd, pMsg, pbResult));
	m_listBox.PreTranslateMessage(m_hWnd, pMsg, pbResult);
	return S_OK;
}

STDMETHODIMP CTimelineControl::OnBeforeCommandInvoke(REFGUID guidCommand, int iCommandSource, ICommand* pCommand)
{
	if (iCommandSource != COMMAND_SOURCE_ACCELERATOR)
		return S_OK;

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

LRESULT CTimelineControl::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	AdjustSizes();
	return 0;
}

void CTimelineControl::AdjustSizes()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	::SetWindowPos(m_listBox.m_hWnd, 0, 0, 0, clientRect.Width(), clientRect.Height(), 0);
}

LRESULT CTimelineControl::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_listBox.SetFocus();
	return 0;
}

LRESULT CTimelineControl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	auto dwExStyle = GetWindowLong(GWL_EXSTYLE);
	dwExStyle |= WS_EX_COMPOSITED;
	SetWindowLong(GWL_EXSTYLE, dwExStyle);
	HrCoCreateInstance(CLSID_PluginSupport, &m_pPluginSupport);
	m_pPluginSupport->InitializePlugins(PNAMESP_TIMELINE_CONTROL, PVIEWTYPE_COMMAND);
	CComQIPtr<IInitializeWithControl> pInit = m_pPluginSupport;
	if (pInit && m_pControl)
	{
		pInit->SetControl(m_pControl);
	}

	m_pPluginSupport->OnInitialized();

	m_popupMenu.CreatePopupMenu();
	HrCoCreateInstance(CLSID_CommandSupport, &m_pCommandSupport);
	m_pCommandSupport->SetMenu(m_popupMenu);
	m_pCommandSupport->InstallCommands(m_pPluginSupport);
	bHandled = FALSE;
	AdjustSizes();
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

STDMETHODIMP CTimelineControl::InsertItem(IVariantObject* pVariantObject, UINT uiIndex)
{
	m_listBox.InsertItem(pVariantObject, uiIndex);
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

		m_listBox.InsertItem(pVariantObject, uiStartIndex == -1 ? -1 : uiStartIndex + i);
	}
	return S_OK;
}

STDMETHODIMP CTimelineControl::RefreshItems(IVariantObject** pItemArray, UINT uiCountArray)
{
	m_listBox.RefreshItems(pItemArray, uiCountArray);
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

LRESULT CTimelineControl::OnItemDoubleClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMCOLUMNCLICK* pNm = reinterpret_cast<NMCOLUMNCLICK*>(pnmh);
	ASSERT_IF_FAILED(Fire_OnItemDoubleClick(pNm->pVariantObject));
	return 0;
}

LRESULT CTimelineControl::OnColumnClick(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	NMCOLUMNCLICK* pNm = reinterpret_cast<NMCOLUMNCLICK*>(pnmh);
	if (!pNm->pColumnsInfoItem)
		return 0;

	CComPtr<IColumnsInfoItem> pColumnsInfoItem = pNm->pColumnsInfoItem;

	BOOL bIsUrl = FALSE;
	ASSERT_IF_FAILED(pColumnsInfoItem->GetRectBoolProp(Twitter::Metadata::Item::VAR_IS_URL, &bIsUrl));

	if (bIsUrl)
	{
		ASSERT_IF_FAILED(Fire_OnColumnClick(pColumnsInfoItem, pNm->pVariantObject));
	}

	return 0;
}

LRESULT CTimelineControl::OnColumnRClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	NMCOLUMNCLICK* pNm = reinterpret_cast<NMCOLUMNCLICK*>(pnmh);

	CComPtr<IVariantObject> pVariantObject;
	ASSERT_IF_FAILED(HrCoCreateInstance(CLSID_VariantObject, &pVariantObject));
	ASSERT_IF_FAILED(pNm->pVariantObject->CopyTo(pVariantObject));

	if (pNm->pColumnsInfoItem)
	{
		CComPtr<IColumnsInfoItem> pColumnsInfoItem = pNm->pColumnsInfoItem;
		ASSERT_IF_FAILED(pVariantObject->SetVariantValue(ObjectModel::Metadata::Table::Column::Object, &CComVar(pColumnsInfoItem)));
	}

	CComQIPtr<IInitializeWithVariantObject> pInitializeWithVariantObject = m_pCommandSupport;
	if (pInitializeWithVariantObject)
	{
		ASSERT_IF_FAILED(pInitializeWithVariantObject->SetVariantObject(pVariantObject));
	}

	CComQIPtr<IIdleHandler> pIdleHandler = m_pCommandSupport;
	ATLASSERT(pIdleHandler);
	if (pIdleHandler)
	{
		BOOL bResult = FALSE;
		ASSERT_IF_FAILED(pIdleHandler->OnIdle(&bResult));
	}

	m_listBox.SendMessage(WM_SETLISTBOX_SCROLL_MODE, SCROLL_OPTION_KEEP_SCROLLPOS);
	m_popupMenu.TrackPopupMenu(0, pNm->x, pNm->y, m_hWnd);
	m_listBox.SendMessage(WM_SETLISTBOX_SCROLL_MODE, SCROLL_OPTION_NONE);
	return 0;
}

LRESULT CTimelineControl::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pCommandSupport)
		return 0;

	auto hWndFocused = GetFocus();
	if (hWndFocused != m_hWnd && !IsChild(hWndFocused))
		return 0;
	BOOL bResult = FALSE;
	LRESULT lResult = 0;
	ASSERT_IF_FAILED(m_pCommandSupport->ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, &lResult, &bResult));
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

HRESULT CTimelineControl::Fire_OnColumnClick(IColumnsInfoItem* pColumnsInfoItem, IVariantObject* pVariantObject)
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
			hr = pConnection->OnColumnClick(pColumnsInfoItem, pVariantObject);
		}
	}
	return hr;
}

HRESULT CTimelineControl::Fire_OnItemDoubleClick(IVariantObject* pVariantObject)
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
			hr = pConnection->OnItemDoubleClick(pVariantObject);
		}
	}
	return hr;
}

STDMETHODIMP CTimelineControl::Load(ISettings *pSettings)
{
	CHECK_E_POINTER(pSettings);
	m_pSettings = pSettings;
	m_listBox.SetSettings(pSettings);
	return S_OK;
}
