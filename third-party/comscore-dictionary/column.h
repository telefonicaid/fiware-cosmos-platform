#ifdef _WIN32
//Version SingleThreaded Modifiable Ver 1.0.1.0
#ifndef _Column_h_DSC_STM_1010
#define _Column_h_DSC_STM_1010

#include "zlib.h"
#include <string.h>
#include <io.h>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

#define COLUMNS		vector<string>
#define BUFFER_SIZE 1024*256
#ifndef ERRORINFO
#define ERRORINFO(MESSEGE)  MESSEGE " :: FileName=(" __FILE__ ") FunctionName=(" __FUNCTION__ "()) LineNumber=" "."
#endif

const char cRecordSeparator	=	'\n';
const char cColumnSeparator	=	'\t';

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

	void Parse(const char* pszString)
	{
		FreeAll();
		const char *pszTemp	=	pszString;
		while(*pszString)
		{
			++m_uiRowSize;
			if(cColumnSeparator == *pszString)
				++m_uiTotalColumns;
			++pszString;
		}
		++m_uiTotalColumns;//For last column.
		++m_uiRowSize;//For counting '\0'.

		m_pszCompleteRow	=	new char[m_uiRowSize];
		if(NULL == m_pszCompleteRow)
			throw exception(ERRORINFO("Memory allocation failure"));
		m_pColumnArray		=	new char*[m_uiTotalColumns];
		if(NULL == m_pColumnArray)
			throw exception(ERRORINFO("Memory allocation failure"));
		strcpy(m_pszCompleteRow, pszTemp);

		m_pColumnArray[0]	=	m_pszCompleteRow;
		unsigned int uiColIndex	=	1;
		for(unsigned int uiIndex=0; m_pszCompleteRow[uiIndex]; ++uiIndex)
		{
			if(cColumnSeparator == m_pszCompleteRow[uiIndex])
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
			throw exception(ERRORINFO("Memory allocation failure"));
		m_pColumnArray		=	new char*[m_uiTotalColumns];
		if(NULL == m_pColumnArray)
			throw exception(ERRORINFO("Memory allocation failure"));
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
			throw exception(ERRORINFO("Memory allocation failure"));
		m_pColumnArray		=	new char*[m_uiTotalColumns];
		if(NULL == m_pColumnArray)
			throw exception(ERRORINFO("Memory allocation failure"));
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
	gzFile	m_gzInputFile;
	char	m_szBuffer[BUFFER_SIZE+1];
	long	m_lCurrentIndexInBuffer;
	long	m_lCurrentBufferSize;
	CRecord			m_objRecord;

private:
	bool RefillBuffer()
	{
		if (-1 == m_lCurrentIndexInBuffer)
		{
			m_lCurrentBufferSize	=	gzread(m_gzInputFile, m_szBuffer, BUFFER_SIZE);
			if(m_lCurrentBufferSize <= 0)
				return false;

			m_szBuffer[m_lCurrentBufferSize]	=	'\0';
			m_lCurrentIndexInBuffer				=	0;
		}
		return true;
	}

public:
	ColExtractor()
	{
		m_lCurrentIndexInBuffer	=	-1;
		m_lCurrentBufferSize	=	0;
		m_gzInputFile			=	NULL;
	}

	~ColExtractor()
	{
		if(NULL != m_gzInputFile)
			gzclose(m_gzInputFile);
	}

	bool ExtractFrom(const char *pszFileName)
	{
		if(0 != access(pszFileName, 0))
			return false;

		m_gzInputFile	=	gzopen(pszFileName, "rb9");
		if(NULL == m_gzInputFile)
			return false;

		RefillBuffer();
		return true;
	}

	bool ExtractFrom(string strFileName)
	{
		return ExtractFrom(strFileName.c_str());
	}

	bool Extract(COLUMNS &objColumns)
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
				pszLeftOver = new char[m_lCurrentBufferSize - m_lCurrentIndexInBuffer+1];
				if(NULL == pszLeftOver)
					throw exception(ERRORINFO("Memory allocation failure"));
				strcpy(pszLeftOver, &m_szBuffer[m_lCurrentIndexInBuffer]);
				m_lCurrentIndexInBuffer	=	-1;

				if(!RefillBuffer())//Since we reached to end of current buffer so refill it.
				{
					if(pszLeftOver != NULL)
					{
						if(strlen(pszLeftOver) == 0)
						{
							delete []pszLeftOver;
							return false;
						}
						m_objRecord.Parse(pszLeftOver);
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
		if (pszLeftOver == NULL)
			m_objRecord.Parse(&m_szBuffer[m_lCurrentIndexInBuffer]);
		else
		{
			char *pszTemp	=	new char[strlen(pszLeftOver)+strlen(m_szBuffer+m_lCurrentIndexInBuffer)+1];
			if(NULL == pszTemp)
				throw exception(ERRORINFO("Memory allocation failure"));
			strcpy(pszTemp, pszLeftOver);
			strcat(pszTemp, m_szBuffer+m_lCurrentIndexInBuffer);
			m_objRecord.Parse(pszTemp);
			delete []pszTemp;
			delete []pszLeftOver;
			pszLeftOver	=	NULL;
		}
		m_lCurrentIndexInBuffer	+=	(pcNewLineInBuffer - &m_szBuffer[m_lCurrentIndexInBuffer])+1;

		if(objColumns.size() != m_objRecord.size())
		{
			while(objColumns.size() < m_objRecord.size())
				objColumns.push_back("");
			while(objColumns.size() > m_objRecord.size())
				objColumns.pop_back();
		}
		for(unsigned int uiCount=0; uiCount<m_objRecord.size(); ++uiCount)
			objColumns[uiCount]	=	m_objRecord[uiCount];

		return true;
	}
};
//----End----ColExtractor-------------------------------------------------

#endif
#else
#include "column_linux.h"
#endif
