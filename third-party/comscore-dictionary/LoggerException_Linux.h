#ifndef _WIN32
#pragma once
#include <exception>
#include <string>
using namespace std ;
class CLoggerException : public exception
{
	public:
		CLoggerException ( const char* pszException ) : exception ( )
		{ m_strMessage = pszException ; }
		CLoggerException ( const CLoggerException& obj )
		{ m_strMessage = obj.m_strMessage ; }
		virtual ~CLoggerException ( ) throw ( )
		{ }
		virtual const char* what ( )
		{ return m_strMessage.c_str ( ) ; }
	private:
		string	m_strMessage ;
} ;
#endif

