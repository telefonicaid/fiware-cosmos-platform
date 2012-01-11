#include "stdafx.h"
#include "Utils.h"

const char INVALID_FILENAME_CHARS[]		=	"\\/:*?\"<>|";

/**************************************************************************
 NAME			:	CheckOutFilePrefixAndCreatePath
 DESCRIPTION	:	Checks and creating(if not exist) strOutputFile path.
					Also checks for valid filename characters after last slash.
 PARAMETERS		:	void
 RETURN TYPE	:	void
**************************************************************************/
void Utils::CheckOutFilePrefixAndCreatePath( const string& strOutputFile )
{
	try
	{
#ifdef _WIN32
		//Checking and creating(if not exist) path.
		//Also checking for valid filename characters after last slash.
		char szDirPath[MAX_PATH];
		char szFileName[MAX_PATH];
		char *pcSlash;
		strcpy(szDirPath, strOutputFile.c_str());
		pcSlash	=	strrchr(szDirPath, '\\');
		if (pcSlash == NULL)			
		{
			strcpy(szFileName, szDirPath);
		}
		else
		{
			strcpy(szFileName, (pcSlash+1));
			*(pcSlash+1)	=	'\0';
			//Checking: Create new path in current application path or full path is given.
			//If first character is '\' then path is complete but drive is not given.
			//If second character is ':' then path is complete and drive is also given.
			//Else find current application path and add it to above path.
			if( szDirPath[1] == ':'	||	szDirPath[0] == '\\' )
			{
				//Do nothing because no need to find application path.
			}
			else//Find application path.
			{
				char szWorkingDirectory[MAX_PATH];
				_getcwd(szWorkingDirectory, MAX_PATH);
				//If the current working directory is the root, the string ends with a backslash ( \ ).
				//If the current working directory is a directory other than the root, the string ends 
				//with the directory name and not with a backslash.
				if(szWorkingDirectory[strlen(szWorkingDirectory) - 1] != '\\')
					strcat(szWorkingDirectory, "\\");
				strcat(szWorkingDirectory, szDirPath);
				strcpy(szDirPath, szWorkingDirectory);
			}
			int ret = SHCreateDirectoryEx(NULL, szDirPath, NULL);			
			if((ret != ERROR_SUCCESS) && (ret != ERROR_ALREADY_EXISTS) && (ret != ERROR_ACCESS_DENIED))
			{
				throw exception("Destination path not found or error in creating destination path.");
			}
		}
		if(strlen(szFileName) == 0)
		{
			throw exception("No output filename specified.");
		}
		else if(strpbrk(szFileName, INVALID_FILENAME_CHARS) != NULL )
		{
			throw exception("Invalid character(s) in output file prefix.");
		}
#else
		char* szFileName = NULL ;
		char szResolvedFileName [ 4096 ] = {0} ;
		string strErrorMessage ;
		size_t stIndexOfLastSlash ;

		if ( string :: npos == ( stIndexOfLastSlash = strOutputFile.rfind ( "/" ) ) )
			szFileName = (char*) strOutputFile.c_str ( ) ;
		else
			szFileName = (char*) strOutputFile.substr ( 0 , stIndexOfLastSlash + 1 ).c_str ( ) ;
		do
		{
				memset ( szResolvedFileName , 0 , 4096 ) ;
				if ( NULL == realpath ( szFileName , szResolvedFileName ) )
				{
						if ( ENOENT != errno )
						{
								strErrorMessage = "While creating directory: [" ;
								strErrorMessage += szFileName ;
								strErrorMessage += "] unable to get full path." ;
								strErrorMessage += "Following error occurred: " ;
								strErrorMessage += "[" ;
								strErrorMessage += strerror ( errno ) ;
								strErrorMessage += "]" ;
								strErrorMessage += "Resolved path: " ;
								strErrorMessage += "[" ;
								strErrorMessage += szResolvedFileName ;
								strErrorMessage += "]" ;
								throw strErrorMessage ;
						}
				}
				if ( 0 != mkdir ( szResolvedFileName , S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH ) )
				{
						if ( errno == EEXIST )
						{
								struct stat objStat ;

								if ( -1 == stat ( szResolvedFileName , &objStat ) )
								{
										strErrorMessage = "While creating directory: [" ;
										strErrorMessage += szFileName ;
										strErrorMessage += "] unable to get stats." ;
										strErrorMessage += "Following error occurred: " ;
										strErrorMessage += "[" ;
										strErrorMessage += strerror ( errno ) ;
										strErrorMessage += "]" ;
										strErrorMessage += "Resolved path: " ;
										strErrorMessage += "[" ;
										strErrorMessage += szResolvedFileName ;
										strErrorMessage += "]" ;
										throw strErrorMessage ;
								}
								if ( ! ( S_IFDIR & objStat.st_mode ) )
								{
										strErrorMessage = "While creating directory: [" ;
										strErrorMessage += szFileName ;
										strErrorMessage += "] found that [" ;
										strErrorMessage += szResolvedFileName ;
										strErrorMessage += "]" ;
										strErrorMessage += " is not a directory" ;
										throw strErrorMessage ;
								}
								break ;
						}
						else
						{
								strErrorMessage = "While creating directory: [" ;
								strErrorMessage += szFileName ;
								strErrorMessage += "] following error occurred: " ;
								strErrorMessage += "[" ;
								strErrorMessage += strerror ( errno ) ;
								strErrorMessage += "]" ;
								throw strErrorMessage ;
						}
				}
		}while ( true ) ;
#endif
	}
	catch(exception& e)
	{
		throw e;
	}	
	catch(...)
	{
#ifdef _WIN32
		throw exception("Unknown Error in function"__FUNCTION__"()");
#else
		ostringstream objOssExceptionMessage ;
		objOssExceptionMessage << "Unknown error in function" ;
		objOssExceptionMessage << __FUNCTION__ ;
		objOssExceptionMessage << "()" ;

		throw objOssExceptionMessage.str ( ) ;
#endif
	}
}



//Code taken from Web Log Parser 1.5.18
void Utils::ParseDomainFromHost(string strHost,string& strDomain/*, string strTopLevelDomains*/)
{

string strTopLevelDomains = "ac|ad|ae|aero|af|ag|ai|al|am|an|ao|aq|ar|arpa|as|asia|at|au|aw|ax|az|ba|bb|bd|be|bf|bg|bh|bi|biz|bj|bm|bn|bo|br|bs|bt|bv|bw|by|bz|ca|cat|cc|cd|cf|cg|ch|ci|ck|cl|cm|cn|co|com|coop|cr|cs|cu|cv|cx|cy|cz|de|dj|dk|dm|do|dz|ec|edu|ee|eg|eh|er|es|et|eu|fi|fj|fk|fm|fo|fr|ga|gb|gd|ge|gf|gg|gh|gi|gl|gm|gn|gov|gp|gq|gr|gs|gt|gu|gw|gy|hk|hm|hn|hr|ht|hu|id|ie|il|im|in|info|int|io|iq|ir|is|it|je|jm|jo|jobs|jp|ke|kg|kh|ki|km|kn|kp|kr|kw|ky|kz|la|lb|lc|li|lk|lr|ls|lt|lu|lv|ly|ma|mc|md|mg|mh|mil|mk|ml|mm|mn|mo|mobi|mp|mq|mr|ms|mt|mu|museum|mv|mw|mx|my|mz|na|name|nc|ne|net|nf|ng|ni|nl|no|np|nr|nt|nu|nz|om|org|pa|pe|pf|pg|ph|pk|pl|pm|pn|pr|pro|ps|pt|pw|py|qa|re|ro|ru|rw|sa|sb|sc|sd|se|sg|sh|si|sj|sk|sl|sm|sn|so|sr|st|su|sv|sy|sz|tc|td|tel|tf|tg|th|tj|tk|tl|tm|tn|to|tp|tr|travel|tt|tv|tw|";
	//---------------------------------------------------------------------
	// Remove Port from Host if present
	//---------------------------------------------------------------------
	size_t iPortPos = strHost.find(":");
	if (iPortPos != string::npos)
	{
		strHost = strHost.substr(0,iPortPos);
	}

	//---------------------------------------------------------------------
	// lowercase the host
	//---------------------------------------------------------------------
	//Commented as per discussion with Ritesh (16 May 2007 Mail, Subject:Re: RE: Emailing: 1359_MergedResult.bcp.gz).
	//	util::tolower(strHost);

	//---------------------------------------------------------------------
	// Remove trailing dots from host
	//---------------------------------------------------------------------
	while ((strHost.length() > 0) && (strHost.substr(strHost.length() - 1, 1) == "."))
	{
		strHost = strHost.substr(0, strHost.length() - 1);
	}

	//---------------------------------------------------------------------
	// Strip Domain from host
	//---------------------------------------------------------------------
	if (strHost.length() == 0)
	{
		//
		// Host and Domain are blank
		//
		strDomain = strHost;
	}
	else
	{
		//
		// Trim Host length to 63
		//
		//Commented as per discussion with Ritesh (16 May 2007 Mail, Subject:Re: RE: Emailing: 1359_MergedResult.bcp.gz).
		//		if (strHost.length() > 63)
		//		{
		//			strHost = strHost.substr(strHost.length() - 63);
		//		}

		//
		// Split Host on "."
		//
		vector<string> strFields;
		strFields.reserve(5);
		util::split_string(strHost, '.', back_inserter(strFields));
		//DSC		strFields	=	ParseString(strHost, ".");

		//
		// Check for IP address as host
		// We will call this an IP type host if every character is numeric and
		// there are exactly four parts.
		//
		bool bIPAddress = 0;
		if (strFields.size() == 4)
		{
			long iValue;
			size_t iHostLength = strHost.length();
			bIPAddress = 1;

			//
			// Check each character in the string against ".0123456789"
			// Stop the loop as soon as we find a character not in the list
			// Ascii for '.' is 46, ascii for the numerics are 48-57.
			//

			for (size_t CharIndex = 0; CharIndex < iHostLength; CharIndex++)
			{
				iValue = strHost[CharIndex];

				if ((iValue != 46) && ((iValue < 48) || (iValue > 57)))
				{
					// We have a non numeric
					// Stop the loop by moving the iterator past the end
					CharIndex = iHostLength + 1;
					bIPAddress = 0;
				}
			}
		}

		if (bIPAddress)
		{
			//
			// This is an IP address
			// Set domain equal to host
			//
			strDomain = strHost;
		}
		else
		{
			//
			// If the host only contains one or two parts
			// set domain equal to host
			//
			if (strFields.size() < 2)
			{
				strDomain = strHost;
			}
			else
			{
				//
				// Build up the domain name
				// Break the host up in segments split on "."
				// The domain is the last segment plus
				// the first segment not containing one of ".co.com.org.net.ac"
				//
				/*string strTest(".co.com.org.net.ac");*/
				strTopLevelDomains = util::replace_all(strTopLevelDomains , string("|"), string("."));
				strTopLevelDomains.insert(0,".");
				strTopLevelDomains.append(".");
				int iLastField = strFields.size() - 1;//tbd - fix warning
				strDomain = strFields[iLastField];
				for (int FieldIndex = (iLastField-1); FieldIndex >= 0; FieldIndex--)
				{
					string strTemp = "." + strFields[FieldIndex] + ".";
					if(strFields[FieldIndex].compare("www") != 0) // take care that "www" does not get appended to DomainName
					{
						strDomain.insert(0, ".");
						strDomain.insert(0, strFields[FieldIndex] );
					}
					if (strTopLevelDomains.find(strTemp) == string::npos)
					{
						//
						// Stop the loop as soon as we find an item from the search list
						//
						break;
						//FieldIndex = -1;
					}
				}
			}
		}
	}
}

//Code taken from Web Log Parser 1.5.18
/*****************************************************************************
 NAME		:	ParseURL_Ver2
 DESCRIPTION	:	Parses a Url to extract domain, host, dir, page, querystring
					and querydata from it.
 PARAMETERS	:	string strURL
			string& strDomai
			string& strHost
			string& strDir
			string& strPage
			string& strQueryString
			string& strQueryData
			string& strProtocol
 RETURN TYPE	:	void
******************************************************************************/
void Utils::ParseURL_Ver2(string strURL, string& strDomain, string& strHost, string& strDir, string& strPage,
					string& strQueryString, string& strQueryData, string& strProtocol)
{
	string strWorkingURL;
	strDomain		=	"";
	strHost			=	"";
	strDir			=	"/";
	strPage			=	"/";
	strQueryString	=	"";
	strQueryData	=	"";
	strProtocol		=	"";

	//strURL = UUDecode(strURL);	//commented to resove bug-id 167 (%** not working properly for search phrase)
//Commented as per discussion with Will Duckworth (20 Apr 2007 Ritesh Forwarded Mail).
/*
	//---------------------------------------------------------------------
	//-- Strip Host from URL	
	//---------------------------------------------------------------------
	size_t iStartSlashPos = strURL.find("//");
	if ((iStartSlashPos != string::npos) && (iStartSlashPos > 2))
	{
		strProtocol = util::replace_all(strURL.substr(0, iStartSlashPos), string(":"), string(""));
//DSC		strProtocol = ReplaceAll(strURL.substr(0, iStartSlashPos), string(":"), string(""));
		size_t iEndSlashPos = strURL.find("/", iStartSlashPos + 2);
		if (iEndSlashPos != string::npos) 
		{
			strHost = strURL.substr(iStartSlashPos + 2, iEndSlashPos - iStartSlashPos - 2);
			strWorkingURL =  strURL.substr(iEndSlashPos + 1, strURL.length() - iStartSlashPos - 1);

			//---------------------------------------------------------------------
			//-- Set querystring = everything after the host
			//-- This dupliacate the dcnProcessXML bevavior
			//---------------------------------------------------------------------
			strQueryString = strURL.substr(iEndSlashPos, strURL.length());

			//---------------------------------------------------------------------
			//-- Remove querystring from working dir
			//---------------------------------------------------------------------
			size_t iQSLoc = strWorkingURL.find("?");
			if (iQSLoc != string::npos)
			{
				strQueryData  = strWorkingURL.substr(iQSLoc + 1);
				strWorkingURL = strWorkingURL.substr(0, iQSLoc);
			}

			//---------------------------------------------------------------------
			// Remove semicolon parameter list from working dir
			//---------------------------------------------------------------------
			size_t iSemicolonLoc = strWorkingURL.find(";");
			if (iSemicolonLoc != string::npos)
			{
				strWorkingURL = strWorkingURL.substr(0, iSemicolonLoc);
			}

			//---------------------------------------------------------------------
			//-- Separate page from dir
			//---------------------------------------------------------------------

			size_t iSlashLoc = strWorkingURL.rfind("/");
			if (iSlashLoc != string::npos)
			{
				strPage = strWorkingURL.substr(iSlashLoc + 1, strWorkingURL.length() - iSlashLoc);
				strDir  = strWorkingURL.substr(0, iSlashLoc);
			}
			else
			{
				strPage = strWorkingURL;
				strDir  = "/";
			}
		}
		else
		{
			strHost = strURL.substr(iStartSlashPos + 2);
		}
	}
	else
*/
	{
		strProtocol = "";
		size_t iEndSlashPos = strURL.find("/", 0);
		if (iEndSlashPos != string::npos) 
		{
			strHost = strURL.substr(0, iEndSlashPos);
			strWorkingURL =  strURL.substr(iEndSlashPos + 1, strURL.length());

			//---------------------------------------------------------------------
			//-- Set querystring = everything after the host
			//-- This dupliacate the dcnProcessXML bevavior
			//---------------------------------------------------------------------
			strQueryString = strURL.substr(iEndSlashPos, strURL.length());

			//---------------------------------------------------------------------
			//-- Remove querystring from working dir
			//---------------------------------------------------------------------
			size_t iQSLoc = strWorkingURL.find("?");
			if (iQSLoc != string::npos)
			{
				strQueryData  = strWorkingURL.substr(iQSLoc + 1);
				strWorkingURL = strWorkingURL.substr(0, iQSLoc);
			}

			//---------------------------------------------------------------------
			// Remove semicolon parameter list from working dir
			//---------------------------------------------------------------------
			size_t iSemicolonLoc = strWorkingURL.find(";");
			if (iSemicolonLoc != string::npos)
			{
				strWorkingURL = strWorkingURL.substr(0, iSemicolonLoc);
			}

			//---------------------------------------------------------------------
			//-- Separate page from dir
			//---------------------------------------------------------------------

			size_t iSlashLoc = strWorkingURL.rfind("/");
			if (iSlashLoc != string::npos)
			{
				strPage = strWorkingURL.substr(iSlashLoc + 1, strWorkingURL.length() - iSlashLoc);
				strDir  = strWorkingURL.substr(0, iSlashLoc);
			}
			else
			{
				strPage = strWorkingURL;
				strDir  = "/";
			}
		}
		else
		{
			strHost = strURL;
		}
	}

	//---------------------------------------------------------------------
	// Remove Port from Host if present
	//---------------------------------------------------------------------
	size_t iPortPos = strHost.find(":");
	if (iPortPos != string::npos)
	{
		strHost = strHost.substr(0,iPortPos);
	}

	//---------------------------------------------------------------------
	// lowercase the host
	//---------------------------------------------------------------------
//Commented as per discussion with Ritesh (16 May 2007 Mail, Subject:Re: RE: Emailing: 1359_MergedResult.bcp.gz).
//	util::tolower(strHost);

	//---------------------------------------------------------------------
	// Remove trailing dots from host
	//---------------------------------------------------------------------
	while ((strHost.length() > 0) && (strHost.substr(strHost.length() - 1, 1) == "."))
	{
		strHost = strHost.substr(0, strHost.length() - 1);
	}

	//---------------------------------------------------------------------
	// Strip Domain from host
	//---------------------------------------------------------------------
	if (strHost.length() == 0)
	{
		//
		// Host and Domain are blank
		//
		strDomain = strHost;
	}
	else
	{
		//
		// Trim Host length to 63
		//
//Commented as per discussion with Ritesh (16 May 2007 Mail, Subject:Re: RE: Emailing: 1359_MergedResult.bcp.gz).
//		if (strHost.length() > 63)
//		{
//			strHost = strHost.substr(strHost.length() - 63);
//		}

		//
		// Split Host on "."
		//
		vector<string> strFields;
		strFields.reserve(5);
		util::split_string(strHost, '.', back_inserter(strFields));
//DSC		strFields	=	ParseString(strHost, ".");

		//
		// Check for IP address as host
		// We will call this an IP type host if every character is numeric and
		// there are exactly four parts.
		//
		bool bIPAddress = 0;
		if (strFields.size() == 4)
		{
			long iValue;
			size_t iHostLength = strHost.length();
			bIPAddress = 1;

			//
			// Check each character in the string against ".0123456789"
			// Stop the loop as soon as we find a character not in the list
			// Ascii for '.' is 46, ascii for the numerics are 48-57.
			//

			for (size_t CharIndex = 0; CharIndex < iHostLength; CharIndex++)
			{
				iValue = strHost[CharIndex];

				if ((iValue != 46) && ((iValue < 48) || (iValue > 57)))
				{
					// We have a non numeric
					// Stop the loop by moving the iterator past the end
					CharIndex = iHostLength + 1;
					bIPAddress = 0;
				}
			}
		}

		if (bIPAddress)
		{
			//
			// This is an IP address
			// Set domain equal to host
			//
			strDomain = strHost;
		}
		else
		{
			//
			// If the host only contains one or two parts
			// set domain equal to host
			//
			if (strFields.size() < 2)
			{
				strDomain = strHost;
			}
			else
			{
				//
				// Build up the domain name
				// Break the host up in segments split on "."
				// The domain is the last segment plus
				// the first segment not containing one of ".co.com.org.net.ac"
				//
				string strTest(".co.com.org.net.ac");
				int iLastField = strFields.size() - 1;//tbd - fix warning
				strDomain = strFields[iLastField];
				for (int FieldIndex = (iLastField-1); FieldIndex >= 0; FieldIndex--)
				{
					string strTemp = "." + strFields[FieldIndex];
					strDomain.insert(0, ".");
					strDomain.insert(0, strFields[FieldIndex] );
					if (strTest.find(strTemp) == string::npos)
					{
						//
						// Stop the loop as soon as we find an item from the search list
						//
						break;
						//FieldIndex = -1;
					}
				}
			}
		}
	}
	//---------------------------------------------------------------------
	// Clean fields
	//---------------------------------------------------------------------
	if (strDir.length() == 0)
	{
		strDir = "/";
	}
	if (strPage.length() == 0)
	{
		strPage = "/";
	}
	if (strQueryString.length() == 0)
	{
		strQueryString = "/";
	}
}

#ifdef _WIN32
void Utils :: MergeFile ( const char* pszSourceFile , const char* pszTargetFile )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	HANDLE hReaderMutex ;
	HANDLE hWriterMutex ;
#ifdef _WIN32
	Utils :: CheckBarrier ( "/csCFD/CacheSynchronizationMutex-ReadOnly" , hReaderMutex ) ;
	Utils :: CheckBarrier ( "/csCFD/CacheSynchronizationMutex-WriteOnly" , hWriterMutex ) ;
#else
	Utils :: CheckBarrier ( "/tmp/csCFD-CacheSynchronizationMutex-ReadOnly" , hReaderMutex ) ;
	Utils :: CheckBarrier ( "/tmp/csCFD-CacheSynchronizationMutex-WriteOnly" , hWriterMutex ) ;
#endif
	if ( ! MoveFile ( pszSourceFile , pszTargetFile ) )
	{
		ostringstream objOssFileRenameStatus ;
		if ( ERROR_ALREADY_EXISTS == GetLastError ( ) )
		{
			objOssFileRenameStatus << "Merging of: [" << pszSourceFile << "] into: [" << pszTargetFile << "] not required." ;
			objOssFileRenameStatus << "Looks like some other Process / Thread has already done the merge" ;
			objLogger.LogINFO ( objOssFileRenameStatus.str ( ).c_str ( ) ) ;
		}
		else
		{
			objOssFileRenameStatus << "Error occurred: [" << GetLastError ( ) << "] while renaming: [" << pszSourceFile << "] to: [" << pszTargetFile << "]" ;
			objLogger.LogERROR ( objOssFileRenameStatus.str ( ).c_str ( ) ) ;
			Utils :: UnCheckBarrier ( hWriterMutex ) ;
			Utils :: UnCheckBarrier ( hReaderMutex ) ;
			throw CCSCFDException ( objOssFileRenameStatus.str ( ) , CS_ERROR_FILE_MOVE_FAILED ) ;
		}
	}
	else
		objLogger.LogINFO ( "Merging of: [" , pszSourceFile , "] into: [" , pszTargetFile , "] done succesfully" ) ;
	Utils :: UnCheckBarrier ( hWriterMutex ) ;
	Utils :: UnCheckBarrier ( hReaderMutex ) ;
}
#else
#endif

#ifdef _WIN32
//Courtesy: http://msdn.microsoft.com/en-us/library/aa363778.aspx
void Utils :: MergeFile2 ( const char* pszSourceFile , const char* pszTargetFile )
{
  HANDLE hFile;
  HANDLE hAppend;
  DWORD  dwBytesRead, dwBytesWritten, dwPos;
  BYTE   buff[4096];

  // Open the existing file.

  hFile = CreateFile(TEXT(pszSourceFile), // open One.txt
			GENERIC_READ,             // open for reading
			0,                        // do not share
			NULL,                     // no security
			OPEN_EXISTING,            // existing file only
			FILE_ATTRIBUTE_NORMAL,    // normal file
			NULL);                    // no attr. template
  if (hFile == INVALID_HANDLE_VALUE)
  {
	  ostringstream objOssSourceFileOpenError ;

	  objOssSourceFileOpenError << "Error: (" << CS_ERROR_FILE_NOT_FOUND << "): " ;
	  objOssSourceFileOpenError << "Could not open: " ;
	  objOssSourceFileOpenError << "[" << pszSourceFile << "]" ;
	  objOssSourceFileOpenError << " ,Error code: " ;
	  objOssSourceFileOpenError << "[" << GetLastError ( ) << "]" ;

	  throw CCSCFDException ( objOssSourceFileOpenError.str ( ) , CS_ERROR_FILE_NOT_FOUND ) ;
  }

  HANDLE hMutex ;
  DWORD dwWaitResult ;
  char* pszMutexName = strdup ( pszTargetFile ) ;
  for ( unsigned int ui = 0 ; ui < strlen ( pszMutexName ) ; ui ++ )
  {
	  if ( pszMutexName [ ui ] == '\\' )
		  pszMutexName [ ui ] = '/' ;
  }
  if ( NULL == ( hMutex = CreateMutex ( NULL , FALSE , pszMutexName ) ) )
  {
	  ostringstream objOssCacheMergeMutexCreationError ;

	  objOssCacheMergeMutexCreationError << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
	  objOssCacheMergeMutexCreationError << "Cache Merge mutex: " ;
	  objOssCacheMergeMutexCreationError << "[" << pszMutexName << "]" ;
	  objOssCacheMergeMutexCreationError << " creation failed. Error code: " ;
	  objOssCacheMergeMutexCreationError << "[" << GetLastError ( ) << "]" ;
	  CloseHandle ( hFile ) ;
	  free ( pszMutexName ) ;

	  throw CCSCFDException ( objOssCacheMergeMutexCreationError.str ( ) , CS_ERROR_OS_ERROR ) ;
  }
  free ( pszMutexName ) ;
  ostringstream objOssErrorMessage ;
  switch ( ( dwWaitResult = WaitForSingleObject ( hMutex , 500L )  ) )
  {
  case WAIT_OBJECT_0:
	  break ;//This process got ownership. Safe to merge
  case WAIT_TIMEOUT:
	  CloseHandle ( hMutex ) ;

	  objOssErrorMessage << "Error: (" << CS_ERROR_TIMEOUT << "): " ;
	  objOssErrorMessage << "Timeout occurred while waiting for mutex object" ;
	  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_TIMEOUT ) ;
  case WAIT_ABANDONED:
	  CloseHandle ( hMutex ) ;

	  objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
	  objOssErrorMessage << "Wait Abandoned" ;
	  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
  default:

	  objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
	  objOssErrorMessage << "Error occurred while waiting for mutex. Return value of WaitForSingleObject: " ;
	  objOssErrorMessage << "[" << dwWaitResult << "]" ;
	  CloseHandle ( hMutex ) ;
	  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
  }

  // Open the existing file, or if the file does not exist,
  // create a new file.

  while ( true )
  {
	  hAppend = CreateFile(TEXT(pszTargetFile), // open Two.txt
				  FILE_APPEND_DATA,         // open for writing
				  0,			          // do not share
				  NULL,                     // no security
				  OPEN_ALWAYS,              // open or create
				  FILE_ATTRIBUTE_NORMAL,    // normal file
				  NULL);                    // no attr. template
	  if ( INVALID_HANDLE_VALUE == hAppend )
	  {
		  if ( ERROR_SHARING_VIOLATION == GetLastError ( ) )
		  {
			fprintf ( stderr, "Waiting for one sec and then checking for existence\n" ) ;
			Sleep ( 100 ) ;

			if ( 0 == _access ( pszSourceFile , 0 ) )
				continue ;	// ReApplying the checks because file existence doesn't mean that nobody else is accessing the file
		  }
		  else
		  {
			  ostringstream objOssErrorMessage ;

			  objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
			  objOssErrorMessage << "Some unknown error: " ;
			  objOssErrorMessage << "[" << GetLastError ( ) << "]" ;
			  objOssErrorMessage << " has occurred" ;

			  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
		  }
	  }
	  else
	  {
		  fprintf ( stderr , "Got access !! Ready To Merge\n" ) ;
		  break ;
	  }
  }

  if (hAppend == INVALID_HANDLE_VALUE)
  {
	  ostringstream objOssErrorMessage ;

	  objOssErrorMessage << "Error: (" << CS_ERROR_FILE_CREATION_FAILED << "): " ;
	  objOssErrorMessage << "Could not open: " ;
	  objOssErrorMessage << "[" << pszTargetFile << "]" ;
	  objOssErrorMessage << " , Error code: " ;
	  objOssErrorMessage << "[" << GetLastError ( ) << "]" ;
	  CloseHandle ( hFile ) ;
	  CloseHandle ( hMutex ) ;

	  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_FILE_CREATION_FAILED ) ;
  }

  //Appending to be done only if size of file is Greater than zero
  {
	  struct stat objStat ;
	  if ( 0 == stat ( pszTargetFile , &objStat ) )
	  {
		  if ( 20 > objStat.st_size )
		  {
			  // Append the first file to the end of the second file.
			  // Lock the second file to prevent another process from
			  // accessing it while writing to it. Unlock the
			  // file when writing is complete.

			  while (ReadFile(hFile, buff, sizeof(buff), &dwBytesRead, NULL)
				  && dwBytesRead > 0)
				{
				dwPos = SetFilePointer(hAppend, 0, NULL, FILE_END);
				LockFile(hAppend, dwPos, 0, dwBytesRead, 0);
				WriteFile(hAppend, buff, dwBytesRead, &dwBytesWritten, NULL);
				UnlockFile(hAppend, dwPos, 0, dwBytesRead, 0);
				}
		  }
	  }
	  else
	  {
		  ostringstream objOssErrorMessage ;

		  objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
		  objOssErrorMessage << "Error occurred: " ;
		  objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
		  objOssErrorMessage << " while stat'ing file: " ;
		  objOssErrorMessage << "[" << pszTargetFile << "]" ;
		  CloseHandle ( hFile ) ;
		  CloseHandle ( hAppend ) ;
		  CloseHandle ( hMutex ) ;
		  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
	  }
  }

  // Close both files.

  CloseHandle(hFile);
  CloseHandle(hAppend);
  CloseHandle ( hMutex ) ;
}
#else
void Utils :: MergeFile ( const char* pszSourceFile , const char* pszTargetFile )
{
        int iWriteFD = -1 ;
        int iReadFD = -1 ;
	mode_t objTargetMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ;
        struct flock objFlock ;

        //iWriteFD = open ( pszTargetFile , O_CREAT | O_WRONLY ) ;
        iWriteFD = open ( pszTargetFile , O_CREAT | O_RDWR , objTargetMode ) ;
        if ( 0 > iWriteFD )
		{
			  ostringstream objOssErrorMessage ;

			  objOssErrorMessage << "Error: (" << CS_ERROR_FILE_CREATION_FAILED << "): " ;
			  objOssErrorMessage << "Could not open: " ;
			  objOssErrorMessage << "[" << pszTargetFile << "]" ;
			  objOssErrorMessage << " , Error code: " ;
			  objOssErrorMessage << "[" << errno << "]" ;
			  objOssErrorMessage << "=[" << strerror ( errno ) << "]" ;
			  close ( iWriteFD ) ;

			  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_FILE_CREATION_FAILED ) ;
		}
        else
        {
                while ( true )
                {
                        objFlock.l_type = F_WRLCK ;
                        objFlock.l_whence = SEEK_SET ;
                        objFlock.l_start = 0 ;
                        objFlock.l_len = 0 ;

                        if ( 0 > fcntl ( iWriteFD , F_SETLK , &objFlock ) )
                        {
                                if ( ( EACCES == errno ) || ( EAGAIN == errno ) )
                                {
                                        fprintf ( stderr , "Waiting until unlocked\n" ) ;
                                        sleep ( 1 ) ;
                                        continue ;
                                }
                                else
                                {
									ostringstream objOssErrorMessage ;
									  objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
									  objOssErrorMessage << "Error occurred: " ; 
									  objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
									  objOssErrorMessage << " while getting lock on: " ;
									  objOssErrorMessage << "[" << pszTargetFile << "]" ;
									  close ( iWriteFD ) ;
									  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
                                }
                        }
                        else
                        {
                                fprintf ( stdout , "Got lock\n" ) ;
                                break ;
                        }
                }
                struct stat objTargetStat ;

                memset ( &objTargetStat , 0 , sizeof ( struct stat ) ) ;
                if ( 0 == stat ( pszTargetFile , &objTargetStat ) )
                {
                        if ( 20 > objTargetStat.st_size )
                        {

                                if ( 0 < ( iReadFD = open ( pszSourceFile , O_RDONLY ) ) )
                                {
                                        size_t stBytesRead = -1 ;
                                        char szData [ 8192 ] = {0} ;

                                        while ( 0 < ( stBytesRead = read ( iReadFD , szData , 8192 ) ) )
                                        {
                                                write ( iWriteFD , szData , stBytesRead ) ;
                                                memset ( szData , 0 , 8192 ) ;
                                        }

                                        close ( iReadFD ) ;
                                }
                        }
                        else fprintf ( stdout , "Already merged, Nothing to do\n" ) ;
                }
				else
				{
					ostringstream objOssErrorMessage ;

					objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
					objOssErrorMessage << "Error occurred: " ;
					objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
					objOssErrorMessage << " while stat'ing file: " ;
					objOssErrorMessage << "[" << pszTargetFile << "]" ;
					close ( iWriteFD ) ;
					close ( iReadFD ) ;
					throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
				}

                objFlock.l_type = F_UNLCK ;
                objFlock.l_whence = SEEK_SET ;
                objFlock.l_start = 0 ;
                objFlock.l_len = 0 ;
                if ( 0 > fcntl ( iWriteFD , F_SETLK , &objFlock ) )
				{
					ostringstream objOssErrorMessage ;

					objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
					objOssErrorMessage << "Error occurred: " ;
					objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
					objOssErrorMessage << " while unlocking file: " ;
					objOssErrorMessage << "[" << pszTargetFile << "]" ;
					close ( iWriteFD ) ;
					close ( iReadFD ) ;
					throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
				}
		else
			fprintf ( stderr , "Lock released\n" ) ;

                close ( iWriteFD ) ;
         }
}

#endif

#ifdef _WIN32
void Utils :: CheckBarrier ( const char* pszMutexName , HANDLE& hMutex )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	do
	{
		  DWORD dwWaitResult ;
		  //char* pszMutexName = "/csCFD/CacheSynchronizationMutex" ;
		  bool bSafeToContinue = false ;

		  if ( NULL == ( hMutex = CreateMutex ( NULL , FALSE , pszMutexName ) ) )
		  {
			  ostringstream objOssCacheMergeMutexCreationError ;

			  objOssCacheMergeMutexCreationError << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
			  objOssCacheMergeMutexCreationError << "Cache Merge mutex: " ;
			  objOssCacheMergeMutexCreationError << "[" << pszMutexName << "]" ;
			  objOssCacheMergeMutexCreationError << " creation failed. Error code: " ;
			  objOssCacheMergeMutexCreationError << "[" << GetLastError ( ) << "]" ;
			  //free ( pszMutexName ) ;

			  throw CCSCFDException ( objOssCacheMergeMutexCreationError.str ( ) , CS_ERROR_OS_ERROR ) ;
		  }

		  ostringstream objOssErrorMessage ;
		  switch ( ( dwWaitResult = WaitForSingleObject ( hMutex , 1000L )  ) )
		  {
		  case WAIT_OBJECT_0:
			  bSafeToContinue = true ;
			  break ;//This process got ownership. Safe to merge
		  case WAIT_TIMEOUT:
			  ReleaseMutex ( hMutex ) ;
			  CloseHandle ( hMutex ) ;

			  fprintf ( stderr , "Timeout occurred while waiting for mutex object. Sleeping for 1 sec and then retrying\n" ) ;
			  Sleep ( 1000 ) ;
			  continue ;
		  case WAIT_ABANDONED:
			  ReleaseMutex ( hMutex ) ;
			  CloseHandle ( hMutex ) ;

			  //objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
			  //objOssErrorMessage << "Wait Abandoned" ;
			  //throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
			  objLogger.LogERROR ( "Wait abandoned" ) ;
		  default:
			  if ( ERROR_ALREADY_EXISTS == GetLastError ( ) )
			  {
				  if ( NULL == ( hMutex = OpenMutex ( SYNCHRONIZE , FALSE , pszMutexName ) ) )
				  {
					  objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
					  objOssErrorMessage << "Error occurred OpenMutex." ;
					  objOssErrorMessage << " GetLastError(): [" << GetLastError ( ) << "]" ;
					  ReleaseMutex ( hMutex ) ;
					  CloseHandle ( hMutex ) ;
					  objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
					  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
				  }

				  switch ( ( dwWaitResult = WaitForSingleObject ( hMutex , 1000L )  ) )
				  {
				  case WAIT_OBJECT_0:
					  bSafeToContinue = true ;
					  break ;//This process got ownership. Safe to merge
				  case WAIT_TIMEOUT:
					  ReleaseMutex ( hMutex ) ;
					  CloseHandle ( hMutex ) ;

					  fprintf ( stderr , "Timeout occurred while waiting for (OpenMutex) mutex object. Sleeping for 1 sec and then retrying\n" ) ;
					  Sleep ( 1000 ) ;
					  continue ;
				  case WAIT_ABANDONED:
					  ReleaseMutex ( hMutex ) ;
					  CloseHandle ( hMutex ) ;

					  //objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
					  //objOssErrorMessage << "Wait Abandoned" ;
					  //throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
					  objLogger.LogERROR ( "(OpenMutex) Wait abandoned" ) ;
				  default:
					  objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
					  objOssErrorMessage << "Error occurred while waiting for (OpenMutex) mutex. Return value of WaitForSingleObject: " ;
					  objOssErrorMessage << "[" << dwWaitResult << "]: GetLastError(): [" << GetLastError ( ) << "]" ;
					  ReleaseMutex ( hMutex ) ;
					  CloseHandle ( hMutex ) ;
					  objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
					  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
					  break ;
				  }
			  }
			  else
			  {
				  objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
				  objOssErrorMessage << "Error occurred while waiting for mutex. Return value of WaitForSingleObject: " ;
				  objOssErrorMessage << "[" << dwWaitResult << "]: GetLastError(): [" << GetLastError ( ) << "]" ;
				  ReleaseMutex ( hMutex ) ;
				  CloseHandle ( hMutex ) ;
				  objLogger.LogERROR ( objOssErrorMessage.str ( ).c_str ( ) ) ;
				  throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
			  }
		  }

		  //Do this in UncheckBarrier - So that no other Process can attempt an access on file
		  //CloseHandle ( hMutex ) ;
		  if ( bSafeToContinue )
			  break ;
	}while ( true ) ;
}

void Utils :: UnCheckBarrier ( HANDLE hMutex )
{
	ReleaseMutex ( hMutex ) ;
	CloseHandle ( hMutex ) ;
}
#else
void Utils :: CheckBarrier ( const char* pszMutexName , HANDLE& hMutex )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;

	int fd = -1 ;
	struct flock objFlock ;
	mode_t objTargetMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ;

	fd = open ( pszMutexName , O_RDWR | O_CREAT , objTargetMode ) ; 
	if ( 0 > fd )
	{
		if ( EEXIST == errno )
		{
			fd = open ( pszMutexName , O_RDWR ) ;
			if ( 0 > fd )
			{
				ostringstream objOssErrorMessage ;

				objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
				objOssErrorMessage << "Some unknown error: " ;
				objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
				objOssErrorMessage << " while requesting write lock on: " ;
				objOssErrorMessage << "[" << pszMutexName << "]" ;

				objLogger.LogERROR ( "CheckBarrier EEXIST failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;

				throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
			}
		}
		else
		{
			ostringstream objOssErrorMessage ;

			objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
			objOssErrorMessage << "Some unknown error: " ;
			objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
			objOssErrorMessage << " while requesting write lock on: " ;
			objOssErrorMessage << "[" << pszMutexName << "]" ;

			objLogger.LogERROR ( "CheckBarrier failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;

			throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
		}
	}
	else
	{
		while ( true )
		{
			objFlock.l_type = F_WRLCK ;
			objFlock.l_whence = SEEK_SET ;
			objFlock.l_start = 0 ;
			objFlock.l_len = 0 ;

			if ( 0 > fcntl ( fd , F_SETLK , &objFlock ) )
			{
				if ( ( EACCES == errno ) || ( EAGAIN == errno ) )
				{
					fprintf ( stderr , "Waiting for one sec and then checking for existence\n" ) ;
					sleep ( 1 ) ;
					continue ;
				}
				else
				{
					ostringstream objOssErrorMessage ;

					objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
					objOssErrorMessage << "Some unknown error: " ;
					objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
					objOssErrorMessage << " while requesting write lock on: " ;
					objOssErrorMessage << "[" << pszMutexName << "]" ;

					objLogger.LogERROR ( "CheckBarrier failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;

					throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
				}
			}
			else
			{
				//fprintf ( stderr , "Got access !! Ready To load from cache\n" ) ;
				break ;
			}
		}
	}

	hMutex = fd ;
}

void Utils :: UnCheckBarrier ( HANDLE hMutex )
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	struct flock objFlock ;
	int fd = hMutex ;

	objFlock.l_type = F_UNLCK ;
	objFlock.l_whence = SEEK_SET ;
	objFlock.l_start = 0 ;
	objFlock.l_len = 0 ;
	if ( 0 > fcntl ( fd , F_SETLK , &objFlock ) )
	{
		ostringstream objOssErrorMessage ;

		objOssErrorMessage << "Error: (" << CS_ERROR_OS_ERROR << "): " ;
		objOssErrorMessage << "Some unknown error: " ;
		objOssErrorMessage << "[" << strerror ( errno ) << "]" ;
		objOssErrorMessage << " while unchecking barrier" ;

		objLogger.LogERROR ( "UnCheckBarrier failed." , objOssErrorMessage.str ( ).c_str ( ) ) ;

		throw CCSCFDException ( objOssErrorMessage.str ( ) , CS_ERROR_OS_ERROR ) ;
	}
	close ( fd ) ;
}
#endif

#ifdef _WIN32
//Code taken from dcnsubdomainsearch
std::string Utils :: GetStringSetting(HKEY hKeyParent, LPCTSTR lpszKeyName, LPCTSTR szValueName, LPCTSTR szDefValue)
{
	CLogger objLogger ( __FUNCTION__ , __LINE__ ) ;
	string strResult ( szDefValue ) ;

    DWORD cbData;
    DWORD dwRet;
	HKEY hKey ;

	if ( ( dwRet = RegOpenKeyEx ( hKeyParent , lpszKeyName , NULL , KEY_READ , &hKey ) ) == ERROR_SUCCESS )
	{

		char szData [ 4096 ] = {0} ;
		cbData = 4095 ;
		dwRet = RegQueryValueEx( hKey,
								 szValueName,
								 NULL,
								 NULL,
								 (LPBYTE) szData,
								 &cbData );
		if( dwRet == ERROR_SUCCESS )
			strResult = szData;
		else
		{
			char szErrorCode [ 128 ] = {0} ;

			sprintf ( szErrorCode , "RegQueryValueEx for: [%08X] returned: [%08X]" , szValueName , dwRet );
			objLogger.LogWARN ( szErrorCode ) ;
		}

		RegCloseKey ( hKey ) ;
	}
	else
	{
		char szErrorCode [ 128 ] = {0} ;

		sprintf ( szErrorCode , "RegOpenKeyEx for: [%08X] returned: [%08X]" , szValueName , dwRet );
		objLogger.LogWARN ( szErrorCode ) ;
	}

	return strResult ;
#if 0
	ATL :: CRegKey TheReg;
	char szValue[255];
	DWORD dwSize = 255;
	LONG lReturnCode = -1 ;

	if (ERROR_SUCCESS == ( lReturnCode = TheReg.Create(hKeyParent, lpszKeyName)))
	{
		dwSize = 255;
		if ( 0 == strcmp ( szValueName , "(Default)" ) )
		{
			if (ERROR_SUCCESS == ( lReturnCode = TheReg.QueryValue(NULL, NULL, szValue, &dwSize) ))
			{
				return std::string(szValue);
			}
			else
			{
				char szErrorCode [ 9 ] = {0} ;

				sprintf ( szErrorCode , "%08X" , lReturnCode );
				objLogger.LogWARN ( "TheReg.QueryValue NULL returned:" , szErrorCode ) ;
			}
		}
		else
		{
			LONG lReturnValue = -1 ;
			if (ERROR_SUCCESS == (lReturnValue = TheReg.QueryValue(szValueName, NULL, szValue, &dwSize)))
			{
				return std::string(szValue);
			}
			else
			{
				char szErrorCode [ 9 ] = {0} ;

				sprintf ( szErrorCode , "%08X" , lReturnCode );
				objLogger.LogWARN ( "TheReg.QueryValue" , szValueName , "returned:" , szErrorCode ) ;
			}
		}
	
		// If not found Create/set registry value

		//Commented this because we aren't interested in creating this key
		//LONG lresult = TheReg.SetStringValue(szValueName, szDefValue);
//		TheReg.SetStringValue(szValueName, NULL, szDefValue, strlen(szDefValue));
	}
	else
	{
		char szErrorCode [ 9 ] = {0} ;

		sprintf ( szErrorCode , "%08X" , lReturnCode );
		objLogger.LogWARN ( "TheReg.Create returned:" , szErrorCode ) ;
	}

	return std::string(szDefValue);
#endif
}
#endif

//#ifndef _WIN32
#if !defined(_WIN32) && defined(ENABLE_LINUX_DATABASE_LOAD)
string extract_error ( const char* szFunctionName , SQLHANDLE handle , SQLSMALLINT type )
{
        SQLINTEGER i = 0 ;
        SQLINTEGER native ;
        SQLCHAR state [ 7 ] ;
        SQLCHAR text [ 256 ] ;
        SQLSMALLINT len ;
        SQLRETURN ret ;
        ostringstream objOssMessage ;

        objOssMessage << "The driver reported following diagnostics while running " << szFunctionName ;
        do
        {
                ret = SQLGetDiagRec
                                (
                                        type ,
                                        handle ,
                                        ++i ,
                                        state ,
                                        &native ,
                                        text ,
                                        sizeof ( text ) ,
                                        &len
                                ) ;
                /*if ( ret == SQL_SUCCESS_WITH_INFO )
                        printf ( "Success  with info \n" ) ;
                else if ( ret == SQL_INVALID_HANDLE )
                        printf ( "Invalid handle\n" ) ;
                else if ( ret == SQL_ERROR )
                        printf ( "i is negative or 0 OR Buffer length is negative" ) ;
                else if ( ret == SQL_NO_DATA )
                        printf ( "RecNumber exceeds available diagnostic records" ) ;*/
                if ( SQL_SUCCEEDED ( ret ) )
                        objOssMessage << "[" << state << ":" << i << ":" << native << ":" << text << "]" ;
                else
                {
                        objOssMessage << "[Following error occurred in SQLGetDiagRec: " ;
                        if ( ret == SQL_SUCCESS_WITH_INFO )
                                objOssMessage << "[Success  with info]" ;
                        else if ( ret == SQL_INVALID_HANDLE )
                                objOssMessage << "[Invalid handle]" ;
                        else if ( ret == SQL_ERROR )
                                objOssMessage << "[i is negative or 0 OR Buffer length is negative]" ;
                        else if ( ret == SQL_NO_DATA )
                                objOssMessage << "[RecNumber exceeds available diagnostic records]" ;
                        else
                                objOssMessage << "[Unknown error]" ;
                        objOssMessage << "]" ;
                }
        }
        while ( ret == SQL_SUCCESS ) ;

        return objOssMessage.str ( ) ;
}
#endif

#ifdef _WIN32
string Utils :: GetVersion(const char* szFileName)
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
#endif
