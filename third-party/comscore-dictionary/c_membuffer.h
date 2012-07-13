#pragma once
#include <vector>
typedef std::vector<void *> T_BUFFER_LIST;


// C_MemBuffer.h : header file
//

/////////////////////////////////////////////////////////////////////////////


#ifndef __WIN32
#define __int16 short
#ifdef ENABLE_LINUX_DATABASE_LOAD
#include <sqltypes.h>
#else
typedef unsigned char BYTE;
#endif
#endif


class C_MemBuffer 
{
public:

	C_MemBuffer();           // protected constructor used by dynamic creation
	~C_MemBuffer();

// Attributes
public:

	int BufferCount();
	int Size();
	void print();
	char* GetPointer(unsigned __int16 buffer,unsigned __int16 index );

// Operations
public:
	char *pAllocBuffer(long lNumBytes,unsigned __int16 &buffer,unsigned __int16 &index );
	char *pAllocBuffer(long lNumBytes);
	char *pAllocString(long lNumChars);

// Implementation
protected:

	T_BUFFER_LIST m_BufferArray;
	char* m_pCurrentBuffer;
	int   m_iIndex;
	long long m_iTotal_Allocation;

};

/////////////////////////////////////////////////////////////////////////////
