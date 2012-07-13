#include "stdafx.h"
#ifdef _WIN32
#include "Logger.h"
#include <direct.h>
#include <time.h>
#include <io.h>
#include <fstream>
using namespace std;

//-------------------------------------------------------------------------------------------------
//----------Start-----------namespace ApplicationInfo----------------------------------------------
namespace ApplicationInfo
{
	string			g_strApplicationCommandArgumentZero;
	string			g_strApplicationNameWithFullPath;
	string			g_strApplicationNameWithoutExtension;
	string			g_strApplicationNameWithExtension;
	string			g_strApplicationPath;
	string			g_strApplicationVersion;
	string			g_strCurrentWorkingDirectory;
	string			g_strComputerName;
	string			g_strUserName;
	string			g_strUserDomain;
	string			g_strUserDnsDomain;
	vector<string>	g_vecPathList;
	unsigned int	g_uiNumberOfProcessors	=	0;

	string GetVersion(const char* szFileName);
	void Initilize(void);
}

void ApplicationInfo::Initilize(void)
{
	try
	{
		// Walk through list of enviroment strings until a NULL is encountered.
		for(unsigned int uiCounter=0; _environ[uiCounter]!=NULL; ++uiCounter)
		{
			if(strnicmp("COMPUTERNAME=", _environ[uiCounter], strlen("COMPUTERNAME=")) == 0)
			{
				ApplicationInfo::g_strComputerName	=	(_environ[uiCounter]+strlen("COMPUTERNAME="));
			}
			else if(strnicmp("USERNAME=", _environ[uiCounter], strlen("USERNAME=")) == 0)
			{
				ApplicationInfo::g_strUserName	=	(_environ[uiCounter]+strlen("USERNAME="));
			}
			else if(strnicmp("USERDOMAIN=", _environ[uiCounter], strlen("USERDOMAIN=")) == 0)
			{
				ApplicationInfo::g_strUserDomain	=	(_environ[uiCounter]+strlen("USERDOMAIN="));
			}
			else if(strnicmp("USERDNSDOMAIN=", _environ[uiCounter], strlen("USERDNSDOMAIN=")) == 0)
			{
				ApplicationInfo::g_strUserDnsDomain	=	(_environ[uiCounter]+strlen("USERDNSDOMAIN="));
			}
			else if(strnicmp("NUMBER_OF_PROCESSORS=", _environ[uiCounter], strlen("NUMBER_OF_PROCESSORS=")) == 0)
			{
				ApplicationInfo::g_uiNumberOfProcessors	=	atoi(_environ[uiCounter]+strlen("NUMBER_OF_PROCESSORS="));
			}
			else if(strnicmp("PATH=", _environ[uiCounter], strlen("PATH=")) == 0)
			{
				istringstream issTemp(_environ[uiCounter]+strlen("PATH="));
				string strTemp;
				while(!issTemp.eof())
				{
					getline(issTemp, strTemp, ';');
					ApplicationInfo::g_vecPathList.push_back(strTemp);
				}
			}
			else
			{
				//Do Nothing. Just discard the data;
			}		
		}

		if(ApplicationInfo::g_strComputerName == "")
		{
			DWORD dwLen	=	1024;
			char	szBuffer[1024];
			if(0 == ::GetComputerName(szBuffer, &dwLen))
				strcpy(szBuffer, "Anonymous Computer");
			ApplicationInfo::g_strComputerName	=	szBuffer;
		}

		if(ApplicationInfo::g_strUserName == "")
		{
			DWORD dwLen	=	1024;
			char	szBuffer[1024];
			if(0 == ::GetUserName(szBuffer, &dwLen))
				strcpy(szBuffer, "Anonymous User");
			ApplicationInfo::g_strUserName	=	szBuffer;
		}

		ApplicationInfo::g_strApplicationCommandArgumentZero	=	__argv[0];
		if(ApplicationInfo::g_strApplicationCommandArgumentZero == "")
			throw exception("g_strApplicationCommandArgumentZero is empty(null).");

		ApplicationInfo::g_strApplicationNameWithFullPath	=	_pgmptr;
		if(ApplicationInfo::g_strApplicationNameWithFullPath == "")
			throw exception("g_strApplicationNameWithFullPath is empty(null).");

		char szApplicationNameWithFullPath[MAX_PATH+MAX_PATH];
		strcpy(szApplicationNameWithFullPath, ApplicationInfo::g_strApplicationNameWithFullPath.c_str());
		char *pcSlash;
		char *pcSlashB	=	strrchr(szApplicationNameWithFullPath, '\\');
		char *pcSlashF	=	strrchr(szApplicationNameWithFullPath, '/');
		pcSlash	=	(pcSlashB>pcSlashF ? pcSlashB : pcSlashF);
		if(pcSlash == NULL)
			throw exception("g_strApplicationPath is empty(null).");
		*pcSlash	=	'\0';
		ApplicationInfo::g_strApplicationPath	=	szApplicationNameWithFullPath;
		if(ApplicationInfo::g_strApplicationPath == "")
			throw exception("(Must not reach here due to above condition)g_strApplicationPath is empty(null).");

		++pcSlash;
		ApplicationInfo::g_strApplicationNameWithExtension	=	pcSlash;
		if(ApplicationInfo::g_strApplicationNameWithExtension == "")
			throw exception("g_strApplicationNameWithExtension is empty(null).");

		char *pcLastDot	=	strrchr(pcSlash, '.');
		if(NULL != pcLastDot)
			*pcLastDot	=	'\0';
		ApplicationInfo::g_strApplicationNameWithoutExtension	=	pcSlash;
		if(ApplicationInfo::g_strApplicationNameWithoutExtension == "")
			throw exception("g_strApplicationNameWithoutExtension is empty(null).");

		char *pszBuffer	=	_getcwd(NULL, 0);
		if(NULL == pszBuffer)
			throw exception("Error in getting current working directory.");
		ApplicationInfo::g_strCurrentWorkingDirectory	=	pszBuffer;
		free(pszBuffer);
		if(ApplicationInfo::g_strCurrentWorkingDirectory == "")
			throw exception("g_strCurrentWorkingDirectory is empty(null).");

		ApplicationInfo::g_strApplicationVersion	=	GetVersion(ApplicationInfo::g_strApplicationNameWithFullPath.c_str());
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

string ApplicationInfo::GetVersion(const char* szFileName)
{
	char szFileVersion[MAX_PATH]		=	"Not Available";
	bool bGotVersion					=	FALSE;
	DWORD dwFileHandle					=	0;
	DWORD dwTempLanguage;
	DWORD dwFileVersionInformationSize	=	0;
	char *szFileVersionInformationData;
	char szTempStr[MAX_PATH];
	LPVOID pVersion						=	NULL;
	UINT uVersionLen					=	0;

	dwFileVersionInformationSize	=	GetFileVersionInfoSize(szFileName, &dwFileHandle);
	szFileVersionInformationData	=	new char [dwFileVersionInformationSize+1];
	bGotVersion						=	GetFileVersionInfo(szFileName, 0, dwFileVersionInformationSize, szFileVersionInformationData);

	if(bGotVersion)
	{
		sprintf(szTempStr,"\\VarFileInfo\\Translation");
		bGotVersion	=	VerQueryValue(szFileVersionInformationData, szTempStr, &pVersion, &uVersionLen);
		if(bGotVersion && uVersionLen == 4) 
		{
			memcpy(&dwTempLanguage,pVersion,4);            
			sprintf(szTempStr,	"\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion",
				(dwTempLanguage & 0xff00)>>8,
				(dwTempLanguage & 0xff),
				(dwTempLanguage & 0xff000000)>>24,
				(dwTempLanguage & 0xff0000)>>16);            
		}
		else 
		{
			sprintf(szTempStr, "\\StringFileInfo\\%04X04B0\\FileVersion", GetUserDefaultLangID());
		}
		bGotVersion	=	VerQueryValue(szFileVersionInformationData, szTempStr, &pVersion, &uVersionLen);
		if(bGotVersion)
		{
			char szTempFileVersion[MAX_PATH]	=	"";
			int iCount, jCount;
			strcpy(szTempFileVersion, (char*)pVersion);
			for (iCount=0,jCount=0; szTempFileVersion[iCount]!='\0'; ++iCount)
			{
				if (szTempFileVersion[iCount] == ' ')
					continue;
				else if ( szTempFileVersion[iCount] == ',' )
				{
					szFileVersion[jCount]	=	'.';
					++jCount;
				}
				else
				{
					szFileVersion[jCount]	=	szTempFileVersion[iCount];
					++jCount;
				}
			}
			szFileVersion[jCount]	=	'\0';
		}
	}
	if(szFileVersionInformationData != NULL)
		delete []szFileVersionInformationData;

	return szFileVersion;
}
//----------End-------------namespace ApplicationInfo----------------------------------------------
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//----------Start-----------General requirement for class CLoggerStaticInfo------------------------
enum ELogDirectory
{
	LD_NotDefined				=	0,
	LD_ApplicationDirectory		=	1,
	LD_CurrentWorkingDirectory	=	2,
	LD_UserDefinedDirectory		=	3
};

enum ELogFilename
{
	LF_NotDefined				=	0,
	LF_ApplicationName			=	1,
	LF_ProcessIdApplicationName	=	2,
	LF_UserDefinedFileName		=	3
};
//----------End-------------General requirement for class CLoggerStaticInfo------------------------
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//----------Start-----------class CLoggerStaticInfo------------------------------------------------
//ELogDirectory	g_objELogDirectory	=	LD_ApplicationDirectory;//Set these in log SetPath etc function.
ELogDirectory	g_objELogDirectory	=	LD_UserDefinedDirectory;//Set these in log SetPath etc function.
//ELogFilename	g_objELogFilename	=	LF_ProcessIdApplicationName;
ELogFilename	g_objELogFilename	=	LF_UserDefinedFileName;

CLoggerStaticInfo::CLoggerStaticInfo(string strLogFileNameWithoutPath, string strLogPathWithoutFileName)
{
	try
	{
		ApplicationInfo::Initilize();
		InitializeCriticalSection(&m_objCriticalSection);
		m_fpLogFile			=	NULL;
		m_hMutex			=	NULL;
		m_objELogLevel		=	LL_DEFAULT;
		m_lMaxLogFileSize	=	10*1024*1024;
		if(g_objELogDirectory==LD_ApplicationDirectory && g_objELogFilename==LF_ProcessIdApplicationName)
			m_bMergeInMainLogFile	=	true;
		else
			m_bMergeInMainLogFile	=	false;

		if(g_objELogDirectory == LD_ApplicationDirectory)
			m_strLogPathWithoutFileName	=	ApplicationInfo::g_strApplicationPath;
		else if(g_objELogDirectory == LD_CurrentWorkingDirectory)
			m_strLogPathWithoutFileName	=	ApplicationInfo::g_strCurrentWorkingDirectory;
		else if(g_objELogDirectory == LD_UserDefinedDirectory)
			m_strLogPathWithoutFileName	=	strLogPathWithoutFileName;
		else
			throw exception("Invalid value of g_objELogDirectory.");

		if(m_strLogPathWithoutFileName == "")
			throw exception("m_strLogPathWithoutFileName is empty.");
		if(m_strLogPathWithoutFileName[m_strLogPathWithoutFileName.size()-1]!='\\' && m_strLogPathWithoutFileName[m_strLogPathWithoutFileName.size()-1]!='/')
			m_strLogPathWithoutFileName	+=	"\\";

		if(g_objELogFilename == LF_ApplicationName)
			m_strLogFileNameWithoutPath	=	ApplicationInfo::g_strApplicationNameWithoutExtension;
		else if(g_objELogFilename == LF_ProcessIdApplicationName)
		{
			m_strLogFileNameWithoutPath	=	ApplicationInfo::g_strApplicationNameWithoutExtension;
			char szTemp[20];
			sprintf(szTemp, "%08X", GetCurrentProcessId());
			m_strLogFileNameWithoutPath	+=	szTemp;
		}
		else if(g_objELogFilename == LF_UserDefinedFileName)
		{
			//Nilesh - May 20, 2011 - Support for appending Process ID & SS2K to log file name to avoid conflicts BEGIN
			char szLogFileSuffix [ 64 ] = {0} ;
			time_t objTimeNow ;
			struct tm objTmJan012000 = { 0 , 0 , 0 , 1 , 0 , 100 } ;
			time_t objTimeJan012000 = mktime ( &objTmJan012000 ) - 86400 ;

			time ( &objTimeNow ) ;
			sprintf ( szLogFileSuffix , "_%08X_%08X" , getpid ( ) , objTimeNow - objTimeJan012000 ) ;
			m_strLogFileNameWithoutPath	=	strLogFileNameWithoutPath;
			m_strLogFileNameWithoutPath.append ( szLogFileSuffix ) ;
			//Nilesh - May 20, 2011 - Support for appending Process ID & SS2K to log file name to avoid conflicts BEGIN
		}
		else
			throw exception("Invalid value of g_objELogFilename.");

		if(m_strLogFileNameWithoutPath == "")
			throw exception("m_strLogFileNameWithoutPath is empty.");
		m_strLogFileNameWithoutPath	+=	".log";

		OpenLogFile();
		CheckAndBackupLogFile();

		char szDate [10];
		char szTime [10];
		_strdate(szDate);
		_strtime(szTime);
		EnterCriticalSection(&m_objCriticalSection);
		fprintf(m_fpLogFile, "\n**************** APPLICATION STARTING INFORMATION ***************************************************************************");
		fprintf(m_fpLogFile, "\nAPPLICATION STARTED AT    : %s-%s", szDate, szTime);
		fprintf(m_fpLogFile, "\nCURRENT WORKING DIRECTORY : %s", ApplicationInfo::g_strCurrentWorkingDirectory.c_str());
		fprintf(m_fpLogFile, "\nCOMPUTER NAME             : %s", ApplicationInfo::g_strComputerName.c_str());
		fprintf(m_fpLogFile, "\nUSER NAME                 : %s", ApplicationInfo::g_strUserName.c_str());
		fprintf(m_fpLogFile, "\nUSER DOMAIN               : %s", ApplicationInfo::g_strUserDomain.c_str());
		fprintf(m_fpLogFile, "\nUSER DNS DOMAIN           : %s", ApplicationInfo::g_strUserDnsDomain.c_str());
		fprintf(m_fpLogFile, "\nAPPLICATION DIRECTORY     : %s", ApplicationInfo::g_strApplicationPath.c_str());
		fprintf(m_fpLogFile, "\nAPPLICATION NAME          : %s", ApplicationInfo::g_strApplicationNameWithExtension.c_str());
		fprintf(m_fpLogFile, "\nAPPLICATION VERSION       : %s", ApplicationInfo::g_strApplicationVersion.c_str());
		fprintf(m_fpLogFile, "\nCOMMAND LINE ARGUMENTS    :");
		for(int iCount=0; iCount<__argc; ++iCount)
			fprintf(m_fpLogFile, "\nARGUMENT(%02d)              : %s", iCount, __argv[iCount]);
		fprintf(m_fpLogFile, "\n**************** APPLICATION STARTING INFORMATION ***************************************************************************");
		fflush(m_fpLogFile);
		LeaveCriticalSection(&m_objCriticalSection);
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

CLoggerStaticInfo::~CLoggerStaticInfo(void)
{
	try
	{
		CloseLogFile();
		//If required MergeThisLogInMainLogFile.
		MergeThisLogInMainLogFile();
		DeleteCriticalSection(&m_objCriticalSection);
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

void CLoggerStaticInfo::OpenLogFile(void)
{
	try
	{
		EnterCriticalSection(&m_objCriticalSection);
		string strLogFileName	=	m_strLogPathWithoutFileName + m_strLogFileNameWithoutPath;
		char szMutexName[MAX_PATH];
		strLogFileName.resize(MAX_PATH-1);
		strcpy(szMutexName, strLogFileName.c_str());
		strupr(szMutexName);
		for(unsigned int uiCounter=0; szMutexName[uiCounter]!='\0'; ++uiCounter)
		{
			if(szMutexName[uiCounter] == '\\')
			szMutexName[uiCounter]	=	'/';
		}

		m_hMutex = CreateMutex(NULL, TRUE, szMutexName);
		if(m_hMutex == NULL)
		{
			ostringstream ossMsg;
			ossMsg << "Log file CreateMutex error :: " << GetLastError();
			throw exception(ossMsg.str().c_str());
		}

		switch(WaitForSingleObject(m_hMutex, 500L)) 
		{
			// The thread got mutex ownership.
		case WAIT_OBJECT_0: 
			break;

		case WAIT_TIMEOUT: //Cannot get mutex ownership due to time-out.
		case WAIT_ABANDONED: //Got ownership of the abandoned mutex object.
			throw exception("Cannot get log file mutex ownership :: Possible reason Log filename currently in use by another process");
		default:
			throw exception("Cannot get log file mutex ownership :: Possible reason Log filename currently in use by another process");
		}
		m_fpLogFile	=	fopen(strLogFileName.c_str(), "a+");
		if(NULL == m_fpLogFile)
		{
			char szTempPath [ 4096 ] = {0} ;
			GetTempPath ( 4095 , szTempPath ) ;
			string strTempPath ( szTempPath ) ;
			strTempPath += "\\" ;
			strTempPath += m_strLogFileNameWithoutPath ;
			m_fpLogFile = fopen ( strTempPath.c_str ( ) , "a+" ) ;
			if ( NULL == m_fpLogFile )
				exit ( EXIT_FAILURE ) ;
			fprintf ( m_fpLogFile , "Standard logging location D:\\cs_logs doesn't exist or is inaccessible. Functionality may be affected" ) ;
			//fprintf ( m_fpLogFile , "Couldn't access log file path" ) ;
			//throw exception(("Error in creating/opening log file :: "+strLogFileName).c_str());
		}
		LeaveCriticalSection(&m_objCriticalSection);
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

void CLoggerStaticInfo::CloseLogFile(void)
{
	try
	{
		EnterCriticalSection(&m_objCriticalSection);
		if(m_fpLogFile != NULL)
			fclose(m_fpLogFile);
		if(m_hMutex != NULL)
		{
			ReleaseMutex(m_hMutex);
			CloseHandle(m_hMutex);
		}
		m_fpLogFile	=	NULL;
		m_hMutex	=	NULL;
		LeaveCriticalSection(&m_objCriticalSection);
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

void CLoggerStaticInfo::SetMaxLogFileSize(long lMaxLogFileSize)
{
	try
	{
		EnterCriticalSection(&m_objCriticalSection);
		m_lMaxLogFileSize	=	lMaxLogFileSize;
		LeaveCriticalSection(&m_objCriticalSection);
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

void CLoggerStaticInfo::SetLogLevel(unsigned int uiLogLevel)
{
	try
	{
		EnterCriticalSection(&m_objCriticalSection);
		m_objELogLevel	=	(ELogLevel)uiLogLevel;
		LeaveCriticalSection(&m_objCriticalSection);
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

void CLoggerStaticInfo::SetLogFilePath(string strLogFileNameWithoutPath, string strLogPathWithoutFileName)
{
	try
	{
		EnterCriticalSection(&m_objCriticalSection);

		CloseLogFile();
		m_bMergeInMainLogFile	=	false;
		g_objELogDirectory = LD_UserDefinedDirectory;
		m_strLogPathWithoutFileName	=	strLogPathWithoutFileName;
		if(m_strLogPathWithoutFileName == "")
			throw exception("m_strLogPathWithoutFileName is empty.");
		if(m_strLogPathWithoutFileName[m_strLogPathWithoutFileName.size()-1]!='\\' && m_strLogPathWithoutFileName[m_strLogPathWithoutFileName.size()-1]!='/')
			m_strLogPathWithoutFileName	+=	"\\";

		g_objELogFilename = LF_UserDefinedFileName;
		m_strLogFileNameWithoutPath	=	strLogFileNameWithoutPath;
		if(m_strLogFileNameWithoutPath == "")
			throw exception("m_strLogFileNameWithoutPath is empty.");
		m_strLogFileNameWithoutPath	+=	".log";

		OpenLogFile();
		LeaveCriticalSection(&m_objCriticalSection);
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

void CLoggerStaticInfo::MergeThisLogInMainLogFile(void)
{
	try
	{
		if(!m_bMergeInMainLogFile)
			return;

		string strLogFileName	=	m_strLogPathWithoutFileName + m_strLogFileNameWithoutPath;
		ifstream fin(strLogFileName.c_str(), ios_base::in | ios_base::binary);
		if(!fin.is_open())
			throw exception(("MergeThisLogInMainLogFile :: Error in opening file :: "+strLogFileName).c_str());

		string strOriginalLogFileName	=	ApplicationInfo::g_strApplicationPath + "\\" + ApplicationInfo::g_strApplicationNameWithoutExtension;
		ofstream fout(strOriginalLogFileName.c_str(), ios_base::app | ios_base::binary);
		if(!fout.is_open())
		{
			fin.close();
			throw exception(("MergeThisLogInMainLogFile :: Error in opening file :: "+strOriginalLogFileName).c_str());
		}

		//Delete backup file if exist.
		char szBackupFileName[MAX_PATH];
		strcpy(szBackupFileName, strLogFileName.c_str());
		strcpy(&szBackupFileName[strlen(szBackupFileName)] -3, "bak");
		bool bBackupFileExist	=	false;
		if(0 == access(szBackupFileName, 0))
		{
			unlink(szBackupFileName);
			bBackupFileExist	=	true;
		}

		if(fout.tellp() >= m_lMaxLogFileSize || bBackupFileExist)
		{
			fout.close();
			unlink(strOriginalLogFileName.c_str());
			fout.open(strOriginalLogFileName.c_str(), ios_base::app | ios_base::binary);
			if(!fout.is_open())
			{
				fin.close();
				throw exception(("MergeThisLogInMainLogFile :: Error in opening file :: "+strOriginalLogFileName).c_str());
			}
		}
		fout << fin.rdbuf();

		fin.close();
		fout.close();
		unlink(strLogFileName.c_str());
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

void CLoggerStaticInfo::CheckAndBackupLogFile(void)
{
	try
	{
		EnterCriticalSection(&m_objCriticalSection);
		if(ftell(m_fpLogFile) >= m_lMaxLogFileSize)
		{
			string strLogFileName	=	m_strLogPathWithoutFileName + m_strLogFileNameWithoutPath;
			char szBackupFileName[MAX_PATH];
			strcpy(szBackupFileName, strLogFileName.c_str());
			strcpy(&szBackupFileName[strlen(szBackupFileName)] -3, "bak");

			if(NULL != m_fpLogFile)
			{
				fclose(m_fpLogFile);
				m_fpLogFile	=	NULL;
			}

			unlink(szBackupFileName);
			if(rename(strLogFileName.c_str(), szBackupFileName))
				throw exception("Error in backup/rename of log file.");

			m_fpLogFile	=	fopen(strLogFileName.c_str(), "a+");
			if(NULL == m_fpLogFile)
				throw exception("Error in backup/opening log file.");
		}
		LeaveCriticalSection(&m_objCriticalSection);
	}
	catch(exception &e)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: %s\n", e.what());
		exit(-1);
	}
	catch(...)
	{
		printf("Error Code :: -1\n");
		printf("Reason     :: Error occured before starting main().\n");
		printf("Message    :: Unknown Error in function "__FUNCTION__"()\n");
		exit(-1);
	}
}

//----------End-------------class CLoggerStaticInfo------------------------------------------------
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//----------Start-----------class CLogger----------------------------------------------------------
CLoggerStaticInfo CLogger::m_objCLoggerStaticInfo;

CLogger::CLogger(const char *pszFunctionName)
{
	m_dwThreadId		=	GetCurrentThreadId();
	m_strFunctionName	=	pszFunctionName;
	LogTRACE("Entering{");
}

CLogger::CLogger(const char *pszFunctionName, long lLineNumber)
{
	m_dwThreadId		=	GetCurrentThreadId();
	m_strFunctionName	=	pszFunctionName;
	//LogTRACE("Entering{ LINE ::", lLineNumber);
}

CLogger::~CLogger(void)
{
	LogTRACE("Exiting}");
}

void CLogger::Log(ELogLevel objELogLevel, const char *pszLogLevel, const char *pszMessage)
{
	if(m_objCLoggerStaticInfo.m_objELogLevel <= objELogLevel)
	{
		m_objCLoggerStaticInfo.CheckAndBackupLogFile();
		char szDate [10];
		char szTime [10];
		_strdate(szDate);
		_strtime(szTime);
		EnterCriticalSection(&m_objCLoggerStaticInfo.m_objCriticalSection);
		fprintf(m_objCLoggerStaticInfo.m_fpLogFile, "\n[0x%08X]%s-%s : %-8s :: %s() <> %s", m_dwThreadId, szDate, szTime, pszLogLevel, m_strFunctionName.c_str(), pszMessage);
		fflush(m_objCLoggerStaticInfo.m_fpLogFile);
		LeaveCriticalSection(&m_objCLoggerStaticInfo.m_objCriticalSection);
	}
}

//----------End-------------class CLogger----------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#endif
