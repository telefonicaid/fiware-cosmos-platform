/*****************************************************************************
// Revision History
// DatabaseManager.cpp: This file contains implementation for DatabaseManager
// Class. This class is used as Raper Class to Insert, Update and Fetch Records
// from Database.
*******************************************************************************
// Date			Author			Version		Description			
// [mm/dd/yy]
*******************************************************************************
//								1.0.0.1		Created


******************************************************************************/
#include "stdafx.h"

#ifdef _WIN32
#include "DatabaseMang.h"
#include "Logger.h"

/*****************************************************************************
 NAME			:	DatabaseManager
 DESCRIPTION	:	-
 PARAMETERS		:	char *CnnStr
 RETURN TYPE	:	-
******************************************************************************/
DatabaseManager::DatabaseManager(char *CnnStr)
{
	CLogger CLogObj("DatabaseManager::DatabaseManager()");
	try
	{
	if(!db.Open("","",CnnStr))
	{	
		//db.GetErrorErrStr(ErrStr);
			throw std::exception("Open database connection.");
	}
	m_CnnStr = CnnStr;
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		throw e;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown error");
		throw std::exception("Unknown error");
	}
}

/*****************************************************************************
 NAME			:	InsertRecord
 DESCRIPTION	:	Inserts the Record in Database
 PARAMETERS		:	Query String
 RETURN TYPE	:	bool
******************************************************************************/
bool DatabaseManager::InsertRecord(_bstr_t Query)
{
	CLogger CLogObj("DatabaseManager::InsertRecord()");	
	
	bool bRet = true;

	try
	{
		while(!db.Execute(Query))
		{	
			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError("\nQuery Execution Failure");
				bRet = false;

				/*if(db.IsDeadLock())
				{
					ostringstream ossErr;
					ossErr << "Deadlock occurred. re-running query - " << Query.GetBSTR();
					CLogObj.LogError(ossErr.str().c_str());
					IsDeadLock = true;
				}*/

				break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
		
		if(db.GetRecordsEffected() == 0)
			bRet = false;
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		bRet = false;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
		bRet = false;
	}

	return bRet;
}

/*****************************************************************************
 NAME			:	UpdateRecord
 DESCRIPTION	:	Updates the Record in Database
 PARAMETERS		:	Query String
 RETURN TYPE	:	bool
******************************************************************************/
bool DatabaseManager::UpdateRecord(_bstr_t Query, bool &IsDeadLock)
{
	CLogger CLogObj("DatabaseManager::UpdateRecord()");	

	bool bRet = true;
	
	try{
		while(!db.Execute(Query))
		{	
			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError("\nQuery Execution Failure");
				bRet = false;

				if(db.IsDeadLock())
				{
					ostringstream ossErr;
					ossErr << "Deadlock occurred. re-running query - " << Query.GetBSTR();
					CLogObj.LogError(ossErr.str().c_str());
					IsDeadLock = true;				
				}
				
				break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
		
		if(db.GetRecordsEffected() == 0)
			bRet = false;
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		bRet = false;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
		bRet = false;
	}

	return bRet;
}

/*****************************************************************************
 NAME			:	UpdateRecord
 DESCRIPTION	:	Updates the Record in Database
 PARAMETERS		:	Query String
 RETURN TYPE	:	bool
******************************************************************************/
bool DatabaseManager::UpdateRecord(_bstr_t Query)
{
	CLogger CLogObj("DatabaseManager::UpdateRecord()");	

	bool bRet = true;
	
	try{
		while(!db.Execute(Query))
		{	
			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError("\nQuery Execution Failure");
				bRet = false;

				/*if(db.IsDeadLock())
				{
					ostringstream ossErr;
					ossErr << "Deadlock occurred. re-running query - " << Query.GetBSTR();
					CLogObj.LogError(ossErr.str().c_str());
					IsDeadLock = true;				
				}*/
				
				break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
		
		if(db.GetRecordsEffected() == 0)
			bRet = false;
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		bRet = false;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
		bRet = false;
	}

	return bRet;
}

/*****************************************************************************
 NAME			:	GetRecord
 DESCRIPTION	:	Get Record from Table Corresponding to Query String
 PARAMETERS		:	Record Fields, Vector to Store the Record fetched, Query String
 RETURN TYPE	:	bool
******************************************************************************/
//get Record having status new from that table
bool DatabaseManager::GetRecord(const std::vector<std::string> &RecordFields, 
								std::vector<_variant_t> &RecordValues, _bstr_t QueryStr)
{	
	CLogger CLogObj("DatabaseManager::GetRecord()");

	Table tbl;
	std::vector<std::string>::const_iterator vtStrIter;

	bool bRet = true;

	while(!db.Execute(QueryStr,tbl))
	{		
		if(db.GetDBReconnectFlag () == false)
		{
			CLogObj.LogError(QueryStr.operator const char *());
			CLogObj.LogError("\nQuery Execution Failure");

			bRet = false;

			/*if(db.IsDeadLock())
			{
				ostringstream ossErr;
				ossErr << "Deadlock occurred. re-running query - " << QueryStr.GetBSTR();
				CLogObj.LogError(ossErr.str().c_str());
				IsDeadLock = true;
			}*/

			break;
		}
		else
		{
			db.Open("","",(char *)m_CnnStr.c_str());
		}
	}
	
	if(!tbl.ISEOF())
	{
		tbl.MoveFirst();
		
		_variant_t Value_v_t;

		for(vtStrIter = RecordFields.begin(); vtStrIter != RecordFields.end(); ++vtStrIter)
		{
			if(tbl.Get((char *)(*vtStrIter).c_str(),Value_v_t) == false)
			{	
				//tbl.GetErrorErrStr(ErrStr);
				//printf("\n%s\n",ErrStr);	
				bRet = false;
				break;
			}
			else
				RecordValues.push_back(Value_v_t);
		}
	}
	else 
		bRet = false;
	
	return bRet;
}

/*****************************************************************************
 NAME			:	GetRecords
 DESCRIPTION	:	Get all the Records fron Table Corresponding to Query String
 PARAMETERS		:	Record Fields, List of Records to be fetched, Query String
 RETURN TYPE	:	bool
******************************************************************************/
//get Record having status new from that table
bool DatabaseManager::GetRecords(const std::vector<std::string> &RecordFields, 
								 std::list<std::vector<_variant_t> >&listRecords_v_t, _bstr_t QueryStr)
{	
	CLogger CLogObj("DatabaseManager::GetRecord()");

	Table tbl;

	std::vector<std::string>::const_iterator vtStrIter;
	std::vector<_variant_t> vtRecordValues;

	bool bRet = true;

	try{
		while(!db.Execute(QueryStr,tbl))
		{	
			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError(QueryStr.operator const char *());
				CLogObj.LogError("\nQuery Execution Failure");

				bRet = false;

			/*	if(db.IsDeadLock())
				{
					ostringstream ossErr;
					ossErr << "Deadlock occurred. re-running query - " << QueryStr.GetBSTR();
					CLogObj.LogError(ossErr.str().c_str());
					IsDeadLock = true;
				}*/
			
				break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
		
		if(tbl.ISEOF())
			bRet = false;
		else
		{
			while(!tbl.ISEOF())
			{
				_variant_t Value_v_t;

				for(vtStrIter = RecordFields.begin(); vtStrIter != RecordFields.end(); ++vtStrIter)
				{
					if(tbl.Get((char *)(*vtStrIter).c_str(),Value_v_t) == false)
					{
						bRet = false;
						break;
					}
					else
						vtRecordValues.push_back(Value_v_t);
				}

				listRecords_v_t.push_back(vtRecordValues);
				vtRecordValues.clear();
				tbl.MoveNext();
			}
		}
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		bRet = false;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
		bRet = false;
	}

	return bRet;
}

/*****************************************************************************
 NAME			:	RollBackTransaction
 DESCRIPTION	:	-
 PARAMETERS		:	-
 RETURN TYPE	:	bool
******************************************************************************/
bool DatabaseManager::RollBackTransaction()
{
	CLogger CLogObj("DatabaseManager::RollBackTransaction()");

	bool bRet = true;

	try
	{
		while(!db.ExecuteRollBack())
		{	
			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError("\nQuery Execution Failure");
				
				bRet = false;

				//if(db.IsDeadLock())
				//	continue;
				//else
					break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		bRet = false;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
		bRet = false;
	}

	return bRet;
}

/*****************************************************************************
 NAME			:	CommitTransaction
 DESCRIPTION	:	-
 PARAMETERS		:	-
 RETURN TYPE	:	bool
******************************************************************************/
bool DatabaseManager::CommitTransaction()
{
	CLogger CLogObj("DatabaseManager::CommitTransaction()");

	bool bRet = true;

	try
	{
		while(!db.ExecuteCommit())
		{	
			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError("\nQuery Execution Failure");
				
				bRet = false;
				
				//if(db.IsDeadLock())
				//	continue;
				//else
					break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		bRet = false;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
		bRet = false;
	}

	return bRet;
}

/*****************************************************************************
 NAME			:	BeginTransaction
 DESCRIPTION	:	-
 PARAMETERS		:	-
 RETURN TYPE	:	bool
******************************************************************************/
bool DatabaseManager::BeginTransaction()
{
	CLogger CLogObj("DatabaseManager::BeginTransaction()");

	bool bRet = true;

	try
	{
		while(!db.ExecuteBegin())
		{	
			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError("\nQuery Execution Failure");
				
				bRet = false;

				break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		bRet = false;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
		bRet = false;
	}

	return bRet;
}

/*****************************************************************************
 NAME			:	GetRecordsAvilable
 DESCRIPTION	:	-
 PARAMETERS		:	Query String
 RETURN TYPE	:	unsigned int
******************************************************************************/
int DatabaseManager::GetRecordsAvilable(_bstr_t Query)
{
	CLogger CLogObj("DatabaseManager::GetRecordsEffected()");
	int n_Ret = -1;

	try
	{
		unsigned int uiRetryCount = 0;
		Table tbl;

		while(!db.Execute(Query, tbl))
		{	
			if(uiRetryCount == 2)
				throw std::exception("Query Execution failed due to Network Disconnection after 2 Retries.");
			
			++uiRetryCount;

			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError("\nQuery Execution Failure");
						
				/*if(db.IsDeadLock())
					IsDeadLock = true;*/
				
				break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
		
		if(tbl.ISEOF() == -1)
			n_Ret = 0;
		else
		{
			n_Ret = 0;
			while(!tbl.ISEOF())
			{
				++n_Ret;
				tbl.MoveNext();
			}
		}
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		n_Ret = -1;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error");
		n_Ret = -1;
	}

	return n_Ret;
}


/*****************************************************************************
 NAME			:	DeleteRecord
 DESCRIPTION	:	-
 PARAMETERS		:	Query String
 RETURN TYPE	:	bool
******************************************************************************/

bool DatabaseManager::DeleteRecord(_bstr_t Query)
{
	CLogger CLogObj("DatabaseManager::DeleteRecord()");	
	bool bRet = true;

	try
	{
		unsigned int uiRetryCount = 0;

		while(!db.Execute(Query))
		{	
			//if(uiRetryCount == 2)
			//	throw std::exception("Query Execution failed due to Network Disconnection after 2 Retries.");
			
			//++uiRetryCount;

			if(db.GetDBReconnectFlag () == false)
			{
				CLogObj.LogError("\nQuery Execution Failure");
				bRet = false;

				break;
			}
			else
			{
				db.Open("","",(char *)m_CnnStr.c_str());
			}
		}
	}
	catch(std::exception &e)
	{
		CLogObj.LogError(e.what());
		bRet = false;
	}
	catch(...)
	{
		CLogObj.LogError("Unknown Error in DatabaseManager::DeleteRecord");
		bRet = false;
	}

	return bRet;
}


bool DatabaseManager::ExecuteStoredProcedure(_bstr_t StoredProc, std::vector<SStoredProcPrameters> &vtArguments)
{
	CLogger CLogObj("DatabaseManager::ExecuteStoredProcedure()");

	Table tbl;
	std::vector<std::string>::const_iterator vtStrIter;

	bool bRet = true;

	while(!db.ExecuteStoredProc(StoredProc, vtArguments))
	{		
		if(db.GetDBReconnectFlag () == false)
		{
			bRet = false;
			break;
		}
		else
		{
			db.Open("","",(char *)m_CnnStr.c_str());
		}
	}
	
	return bRet;
}

bool DatabaseManager::ExecuteStoredProcedure(_bstr_t StoredProc, std::vector<SStoredProcPrameters> &vtArguments,
											 std::vector<std::string>RecordFields, std::list< std::vector<_variant_t> >&listRecords_v_t)
{
	CLogger CLogObj("DatabaseManager::ExecuteStoredProcedure()");

	bool bRet = true;

	while(!db.ExecuteStoredProc(StoredProc, vtArguments, RecordFields, listRecords_v_t ))
	{		
		if(db.GetDBReconnectFlag () == false)
		{
			bRet = false;
			break;
		}
		else
		{
			db.Open("","",(char *)m_CnnStr.c_str());
		}
	}
	
	return bRet;
}

#endif
