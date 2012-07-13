#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "CFDMain.h"

using namespace std;

#ifdef __cplusplus
extern "C"
{
	CCFDMain objCCFDMain;
	
	bool InitFromTermsInDomainFlatFile(int iMode, const char* szTermsInDomainFlatFileName)
	{
		try
		{
			BSTR bstrTermsInDomainFlatFileName = szTermsInDomainFlatFileName;
			return objCCFDMain.InitFromTermsInDomainFlatFile(iMode, bstrTermsInDomainFlatFileName);
		}
		catch(...)
		{
			return false;
		}
	}
	
	bool LoadCSDictionary (int iMode, const char* szTermsInDomainFlatFileName, const char* szDictionaryName)
	{
		try
		{
			BSTR bstrTermsInDomainFlatFileName = szTermsInDomainFlatFileName;
			BSTR bstrDictionaryName = szDictionaryName;
			
			bool bRet = objCCFDMain.InitFromTermsInDomainFlatFile(iMode, bstrTermsInDomainFlatFileName);
			if(bRet)
				return objCCFDMain.LoadDictionary(bstrDictionaryName);
			else
				return false;
		}
		catch(...)
		{
			return false;
		}
	}
	
	bool ApplyDictionaryUsingUrl (const char* szURL, long* lPatternID)
	{
		bool bRet;
		try
		{
			BSTR bstrURL = szURL;

			bRet =  objCCFDMain.ApplyDictionaryUsingUrl(bstrURL, lPatternID);
		}
		catch(...)
		{
			return false;
		}
		return bRet; 
	}
	
	bool ApplyDictionaryUsingUrl2 (BSTR bstrURL, BSTR bstrDomain, LONG* lPatternID)
	{
		bool bRet;
		try
		{
			bRet =  objCCFDMain.ApplyDictionaryUsingUrl2(bstrURL, bstrDomain, lPatternID);
		}
		catch(...)
		{
			return false;
		}
		return bRet;
	}
}

#endif
#endif
