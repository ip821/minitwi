// PictureWindowCopyCommand.h : Declaration of the CPictureWindowCopyCommand

#pragma once
#include "resource.h"       // main symbols
#include "twview_i.h"

using namespace ATL;
using namespace IP;

// CPictureWindowCopyCommand

class ATL_NO_VTABLE CPictureWindowCopyCommand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPictureWindowCopyCommand, &CLSID_PictureWindowCopyCommand>,
	public ICommand,
	public IAcceleratorSupport,
	public IInitializeWithVariantObject
{
public:
	CPictureWindowCopyCommand()
	{
	}

	DECLARE_NO_REGISTRY()

	BEGIN_COM_MAP(CPictureWindowCopyCommand)
		COM_INTERFACE_ENTRY(ICommand)
		COM_INTERFACE_ENTRY(IAcceleratorSupport)
		COM_INTERFACE_ENTRY(IInitializeWithVariantObject)
	END_COM_MAP()

private:
	CComPtr<IVariantObject> m_pVariantObject;
public:

	STDMETHOD(GetCommandText)(REFGUID guidCommand, BSTR* bstrText);
	STDMETHOD(InstallMenu)(IMenu* pMenu);
	STDMETHOD(Invoke)(REFGUID guidCommand);
	STDMETHOD(SetVariantObject)(IVariantObject* pVariantObject);
	STDMETHOD(GetAccelerator)(REFGUID guidCommand, TACCEL *pAccel);
};

OBJECT_ENTRY_AUTO(__uuidof(PictureWindowCopyCommand), CPictureWindowCopyCommand)
