// dllmain.h : Declaration of module class.

class CtwthemeModule : public ATL::CAtlDllModuleT< CtwthemeModule >
{
public :
	DECLARE_LIBID(LIBID_twthemeLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TWTHEME, "{94DBF5B6-E8EA-4E78-A40F-260A57B03DA6}")
};

extern class CtwthemeModule _AtlModule;
