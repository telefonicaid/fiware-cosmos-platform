#ifndef _WIN32

#ifndef _Column_h_DSC_
#define _Column_h_DSC_

#include <zlib.h>
#include <string.h>
//#include <io.h>
#include <vector>
#include <string>
#include <sstream>
#include <pthread.h>
#include <sys/time.h>
using namespace std;
 
#define COLUMNS				CRecord
#define BUFFER_SIZE			1024*256
#define MAX_NO_OF_BUFFER	8
#ifndef ERRORINFO
//#define ERRORINFO(MESSEGE)  MESSEGE " :: FileName=(" __FILE__ ") FunctionName=(" __FUNCTION__ "()) LineNumber=" "."
#define ERRORINFO(MESSEGE)  MESSEGE " :: FileName= " __FILE__ " FunctionName= "
#endif

const char cRecordSeparator	=	'\n';
//const char cColumnSeparator	=	'\t';

class ColException : public exception
{
private:
 	string m_strMsg;

public:
	virtual ~ColException() throw()
	{
	}
	
	ColException () throw() : exception()
	{
		m_strMsg = "";
	}
    
	ColException (const ColException &obj) throw()
	{
		m_strMsg = obj.m_strMsg;
	}

	ColException (const string &strDescription) throw() : exception()
	{
		m_strMsg = strDescription;
	}

	virtual const char* what() const throw()
	{
		return m_strMsg.c_str();
	}

};


template<class T, void*(T::*mem_fn)(void*)>
void* thunk(void* p)
{
	(static_cast<T*>(p)->*mem_fn)(p);
	return 0;
}

//----Start----CGzFileReader-----------------------------------------------
class CGzFileReader
{
private:
	gzFile				m_gzFile;
	bool				m_bIsEofOccurred;
	bool				m_bTerminateThread;
	pair<char*,long>	m_BufferCircularQueue[MAX_NO_OF_BUFFER];
	long				m_lFrontBuffer;
	long				m_lRearBuffer;
	//CRITICAL_SECTION	m_objCriticalSection;
	pthread_mutex_t m_objCriticalSection ;
	pthread_t		m_hThread;
//	HANDLE				m_hBufferAvailableEvent;
	pthread_cond_t m_hBufferAvailableEvent ;
//	HANDLE				m_hReadFileEvent;
	pthread_cond_t m_hReadFileEvent ;
	string				m_strError;

private:
	void* ThreadFunGzFileReader(void *ThisPtr)
	{
		CGzFileReader	*CGzFileReaderPtr	=	(CGzFileReader*)ThisPtr;
		try
		{
			CGzFileReaderPtr->Read();
		}
		catch(ColException &e)
		{
			((CGzFileReader*)ThisPtr)->m_strError	=	e.what();
		}
		catch(...)
		{
			((CGzFileReader*)ThisPtr)->m_strError	=	"Unknown Exception in ThreadFunGzFileReader of GzFileReader";
		}
		return 0;
	}

	void Read(void)
	{
		while(!m_bTerminateThread)
		{
			pthread_mutex_lock(&m_objCriticalSection);

			if((m_lRearBuffer+1-m_lFrontBuffer)%MAX_NO_OF_BUFFER == 0)
			{
				/*if(!ResetEvent(m_hReadFileEvent))
				{
					pthread_mutex_unlock(&m_objCriticalSection);
					throw exception(ERRORINFO("m_hReadFileEvent :: ResetEvent Failed"));
				}*/
				//pthread_mutex_unlock(&m_objCriticalSection);
				//WaitForSingleObject(m_hReadFileEvent, INFINITE);
				struct timeval time;
				struct timespec timeout;
				gettimeofday(&time, NULL);
				timeout.tv_sec = time.tv_sec + 1;
				timeout.tv_nsec = time.tv_usec * 1000;
				pthread_cond_timedwait(&m_hReadFileEvent,&m_objCriticalSection,&timeout);
				pthread_mutex_unlock(&m_objCriticalSection);
			}
			else
			{
				long lPrevRearBuffer	=	m_lRearBuffer;
				long lPrevFrontBuffer	=	m_lFrontBuffer;
				if(-1 == m_lRearBuffer)
					m_lRearBuffer	=	m_lFrontBuffer	=	0;
				else if(MAX_NO_OF_BUFFER-1 == m_lRearBuffer)
					m_lRearBuffer	=	0;
				else
					++m_lRearBuffer;
				m_BufferCircularQueue[m_lRearBuffer].first	=	new char[BUFFER_SIZE+1];
				if(NULL == m_BufferCircularQueue[m_lRearBuffer].first)
				{
					pthread_mutex_unlock(&m_objCriticalSection);
					throw ColException(ERRORINFO("m_BufferCircularQueue :: Memory allocation failure") + string( __func__ ));
				}
				m_BufferCircularQueue[m_lRearBuffer].second	=	gzread(m_gzFile, m_BufferCircularQueue[m_lRearBuffer].first, BUFFER_SIZE);
				if(m_BufferCircularQueue[m_lRearBuffer].second <= 0)
				{
					m_bIsEofOccurred	=	true;
					m_bTerminateThread	=	true;
					if(m_BufferCircularQueue[m_lRearBuffer].first != NULL)
						delete []m_BufferCircularQueue[m_lRearBuffer].first;
					m_lRearBuffer	=	lPrevRearBuffer;
					m_lFrontBuffer	=	lPrevFrontBuffer;

				}
				/*if(!SetEvent(m_hBufferAvailableEvent))
				{
					pthread_mutex_unlock(&m_objCriticalSection);
					throw exception(ERRORINFO("m_hBufferAvailableEvent :: SetEvent Failed"));
				}*/
				pthread_cond_signal(&m_hBufferAvailableEvent);
				pthread_mutex_unlock(&m_objCriticalSection);
			}
		}
		/*if(!SetEvent(m_hThreadTerminated))
		{
			pthread_mutex_unlock(&m_objCriticalSection);
			throw exception(ERRORINFO("m_hThreadTerminated :: SetEvent Failed"));
		}*/
		//pthread_cond_signal(&m_hThreadTerminated);
	}

public:
	CGzFileReader(void)
	{
		//InitializeCriticalSection(&m_objCriticalSection);
		pthread_mutex_init ( &m_objCriticalSection , NULL ) ;
		m_strError			=	"";
		m_gzFile			=	NULL;
		m_bIsEofOccurred	=	false;
		m_bTerminateThread	=	false;
		m_lFrontBuffer		=	-1;
		m_lRearBuffer		=	-1;
		for(long lCount=0; lCount<MAX_NO_OF_BUFFER; ++lCount)
		{
			m_BufferCircularQueue[lCount].first		=	NULL;
			m_BufferCircularQueue[lCount].second	=	-1;
		}

		//m_hBufferAvailableEvent	=	CreateEvent(NULL, TRUE, FALSE, NULL);
		pthread_cond_init ( &m_hBufferAvailableEvent , NULL ) ;
		
		/*
		if(NULL == m_hBufferAvailableEvent)
			throw exception(ERRORINFO("m_hBufferAvailableEvent :: CreateEvent failed"));
		*/	

		pthread_cond_init ( &m_hReadFileEvent , NULL ) ;
		/*
		m_hReadFileEvent	=	CreateEvent(NULL, TRUE, TRUE, NULL);
		if(NULL == m_hReadFileEvent)
			throw exception(ERRORINFO("m_hReadFileEvent :: CreateEvent failed"));
		*/	
		
		//pthread_cond_init ( &m_hThreadTerminated , NULL ) ;
		/*
		m_hThreadTerminated	=	CreateEvent(NULL, TRUE, FALSE, NULL);
		if(NULL == m_hThreadTerminated)
			throw exception(ERRORINFO("m_hThreadTerminated :: CreateEvent failed"));
		*/
		
	}

	~CGzFileReader(void)
	{
		pthread_mutex_lock(&m_objCriticalSection);
		m_bTerminateThread	=	true;
		pthread_mutex_unlock(&m_objCriticalSection);
		//WaitForSingleObject(m_hThreadTerminated, INFINITE);

		pthread_join(m_hThread,NULL);
		//pthread_cond_wait(&m_hThreadTerminated,&m_objCriticalSection);

		//pthread_mutex_lock(&m_objCriticalSection);
		if(-1 != m_lFrontBuffer)
		{
			long lCount	=	m_lFrontBuffer;
			ReleaseBuffer(m_BufferCircularQueue[lCount].first);
			while(lCount != m_lRearBuffer)
			{
				if(MAX_NO_OF_BUFFER-1 == lCount)
					lCount	=	0;
				else
					++lCount;
				ReleaseBuffer(m_BufferCircularQueue[lCount].first);
			}
		}

		if(NULL != m_gzFile)
			gzclose(m_gzFile);

		//CloseHandle(m_hBufferAvailableEvent);
		pthread_cond_destroy(&m_hBufferAvailableEvent);
		//CloseHandle(m_hReadFileEvent);
		pthread_cond_destroy(&m_hReadFileEvent);
		// doubt
		//pthread_cond_destroy(&m_hThreadTerminated);


		pthread_mutex_destroy(&m_objCriticalSection);
		if(m_strError.size() > 0)
			throw ColException(m_strError.c_str() + string( __func__ ));
	}

	bool OpenGzFile(const char *pszFileName)
	{
		if(0 != access(pszFileName, 0))
			return false;

		
		pthread_mutex_lock(&m_objCriticalSection);
		m_gzFile	=	gzopen(pszFileName, "rb9");
		if(NULL == m_gzFile)
		{
			pthread_mutex_unlock(&m_objCriticalSection);
			return false;
		}
		pthread_mutex_unlock(&m_objCriticalSection);

		//m_hThread	=	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunGzFileReader, this, 0, NULL);
		//if(pthread_create(&m_hThread,NULL,ThreadFunGzFileReader,(void*)this)!=0)
		if(pthread_create(&m_hThread,NULL,thunk<CGzFileReader, &CGzFileReader::ThreadFunGzFileReader>,(void*)this)!=0)
			throw ColException(ERRORINFO("Thread Creation Failed") + string( __func__ ));

		return true;
	}

	long GetBuffer(char*& szBuffer)
	{
		long bRetVal	=	-1;
		pthread_mutex_lock(&m_objCriticalSection);

		while(-1 == m_lFrontBuffer)//m_BufferCircularQueue is empty.
		{
			if(m_bIsEofOccurred)
			{
				pthread_mutex_unlock(&m_objCriticalSection);
				return -1;
			}
			//pthread_mutex_unlock(&m_objCriticalSection);
			//WaitForSingleObject(m_hBufferAvailableEvent, 100);
			pthread_cond_wait(&m_hBufferAvailableEvent,&m_objCriticalSection);
		}
		{
			szBuffer	=	m_BufferCircularQueue[m_lFrontBuffer].first;
			bRetVal		=	m_BufferCircularQueue[m_lFrontBuffer].second;
			m_BufferCircularQueue[m_lFrontBuffer].first		=	NULL;
			m_BufferCircularQueue[m_lFrontBuffer].second	=	-1;
			if(m_lFrontBuffer == m_lRearBuffer)
			{
				m_lFrontBuffer	=	m_lRearBuffer	=	-1;
				/*if(!ResetEvent(m_hBufferAvailableEvent))
				{
					pthread_mutex_unlock(&m_objCriticalSection);
					throw exception(ERRORINFO("m_hBufferAvailableEvent :: ResetEvent Failed"));
				}*/

			}
			else if(MAX_NO_OF_BUFFER-1 == m_lFrontBuffer)
				m_lFrontBuffer	=	0;
			else
				++m_lFrontBuffer;
			/*if(!SetEvent(m_hReadFileEvent))
			{
				pthread_mutex_unlock(&m_objCriticalSection);
				throw exception(ERRORINFO("m_hReadFileEvent :: SetEvent Failed"));
			}*/
			pthread_cond_signal(&m_hReadFileEvent);
		}

		pthread_mutex_unlock(&m_objCriticalSection);

		return bRetVal;
	}

	void ReleaseBuffer(char*& szBuffer)
	{
		if(NULL != szBuffer)
			delete []szBuffer;
		szBuffer	=	NULL;
	}
};
//----End----CGzFileReader-------------------------------------------------

//----Start----CRecord-----------------------------------------------------
class CRecord
{
	friend class ColExtractor;
private:
	char			*m_pszCompleteRow;
	char			**m_pColumnArray;
	unsigned int	m_uiRowSize;
	unsigned int	m_uiTotalColumns;

private:
	void FreeAll()
	{
		m_uiRowSize			=	0;
		m_uiTotalColumns	=	0;
		if(m_pszCompleteRow != NULL)
			delete []m_pszCompleteRow;
		if(m_pColumnArray != NULL)
			delete []m_pColumnArray;
	}

	void Parse(const char* pszString, char cColumnSep)
	{
		FreeAll();
		const char *pszTemp	=	pszString;
		while(*pszString)
		{
			++m_uiRowSize;
			if(cColumnSep == *pszString)
				++m_uiTotalColumns;
			++pszString;
		}
		++m_uiTotalColumns;//For last column.
		++m_uiRowSize;//For counting '\0'.

		m_pszCompleteRow	=	new char[m_uiRowSize];
		if(NULL == m_pszCompleteRow)
			throw ColException(ERRORINFO("Memory allocation failure") + string( __func__ ));
		m_pColumnArray		=	new char*[m_uiTotalColumns];
		if(NULL == m_pColumnArray)
			throw ColException(ERRORINFO("Memory allocation failure") + string( __func__ ));
		strcpy(m_pszCompleteRow, pszTemp);

		m_pColumnArray[0]	=	m_pszCompleteRow;
		unsigned int uiColIndex	=	1;
		for(unsigned int uiIndex=0; m_pszCompleteRow[uiIndex]; ++uiIndex)
		{
			if(cColumnSep == m_pszCompleteRow[uiIndex])
			{
				m_pszCompleteRow[uiIndex]	=	'\0';
				m_pColumnArray[uiColIndex]	=	m_pszCompleteRow+uiIndex+1;
				++uiColIndex;
			}
		}
	}

public:
	CRecord()
	{
		m_pszCompleteRow	=	NULL;
		m_pColumnArray		=	NULL;
		m_uiRowSize			=	0;
		m_uiTotalColumns	=	0;
	}

	CRecord(const CRecord &objRecord)
	{
		m_uiRowSize			=	objRecord.m_uiRowSize;
		m_uiTotalColumns	=	objRecord.m_uiTotalColumns;
		m_pszCompleteRow	=	new char[m_uiRowSize];
		if(NULL == m_pszCompleteRow)
			throw ColException(ERRORINFO("Memory allocation failure") + string( __func__ ));
		m_pColumnArray		=	new char*[m_uiTotalColumns];
		if(NULL == m_pColumnArray)
			throw ColException(ERRORINFO("Memory allocation failure") + string( __func__ ));
		memcpy(m_pszCompleteRow, objRecord.m_pszCompleteRow, m_uiRowSize);
		if(m_uiTotalColumns > 0)
		{
			m_pColumnArray[0]	=	m_pszCompleteRow;
			for(unsigned int uiCount=1; uiCount<m_uiTotalColumns; ++uiCount)
				m_pColumnArray[uiCount]	=	m_pszCompleteRow + (objRecord.m_pColumnArray[uiCount] - objRecord.m_pszCompleteRow);
		}
	}

	~CRecord()
	{
		if(m_pszCompleteRow != NULL)
			delete []m_pszCompleteRow;
		if(m_pColumnArray != NULL)
			delete []m_pColumnArray;
	}

	void Copy(vector<string> &vecString)
	{
		vecString.clear();
		for(unsigned int uiIndex=0; uiIndex<m_uiTotalColumns; ++uiIndex)
			vecString.push_back(m_pColumnArray[uiIndex]);
	}

	void operator=(const CRecord &objRecord)
	{
		FreeAll();
		m_uiRowSize			=	objRecord.m_uiRowSize;
		m_uiTotalColumns	=	objRecord.m_uiTotalColumns;
		m_pszCompleteRow	=	new char[m_uiRowSize];
		if(NULL == m_pszCompleteRow)
			throw ColException(ERRORINFO("Memory allocation failure") + string( __func__ ));
		m_pColumnArray		=	new char*[m_uiTotalColumns];
		if(NULL == m_pColumnArray)
			throw ColException(ERRORINFO("Memory allocation failure") + string( __func__ ));
		memcpy(m_pszCompleteRow, objRecord.m_pszCompleteRow, m_uiRowSize);
		if(m_uiTotalColumns > 0)
		{
			m_pColumnArray[0]	=	m_pszCompleteRow;
			for(unsigned int uiCount=1; uiCount<m_uiTotalColumns; ++uiCount)
				m_pColumnArray[uiCount]	=	m_pszCompleteRow + (objRecord.m_pColumnArray[uiCount] - objRecord.m_pszCompleteRow);
		}
	}

	const char* operator[](unsigned int uiIndex) const
	{
		if(uiIndex < m_uiTotalColumns)
			return m_pColumnArray[uiIndex];
		else
			return NULL;
	}

	unsigned int size() const
	{
		return m_uiTotalColumns;
	}

	unsigned int TotalSize() const
	{
		return m_uiRowSize-1;
	}
};
//----End----CRecord------------------------------------------------------

//----Start----ColExtractor-----------------------------------------------
class ColExtractor
{
private:
	CGzFileReader	m_objGzFileReader;
	char			*m_szBuffer;
	long			m_lCurrentIndexInBuffer;
	long			m_lCurrentBufferSize;
	char 			m_cColumnSep; 

private:
	bool RefillBuffer()
	{
		if(-1 == m_lCurrentIndexInBuffer)
		{
			m_objGzFileReader.ReleaseBuffer(m_szBuffer);
			m_lCurrentBufferSize	=	m_objGzFileReader.GetBuffer(m_szBuffer);

			if(m_lCurrentBufferSize <= 0)
				return false;

			m_szBuffer[m_lCurrentBufferSize]	=	'\0';
			m_lCurrentIndexInBuffer				=	0;
		}
		return true;
	}

public:
	ColExtractor( char cColumnSep =	'\t')
	{
		m_lCurrentIndexInBuffer	=	-1;
		m_lCurrentBufferSize	=	0;
		m_szBuffer		=	NULL;
		m_cColumnSep		=	cColumnSep;
	}

	~ColExtractor()
	{
		m_objGzFileReader.ReleaseBuffer(m_szBuffer);
	}

	bool ExtractFrom(const char *pszFileName)
	{
		bool bRetVal	=	m_objGzFileReader.OpenGzFile(pszFileName);
		RefillBuffer();

		return bRetVal;
	}

	bool ExtractFrom(string strFileName)
	{
		return ExtractFrom(strFileName.c_str());
	}

	bool Extract(CRecord &objRecord)
	{
		char *pszLeftOver	=	NULL;
		char *pcNewLineInBuffer;
		do 
		{
			if(m_lCurrentIndexInBuffer == -1)
				return false;

			pcNewLineInBuffer	=	strchr(&m_szBuffer[m_lCurrentIndexInBuffer], cRecordSeparator);

			if(NULL == pcNewLineInBuffer)//We reached to end of current buffer.
			{
				if(pszLeftOver != NULL)
				{//This condition will occur only if cRecordSeparator is never found in complete m_szBuffer.
					char *pszTempLeftOver	=	new char[BUFFER_SIZE+1+strlen(pszLeftOver)];
					if(NULL == pszTempLeftOver)
						throw ColException(ERRORINFO("Memory allocation failure"));
					strcpy(pszTempLeftOver, pszLeftOver);
					strcat(pszTempLeftOver, m_szBuffer);
					delete []pszLeftOver;
					pszLeftOver	=	pszTempLeftOver;
				}
				else
				{
				pszLeftOver = new char[m_lCurrentBufferSize - m_lCurrentIndexInBuffer+1];
				if(NULL == pszLeftOver)
					throw ColException(ERRORINFO("Memory allocation failure") + string( __func__ ));
				strcpy(pszLeftOver, &m_szBuffer[m_lCurrentIndexInBuffer]);
				}
				m_lCurrentIndexInBuffer	=	-1;

				if(!RefillBuffer())//Since we reached to end of current buffer so refill it.
				{
					if(pszLeftOver != NULL)
					{
						if(strlen(pszLeftOver) == 0)
							return false;
						objRecord.Parse(pszLeftOver, m_cColumnSep);
						delete []pszLeftOver;
						return true;
					}
					return false;
				}
			}
			else
				break;
		}while(true);

		*pcNewLineInBuffer	=	'\0';
		if(pszLeftOver == NULL)
			objRecord.Parse(&m_szBuffer[m_lCurrentIndexInBuffer], m_cColumnSep);
		else
		{
			char *pszTemp	=	new char[strlen(pszLeftOver)+strlen(m_szBuffer+m_lCurrentIndexInBuffer)+1];
			if(NULL == pszTemp)
				throw ColException(ERRORINFO("Memory allocation failure") + string( __func__ ));
			strcpy(pszTemp, pszLeftOver);
			strcat(pszTemp, m_szBuffer+m_lCurrentIndexInBuffer);
			objRecord.Parse(pszTemp, m_cColumnSep);
			delete []pszTemp;
			delete []pszLeftOver;
			pszLeftOver	=	NULL;
		}
		m_lCurrentIndexInBuffer	+=	(pcNewLineInBuffer - &m_szBuffer[m_lCurrentIndexInBuffer])+1;

		return true;
	}
};
//----End----ColExtractor-------------------------------------------------

#endif

#endif
