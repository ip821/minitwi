// PictureWindowSaveCommand.h : Declaration of the CPictureWindowSaveCommand

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"

using namespace ATL;
using namespace IP;

// CPictureWindowSaveCommand

class ATL_NO_VTABLE CPictureWindowSaveCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPictureWindowSaveCommand, &CLSID_PictureWindowSaveCommand>,
	public ICommand2,
	public IAcceleratorSupport,
	public IInitializeWithVariantObject,
	public IPluginSupportNotifications
{
public:
	CPictureWindowSaveCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CPictureWindowSaveCommand)
		COM_INTERFACE_ENTRY(ICommand)
		COM_INTERFACE_ENTRY(ICommand2)
		COM_INTERFACE_ENTRY(IAcceleratorSupport)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
		COM_INTERFACE_ENTRY(IPluginSupportNotifications)
	END_COM_MAP()

private:
	CComPtr<IVariantObject> m_pVariantObject;
	CComPtr<IServiceProvider> m_pServiceProvider;
public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText);
	STDMETHOD(InstallMenu)(IMenu* pMenu);
	STDMETHOD(Invoke)(REFGUID guidCommand);
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
	STDMETHOD(GetAccelerator)(REFGUID guidCommand, TACCEL *pAccel);
	STDMETHOD(GetEnabled)(REFGUID guidCommand, BOOL *pbEnabled);
	METHOD_EMPTY(STDMETHOD(GetHBITMAP)(REFGUID guidCommand, HBITMAP *pHBITMAP));
	METHOD_EMPTY(STDMETHOD(InstallToolbar)(IToolbar *pToolbar));
	STDMETHOD(OnInitialized)(IServiceProvider* pServiceProvider);
	STDMETHOD(OnShutdown)();
};

OBJECT_ENTRY_AUTO(__uuidof(PictureWindowSaveCommand), CPictureWindowSaveCommand)
