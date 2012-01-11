/*****************************************************************************
// Revision History
// Database.h: This file contains declarations for Database Class 
// and Table Class. This class is used to Insert, Update and Fetch Records from Database
*******************************************************************************
// Date			Author			Version		Description			
// [mm/dd/yy]
*******************************************************************************
//								1.0.0.1		Created
// 09/22/06						1.0.1.2	    Code Reorganized
******************************************************************************/

#ifdef _WIN32

#ifndef __DATABASE__
#define __DATABASE__

#define CATCHERROR(ptr,a) catch(_com_error &e) { ErrorHandler(e,m_ErrStr);	ptr=NULL; return a;	}

#define CATCHERRGET	catch(_com_error &e) { ErrorHandler(e,m_ErrStr); m_ErrStr << m_ErrStr << "\n**For Field Name:" << FieldName; return 0; }


#include "stdafx.h"
#include <sstream>
using namespace std;

struct SStoredProcPrameters
{
	int m_iParameterDirection; //1 input & 2 output
	int m_iDataType;
	int m_iDataTypeSize;
	std::string m_ParameterName;
	_variant_t m_ParameterValue;
};


//#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
//              rename("EOF", "EndOfFile")

typedef _RecordsetPtr	RecPtr;
typedef _ConnectionPtr	CnnPtr; 
typedef _CommandPtr CommPtr; 

#define DBRETRYCONNECTTIME 20000
enum DBConnectionState
{
	DB_INETDOWN = 0,
	DB_SERVERDOWN,
	DB_CONNECTIONCLOSED,
	DB_CONNECTIONNOTTRUSTED,
	DB_UNSPECIFIEDERROR,
	DB_NOERROR,
	DB_DEADLOCK
};

class Database;
class Table;

//! Database Class Provides basic Database access functionality
class Database
{
public:
	//! ADODB Connection Object Pointer
	CnnPtr m_Cnn;

	//ADODB Command Object Pointer
	CommPtr m_CommPtr;

	ostringstream m_ErrStr;
	Database();

	//! Release the critical section resource
	~Database () {DeleteCriticalSection(&m_SyncCSection);}

	//! Function Create Instance of Connection Object & Opens Physical Connection,
	//! takes UserName, Password, Connection String as Parameter
	bool Open(char* UserName, char* Pwd,char* CnnStr);

	//! Executes the Query Specfied as parameter,
	//! Internally uses Connection Objects Executes function
	bool Execute(_bstr_t CmdStr);

	//! Executes the Query Specfied as parameter & Store the Records in Table Object,
	//! Internally uses Connection Objects Executes function
	bool Execute(_bstr_t CmdStr, Table& Tbl);
	
	//! Executes the Query Specfied as parameter with Transaction set,
	//! Internally uses Connection Objects Executes function
	//bool Execute(_bstr_t CmdStr, unsigned int TranFlag);
	
	bool ExecuteStoredProc(_bstr_t StoredProc, std::vector<SStoredProcPrameters>&vtParameters);

	bool ExecuteStoredProc(_bstr_t StoredProc, std::vector<SStoredProcPrameters>&vtParameters, 
								 std::vector<std::string>RecordFields, std::list< std::vector<_variant_t> >&listRecords_v_t);

	bool ExecuteRollBack();
	bool ExecuteCommit();
	bool ExecuteBegin();

	//! Function Closes the ADODB Connection
	bool Close();
	
	//! Returns the Record Effected
	long GetRecordsEffected(){ return RecordsEffected.lVal; }

	void GetErrorErrStr(char* ErrStr);

	bool GetDBReconnectFlag(){return m_bDBReconnect;}

	bool IsDeadLock() { return m_bDeadlock; }

private:

	VARIANT RecordsEffected;

	DBConnectionState GetLastDBError();
	bool m_bDBReconnect;
	bool m_bDeadlock;
	CRITICAL_SECTION m_SyncCSection; 
};


//! Table Class Provides basic Functionality for accessing Record Set
class Table
{
public:

	//! ADODB RecordSet Pointer
	RecPtr m_Rec;
	ostringstream m_ErrStr;
	Table();

	HRESULT CloseRecordSet() 
	{ 		
		if(m_Rec) 
		{
			HRESULT hr = m_Rec->Close();
			return hr;
		}
	}

	//! Checks for End of Record Set
	int ISEOF();

	//! Moves Record Set Pointer to First Record from Current Position
	HRESULT MoveFirst();
	
	//! Moves Record Set Pointer to Next Record from Current Position
	HRESULT MoveNext();

	//! Moves Record Set Pointer to Previous Record from Current Position
	HRESULT MovePrevious();

	//! Moves Record Set Pointer to Last Record from Current Position
	HRESULT MoveLast();

	//! Returns the Char Value of Char Field 
	bool Get(char* FieldName, char* FieldValue);

	//! Returns the Unsigned Long Value of Char Field 
	bool Get(char* FieldName,unsigned long& FieldValue);

	//! Returns the Variant Value of Char Field 
	bool Get(char* FieldName, _variant_t &FieldValue);
};

#endif

#endif
