// dllmain.h : Declaration of module class.

#ifdef _WIN32

class CcsCFDModule : public ATL::CAtlDllModuleT< CcsCFDModule >
{
public :
	DECLARE_LIBID(LIBID_csCFDLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_CSCFD, "{E1CA4B01-1214-4A2A-B9C2-7F8C0B806859}")
};

extern class CcsCFDModule _AtlModule;

#endif
