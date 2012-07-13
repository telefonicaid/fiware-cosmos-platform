/*****************************************************************************
// Revision History
// DatabaseManager.h: This file contains declarations for DatabaseManager Class. 
// This class is used as Raper Class to Insert, Update and Fetch Records from Database
*******************************************************************************
// Date			Author			Version		Description			
// [mm/dd/yy]
*******************************************************************************
//								1.0.0.1		Created
// 09/22/06						1.0.1.2	    Code Reorganized
******************************************************************************/
#ifdef _WIN32

#ifndef __DATABASEMANAGER__
#define __DATABASEMANAGER__

#include "stdafx.h"
#include "Database.h"




//DatabaseManager class to perform databse operations
class  DatabaseManager
{
public:

	DatabaseManager(char *);
		
	//! Returns the Record Fetched by Executing the Query
	bool GetRecord(const std::vector<std::string> &, std::vector<_variant_t> &, _bstr_t);	

	//! Returns all the Records Fetched by Executing the Query
	bool GetRecords(const std::vector<std::string> &, std::list<std::vector<_variant_t> >&, _bstr_t);	
	
	//! Update the Record in Database Table
	bool UpdateRecord(_bstr_t Query);

	//! Update the Record in Database Table
	bool UpdateRecord(_bstr_t Query, bool &bDeadLock);

	//! Insert new Record in Database Table
	bool InsertRecord(_bstr_t Query);

	//! Delete Record in Database Table
	bool DeleteRecord(_bstr_t Query);

	//! returns the avilable or effected records by the query
	int GetRecordsAvilable(_bstr_t Query);

	//!
	bool ExecuteStoredProcedure(_bstr_t StoredProc, std::vector<SStoredProcPrameters> &vtArguments);

	
	
	
	bool ExecuteStoredProcedure(_bstr_t StoredProc, std::vector<SStoredProcPrameters> &vtArguments,
		std::vector<std::string>vtFields, std::list< std::vector<_variant_t> >&vtRecords);

	//! Begins database transaction
	bool BeginTransaction();

	//! Commit database transaction
	bool CommitTransaction();

	//! Rollback database transaction
	bool RollBackTransaction();

	//! Singleton implementation - destroyes the DatabaseManager object
	static bool ReleseDatabaseManagerObject();

	//! Destructor - Closes the Database Connection
	virtual ~DatabaseManager()
	{ db.Close(); }
	
private:
	
	char *ErrStr;
	Database db;
	HANDLE hRecordLock;

	std::string m_CnnStr;
};
#endif //__DATABASEMANAGER__

#endif
