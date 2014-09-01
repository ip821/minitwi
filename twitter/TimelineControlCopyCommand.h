// CopyCommand.h : Declaration of the CTimelineControlCopyCommand

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"
#include "..\ViewMdl\IInitializeWithControlImpl.h"

using namespace ATL;

// CTimelineControlCopyCommand

class ATL_NO_VTABLE CTimelineControlCopyCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineControlCopyCommand, &CLSID_TimelineControlCopyCommand>,
	public ICommand,
	public IInitializeWithControlImpl,
	public IInitializeWithVariantObject,
	public IInitializeWithColumnName
{
public:
	CTimelineControlCopyCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CTimelineControlCopyCommand)
		COM_INTERFACE_ENTRY(IInitializeWithControl)
		COM_INTERFACE_ENTRY(ICommand)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IInitializeWithColumnName)
	END_COM_MAP()

private:
	CComPtr<IVariantObject> m_pVariantObject;
	CString m_strColumnName;
public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText);
	STDMETHOD(InstallMenu)(IMenu* pMenu);
	STDMETHOD(Invoke)(REFGUID guidCommand);
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
	STDMETHOD(SetColumnName)(LPCTSTR lpszColumnName);
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineControlCopyCommand), CTimelineControlCopyCommand)
