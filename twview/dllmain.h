// dllmain.h : Declaration of module class.

class CtwviewModule : public ATL::CAtlDllModuleT< CtwviewModule >
{
public :
	DECLARE_LIBID(LIBID_twviewLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TWVIEW, "{42CEB9E3-5FD7-42A6-B327-3D72218A24E0}")
};

extern class CtwviewModule _AtlModule;
