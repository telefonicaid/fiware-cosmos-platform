#ifndef CCSCFDEXCEPTION_H
#define CCSCFDEXCEPTION_H

#include <exception>
#include <string>
#include "stdafx.h"

using namespace std ;

class CCSCFDException : public exception
{
public:
	CCSCFDException ( ) throw ( ) 
	{
		m_strMessage = "" ;
		m_iErrorCode = -1 ;
	}
	CCSCFDException ( const string& strMessage , const int& iErrorCode ) throw ( ) 
	{
		m_strMessage = strMessage ;
		m_iErrorCode = iErrorCode ;
	}
	CCSCFDException ( const CCSCFDException& objOther ) throw ( ) 
	{
		m_strMessage = objOther.m_strMessage ;
		m_iErrorCode = objOther.m_iErrorCode ;
	}
	virtual ~CCSCFDException ( ) throw ( ) 
	{
	}

	const char* what ( ) throw ( )
	{ return m_strMessage.c_str ( ) ; }
private:
	string	m_strMessage ;
	int		m_iErrorCode ;
} ;

#endif

