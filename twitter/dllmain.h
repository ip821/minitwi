// dllmain.h : Declaration of module class.

class CtwitterModule : public ATL::CAtlDllModuleT< CtwitterModule >
{
public :
	DECLARE_LIBID(LIBID_twitterLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TWITTER, "{7840E48F-EF22-45A9-BD5B-8FFEDD2B4505}")
};

extern class CtwitterModule _AtlModule;
