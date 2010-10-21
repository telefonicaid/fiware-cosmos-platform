#ifndef _H_AU_MAP
#define _H_AU_MAP

namespace au {

	/**
	 Function to easyly insert elements in a std::map < value , Pointer* >
	*/

	template <typename T,class T2>
	void insertInMap( std::map<T,T2*> &map , T key , T2* value )
	{
	map.insert( std::pair<T,T2*>( key, value) );
	}

	/**
	 Function to easyly get pointers in a std::map < value , Pointer* >
	 NULL if not found
	*/

	template<typename K, class V> 
	V* findInMap(  std::map<K,V*> &m , K key ) 
	{
	typename std::map<K, V* >::iterator iter = m.find(key);

	if( iter == m.end() )
	  return NULL;
	return iter->second;
	}

	/** 
	  Function to remove a particular entry if exist
	  Return if it really existed
	*/

	template<typename K, class V> 
	bool removeInMap(  std::map<K,V*> &m , K key ) 
	{
	typename std::map<K, V* >::iterator iter = m.find(key);

	if( iter == m.end() )
	  return false;
	else
	  {
	m.erase( iter );
	return true;
	  }
	}
	
	template<typename K, class V> 
    V* extractFromMap(  std::map<K,V*> &m , K key )
	{
		typename std::map<K, V* >::iterator iter = m.find(key);
		
		if( iter == m.end() )
			return NULL;
		else
		{
			V* v = iter->second;
			m.erase(iter);
			return v;
		}
		
	}
	
  
}

#endif
