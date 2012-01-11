

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Fri May 20 11:23:40 2011
 */
/* Compiler settings for csCFD.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __csCFD_i_h__
#define __csCFD_i_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ICFDMain_FWD_DEFINED__
#define __ICFDMain_FWD_DEFINED__
typedef interface ICFDMain ICFDMain;
#endif 	/* __ICFDMain_FWD_DEFINED__ */


#ifndef __CFDMain_FWD_DEFINED__
#define __CFDMain_FWD_DEFINED__

#ifdef __cplusplus
typedef class CFDMain CFDMain;
#else
typedef struct CFDMain CFDMain;
#endif /* __cplusplus */

#endif 	/* __CFDMain_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ICFDMain_INTERFACE_DEFINED__
#define __ICFDMain_INTERFACE_DEFINED__

/* interface ICFDMain */
/* [unique][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICFDMain;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E45F72C2-751A-45FA-85EF-50F881ECB81D")
    ICFDMain : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ INT lMode) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Cachepath( 
            /* [in] */ BSTR bstrCachePath) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FilterHostList( 
            /* [in] */ BSTR bstrHosts) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FilterHostFile( 
            /* [in] */ BSTR bstrHostFileName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetAppsMask( 
            /* [in] */ INT iAppMask) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DisableNEDOM( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ParseDomainFromHost( 
            /* [in] */ BSTR bstrHost,
            /* [retval][out] */ BSTR *bstrDomain) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ParseURL( 
            /* [in] */ BSTR bstrURL,
            /* [out] */ BSTR *bstrDomain,
            /* [out] */ BSTR *bstrHost,
            /* [out] */ BSTR *bstrDir,
            /* [out] */ BSTR *bstrPage,
            /* [out] */ BSTR *bstrQueryString) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetStats( 
            /* [out] */ LONG *plRecordsLoaded,
            /* [out] */ LONG *plCacheDate,
            /* [out] */ LONG *plNEDOMRecords) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE LoadDictionary( 
            /* [in] */ BSTR bstrDictionaryName) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyDictionary( 
            /* [in] */ BSTR bstrHost,
            /* [in] */ BSTR bstrQS,
            /* [retval][out] */ LONG *lPatternID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyDictionaryUsingUrl( 
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ LONG *lPatternID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyDictionary2( 
            /* [in] */ BSTR bstrHost,
            /* [in] */ BSTR bstrQS,
            /* [in] */ BSTR bstrDomainName,
            /* [retval][out] */ LONG *lPatternID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyDictionaryUsingUrl2( 
            /* [in] */ BSTR bstrURL,
            /* [in] */ BSTR bstrDomainName,
            /* [retval][out] */ LONG *lPatternID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CalculateDomainID( 
            /* [in] */ BSTR bstrInputString,
            /* [out] */ BSTR *bstrDomainID,
            /* [retval][out] */ LONGLONG *pllDomainID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE CalcMD5( 
            /* [in] */ BSTR bstrInputString,
            /* [retval][out] */ BSTR *bstrOutputHash) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetDictionaryList( 
            /* [retval][out] */ BSTR *bstrDatabaseDictionaryTitles) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyDictionaryAndComputeWeight( 
            BSTR bstrHost,
            BSTR bstrQS,
            /* [out] */ INT *iWeight,
            /* [retval][out] */ LONG *lPatternID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyDictionaryUsingUrlAndComputeWeight( 
            BSTR bstrUrl,
            /* [out] */ INT *iWeight,
            /* [retval][out] */ LONG *lPatternID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyDictionaryAndComputeWeight2( 
            BSTR bstrHost,
            BSTR bstrQS,
            BSTR bstrDomainName,
            /* [out] */ INT *iWeight,
            /* [retval][out] */ LONG *lPatternID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ApplyDictionaryUsingUrlAndComputeWeight2( 
            BSTR bstrUrl,
            BSTR bstrDomainName,
            /* [out] */ INT *iWeight,
            /* [retval][out] */ LONG *lPatternID) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE DisableNonEssentialDomainExclusion( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE InitFromTermsInDomainFlatFile( 
            /* [in] */ INT iMode,
            /* [in] */ BSTR bstrTermsInDomainFlatFileName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICFDMainVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICFDMain * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICFDMain * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICFDMain * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICFDMain * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICFDMain * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICFDMain * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICFDMain * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Init )( 
            ICFDMain * This,
            /* [in] */ INT lMode);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Cachepath )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrCachePath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FilterHostList )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrHosts);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FilterHostFile )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrHostFileName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetAppsMask )( 
            ICFDMain * This,
            /* [in] */ INT iAppMask);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DisableNEDOM )( 
            ICFDMain * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ParseDomainFromHost )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrHost,
            /* [retval][out] */ BSTR *bstrDomain);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ParseURL )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrURL,
            /* [out] */ BSTR *bstrDomain,
            /* [out] */ BSTR *bstrHost,
            /* [out] */ BSTR *bstrDir,
            /* [out] */ BSTR *bstrPage,
            /* [out] */ BSTR *bstrQueryString);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetStats )( 
            ICFDMain * This,
            /* [out] */ LONG *plRecordsLoaded,
            /* [out] */ LONG *plCacheDate,
            /* [out] */ LONG *plNEDOMRecords);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *LoadDictionary )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrDictionaryName);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyDictionary )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrHost,
            /* [in] */ BSTR bstrQS,
            /* [retval][out] */ LONG *lPatternID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyDictionaryUsingUrl )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrURL,
            /* [retval][out] */ LONG *lPatternID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyDictionary2 )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrHost,
            /* [in] */ BSTR bstrQS,
            /* [in] */ BSTR bstrDomainName,
            /* [retval][out] */ LONG *lPatternID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyDictionaryUsingUrl2 )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrURL,
            /* [in] */ BSTR bstrDomainName,
            /* [retval][out] */ LONG *lPatternID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CalculateDomainID )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrInputString,
            /* [out] */ BSTR *bstrDomainID,
            /* [retval][out] */ LONGLONG *pllDomainID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *CalcMD5 )( 
            ICFDMain * This,
            /* [in] */ BSTR bstrInputString,
            /* [retval][out] */ BSTR *bstrOutputHash);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetDictionaryList )( 
            ICFDMain * This,
            /* [retval][out] */ BSTR *bstrDatabaseDictionaryTitles);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyDictionaryAndComputeWeight )( 
            ICFDMain * This,
            BSTR bstrHost,
            BSTR bstrQS,
            /* [out] */ INT *iWeight,
            /* [retval][out] */ LONG *lPatternID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyDictionaryUsingUrlAndComputeWeight )( 
            ICFDMain * This,
            BSTR bstrUrl,
            /* [out] */ INT *iWeight,
            /* [retval][out] */ LONG *lPatternID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyDictionaryAndComputeWeight2 )( 
            ICFDMain * This,
            BSTR bstrHost,
            BSTR bstrQS,
            BSTR bstrDomainName,
            /* [out] */ INT *iWeight,
            /* [retval][out] */ LONG *lPatternID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ApplyDictionaryUsingUrlAndComputeWeight2 )( 
            ICFDMain * This,
            BSTR bstrUrl,
            BSTR bstrDomainName,
            /* [out] */ INT *iWeight,
            /* [retval][out] */ LONG *lPatternID);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *DisableNonEssentialDomainExclusion )( 
            ICFDMain * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *InitFromTermsInDomainFlatFile )( 
            ICFDMain * This,
            /* [in] */ INT iMode,
            /* [in] */ BSTR bstrTermsInDomainFlatFileName);
        
        END_INTERFACE
    } ICFDMainVtbl;

    interface ICFDMain
    {
        CONST_VTBL struct ICFDMainVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICFDMain_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICFDMain_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICFDMain_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICFDMain_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICFDMain_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICFDMain_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICFDMain_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICFDMain_Init(This,lMode)	\
    ( (This)->lpVtbl -> Init(This,lMode) ) 

#define ICFDMain_Cachepath(This,bstrCachePath)	\
    ( (This)->lpVtbl -> Cachepath(This,bstrCachePath) ) 

#define ICFDMain_FilterHostList(This,bstrHosts)	\
    ( (This)->lpVtbl -> FilterHostList(This,bstrHosts) ) 

#define ICFDMain_FilterHostFile(This,bstrHostFileName)	\
    ( (This)->lpVtbl -> FilterHostFile(This,bstrHostFileName) ) 

#define ICFDMain_SetAppsMask(This,iAppMask)	\
    ( (This)->lpVtbl -> SetAppsMask(This,iAppMask) ) 

#define ICFDMain_DisableNEDOM(This)	\
    ( (This)->lpVtbl -> DisableNEDOM(This) ) 

#define ICFDMain_ParseDomainFromHost(This,bstrHost,bstrDomain)	\
    ( (This)->lpVtbl -> ParseDomainFromHost(This,bstrHost,bstrDomain) ) 

#define ICFDMain_ParseURL(This,bstrURL,bstrDomain,bstrHost,bstrDir,bstrPage,bstrQueryString)	\
    ( (This)->lpVtbl -> ParseURL(This,bstrURL,bstrDomain,bstrHost,bstrDir,bstrPage,bstrQueryString) ) 

#define ICFDMain_GetStats(This,plRecordsLoaded,plCacheDate,plNEDOMRecords)	\
    ( (This)->lpVtbl -> GetStats(This,plRecordsLoaded,plCacheDate,plNEDOMRecords) ) 

#define ICFDMain_LoadDictionary(This,bstrDictionaryName)	\
    ( (This)->lpVtbl -> LoadDictionary(This,bstrDictionaryName) ) 

#define ICFDMain_ApplyDictionary(This,bstrHost,bstrQS,lPatternID)	\
    ( (This)->lpVtbl -> ApplyDictionary(This,bstrHost,bstrQS,lPatternID) ) 

#define ICFDMain_ApplyDictionaryUsingUrl(This,bstrURL,lPatternID)	\
    ( (This)->lpVtbl -> ApplyDictionaryUsingUrl(This,bstrURL,lPatternID) ) 

#define ICFDMain_ApplyDictionary2(This,bstrHost,bstrQS,bstrDomainName,lPatternID)	\
    ( (This)->lpVtbl -> ApplyDictionary2(This,bstrHost,bstrQS,bstrDomainName,lPatternID) ) 

#define ICFDMain_ApplyDictionaryUsingUrl2(This,bstrURL,bstrDomainName,lPatternID)	\
    ( (This)->lpVtbl -> ApplyDictionaryUsingUrl2(This,bstrURL,bstrDomainName,lPatternID) ) 

#define ICFDMain_CalculateDomainID(This,bstrInputString,bstrDomainID,pllDomainID)	\
    ( (This)->lpVtbl -> CalculateDomainID(This,bstrInputString,bstrDomainID,pllDomainID) ) 

#define ICFDMain_CalcMD5(This,bstrInputString,bstrOutputHash)	\
    ( (This)->lpVtbl -> CalcMD5(This,bstrInputString,bstrOutputHash) ) 

#define ICFDMain_GetDictionaryList(This,bstrDatabaseDictionaryTitles)	\
    ( (This)->lpVtbl -> GetDictionaryList(This,bstrDatabaseDictionaryTitles) ) 

#define ICFDMain_ApplyDictionaryAndComputeWeight(This,bstrHost,bstrQS,iWeight,lPatternID)	\
    ( (This)->lpVtbl -> ApplyDictionaryAndComputeWeight(This,bstrHost,bstrQS,iWeight,lPatternID) ) 

#define ICFDMain_ApplyDictionaryUsingUrlAndComputeWeight(This,bstrUrl,iWeight,lPatternID)	\
    ( (This)->lpVtbl -> ApplyDictionaryUsingUrlAndComputeWeight(This,bstrUrl,iWeight,lPatternID) ) 

#define ICFDMain_ApplyDictionaryAndComputeWeight2(This,bstrHost,bstrQS,bstrDomainName,iWeight,lPatternID)	\
    ( (This)->lpVtbl -> ApplyDictionaryAndComputeWeight2(This,bstrHost,bstrQS,bstrDomainName,iWeight,lPatternID) ) 

#define ICFDMain_ApplyDictionaryUsingUrlAndComputeWeight2(This,bstrUrl,bstrDomainName,iWeight,lPatternID)	\
    ( (This)->lpVtbl -> ApplyDictionaryUsingUrlAndComputeWeight2(This,bstrUrl,bstrDomainName,iWeight,lPatternID) ) 

#define ICFDMain_DisableNonEssentialDomainExclusion(This)	\
    ( (This)->lpVtbl -> DisableNonEssentialDomainExclusion(This) ) 

#define ICFDMain_InitFromTermsInDomainFlatFile(This,iMode,bstrTermsInDomainFlatFileName)	\
    ( (This)->lpVtbl -> InitFromTermsInDomainFlatFile(This,iMode,bstrTermsInDomainFlatFileName) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICFDMain_INTERFACE_DEFINED__ */



#ifndef __csCFDLib_LIBRARY_DEFINED__
#define __csCFDLib_LIBRARY_DEFINED__

/* library csCFDLib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_csCFDLib;

EXTERN_C const CLSID CLSID_CFDMain;

#ifdef __cplusplus

class DECLSPEC_UUID("9E5821DB-777E-4D90-9319-23D09C9F27A3")
CFDMain;
#endif
#endif /* __csCFDLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


