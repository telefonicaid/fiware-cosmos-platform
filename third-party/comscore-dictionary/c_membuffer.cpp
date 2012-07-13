// C_MemBuffer.cpp : implementation file
//
#include "stdafx.h"
#include "c_membuffer.h"

long iNew = 0;
long lDelete = 0;

#define SHORT_MAX 65535

/////////////////////////////////////////////////////////////////////////////

//const int MAX_SIZE = 16382;

const int MAX_SIZE =  65528;

/////////////////////////////////////////////////////////////////////////////

C_MemBuffer::C_MemBuffer()
{
	iNew++;

	m_iIndex = 0;
	m_pCurrentBuffer = new char[MAX_SIZE+2];
	m_BufferArray.push_back(m_pCurrentBuffer);
	m_iTotal_Allocation = 0;
}

/////////////////////////////////////////////////////////////////////////////

C_MemBuffer::~C_MemBuffer()
{
	lDelete++;

	BYTE* pTempBuffer;

	for (int i=0; i < (int)m_BufferArray.size(); i++)
	{
		pTempBuffer = (BYTE *)m_BufferArray[i];
		delete[] pTempBuffer;
	}

	m_pCurrentBuffer = NULL;
	m_iIndex = 0;
}

/////////////////////////////////////////////////////////////////////////////

int C_MemBuffer::BufferCount()
{
	return m_BufferArray.size();
}

char* C_MemBuffer::GetPointer(unsigned __int16 buffer,unsigned __int16 index )
{
	//printf("\n %d\t%d",buffer,index);
	if( buffer != SHORT_MAX && index != SHORT_MAX )
		return (char*) m_BufferArray[buffer]+index;
	else
		return NULL;
}

void C_MemBuffer::print()
{
	printf("\n Buffer Count - %i\n Total Allocation per buffer - %lld",m_BufferArray.size(),m_iTotal_Allocation / m_BufferArray.size());
}



/////////////////////////////////////////////////////////////////////////////

int C_MemBuffer::Size()
{
	return m_BufferArray.capacity() * (MAX_SIZE + 2);
}

/////////////////////////////////////////////////////////////////////////////
// C_MemBuffer message handlers

char *C_MemBuffer::pAllocBuffer(long lNumBytes,unsigned __int16 &buffer,unsigned __int16 &index )

{
	m_iTotal_Allocation++;
	char* pBuffer;

	//
	// Allocate a new buffer if current one is full
	//

	if ((m_iIndex + lNumBytes) >= MAX_SIZE)
	{
		m_iIndex = 0;
		m_pCurrentBuffer = new char[MAX_SIZE+2];
		m_BufferArray.push_back(m_pCurrentBuffer);
	}

	//
	// Get pointer to next available memory chunk
	//

	if ((m_iIndex + lNumBytes) < MAX_SIZE)
	{
		pBuffer = &(m_pCurrentBuffer[m_iIndex]); 
		index = m_iIndex;
		buffer = m_BufferArray.size() - 1;
		m_iIndex += lNumBytes;
	}
	else
	{
		// Error - Largest Memory block is not big enough to hold string
		return NULL;
	}

	return pBuffer;
}

char *C_MemBuffer::pAllocBuffer(long lNumBytes)

{
	m_iTotal_Allocation++;
	char* pBuffer;

	//
	// Allocate a new buffer if current one is full
	//

	if ((m_iIndex + lNumBytes) >= MAX_SIZE)
	{
		m_iIndex = 0;
		m_pCurrentBuffer = new char[MAX_SIZE+2];
		m_BufferArray.push_back(m_pCurrentBuffer);
	}

	//
	// Get pointer to next available memory chunk
	//

	if ((m_iIndex + lNumBytes) < MAX_SIZE)
	{
		pBuffer = &(m_pCurrentBuffer[m_iIndex]); 
		m_iIndex += lNumBytes;
	}
	else
	{
		// Error - Largest Memory block is not big enough to hold string
		return NULL;
	}

	return pBuffer;
}

/////////////////////////////////////////////////////////////////////////////

char *C_MemBuffer::pAllocString(long lNumChars)
{
	m_iTotal_Allocation++;
	long lNumBytes = (lNumChars + 1) * sizeof(char *);
	
	return (char *)pAllocBuffer(lNumBytes);
}

/////////////////////////////////////////////////////////////////////////////

// End of File
