#pragma once
class CCFDBase
{
public:
	CCFDBase(void);
	virtual ~CCFDBase(void);

	virtual bool LoadRow
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
		) = 0 ;
	virtual long Apply ( const char* pszHost , const char* pszDomain , const char* pszQueryString , int& iWeight ) = 0 ;

	//Functions for Ad Net
	virtual bool LoadAdNetRow
		(
			const char* pszRequestURLWildcard ,
			const char* pszReferURLWildcard ,
			const long& lPatternID ,
			const int& iPrecedence ,
			const int& iSubchannelPrecedence 
		) = 0 ;
	virtual long ApplyAdNet ( const char* pszRequest , const char* pszRefer ) = 0 ;
};

