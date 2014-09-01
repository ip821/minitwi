// dllmain.h : Declaration of module class.

class CtwiconnModule : public ATL::CAtlDllModuleT< CtwiconnModule >
{
public :
	DECLARE_LIBID(LIBID_twiconnLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TWICONN, "{326520F7-6354-4CBB-BDE0-5A2A7D21CC5E}")
};

extern class CtwiconnModule _AtlModule;
