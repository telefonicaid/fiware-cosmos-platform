#pragma once
#include <string>
#include <algorithm>
#include <map>

#include "c_membuffer.h"
#include "CFDBase.h"

#ifndef _WIN32
#define __int16 short
#endif

using namespace std;

class CCFDTree : public CCFDBase
{	
private:
	struct Value
	{
		long prec;
		long subprec;	
		long pattern_id;
		long parent_id;
		bool bDefault;

		Value()
		{
			prec = 0;
			subprec = 0;
			pattern_id = 0;
			parent_id = 0;
			bDefault = false;
		}
	};

	// tree node 
	struct node
	{
		char chKey;
		pair<unsigned __int16,unsigned __int16> sibling;
		//struct node *sibling;
		pair<unsigned __int16,unsigned __int16> childs;
		//struct node *childs;
		//struct Value *pValue;	
		pair<unsigned __int16,unsigned __int16> value;
		//unsigned __int16 &buffer,unsigned __int16 &index
	};

	long m_TotalNodes;	

public:
	typedef struct Value VALUE;	
	typedef struct node TREE;
private:
	TREE* root;
	TREE* insert_node(char chKey, TREE* parent);
	bool insert(const char* szKey, VALUE *value, TREE* root, pair<unsigned __int16,unsigned __int16> &value_pair);
	bool insert2(const char* szKeyHost, const char* szKeyQS,VALUE *value, TREE* root);
	VALUE* mysearch(char* pKey, TREE* root);
	void printftree(FILE *fp,string value, TREE* parent);
	TREE* search(char chKey, TREE* parent);
	TREE* make_root(char chKey);
public:
	CCFDTree(C_MemBuffer *pMembuffer, bool bNEDOM = true, int iAppsMask = 1);
	~CCFDTree(void);

	bool insert(const char* szKey, VALUE *value, pair<unsigned __int16,unsigned __int16> &value_pair);
	//VALUE* CCFDTree::MatchURL(char* pKey);
	VALUE* MatchURL(char* pKey);

	bool LoadRow
		(
			const char* pszPrefix ,
			const char* pszEntityName ,
			const char* pszPageMask ,
			const char* pszPropertyName ,
			const char* pszAppsMask ,
			const long& lPatternID ,
			const int& iPrecedence ,
			const int& iSubChannelPrecedence ,
			bool& bIsNEDOM
		) ;
	long Apply ( const char* pszPrefix , const char* pszDomain , const char* pszQueryString , int& iWeight ) ;

	bool LoadAdNetRow
		(
			const char* pszRequestURLWildcard ,
			const char* pszReferURLWildcard ,
			const long& lPatternID ,
			const int& iPrecedence ,
			const int& iSubchannelPrecedence
		) ;
	long ApplyAdNet ( const char* pszRequest , const char* pszRefer ) ;

	C_MemBuffer *m_pMembuffer;
	
private:
	bool	m_bNEDOM ;
	int		m_iAppsMask ;
};

