#pragma once

#include "stdafx.h"

#include "CFDTree.h"
#include <map>
#include <set>
#include "c_membuffer.h"

#include "CSCFDException.h"
#include "ErrorCodes.h"
#include <sstream>

#ifndef _WIN32
#ifdef ENABLE_LINUX_DATABASE_LOAD
#include <sql.h>
#include <sqlext.h>
#include <uuid/uuid.h>
#endif
#endif

using namespace std ;

class CTermsInDomain
{
private:
	CCFDTree *m_pRoot;

	template<class E, class TR, class A, class OutIt>
	int split_string(const std::basic_string<E, TR, A>& str, E split_char, OutIt out)
	{
		const int len = str.size();
		int start = 0;

		int count = 0;
		while (start <= len)
		{
			int end = start;

			while (end < len && str[end] != split_char)
			{
				end += 1;
			}

			*out = str.substr(start, end - start);
			++out;

			start = end + 1;
			count += 1;
		}

		return count;
	};


public:
	CTermsInDomain(C_MemBuffer *pMembuffer, const char* pszCachePath = NULL);
	~CTermsInDomain(void);
	bool LoadTermInDomainTable(string strTermsFile);
	string GetDomainFromHost(std::string &strHost);
	bool bLoadTable();
	void SetCachePath ( const char* pszCachePath ) ;
	
#ifdef _WIN32
	_ConnectionPtr m_dbConn_Input;
	bool OpenDBConn_Input(string conn_string );
	string CheckString( _RecordsetPtr pRecordset, string column_name );	
#endif
	C_MemBuffer *m_pMembuffer;

	string	m_strMMXIDataSource ;
	string	m_strCachePath ;
};

