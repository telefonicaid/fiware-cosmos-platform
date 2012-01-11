#ifdef _WIN32
#pragma once

#include <windows.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

#define HD_LogParameterSeparator	' '
enum ELogLevel
{
	LL_DEFAULT	=	0,
	LL_Default	=	0,
	LL_TRACE	=	0,
	LL_Trace	=	0,
	LL_DEBUG	=	1,
	LL_Debug	=	1,
	LL_INFO		=	2,
	LL_Info		=	2,
	LL_MESSAGE	=	3,
	LL_Message	=	3,
	LL_WARN		=	4,
	LL_Warn		=	4,
	LL_ERROR	=	5,
	LL_Error	=	5,
	LL_CRITICAL	=	6,
	LL_Critical	=	6,
	LL_FATAL	=	7,
	LL_Fatal	=	7,
	LL_LOG_NONE	=	8,
	LL_Log_None	=	8

};

class CLoggerStaticInfo//Singleton class can be used here. But not used because object creation check will be there every time we create CLogger object.
{
	friend class CLogger;
	HANDLE				m_hMutex; 
	CRITICAL_SECTION	m_objCriticalSection;
	FILE				*m_fpLogFile;
	string				m_strLogFileNameWithoutPath;
	string				m_strLogPathWithoutFileName;
	ELogLevel			m_objELogLevel;
	long				m_lMaxLogFileSize;
	bool				m_bMergeInMainLogFile;

private:
	CLoggerStaticInfo(string strLogFileNameWithoutPath="csCFD", string strLogPathWithoutFileName="D:\\cs_logs\\");
	~CLoggerStaticInfo(void);
	void OpenLogFile(void);
	void CloseLogFile(void);
	void MergeThisLogInMainLogFile(void);
	void CheckAndBackupLogFile(void);

public:
	void SetMaxLogFileSize(long lMaxLogFileSize);
	void SetLogLevel(unsigned int uiLogLevel);
	void SetLogFilePath(string strLogFileNameWithoutPath, string strLogPathWithoutFileName);
};

class CLogger
{
public:
	static CLoggerStaticInfo	m_objCLoggerStaticInfo;

private:
	DWORD						m_dwThreadId;
	string						m_strFunctionName;

private:
	void Log(ELogLevel objELogLevel, const char *pszLogLevel, const char *pszMessage);

public:
	CLogger(const char *pszFunctionName);
	CLogger(const char *pszFunctionName, long lLineNumber);
	~CLogger(void);

#define CLoggerDefineFunction(FunctionName)																																			\
	template<class T1> void Log##FunctionName(T1 varT1)																																\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1;																																						\
		Log(LL_##FunctionName, #FunctionName, ossMessage.str().c_str());																											\
	}																																												\
	template<class T1, class T2> void Log##FunctionName(T1 varT1, T2 varT2)																											\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1 << HD_LogParameterSeparator << varT2;																													\
		Log(LL_##FunctionName, #FunctionName, ossMessage.str().c_str());																											\
	}																																												\
	template<class T1, class T2, class T3> void Log##FunctionName(T1 varT1, T2 varT2, T3 varT3)																						\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1 << HD_LogParameterSeparator << varT2 << HD_LogParameterSeparator << varT3;																				\
		Log(LL_##FunctionName, #FunctionName, ossMessage.str().c_str());																											\
	}																																												\
	template<class T1, class T2, class T3, class T4> void Log##FunctionName(T1 varT1, T2 varT2, T3 varT3, T4 varT4)																	\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1 << HD_LogParameterSeparator << varT2 << HD_LogParameterSeparator << varT3 << HD_LogParameterSeparator << varT4;											\
		Log(LL_##FunctionName, #FunctionName, ossMessage.str().c_str());																											\
	}																																												\
	template<class T1, class T2, class T3, class T4, class T5> void Log##FunctionName(T1 varT1, T2 varT2, T3 varT3, T4 varT4, T5 varT5)												\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1 << HD_LogParameterSeparator << varT2 << HD_LogParameterSeparator << varT3 << HD_LogParameterSeparator << varT4 << HD_LogParameterSeparator << varT5;	\
		Log(LL_##FunctionName, #FunctionName, ossMessage.str().c_str());																											\
	}
	CLoggerDefineFunction(TRACE)
	CLoggerDefineFunction(DEBUG)
	CLoggerDefineFunction(INFO)
	CLoggerDefineFunction(MESSAGE)
	CLoggerDefineFunction(WARN)
	CLoggerDefineFunction(ERROR)
	CLoggerDefineFunction(CRITICAL)
	CLoggerDefineFunction(FATAL)
#undef CLoggerDefineFunction

#define CLoggerDefineFunctionUpr(FunctionName)																								\
	template<class T1> void Log##FunctionName(T1 varT1)																																\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1;																																						\
		char szFunctionName[]	=	#FunctionName;																																	\
		Log(LL_##FunctionName, strupr(szFunctionName), ossMessage.str().c_str());																									\
	}																																												\
	template<class T1, class T2> void Log##FunctionName(T1 varT1, T2 varT2)																											\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1 << HD_LogParameterSeparator << varT2;																													\
		char szFunctionName[]	=	#FunctionName;																																	\
		Log(LL_##FunctionName, strupr(szFunctionName), ossMessage.str().c_str());																									\
	}																																												\
	template<class T1, class T2, class T3> void Log##FunctionName(T1 varT1, T2 varT2, T3 varT3)																						\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1 << HD_LogParameterSeparator << varT2 << HD_LogParameterSeparator << varT3;																				\
		char szFunctionName[]	=	#FunctionName;																																	\
		Log(LL_##FunctionName, strupr(szFunctionName), ossMessage.str().c_str());																									\
	}																																												\
	template<class T1, class T2, class T3, class T4> void Log##FunctionName(T1 varT1, T2 varT2, T3 varT3, T4 varT4)																	\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1 << HD_LogParameterSeparator << varT2 << HD_LogParameterSeparator << varT3 << HD_LogParameterSeparator << varT4;											\
		char szFunctionName[]	=	#FunctionName;																																	\
		Log(LL_##FunctionName, strupr(szFunctionName), ossMessage.str().c_str());																									\
	}																																												\
	template<class T1, class T2, class T3, class T4, class T5> void Log##FunctionName(T1 varT1, T2 varT2, T3 varT3, T4 varT4, T5 varT5)												\
	{																																												\
		ostringstream ossMessage;																																					\
		ossMessage << varT1 << HD_LogParameterSeparator << varT2 << HD_LogParameterSeparator << varT3 << HD_LogParameterSeparator << varT4 << HD_LogParameterSeparator << varT5;	\
		char szFunctionName[]	=	#FunctionName;																																	\
		Log(LL_##FunctionName, strupr(szFunctionName), ossMessage.str().c_str());																									\
	}
	CLoggerDefineFunctionUpr(Trace)
	CLoggerDefineFunctionUpr(Debug)
	CLoggerDefineFunctionUpr(Info)
	CLoggerDefineFunctionUpr(Message)
	CLoggerDefineFunctionUpr(Warn)
	CLoggerDefineFunctionUpr(Error)
	CLoggerDefineFunctionUpr(Critical)
	CLoggerDefineFunctionUpr(Fatal)
#undef CLoggerDefineFunctionUpr
};


namespace ApplicationInfo
{
	extern string			g_strApplicationCommandArgumentZero;
	extern string			g_strApplicationNameWithFullPath;
	extern string			g_strApplicationNameWithoutExtension;
	extern string			g_strApplicationNameWithExtension;
	extern string			g_strApplicationPath;
	extern string			g_strApplicationVersion;
	extern string			g_strCurrentWorkingDirectory;
	extern string			g_strComputerName;
	extern string			g_strUserName;
	extern string			g_strUserDomain;
	extern string			g_strUserDnsDomain;
	extern vector<string>	g_vecPathList;
	extern unsigned int		g_uiNumberOfProcessors;
}
#else
#include "Logger_Linux.h"
#include "LoggerException_Linux.h"
#endif

