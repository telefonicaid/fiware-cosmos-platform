
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
		
		valueSize = (DataSizeFunction *) malloc( sizeof( DataSizeFunction) * num_inputs );
        
        std::vector<KVFormat> inputFormats = operation->getInputFormats();
        
		// Get the rigth functions to process input key-values
		for (int i = 0 ; i < (int)inputFormats.size() ;i++)
		{
			Data *keyData	= ModulesManager::shared()->getData( inputFormats[i].keyFormat );
			Data *valueData	= ModulesManager::shared()->getData( inputFormats[i].valueFormat );
			
			if( !keyData )
				LM_X(1,("Internal error:"));
            
			if( !valueData )
				LM_X(1,("Internal error:"));
            
			
			keySize = keyData->getSizeFunction();		// Common to all inputs
			valueSize[i] = valueData->getSizeFunction();	
		}			
		
		compare = operation->getInputCompareFunction();
        
        compareKey = operation->getInputCompareByKeyFunction();

        
        inputStructs = (KVSetStruct*) malloc( sizeof(KVSetStruct) * num_inputs );
        
    }

    
	KVInputVector::KVInputVector( int _num_inputs )
	{
		num_inputs = _num_inputs;
		
		_kv = NULL;
		kv  = NULL;
		
		max_num_kvs = 0;// Maximum number of kvs to be processes
		num_kvs = 0;// Current numner of key-values pairs
		
		valueSize = (DataSizeFunction *) malloc( sizeof( DataSizeFunction) * num_inputs );
        
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
        
        free( valueSize );
	}
	

	
	void KVInputVector::addKVs( ProcessSharedFile& file )
	{
		int input = file.header->input;
		
		// Get the rigth size function
		DataSizeFunction _valueSize = valueSize[input];
		
		// Get the pointer to data
		char *data = file.data + file.offset;
		
		// Get the info we should read
		KVInfo info = file.info[ file.hg ];
		
		// Local offset
		size_t offset = 0;
		
		// Process a set of key values
		for (size_t i = 0 ; i < info.kvs ; i++)
		{
			kv[ num_kvs ].key = data + offset;
			
			offset += keySize( data + offset );
			
			kv[ num_kvs ].value = data + offset;
			
			offset += _valueSize( data + offset );
			
			kv[num_kvs].input = input;
			
			num_kvs++;
		}
		
		// Make sure the parsing is OK!
		if( offset != info.size )
			LM_X(1,("Error adding key-values to a KVInputVector, with file.hg:%d, info.kvs:%lu, offset(%lu) != info.size(%lu)\n", file.hg, info.kvs, offset, info.size));
		
		// update the  file for the next round
		file.hg++;
		file.offset += info.size;
		
	}
	
    void KVInputVector::addKVs(int input , KVInfo info , char *data )
    {
		// Get the rigth size function
		DataSizeFunction _valueSize = valueSize[input];
		
		// Local offset
		size_t offset = 0;
		
		// Process a set of key values
		for (size_t i = 0 ; i < info.kvs ; i++)
		{
			kv[ num_kvs ].key = data + offset;
			
			offset += keySize( data + offset );
			
			kv[ num_kvs ].value = data + offset;
			
			offset += _valueSize( data + offset );
			
			kv[num_kvs].input = input;
			
			num_kvs++;
		}
		
		// Make sure the parsing is OK!
		if( offset != info.size )
			LM_X(1,("Error adding key-values to a KVInputVector for input %d (%s)" , input , info.str().c_str() ));
        
    }
    
    
	// global sort function key - input - value used in reduce operations
	
	void KVInputVector::sort()
	{
		if( num_kvs > 0 )
			std::sort( _kv , _kv + num_kvs , compare );
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
        while( ( pos_end < num_kvs ) && ( compareKey( _kv[pos_begin] , _kv[pos_end] ) == 0) )
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
