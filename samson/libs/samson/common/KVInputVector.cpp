
#include "KVInputVector.h"		// Own interface
#include <algorithm>            // std::sort

#include "samson/module/ModulesManager.h"   // samson::ModulesManager

namespace samson
{
#pragma mark KVInputVectorBase
	
	void KVInputVectorBase::prepareInput( size_t _max_num_kvs )
	{
		if( _max_num_kvs > max_num_kvs )
		{
			if( _kv )
				free( _kv );
			if( kv )
				free ( kv );
			
			// Set a new maximum number of kvs
			max_num_kvs = _max_num_kvs;
			
			_kv = (KV**) malloc( sizeof(KV*) * _max_num_kvs );
			kv  = (KV*) malloc( sizeof(KV) * max_num_kvs );
			
		}
		
		// Set the pointers to internal structure ( to sort again )
		for (size_t i = 0 ; i < _max_num_kvs ; i++)
			_kv[i] = &kv[i];
		
		num_kvs = 0;
	}
  
  
#pragma mark KVInputVector
  
  KVInputVector::KVInputVector( Operation* operation )
  {
    // Take the number of inputs
    num_inputs = operation->getNumInputs();
    
		_kv = NULL;
		kv  = NULL;
		
		max_num_kvs = 0;// Maximum number of kvs to be processes
		num_kvs = 0;// Current numner of key-values pairs
    
    std::vector<KVFormat> inputFormats = operation->getInputFormats();
    
    if( inputFormats.size() == 0 )
    {
      LM_W(("Operation %s has no inputs" , operation->_name.c_str() ));
      return;
    }
    
    Data *keyData	= ModulesManager::shared()->getData( inputFormats[0].keyFormat );
    if( !keyData )
      LM_X(1,("Internal error:"));
    
    keyDataInstance = (DataInstance*)keyData->getInstance();
    if( !keyDataInstance )
      LM_X(1,("Internal error:"));
    
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			if( !valueData )
				LM_X(1,("Internal error:"));
      
      DataInstance* valueDataInstance = (DataInstance*)valueData->getInstance();
      if( !valueDataInstance )
        LM_X(1,("Internal error:"));
      
      valueDataInstances.push_back(valueDataInstance);
		}
    
    // Alloc necessary space for KVSetStruct ( used in 3rd party interface )
    inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
  }
  
	KVInputVector::KVInputVector( int _num_inputs )
	{
		num_inputs = _num_inputs;
		
		_kv = NULL;
		kv  = NULL;
		
		max_num_kvs = 0;  // Maximum number of kvs to be processes
		num_kvs = 0;      // Current number of key-values pairs
		
    inputStructs = NULL;
	}
	
	KVInputVector::~KVInputVector()
	{
		if( _kv )
			free( _kv );
		if( kv )
			free ( kv );
    
    if( inputStructs )
      free( inputStructs );
    
	}
  
  void KVInputVector::addKVs(int input , KVInfo info , KV* kvs )
  {
		for (size_t i = 0 ; i < info.kvs ; i++)
    {
      kv[num_kvs] = kvs[i];
      kv[num_kvs].input = input;
      num_kvs++;
    }
  }
	
  void KVInputVector::addKVs(int input , KVInfo info , char *data )
  {
    if( input >= (int)valueDataInstances.size() )
    {
      LM_W(("Error adding key-values to a KVInputVector. Ignoring..."));
      return;
    }
    
		// Get the right data instance
		DataInstance* valueDataInstance = valueDataInstances[input];
		
		// Local offset
		size_t offset = 0;
		
		// Process a set of key values
		for (size_t i = 0 ; i < info.kvs ; i++)
		{
			kv[ num_kvs ].key = data + offset;
			
			kv[ num_kvs ].key_size = keyDataInstance->parse( data + offset );
			offset += kv[ num_kvs ].key_size;
      
			kv[ num_kvs ].value = data + offset;
			
			kv[ num_kvs ].value_size = valueDataInstance->parse( data + offset );
      offset += kv[ num_kvs ].value_size;
			
			kv[num_kvs].input = input;
			
      kv[num_kvs].pos = num_kvs;
      
			num_kvs++;
		}
		
		// Make sure the parsing is OK!
		if( offset != info.size )
		{
			LM_X(1,("Error adding key-values to a KVInputVector for input %d (%s). (Offset %lu != info.size %lu) KVS num_kvs:%lu / max_num_kvs:%lu "
              , input , info.str().c_str() , offset , info.size , num_kvs , max_num_kvs  ));
		}
  }
  
  std::string str_kv( KV* kv )
  {
    std::ostringstream output;
    output << au::str("KV[%d][%d]" , kv->key_size , kv->value_size );
    return output.str();
  }
  
  bool equalKV( KV* kv1 , KV* kv2 )
  {
    if( kv1->key_size != kv2->key_size )
      return false;
    for (int i = 0 ; i < kv1->key_size ; i++ )
      if( kv1->key[i] != kv2->key[i] )
        return false;
    
    return true;
  }
  
  bool compareKV( KV* kv1 , KV* kv2 )
  {
    
    if( kv1->key_size < kv2->key_size )
      return true;
    
    if( kv1->key_size > kv2->key_size )
      return false;
    
    for (int i = 0 ; i < kv1->key_size ; i++ )
    {
      
      if( kv1->key[i] < kv2->key[i] )
        return true;
      
      if( kv1->key[i] > kv2->key[i] )
        return false;
    }
    
    // Compare by input
    if( kv1->input != kv2->input )
      return kv1->input < kv2->input;
    
    // Same key!
    
    if( kv1->value_size < kv2->value_size )
      return true;
    
    if( kv1->value_size > kv2->value_size )
      return false;
    
    for (int i = 0 ; i < kv1->value_size ; i++ )
    {
      
      if( kv1->value[i] < kv2->value[i] )
        return true;
      
      if( kv1->value[i] > kv2->value[i] )
        return false;
    }
    
    
    // Same key and value!
    
    // If they are the same....
    return false;
  }
  
  
	// global sort function key - input - value used in reduce operations
	
	void KVInputVector::sort()
	{
		if( num_kvs > 0 )
			std::sort( _kv , _kv + num_kvs , compareKV );
	}
  
  void KVInputVector::sortAndMerge( size_t middle_pos )
  {
    if( middle_pos > num_kvs )
      LM_X(1,("Internal error"));
    
    // Sort the first part of the vector
    std::sort( _kv , _kv + middle_pos , compareKV );
    
    // Merge with the second part of the vector( supposed to be sorted )
    std::inplace_merge(_kv, _kv+middle_pos, _kv+num_kvs , compareKV);
    
  }
  
  
  void KVInputVector::init()
  {
    // Process all the key-values in order
    pos_begin = 0;	// Position where the next group of key-values begin
    pos_end	 = 1;	// Position where the next group of key-values finish
  }
  
  
  KVSetStruct* KVInputVector::getNext()
  {
    if( pos_begin >= num_kvs )
      return NULL;
    
    // Identify the number of key-values with the same key
    while( ( pos_end < num_kvs ) && equalKV( _kv[pos_begin] , _kv[pos_end] ) )
      pos_end++;
    
    // Create the necessary elements for the output KVSetStruct structure
    size_t pos_pointer = pos_begin;
    for (int i = 0 ; i < num_inputs ;i++)
    {
      if( (pos_pointer == pos_end) || ( _kv[pos_pointer]->input != i) )
        inputStructs[i].num_kvs = 0;
      else
      {
        inputStructs[i].kvs = &_kv[pos_pointer];
        inputStructs[i].num_kvs = 0;
        while( ( pos_pointer < pos_end ) && ( _kv[pos_pointer]->input == i) )
        {
          inputStructs[i].num_kvs++;
          pos_pointer++;
        }
      }
    }
    
    // Go to the next position
    pos_begin = pos_end;
    pos_end = pos_begin + 1;
    
    return inputStructs;
    
  }
	
  
}
