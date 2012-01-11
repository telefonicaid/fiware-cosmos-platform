/*****************************************************************************
// Revision History
// Database.cpp: This file contains implementation for Database Class  and Table
// Class. This class is used to Insert, Update and Fetch Records from Database.
*******************************************************************************
// Date			Author			Version		Description			
// [mm/dd/yy]
*******************************************************************************
//								1.0.0.1		Created


******************************************************************************/
#include "stdafx.h"

#ifdef _WIN32
#include "Database.h"
#include "Logger.h"


/*****************************************************************************
 NAME			:	ErrorHandler
 DESCRIPTION	:	Handles the ComErrors generated while accessing the Database
 PARAMETERS		:	_com_error & ostringstream
 RETURN TYPE	:	-
******************************************************************************/
void ErrorHandler(_com_error &e, ostringstream &ErrStr)
{
	CLogger CLogObj("ErrorHandler()");

	try
	{
		ErrStr.str("");
		ErrStr << "Error:\n";
		ErrStr << "Code meaning = \n" << (char *)e.ErrorMessage();
		ErrStr << "Source = \n" << e.Source().GetBSTR();
		ErrStr << "Description = " << e.Description().GetBSTR();
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
	}
}


/*****************************************************************************
 NAME			:	Database
 DESCRIPTION	:	Constructor
 PARAMETERS		:	-
 RETURN TYPE	:	-
******************************************************************************/
Database::Database()
{
	CLogger CLogObj("Database::Database()");

	InitializeCriticalSection(&m_SyncCSection);

	m_Cnn = NULL;
	m_CommPtr = NULL;
	m_bDBReconnect = true;
	m_bDeadlock = false;
}

/*****************************************************************************
 NAME			:	GetErrorErrStr
 DESCRIPTION	:	Get error string
 PARAMETERS		:	char pointer
 RETURN TYPE	:	-
******************************************************************************/
void Database::GetErrorErrStr(char* ErrStr)
{
	CLogger CLogObj("Database::GetErrorErrStr");
	CLogObj.LogError(ErrStr);
}

/*****************************************************************************
 NAME			:	Open
 DESCRIPTION	:	Cretes a ADODB Connection Object and Opens the Physical Connection
 PARAMETERS		:	username, password, DNS Connection string
 RETURN TYPE	:	bool
******************************************************************************/
//Create the Database Connection
bool Database::Open(char* UserName, char* Pwd,char* CnnStr)
{
	/////////////////////////////////////
	EnterCriticalSection(&m_SyncCSection);
	////////////////////////////////////

	CLogger CLogObj("Database::Open()");
	HRESULT hr;
	bool bRet = false;

	//Reconnection is not required
	if(m_bDBReconnect == false)
	{
		LeaveCriticalSection(&m_SyncCSection);
		return true;
	}
	
	try
	{
		//Creating Instance of Connection Object
		hr = m_Cnn.CreateInstance( __uuidof( Connection ) );
		
		if(FAILED(hr))
			throw _com_error(hr,0,false);	
		



		//Trying to acquire Connection
		unsigned int uiReconnect = 0;
		while(1)
		{
			try
			{
				if(uiReconnect == 10)
				{
					CLogObj.LogError ("Database Connection Failed after Retry count:10");
					break;
				}				

				if(m_Cnn->Open(CnnStr, UserName, Pwd, NULL) == S_OK)
				{
					bRet = true;
					m_bDBReconnect = false;

					m_bDeadlock = false;

					break;
				}
				else
				{
					++uiReconnect;
					Sleep(DBRETRYCONNECTTIME);
					CLogObj.LogInfo ("Database Connection Retry count:",uiReconnect);
				}
			}
			catch(_com_error &e)
			{
				++uiReconnect;
				ErrorHandler(e,m_ErrStr);
				CLogObj.LogError(m_ErrStr.str().c_str());	
				Sleep(DBRETRYCONNECTTIME);
			}
			catch(std::exception &e)
			{
				++uiReconnect;
				CLogObj.LogError(e.what());
				Sleep(DBRETRYCONNECTTIME);
			}
			catch(...)
			{
				++uiReconnect;
				CLogObj.LogError("Unknown Error in Database::Open()");
				Sleep(DBRETRYCONNECTTIME);
			}
		}
	}
	catch(_com_error &e)
	{ 
		ErrorHandler(e,m_ErrStr);	
		CLogObj.LogError(m_ErrStr.str().c_str());
		bRet = false ;	
	}

	//if(bRet)
	//	m_ErrStr << "Success";
	//else
	//	m_ErrStr << "Failed";

	/////////////////////////////////////
	LeaveCriticalSection(&m_SyncCSection);
	/////////////////////////////////////

	return bRet;
}

/*****************************************************************************
 NAME			:	Close
 DESCRIPTION	:	Close a ADODB Connection
 PARAMETERS		:	-
 RETURN TYPE	:	bool
******************************************************************************/
bool Database::Close()
{
	try
	{
		// Closing Physical Connection
		m_Cnn->Close();				
	}
	CATCHERROR(m_Cnn,false)

	//m_ErrStr << "Success";

	return true;
}

/*****************************************************************************
 NAME			:	Execute
 DESCRIPTION	:	Executes the Query String
 PARAMETERS		:	_bstr_t Query String
 RETURN TYPE	:	bool
******************************************************************************/
bool Database::Execute(_bstr_t CmdStr)
{	
	CLogger CLogObj("Database::Execute(cmd)");

	ostringstream ossQuery;
	RecordsEffected.vt = VT_I4;
	
	try
	{

		//Returns the Records Effected
		m_Cnn->Execute(CmdStr,&RecordsEffected,0);
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		CLogObj.LogError(m_ErrStr.str().c_str());	
		
		ossQuery << "Query Failed : " << CmdStr;
		CLogObj.LogError(ossQuery.str().c_str());
		
		if(GetLastDBError() != DBConnectionState::DB_NOERROR)
		{
			CLogObj.LogError("Invalid Database Connection");	
			m_bDBReconnect = true;
		}
		
		return false;
	}
	
	//m_ErrStr << "Success";
	return true;
}

/*****************************************************************************
 NAME			:	ExecuteStoredProc
 DESCRIPTION	:	Executes the Query String
 PARAMETERS		:	_bstr_t Query String
 RETURN TYPE	:	bool
******************************************************************************/
bool Database::ExecuteStoredProc(_bstr_t StoredProc, std::vector<SStoredProcPrameters>&vtParameters)
{
	CLogger CLogObj("Database::ExecuteStoredProc");

	ostringstream ossQuery;
	RecordsEffected.vt = VT_I4;
	
	try
	{
		m_Cnn->Errors->Clear();
		
		HRESULT hr = m_CommPtr.CreateInstance( __uuidof( Command ) );

		if(FAILED(hr))
			throw _com_error(hr,0,false);	

		m_CommPtr->ActiveConnection = m_Cnn;

		m_CommPtr->CommandType = adCmdStoredProc;
		m_CommPtr->CommandText = StoredProc; 

		std::vector<_ParameterPtr>_ParameterPtrList;

		for(long lParam = 0; lParam < vtParameters.size(); ++lParam)
		{
			_ParameterPtr pParam = m_CommPtr->CreateParameter(
												_bstr_t( (char *) vtParameters[lParam].m_ParameterName.c_str() ),
												DataTypeEnum(vtParameters[lParam].m_iDataType),
												ParameterDirectionEnum(vtParameters[lParam].m_iParameterDirection),
												vtParameters[lParam].m_iDataTypeSize,
												vtParameters[lParam].m_ParameterValue);			

			m_CommPtr->Parameters->Append(pParam);
			_ParameterPtrList.push_back(pParam);
		}

		_variant_t vNull;
		vNull.vt = VT_ERROR;
		vNull.scode = DISP_E_PARAMNOTFOUND; 
		
		m_CommPtr->Execute( NULL, &vNull, adCmdStoredProc );
				
		for(long lParam = 0; lParam < _ParameterPtrList.size(); ++lParam )
			vtParameters[lParam].m_ParameterValue = _ParameterPtrList[lParam]->GetValue();			

		m_CommPtr.Detach()->Release();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		CLogObj.LogError(m_ErrStr.str().c_str());	
		
		ossQuery << "Query Failed : " << StoredProc;
		CLogObj.LogError(ossQuery.str().c_str());
		
		if(GetLastDBError() != DBConnectionState::DB_NOERROR)
		{
			CLogObj.LogError("Invalid Database Connection");	
			m_bDBReconnect = true;
		}
		
		return false;
	}
	
	return true;
}

/*****************************************************************************
 NAME			:	ExecuteStoredProc
 DESCRIPTION	:	Executes the Query String
 PARAMETERS		:	_bstr_t Query String
 RETURN TYPE	:	bool
******************************************************************************/
bool Database::ExecuteStoredProc(_bstr_t StoredProc, std::vector<SStoredProcPrameters>&vtParameters, 
								 std::vector<std::string>RecordFields, std::list< std::vector<_variant_t> >&listRecords_v_t)
{
	CLogger CLogObj("Database::ExecuteStoredProc");
	bool bRet = true;
	long lRows = 0;

	ostringstream ossQuery;
	RecordsEffected.vt = VT_I4;
	Table Tbl;
	std::vector<std::string>::const_iterator vtStrIter;
	std::vector<_variant_t> vtRecordValues;

	try
	{
		m_Cnn->Errors->Clear();
		
		HRESULT hr = m_CommPtr.CreateInstance( __uuidof( Command ) );

		if(FAILED(hr))
			throw _com_error(hr,0,false);	

		m_CommPtr->ActiveConnection = m_Cnn;

		m_CommPtr->CommandType = adCmdStoredProc;
		m_CommPtr->CommandText = StoredProc; 
		m_CommPtr->CommandTimeout = 50000;

		std::vector<_ParameterPtr>_ParameterPtrList;

		for(long lParam = 0; lParam < vtParameters.size(); ++lParam)
		{
			_ParameterPtr pParam = m_CommPtr->CreateParameter(
												_bstr_t( (char *) vtParameters[lParam].m_ParameterName.c_str() ),
												DataTypeEnum(vtParameters[lParam].m_iDataType),
												ParameterDirectionEnum(vtParameters[lParam].m_iParameterDirection),
												vtParameters[lParam].m_iDataTypeSize,
												vtParameters[lParam].m_ParameterValue);			

			m_CommPtr->Parameters->Append(pParam);
			_ParameterPtrList.push_back(pParam);
		}

		_variant_t vNull;
		vNull.vt = VT_ERROR;
		vNull.scode = DISP_E_PARAMNOTFOUND; 
		
		Tbl.m_Rec = m_CommPtr->Execute( NULL, &vNull, adCmdStoredProc );
		
		if(Tbl.ISEOF())
			bRet = false;
		else
		{
			while(!Tbl.ISEOF())
			{
				_variant_t Value_v_t;
				lRows++;

				for(vtStrIter = RecordFields.begin(); vtStrIter != RecordFields.end(); ++vtStrIter)
				{
					if(Tbl.Get((char *)(*vtStrIter).c_str(),Value_v_t) == false)
					{
						bRet = false;
						break;
					}
					else
						vtRecordValues.push_back(Value_v_t);
				}
				if( lRows % 100000 == 0 )
				{
					printf("\n %d",lRows);					
				}

				listRecords_v_t.push_back(vtRecordValues);
				vtRecordValues.clear();
				Tbl.MoveNext();
			}
		}

		Tbl.m_Rec->Close();

		for(long lParam = 0; lParam < _ParameterPtrList.size(); ++lParam )
			vtParameters[lParam].m_ParameterValue = _ParameterPtrList[lParam]->GetValue();			

		m_CommPtr.Detach()->Release();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		CLogObj.LogError(m_ErrStr.str().c_str());	
		
		ossQuery << "Query Failed : " << StoredProc;
		CLogObj.LogError(ossQuery.str().c_str());
		
		if(GetLastDBError() != DBConnectionState::DB_NOERROR)
		{
			CLogObj.LogError("Invalid Database Connection");	
			m_bDBReconnect = true;
		}
		
		bRet = false;
	}
	
	return bRet;
}

/*****************************************************************************
 NAME			:	Execute
 DESCRIPTION	:	Executes the Query String and Stroes the Records in Talbe Object
 PARAMETERS		:	_bstr_t Query String, Table Class Reference
 RETURN TYPE	:	bool
******************************************************************************/
bool Database::Execute(_bstr_t CmdStr, Table& Tbl)
{
	CLogger CLogObj("Database::Execute(CmdStr,Table)");

	RecPtr t_Rec=NULL;
	ostringstream ossQuery;


	try
	{
		m_Cnn->Errors->Clear();
	}
	catch(...)
	{
	}
	
	try
	{

		//Returns the Set of Records Effected
		t_Rec=m_Cnn->Execute(CmdStr,NULL,1);
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		CLogObj.LogError(m_ErrStr.str().c_str());	

		ossQuery << "Query Failed : " << CmdStr;
		CLogObj.LogError(ossQuery.str().c_str());

		if(GetLastDBError() != DBConnectionState::DB_NOERROR)
		{
			m_bDBReconnect = true;
			CLogObj.LogError("Invalid Database Connection");
		}

		return false;
	}

	//m_ErrStr << "Success";
	Tbl.m_Rec=t_Rec;
	
	return true;
}

bool Database::ExecuteRollBack()
{
	CLogger CLogObj("Database::ExecuteRollBack()");

	RecordsEffected.vt = VT_I4;
		
	try
	{
		m_Cnn->Errors->Clear ();
	}
	catch(...)
	{
	}

	try{

		m_Cnn->RollbackTrans();
	}
	catch(_com_error &e){

		ErrorHandler(e,m_ErrStr);	
		CLogObj.LogError(m_ErrStr.str().c_str());

		if(GetLastDBError() != DBConnectionState::DB_NOERROR)
		{
			CLogObj.LogError("Invalid Database Connection");	
			m_bDBReconnect = true;
		}
		
		return false;	
	}		

	//m_ErrStr << "Success";
	return true;
}

/*****************************************************************************
 NAME			:	ExecuteCommit
 DESCRIPTION	:	Commit Tranaction
 PARAMETERS		:	-
 RETURN TYPE	:	bool
******************************************************************************/
bool Database::ExecuteCommit()
{
	CLogger CLogObj("Database::ExecuteCommit()");

	RecordsEffected.vt = VT_I4;
		
	try
	{
		m_Cnn->Errors->Clear ();
	}
	catch(...)
	{
	}

	try
	{
		m_Cnn->CommitTrans();
	}
	catch(_com_error &e){

		ErrorHandler(e,m_ErrStr);	
		CLogObj.LogError(m_ErrStr.str().c_str());

		if(GetLastDBError() != DBConnectionState::DB_NOERROR)
		{
			CLogObj.LogError("Invalid Database Connection");	
			m_bDBReconnect = true;
		}
		
		return false;	
	}		

	//m_ErrStr << "Success";
	return true;
}

/*****************************************************************************
 NAME			:	ExecuteBegin
 DESCRIPTION	:	Begin Tranaction
 PARAMETERS		:	-
 RETURN TYPE	:	bool
******************************************************************************/
bool Database::ExecuteBegin()
{
	CLogger CLogObj("Database::ExecuteBegin()");

	RecordsEffected.vt = VT_I4;
	long IsTrans = 0;
		
	try
	{
		m_Cnn->Errors->Clear ();
	}
	catch(...)
	{
	}

	try
	{
		IsTrans = m_Cnn->BeginTrans();
	}
	catch(_com_error &e){

		ErrorHandler(e,m_ErrStr);	
		CLogObj.LogError(m_ErrStr.str().c_str());

		if(GetLastDBError() != DBConnectionState::DB_NOERROR)
		{
			CLogObj.LogError("Invalid Database Connection");	
			m_bDBReconnect = true;
		}
		
		return false;	
	}		

	//m_ErrStr << "Success";
	
	if(IsTrans == 0)
		return false;
	else
		return true;
}

/*****************************************************************************
 NAME			:	GetLastDBError
 DESCRIPTION	:	-
 PARAMETERS		:	-
 RETURN TYPE	:	DBConnectionState
******************************************************************************/
DBConnectionState Database::GetLastDBError()
{
	CLogger CLogObj("Database::GetLastDBError()");
	DBConnectionState dbConnectionState = DBConnectionState::DB_NOERROR;
	
	try
	{
		m_bDeadlock = false;

		int nErrorCnt = m_Cnn->Errors->GetCount ();
		if(nErrorCnt > 0)
		{
			for (int i = 0;i<nErrorCnt;i++)
			{
				long lErrorNum = m_Cnn->Errors->Item [i]->GetNumber ();
				CLogObj.LogError (m_Cnn->Errors->Item [i]->GetDescription ().operator const char*(), lErrorNum);
				switch(lErrorNum)
				{
				//case -2147467259:
				//	dbConnectionState = DBConnectionState::DB_DEADLOCK;
				//	m_bDeadlock = true;
				//	break;

				case 1205:
					dbConnectionState = DBConnectionState::DB_DEADLOCK;
					m_bDeadlock = true;
					break;
				
				case 1211:
					dbConnectionState = DBConnectionState::DB_DEADLOCK;
					m_bDeadlock = true;
					break;

				case 0x80004005:
					dbConnectionState = DBConnectionState::DB_INETDOWN;
					break;

				case 0xE7D://ADODB::ErrorValueEnum::adErrInvalidConnection:
					dbConnectionState = DBConnectionState::DB_CONNECTIONCLOSED;
					break;
				default:
					long lNativeErrorNum = m_Cnn->Errors->Item [i]->GetNativeError();
					if(lNativeErrorNum == 0x2 ||lNativeErrorNum == 0x274D ||lNativeErrorNum == 0x6D9)
					{
						dbConnectionState = DBConnectionState::DB_SERVERDOWN;
					}
					else if(lNativeErrorNum == 0x4814)
					{
						dbConnectionState = DBConnectionState::DB_CONNECTIONNOTTRUSTED;
					}
				}
			}
		}
	}
	catch(...)
	{
		dbConnectionState = DBConnectionState::DB_UNSPECIFIEDERROR;
	}

	try
	{
		m_Cnn->Errors->Clear ();
	}
	catch(...)
	{
	}

	return dbConnectionState;
}

/*****************************************************************************
 NAME			:	Table
 DESCRIPTION	:	Constructor
 PARAMETERS		:	-
 RETURN TYPE	:	-
******************************************************************************/
Table::Table()
{
	m_Rec=NULL;
}

/*****************************************************************************
 NAME			:	ISEOF
 DESCRIPTION	:	Checks for End of Record in Record Set
 PARAMETERS		:	-
 RETURN TYPE	:	int
******************************************************************************/
int Table::ISEOF()
{
	int rs;

	if(m_Rec==NULL)
	{
		//m_ErrStr << "Invalid Record";
		return -1;
	}
	try{
		rs=m_Rec->EndOfFile;
	}
	CATCHERROR(m_Rec,-2)

	//m_ErrStr << "Success";
	return rs;
}

/*****************************************************************************
 NAME			:	Get
 DESCRIPTION	:	Extracts the Field Value from a Given Record Corresponding to Partcular Field
 PARAMETERS		:	char *FieldName, char *Fieldvalue
 RETURN TYPE	:	bool
******************************************************************************/
bool Table::Get(char* FieldName, char* FieldValue)
{
	try
	{	
		_variant_t  vtValue;
		//Gets the value of the given Field from the Record Fetched
		vtValue = m_Rec->Fields->GetItem(FieldName)->GetValue();
		sprintf(FieldValue,"%s",(LPCSTR)((_bstr_t)vtValue.bstrVal));
	}
	CATCHERRGET

	//m_ErrStr << "Success";
	return true;
}

/*****************************************************************************
 NAME			:	Get
 DESCRIPTION	:	Extracts the Field Value from a Given Record Corresponding to Partcular Field
 PARAMETERS		:	char *FieldName, Unsigned long
 RETURN TYPE	:	bool
******************************************************************************/
bool Table::Get(char* FieldName,unsigned long& FieldValue)
{
	try
	{
		_variant_t  vtValue;
		//Gets the value of the given Field from the Record Fetched
		vtValue = m_Rec->Fields->GetItem(FieldName)->GetValue();
		FieldValue=vtValue.ulVal;
	}
	CATCHERRGET

	//m_ErrStr << "Success";
	return true;
}

/*****************************************************************************
 NAME			:	Get
 DESCRIPTION	:	Extracts the Field Value from a Given Record Corresponding to Partcular Field
 PARAMETERS		:	char *FieldName, Variant Fieldvalue
 RETURN TYPE	:	bool
******************************************************************************/
bool Table::Get(char* FieldName, _variant_t &FieldValue)
{
	try
	{	
		FieldValue = m_Rec->Fields->GetItem(FieldName)->GetValue();
	}
	CATCHERRGET

	//m_ErrStr << "Success";
	return true;
}

/*****************************************************************************
 NAME			:	MoveNext
 DESCRIPTION	:	Takes to the Next Record in Record Set
 PARAMETERS		:	-
 RETURN TYPE	:	HRESULT
******************************************************************************/
HRESULT Table::MoveNext()
{
	HRESULT hr;
	try
	{
		hr=m_Rec->MoveNext();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		//m_Rec=NULL;
		return -2;
	}
	//m_ErrStr << "Success";
	return hr;
}

/*****************************************************************************
 NAME			:	MovePrevious
 DESCRIPTION	:	Moves to the Previous Record in Record Set
 PARAMETERS		:	-
 RETURN TYPE	:	HRESULT
******************************************************************************/
HRESULT Table::MovePrevious()
{
	HRESULT hr;
	try
	{
		hr=m_Rec->MovePrevious();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		//m_Rec=NULL;
		return -2;
	}
	//m_ErrStr << "Success";
	return hr;
}

/*****************************************************************************
 NAME			:	MoveFirst
 DESCRIPTION	:	Moves to the first Record in Record Set
 PARAMETERS		:	-
 RETURN TYPE	:	HRESULT
******************************************************************************/
HRESULT Table::MoveFirst()
{
	HRESULT hr;
	try
	{
		hr=m_Rec->MoveFirst();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		//m_Rec=NULL;
		return -2;
	}
	//m_ErrStr << "Success";
	return hr;
}

/*****************************************************************************
 NAME			:	MoveLast
 DESCRIPTION	:	Moves to the Last Record in Record Set
 PARAMETERS		:	-
 RETURN TYPE	:	HRESULT
******************************************************************************/
HRESULT Table::MoveLast()
{
	HRESULT hr;
	try
	{
		hr=m_Rec->MoveLast();
	}
	catch(_com_error &e)
	{
		ErrorHandler(e,m_ErrStr);
		//m_Rec=NULL;
		return -2;
	}
	//m_ErrStr << "Success";
	return hr;
}

/*****************************************************************************
 NAME			:	Execute
 DESCRIPTION	:	Executes the Query String as a Transaction and Begin, Commits & Rollback Tranaction
 PARAMETERS		:	_bstr_t Query String, Transaction Flag
 RETURN TYPE	:	bool
******************************************************************************/
/*
bool Database::Execute(_bstr_t CmdStr, unsigned int TranFlag)
{

	CLogger CLogObj("Database::Execute(cmd,CommitFlag)");

	RecordsEffected.vt = VT_I4;
		ostringstream ossQuery;

		
	try
	{
		m_Cnn->Errors->Clear ();
	}
	catch(...)
	{
	}

	try{

		if(TranFlag == 0)
			m_Cnn->BeginTrans();

		m_Cnn->Execute(CmdStr,&RecordsEffected,NULL);
	}
	catch(_com_error &e){

		ErrorHandler(e,m_ErrStr);	
		CLogObj.LogError(m_ErrStr.str().c_str());

		ossQuery << "Query Failed " << CmdStr;
		CLogObj.LogError(ossQuery.str().c_str());

		m_Cnn->RollbackTrans();

		if(GetLastDBError() != DBConnectionState::DB_NOERROR)
		{
			CLogObj.LogError("Invalid Database Connection");	
			m_bDBReconnect = true;
		}
		
		return false;	
	}		

	if(TranFlag == 2)
	{
		if(RecordsEffected.lVal == 0)
			m_Cnn->RollbackTrans();		
		else
			m_Cnn->CommitTrans();
	}

	m_ErrStr << "Success";
	return true;
}
*/

/*****************************************************************************
 NAME			:	ExecuteRollback
 DESCRIPTION	:	Rollback Tranaction
 PARAMETERS		:	-
 RETURN TYPE	:	bool
******************************************************************************/

#endif
