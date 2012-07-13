#include "stdafx.h"
#include "CFDTree.h"
#include "c_membuffer.h"
#include <stdio.h>
#include <string.h>

#define SHORT_MAX 65535
#define DEFAULT_NULL make_pair(SHORT_MAX,SHORT_MAX)
//#define SP_DEBUG

CCFDTree::CCFDTree(C_MemBuffer *pMembuffer, bool bNEDOM, int iAppsMask)
{
	m_TotalNodes=0;
	m_pMembuffer = pMembuffer;
	// create root node
	//root = (TREE*) malloc(sizeof(TREE));
	root = (TREE*) m_pMembuffer->pAllocBuffer(sizeof(TREE));
	root->chKey = '%';
	root->childs = DEFAULT_NULL;
	root->sibling = DEFAULT_NULL;
	root->value.first = SHORT_MAX;
	root->value.second = SHORT_MAX;

	m_bNEDOM = bNEDOM ;
	m_iAppsMask = iAppsMask ;
}

CCFDTree::~CCFDTree(void)
{
	//Nilesh : Apr 27, 2011 - Commented this memory release reason being that m_pMemBuffer has been passed to CCFDTree by its user
	//And so its user must be responsible for freeing it
	//if ( m_pMembuffer )
	//{
	//	delete m_pMembuffer ;
	//	m_pMembuffer = NULL ;
	//}
}



	
CCFDTree::TREE* CCFDTree::make_root(char chKey)
{
	TREE* node = NULL;
	//node = (TREE*) malloc(sizeof(TREE));
	node = (TREE*) m_pMembuffer->pAllocBuffer(sizeof(TREE));
	node->chKey = chKey;
	node->childs = DEFAULT_NULL;
	node->sibling = DEFAULT_NULL;
	node->value.first = SHORT_MAX;
	node->value.second = SHORT_MAX;
	//gTotalNodes = 1;
	return node;
}

CCFDTree::TREE* CCFDTree::insert_node(char chKey, CCFDTree::TREE* parent)
{
	//	printf(" 41 ");
	TREE* node = NULL;
	TREE* retVal = NULL;

	if( parent->childs.first == SHORT_MAX && parent->childs.second == SHORT_MAX)
	{
		//		printf(" 58 ");
		// create first child
		//node = (TREE*) malloc(sizeof(TREE));
		pair<unsigned __int16,unsigned __int16> node_pair;
		node = (TREE*) m_pMembuffer->pAllocBuffer(sizeof(TREE),node_pair.first,node_pair.second);
		if( node == NULL )
		{
			printf("\n Unable to allocate memory");
			exit (0);
		}
		node->chKey = chKey;
		node->childs = DEFAULT_NULL;
		node->sibling = DEFAULT_NULL;
		node->value = DEFAULT_NULL;

		parent->childs = node_pair;		
		retVal = node;

		m_TotalNodes++;
		//	printf(" 66 ");
	}
	else
	{
		node = ( TREE* ) m_pMembuffer->GetPointer(parent->childs.first,parent->childs.second);		
		TREE* lastNode  = NULL;
		//		printf(" 72 ");

		while(node != NULL )
		{
			if( node->chKey == chKey)
			{
				retVal = node;
				return retVal;
			}	
			else if( node->chKey == '%' )
				break;

			lastNode = node;
			node = ( TREE* ) m_pMembuffer->GetPointer(node->sibling.first,node->sibling.second);
		}

		//	printf(" 88 ");
		//TREE *newnode = (TREE*) malloc(sizeof(TREE));
		pair<unsigned __int16,unsigned __int16> newnode_pair;
		TREE *newnode = (TREE*) m_pMembuffer->pAllocBuffer(sizeof(TREE),newnode_pair.first,newnode_pair.second);
		newnode->chKey = chKey;
		newnode->childs = DEFAULT_NULL;
		newnode->sibling = DEFAULT_NULL;
		newnode->value = DEFAULT_NULL;
		
		retVal = newnode;
		m_TotalNodes++;

		if( node == NULL)
		{ // char node found, create a new node			
			lastNode->sibling = newnode_pair;			
		}
		else
		{
			if( lastNode != NULL)
			{
				pair<unsigned __int16,unsigned __int16> temp_pair = lastNode->sibling;
				lastNode->sibling = newnode_pair;
				newnode->sibling = temp_pair;
			}
			else
			{
				pair<unsigned __int16,unsigned __int16> temp_pair = parent->childs;
				parent->childs = newnode_pair;
				newnode->sibling = temp_pair;

			}
		}
	}

	return retVal;
}

bool CCFDTree::insert(const char* szKey, VALUE *value, pair<unsigned __int16,unsigned __int16> &value_pair)
{
	return insert(szKey,value,root,value_pair);
}

bool CCFDTree::insert(const char* szKey, VALUE *value, TREE* root, pair<unsigned __int16,unsigned __int16> &value_pair)
{
	int index =0;
	TREE *parentNode = root;
	TREE *nextNode = root;
	while(szKey[index] != '\0')
	{
		parentNode = nextNode;
		//	printf("\n %c",szKey[index]);
		nextNode = insert_node(szKey[index],parentNode);		
		index++;
	}

	if(nextNode->value.first != SHORT_MAX && nextNode->value.second != SHORT_MAX)
	{
		VALUE* val1 = ((VALUE*)m_pMembuffer->GetPointer(nextNode->value.first,nextNode->value.second));
		VALUE* val2 = ((VALUE*)m_pMembuffer->GetPointer(value_pair.first,value_pair.second));

		if( val2->subprec > val1->subprec)
			nextNode->value = value_pair;
		else if( val2->subprec == val1->subprec &&  val2->prec > val1->prec)
			nextNode->value = value_pair;
	}
	else
		nextNode->value = value_pair;

	value->parent_id = (long) parentNode;
	if( nextNode->chKey == '%')
	{
		if( parentNode->value.first == SHORT_MAX && parentNode->value.second == SHORT_MAX )
		{
			parentNode->value = value_pair;
			value->bDefault = true;
		}
		value->bDefault = true;		
	}
	else
		value->bDefault = false;
		
	return true;
}

bool CCFDTree::insert2(const char* szKeyHost, const char* szKeyQS,VALUE *value, TREE* root)
{
	string sKey = szKeyHost;
	sKey.append("\t");
	sKey += szKeyQS;
	return true;
}

CCFDTree::VALUE* decide(CCFDTree::VALUE* pattern1,CCFDTree::VALUE* pattern2)
{
	if( pattern1->parent_id == pattern2->parent_id)
	{
		if(pattern1->bDefault == true && pattern2->bDefault == false)
			return pattern2;
		else if(pattern1->bDefault == false && pattern2->bDefault == true)
			return pattern1;
	}

	if( pattern1->subprec > pattern2->subprec)
		return pattern1;
	else if( pattern1->subprec == pattern2->subprec &&  pattern1->prec > pattern2->prec)
		return pattern1;

	/*if( pattern1->prec > pattern2->prec)
		return pattern1;
	else if( pattern1->prec == pattern2->prec &&  pattern1->subprec > pattern2->subprec)
		return pattern1;*/

	return pattern2;	
}


CCFDTree::TREE* CCFDTree::search(char chKey, CCFDTree::TREE* parent)
{
	TREE* node = NULL;

	if( parent->childs.first == SHORT_MAX && parent->childs.second == SHORT_MAX)
	{
		return NULL;
	}
	else
	{
		
		node = ( TREE* ) m_pMembuffer->GetPointer(parent->childs.first,parent->childs.second);

		while(node != NULL)
		{
			if( node->chKey == chKey || node->chKey == '%')
			{
				return node;
			}
			node = ( TREE* ) m_pMembuffer->GetPointer(node->sibling.first,node->sibling.second);
		}
	}

	return NULL;
}


void CCFDTree::printftree(FILE *fp,string value, CCFDTree::TREE* parent)
{
	/*TREE* node = NULL;
	value += parent->chKey;

	if( parent->childs.first == SHORT_MAX && parent->childs.second == SHORT_MAX)
	{
		if( parent->value.first != SHORT_MAX && parent->value.second != SHORT_MAX )
		{
			fprintf(fp,"%d\t%s\n",((VALUE*)m_pMembuffer->GetPointer(parent->value.first,parent->value.second))->pattern_id,value.c_str());
		}
		else
			fprintf(fp,"0\t%s\n",value.c_str());
	}
	else
	{
		node = ( TREE* ) m_pMembuffer->GetPointer(parent->childs.first,parent->childs.second);	

		while(node != NULL)
		{
			printftree(fp,value,node);
			node = ( TREE* ) m_pMembuffer->GetPointer(node->sibling.first,node->sibling.second);
		}
	}*/	
}

CCFDTree::VALUE* CCFDTree::mysearch(char* pKey, CCFDTree::TREE* root)
{
#ifdef SP_DEBUG
	static int count = 1;
	printf("\n mysearch %d- %s  --- %c",count,pKey,root->chKey);
#endif 
	VALUE* lRetVal = 0;
	TREE *nextNode = root;
	VALUE* newRet = NULL;

	if(*pKey != '\0')
	{

		nextNode = search(*pKey,nextNode);	

		if( nextNode == NULL)
			return 0;	
		else
		{			
			if( nextNode->chKey == '%')
			{		
				
				while(*pKey != '\0')
                {     
                      newRet = mysearch(pKey,nextNode);  
                      if( newRet != NULL)
                      {     
                            if(lRetVal != NULL)
                                  lRetVal = decide(newRet,lRetVal);
                            else
                                  lRetVal = newRet;
                      }
                      pKey++;
                }


				//while(*pKey != '\0')
				//{	
				//	//lRetVal = mysearch(pKey,nextNode,domain);	
				//	//if( lRetVal != NULL)
				//	//	return lRetVal;
				//	//pKey++;

				//}

				/*if( lRetVal != NULL && nextNode != NULL)
				{
					if( nextNode->pValue != NULL)
						lRetVal = decide(nextNode->pValue,lRetVal);
				}
				else if( lRetVal == NULL && nextNode != NULL)
				{
					lRetVal = nextNode->pValue;
				}

				return lRetVal;*/
			}
			else
			{
				lRetVal = mysearch(pKey+1,nextNode);
				//if( lRetVal == 0)
				{
					//VALUE* newRet = NULL;
					while( nextNode!= NULL && nextNode->chKey != '%')
						nextNode = ( TREE* ) m_pMembuffer->GetPointer(nextNode->sibling.first,nextNode->sibling.second);

					if( nextNode!= NULL)
					{
						while(*pKey != '\0')
						{	
							newRet = mysearch(pKey,nextNode);	
							if( newRet != NULL)
							{	
								if(lRetVal != NULL)
									lRetVal = decide(newRet,lRetVal);
								else
									lRetVal = newRet;							
							}
							pKey++;
						}
					}

					// check if default pattern exits 
				}
			}
		}
	}
	else
	{
		if( nextNode->value.first == SHORT_MAX && nextNode->value.second == SHORT_MAX)
		{
			// check if sibling contain wildcard
			while( nextNode!= NULL && nextNode->chKey != '%')
				nextNode = ( TREE* ) m_pMembuffer->GetPointer(nextNode->sibling.first,nextNode->sibling.second);
		}
	}

	if( lRetVal != NULL && nextNode != NULL)
	{
		if(  nextNode->value.first != SHORT_MAX && nextNode->value.second != SHORT_MAX)
			lRetVal = decide(((VALUE*)m_pMembuffer->GetPointer(nextNode->value.first,nextNode->value.second)),lRetVal);
	}
	else if( lRetVal == NULL && nextNode != NULL)
	{
		if( nextNode->value.first != SHORT_MAX && nextNode->value.second != SHORT_MAX )
			lRetVal = ((VALUE*)m_pMembuffer->GetPointer(nextNode->value.first,nextNode->value.second));
		else
			lRetVal = NULL;
	}
		
	//if( lRetVal == NULL && nextNode != NULL)
	//	lRetVal = nextNode->pValue;
#ifdef SP_DEBUG
	count++;
	if( lRetVal == NULL)
		printf("\n mysearch %d- %s  --- %c",count,pKey,root->chKey);
	else
		printf("\n mysearch %d- %s -- %d --- %c",count,pKey,lRetVal->pattern_id,root->chKey);
#endif 
	return lRetVal;
}

CCFDTree::VALUE* CCFDTree::MatchURL(char* pKey)
{
#ifdef SP_DEBUG
	printf("\n MatchURL - %s",pKey);
#endif 
	return mysearch(pKey,root);
}

bool CCFDTree :: LoadRow
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
					)
{
	if (( lPatternID==8427705 ) || ( lPatternID==8427706 ) || ( lPatternID==8427707 ) )
	{
		//fprintf ( stderr , "Not loading PatternID: [%i]\n" , lPatternID ) ;
		return false ;
	}
	string strWildcard ( pszPrefix ) ;
	int ( *ptrFun ) ( int ) = toupper ;
	size_t stLocation = string :: npos ;
	string strPropertyName ( pszPropertyName ) ;
	bool bRetVal = false ;

	bIsNEDOM = false ;

	if ( ( strlen(pszPrefix) > 0 ) && ( 0 != strcmp ( pszPrefix , "%" ) ) )
		strWildcard.append ( "." ) ;

	//strWildcard.append ( pszEntityName ) ;
	strWildcard.append ( "\t" ) ;
	strWildcard.append ( pszPageMask ) ;

	transform ( strWildcard.begin ( ) , strWildcard.end ( ) , strWildcard.begin ( ) , ptrFun ) ;

	while ( string :: npos != ( stLocation = strWildcard.find ( "%%" ) ) )
		strWildcard.erase ( stLocation , 1 ) ;

	pair < unsigned __int16 , unsigned __int16 > value_pair ;
	CCFDTree :: VALUE* pValue = (CCFDTree :: VALUE*) m_pMembuffer->pAllocBuffer ( sizeof ( CCFDTree :: VALUE ) , value_pair.first , value_pair.second ) ;

	/*if ( string :: npos != ( stLocation = strPropertyName.find ( '\r' ) ) )
		strPropertyName.erase ( stLocation , 1 ) ;*/

	pValue->pattern_id = lPatternID ;
	pValue->prec = iPrecedence ;
	pValue->subprec = iSubChannelPrecedence ;
	//Using App Mask for identifying NEDOMs - As informed by Jason
	if ( atol ( pszAppsMask ) & m_iAppsMask )
	{
		pValue->pattern_id *= -1 ;
		bIsNEDOM = true ;

		// Load if user didn't disable NEDOMS
		if( m_bNEDOM == true )
			bRetVal = CCFDTree :: insert ( strWildcard.c_str ( ) , pValue , value_pair ) ;
	}
	else
		bRetVal = CCFDTree :: insert ( strWildcard.c_str ( ) , pValue , value_pair ) ;

	return bRetVal ;
}

long CCFDTree :: Apply ( const char* pszHost , const char* pszDomain , const char* pszQueryString , int& iWeight )
{
	long lPatternID = 1 ;
	string strKey ( pszHost ) ;
	CCFDTree :: VALUE* pSearchResult = NULL ;
	int ( *ptrFun ) ( int ) = toupper ;

	// remove domain name from host
	//long pos = strKey.find(pszDomain);

	//Nilesh - June 27, 2011 - Support for searching for Domain name in Host from the back.
	//Done for handling case of sanbernardino.elclasificado.com.elclasificado.com
	long pos = strKey.rfind(pszDomain);
	if( pos != -1)
		strKey.erase(pos,strlen(pszDomain));

	// prepare perfix + page pask combincation with a tab in between
	strKey += "\t";
	strKey += pszQueryString ;

	// convert the final url to UPPERCASE
	transform ( strKey.begin ( ) , strKey.end ( ) , strKey.begin ( ) , ptrFun ) ;

	//Nilesh - May 17, 2011 - Support for removing % from entire URL - Requested by Santosh on May 16, 2011 2123 hrs EDT BEGIN
    size_t stPercentLocation = string :: npos ;
    size_t stStartLocation = 0 ;

    while ( string :: npos != ( stPercentLocation = strKey.find ( "%" , stStartLocation ) ) )
    {
            strKey.erase ( stPercentLocation , 1 ) ;
            stStartLocation = stPercentLocation + 1 ;
    }
	//Nilesh - May 17, 2011 - Support for removing % from entire URL - Requested by Santosh on May 16, 2011 2123 hrs EDT END

	if ( ( pSearchResult = CCFDTree :: MatchURL ( (char*) strKey.c_str ( ) ) ) )
	{
		lPatternID = pSearchResult->pattern_id ;
		iWeight = ( pSearchResult->subprec * 10000 ) + pSearchResult->prec ;
	}

	return lPatternID ;
}

bool CCFDTree :: LoadAdNetRow
		(
			const char* pszRequestURLWildcard ,
			const char* pszReferURLWildcard ,
			const long& lPatternID ,
			const int& iPrecedence ,
			const int& iSubChannelPrecedence
		)
{
	pair < unsigned __int16 , unsigned __int16 > value_pair ;
	CCFDTree :: VALUE* pValue = (CCFDTree :: VALUE*) m_pMembuffer->pAllocBuffer ( sizeof ( CCFDTree :: VALUE) , value_pair.first , value_pair.second ) ;
	string strKey ( pszRequestURLWildcard ) ;

	strKey += "\t" ;
	strKey.append ( pszReferURLWildcard ) ;

	pValue->pattern_id = lPatternID ;
	pValue->prec = iPrecedence ;
	pValue->subprec = iSubChannelPrecedence ;

	return CCFDTree :: insert ( strKey.c_str ( ) , pValue , value_pair ) ;
}

long CCFDTree :: ApplyAdNet ( const char* pszRequest , const char* pszRefer )
{
	string strKey ( pszRequest ) ;
	CCFDTree::VALUE* pResult ;
	long lRetVal = 1 ;

	strKey += "\t" ;
	strKey.append ( pszRefer ) ;

	if ( ( pResult = CCFDTree :: MatchURL ( (char*) ( strKey ).c_str ( ) ) ) )
		lRetVal = pResult->pattern_id ;

	return lRetVal ;
}

