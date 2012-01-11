// CFDMain.h : Declaration of the CCFDMain

#pragma once

#include <string>
#include <set>
using namespace std ;

#ifdef _WIN32

#include <Windows.h>
#include "csCFD_i.h"
#include "resource.h"       // main symbols
#include <comsvcs.h>

using namespace ATL;

#else
typedef string BSTR ;
typedef long LONG ;
typedef int INT ;
typedef long long LONGLONG ;

#define S_OK            true
#define E_UNEXPECTED    false
#define E_FAIL          false
#endif

#if (defined (_WIN32)) || (defined(CSCFD_BUILD))
#include "c_membuffer.h"
#include "TermsInDomain.h"
#include "CFDManager.h"
#include "Utils.h"
#include "CSCFDException.h"
#endif

struct SProfilingStatistics
{
	unsigned long long	m_ullCallCount ;
	unsigned long long	m_ullTime ;

	SProfilingStatistics ( )
	{
		m_ullCallCount = m_ullTime = 0 ;
	}
} ;

// CCFDMain

#ifdef _WIN32
class ATL_NO_VTABLE CCFDMain :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CCFDMain, &CLSID_CFDMain>,
	public IDispatchImpl<ICFDMain, &IID_ICFDMain, &LIBID_csCFDLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
#else
class C_MemBuffer ;
class CTermsInDomain ;
class CCFDManager ;
	class CCFDMain
#endif
{
public:
	CCFDMain() ;
	~CCFDMain() ;

#ifdef _WIN32
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CFDMAIN)

DECLARE_NOT_AGGREGATABLE(CCFDMain)

BEGIN_COM_MAP(CCFDMain)
	COM_INTERFACE_ENTRY(ICFDMain)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()
#endif



// ICFDMain
public:
#ifdef _WIN32
	STDMETHOD(Init)(INT lMode);
	STDMETHOD(Cachepath)(BSTR bstrCachePath);
	STDMETHOD(FilterHostList)(BSTR bstrHosts);
	STDMETHOD(FilterHostFile)(BSTR bstrHostFileName);
	STDMETHOD(SetAppsMask)(INT iAppMask);
	STDMETHOD(DisableNEDOM)(void);
	STDMETHOD(ParseDomainFromHost)(BSTR bstrHost, BSTR* bstrDomain);
	STDMETHOD(ParseURL)(BSTR bstrURL, BSTR* bstrDomain, BSTR* bstrHost, BSTR* bstrDir, BSTR* bstrPage, BSTR* bstrQueryString);
	STDMETHOD(GetStats)(LONG* plRecordsLoaded, LONG* plCacheDate, LONG* plNEDOMRecords);
	STDMETHOD(LoadDictionary)(BSTR bstrDictionaryName);
	STDMETHOD(ApplyDictionary)(BSTR bstrHost, BSTR bstrQS, LONG* lPatternID);
	STDMETHOD(ApplyDictionaryUsingUrl)(BSTR bstrURL, LONG* lPatternID);
	STDMETHOD(ApplyDictionary2)(BSTR bstrHost, BSTR bstrQS, BSTR bstrDomain, LONG* lPatternID);
	STDMETHOD(ApplyDictionaryUsingUrl2)(BSTR bstrURL, BSTR bstrDomain, LONG* lPatternID);
	STDMETHOD(CalculateDomainID)(BSTR bstrInputString, BSTR* bstrDomainID, LONGLONG* pllDomainID);
	STDMETHOD(CalcMD5)(BSTR bstrInputString, BSTR* bstrOutputHash);
	STDMETHOD(GetDictionaryList)(BSTR* bstrDatabaseDictionaryTitles);
#else
        bool Init (INT lMode);
        bool Cachepath (BSTR bstrCachePath);
        bool FilterHostList (BSTR bstrHosts);
        bool FilterHostFile (BSTR bstrHostFileName);
        bool SetAppsMask (INT iAppMask);
        bool DisableNEDOM (void);
        bool ParseDomainFromHost (BSTR bstrHost, BSTR* bstrDomain);
        bool ParseURL (BSTR bstrURL, BSTR* bstrDomain, BSTR* bstrHost, BSTR* bstrDir, BSTR* bstrPage, BSTR* bstrQueryString);
        bool GetStats (LONG* plRecordsLoaded, LONG* plCacheDate, LONG* plNEDOMRecords);
        bool LoadDictionary (BSTR bstrDictionaryName);
        bool ApplyDictionary (BSTR bstrHost, BSTR bstrQS, LONG* lPatternID);
        bool ApplyDictionaryUsingUrl (BSTR bstrURL, LONG* lPatternID);
        bool ApplyDictionary2 (BSTR bstrHost, BSTR bstrQS, BSTR bstrDomain, LONG* lPatternID);
        bool ApplyDictionaryUsingUrl2 (BSTR bstrURL, BSTR bstrDomain, LONG* lPatternID);
        bool CalculateDomainID (BSTR bstrInputString, BSTR* bstrDomainID, LONGLONG* pllDomainID);
        bool CalcMD5 (BSTR bstrInputString, BSTR* bstrOutputHash);
        bool GetDictionaryList (BSTR* bstrDatabaseDictionaryTitles);
#endif

private:
	C_MemBuffer*	m_pTermsInDomainMemBuffer;
	CTermsInDomain*	m_pTermsInDomain;
	CCFDManager*	m_pObjCFDManager ;
	string			m_strCachePath ;
	set < string >	m_setAdNetDomain ;
	int				m_iMode ;
	string			m_strDictionaryName ;

#ifdef _WIN32
	LARGE_INTEGER			m_liTimerFrequency ;
#endif
public:
#ifdef _WIN32
	STDMETHOD(ApplyDictionaryAndComputeWeight)(BSTR bstrHost, BSTR bstrQS, INT* iWeight, LONG* lPatternID);
	STDMETHOD(ApplyDictionaryUsingUrlAndComputeWeight)(BSTR bstrUrl, INT* iWeight, LONG* lPatternID);
	STDMETHOD(ApplyDictionaryAndComputeWeight2)(BSTR bstrHost, BSTR bstrQS, BSTR bstrDomainName, INT* iWeight, LONG* lPatternID);
	STDMETHOD(ApplyDictionaryUsingUrlAndComputeWeight2)(BSTR bstrUrl, BSTR bstrDomainName, INT* iWeight, LONG* lPatternID);
	STDMETHOD(EnableNonEssentialDomainExclusion)(void);
#else
	bool ApplyDictionaryAndComputeWeight (BSTR bstrHost, BSTR bstrQS, INT* iWeight, LONG* lPatternID);
	bool ApplyDictionaryUsingUrlAndComputeWeight (BSTR bstrUrl, INT* iWeight, LONG* lPatternID);
	bool ApplyDictionaryAndComputeWeight2(BSTR bstrHost, BSTR bstrQS, BSTR bstrDomainName, INT* iWeight, LONG* lPatternID);
	bool ApplyDictionaryUsingUrlAndComputeWeight2(BSTR bstrUrl, BSTR bstrDomainName, INT* iWeight, LONG* lPatternID);
	bool EnableNonEssentialDomainExclusion (void);
#endif

#ifdef _WIN32
	STDMETHOD(InitFromTermsInDomainFlatFile)(INT iMode, BSTR bstrTermsInDomainFlatFileName);
#else
	bool InitFromTermsInDomainFlatFile (INT iMode, BSTR bstrTermsInDomainFlatFileName);
#endif

#ifdef _WIN32
	STDMETHOD(LoadDictionary_AdNet)(BSTR bstrConfiguration, LONG* lRetVal);
	STDMETHOD(ApplyDictionary_AdNet)(BSTR bstrRequestURL, BSTR bstrReferURL, LONG* lPatternID);
	STDMETHOD(IsDictionaryChangedInDB)(BOOL* bRetVal);
#else
	bool LoadDictionary_AdNet (BSTR bstrConfiguration, LONG* lRetVal);
	bool ApplyDictionary_AdNet (BSTR bstrRequestURL, BSTR bstrReferURL, LONG* lPatternID);
#ifdef ENABLE_LINUX_DATABASE_LOAD
	bool IsDictionaryChangedInDB (bool* bRetVal);
#endif
#endif

#ifdef _WIN32
	STDMETHOD(LoadDomainList_String)(BSTR* bstrDomainList);
	STDMETHOD(LoadDomainList_Array)(SAFEARRAY** arrayDomain);
	STDMETHOD(LoadDomainList)(LONG** handle);
	STDMETHOD(GetNextDomain)(LONG* handle, BSTR* bstrDomain);
	STDMETHOD(UnloadDomainList)(LONG** handle);
#endif

#ifdef _WIN32
	STDMETHOD(GetMode)(INT* iMode);
	STDMETHOD(GetLoadDictionaryArg)(BSTR* bstrDictionaryArg);
#else
	bool GetMode (INT* iMode);
	bool GetLoadDictionaryArg (BSTR* bstrDictionaryArg);
#endif

	SProfilingStatistics m_objpsApplyDictionary ;
	SProfilingStatistics m_objpsApplyDictionaryUsingUrl ;
	SProfilingStatistics m_objpsApplyDictionary2 ;
	SProfilingStatistics m_objpsApplyDictionaryUsingUrl2 ;

	SProfilingStatistics m_objpsApplyDictionaryAndComputeWeight ;
	SProfilingStatistics m_objpsApplyDictionaryUsingUrlAndComputeWeight ;
	SProfilingStatistics m_objpsApplyDictionaryAndComputeWeight2 ;
	SProfilingStatistics m_objpsApplyDictionaryUsingUrlAndComputeWeight2 ;
	SProfilingStatistics m_objpsApplyDictionary_AdNet ;
};

#ifdef _WIN32
OBJECT_ENTRY_AUTO(__uuidof(CFDMain), CCFDMain)
#endif
