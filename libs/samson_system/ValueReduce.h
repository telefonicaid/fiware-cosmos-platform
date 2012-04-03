/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_system_reduce_VALUE_REDUCE
#define _H_SAMSON_system_reduce_VALUE_REDUCE


#include <samson/module/samson.h>
#include <samson_system/Value.h>


namespace samson{
    namespace system{

        // ---------------------------------------------
        // ValueList
        // ---------------------------------------------
        
        class ValueList
        {
            int max_num_elements;
            
            // List of values
            Value **values;
            int *counters;
            
        public:
            
            ValueList( int _max_num_elements );
            
            void init()
            {
                for (int i = 0 ; i < max_num_elements ; i++ )
                    counters[i] = 0; // Init counter to 0
            }
            
            void push( Value* value , int counter )
            {
                
                for( int i = 0 ; i < max_num_elements ; i++ )
                    if( counter > counters[i] )
                    {
                        
                        // Take the last and get the content
                        Value *tmp = values[ max_num_elements -1 ];
                        tmp->copyFrom(value);
                        
                        // Move the rest of pointers and counter
                        for( int j = (max_num_elements-2 ) ; j >= i  ; j-- )
                        {
                            values[j+1] = values[j];
                            counters[j+1] = counters[j];
                        }
                        
                        // Inser the new one
                        values[i] = tmp;
                        counters[i] = counter;
                        
                        break;
                    }
                
                
            }
            
            void set_tops( Value* value )
            {
                // Spetial case for 1 top element to not create double linked vector [[concept num]]
                if( max_num_elements == 1 )
                {
                    if( counters[0] == 0 )
                        value->set_as_void();
                    else
                    {
                        value->set_as_vector();
                        value->add_value_to_vector()->copyFrom( values[0] );
                        value->add_value_to_vector()->set_double( counters[0] );
                    }
                }
                
                value->set_as_vector();
                for ( int i = 0 ; i < max_num_elements ; i++ )
                {
                    if( counters[i] > 0 )
                    {
                        Value *tmp = value->add_value_to_vector();
                        
                        tmp->set_as_vector();
                        tmp->add_value_to_vector()->copyFrom( values[i] );
                        tmp->add_value_to_vector()->set_double( counters[i] );
                    }
                }
            }
            
        };
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce
        //
        //       Generic operation for a reduce operation
        // -----------------------------------------------------------------------------------------------
        
        class ValueReduce
        {
            
        public:
            
            virtual void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )=0;
            virtual void init( std::string command  )
            {
                // Optional method to receive extra parameters
            }
            
        };
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_all
        //
        //       Create a vector with all values for each key
        // -----------------------------------------------------------------------------------------------
        
        class ValueReduce_all : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
        public:
            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // Parse common key
                key.parse( inputs[0].kvs[0]->key );
                
                value.set_as_vector();
                for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
                {
                    value.add_value_to_vector()->parse( inputs[0].kvs[i]->value );
                }
                
                writer->emit( 0 , &key , &value );
                return;
            }
            
        };
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_unique
        //
        //       Create a vector with all unique values for each key
        // -----------------------------------------------------------------------------------------------
        
        class ValueReduce_unique : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
            samson::system::Value output_value;
            
        public:
            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // Parse common key
                key.parse( inputs[0].kvs[0]->key );
                
                output_value.set_as_vector();
                
                size_t pos = 0;
                while( pos < inputs[0].num_kvs )
                {
                    value.parse( inputs[0].kvs[pos]->value );
                    int counter = inputs[0].kvs_with_equal_value( pos );
                    
                    output_value.add_value_to_vector()->copyFrom( &value );
                    
                    pos += counter;
                }
                
                writer->emit( 0 , &key , &output_value );
                return;
            }
        };
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_unique_counter
        //
        //       Create a vector with all unique values for each key and an individual counter for each one 
        // -----------------------------------------------------------------------------------------------
        
        class ValueReduce_unique_counter : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
            samson::system::Value output_value;
            
        public:
            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // Parse common key
                key.parse( inputs[0].kvs[0]->key );
                
                output_value.set_as_vector();
                
                
                size_t pos = 0;
                while( pos < inputs[0].num_kvs )
                {
                    value.parse( inputs[0].kvs[pos]->value );
                    int counter = inputs[0].kvs_with_equal_value( pos );
                    
                    samson::system::Value* new_value = output_value.add_value_to_vector();
                    
                    new_value->set_as_vector();
                    new_value->add_value_to_vector()->copyFrom( &value );
                    new_value->add_value_to_vector()->set_double( counter );
                    
                    pos += counter;
                }
                
                writer->emit( 0 , &key , &output_value );
                return;
            }
        };
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_top
        //
        //       Emit only the most popular value 
        // -----------------------------------------------------------------------------------------------
        
        class ValueReduce_top : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
            ValueList * list;
            
        public:
            
            
            ValueReduce_top()
            {
                list = NULL;
            }
            
            ~ValueReduce_top()
            {
                if (list )
                    delete list;
            }
            
            void init( std::string command  )
            {
                au::CommandLine cmdLine;
                cmdLine.parse( command );
                
                int num = 1;
                if( cmdLine.get_num_arguments() > 1 )
                    num = atoi( cmdLine.get_argument(1).c_str() );
                if( num <= 0 )
                    num = 1;
                
                list = new ValueList( num );
            }
            
            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // Init the list
                list->init();
                
                // Parse common key
                key.parse( inputs[0].kvs[0]->key );
                
                // Parse all the values ( grouping by 
                size_t pos = 0;
                while( pos < inputs[0].num_kvs )
                {
                    value.parse( inputs[0].kvs[pos]->value );
                    int counter = inputs[0].kvs_with_equal_value( pos );
                    
                    // Push to the list
                    list->push( &value , counter );
                    
                    // Skip this value
                    pos += counter;
                }
                
                // Prepare and emit output
                list->set_tops(&value);
                writer->emit( 0 , &key , &value );
                return;        
            }
        };

        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_top_concept
        //
        //       Emit only the most popular value 
        // -----------------------------------------------------------------------------------------------
        
        class ValueReduce_top_concept : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
            ValueList * list;
            
        public:
            
            
            ValueReduce_top_concept()
            {
                list = NULL;
            }
            
            ~ValueReduce_top_concept()
            {
                if (list )
                    delete list;
            }
            
            void init( std::string command  )
            {
                au::CommandLine cmdLine;
                cmdLine.parse( command );
                
                int num = 1;
                if( cmdLine.get_num_arguments() > 1 )
                    num = atoi( cmdLine.get_argument(1).c_str() );
                if( num == 0 )
                    num = 1;
                
                list = new ValueList( num );
            }
            
            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // Init the list
                list->init();
                
                // Parse common key
                key.parse( inputs[0].kvs[0]->key );
                
                // Parse all the values ( grouping by 
                for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
                {
                    // Parse the value
                    value.parse( inputs[0].kvs[i]->value );

                    if( !value.isVector() )
                        continue;
                    if( value.get_vector_size() != 2 )
                        continue;
                    
                    int counter = value.get_value_from_vector(1)->getDouble();
                    
                    // Push to the list
                    list->push( value.get_value_from_vector(0) , counter );
                }
                
                // Prepare and emit output
                list->set_tops(&value);
                writer->emit( 0 , &key , &value );
                return;        
            }
        };        
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_sum
        //
        //       Emit the sum of values ( assuming numbers )
        // -----------------------------------------------------------------------------------------------
        
        
        class ValueReduce_sum : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
            samson::system::Value output_value;
            
        public:
            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // Parse common key
                key.parse( inputs[0].kvs[0]->key );
                
                double total = 0;
                for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
                {
                    value.parse( inputs[0].kvs[i]->value );
                    double tmp =  value.getDouble();
                    total += tmp;
                }
                
                value = total;
                writer->emit( 0 , &key , &value );
                return;
            }
        };
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_average
        //
        //       Emit the average of values ( assuming numbers )
        // -----------------------------------------------------------------------------------------------
        
        
        class ValueReduce_average : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
            samson::system::Value output_value;
            
        public:
            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // Parse common key
                key.parse( inputs[0].kvs[0]->key );
                
                double total = 0;
                for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
                {
                    value.parse( inputs[0].kvs[i]->value );
                    double tmp =  value.getDouble();
                    total += tmp;
                }
                total /= (double) inputs[0].num_kvs;
                value = total;
                writer->emit( 0 , &key , &value );
                return;
            }
        };
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_update_sum
        //
        //       Emit the average of values ( assuming numbers )
        // -----------------------------------------------------------------------------------------------
        
        
        class ValueReduce_update_sum : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
            samson::system::Value output_value;
            
            int time_span;
            bool emit;
            
            double factor; // Forgetting facto
            
        public:
            
            void init( std::string command  )
            {
                au::CommandLine cmdLine;
                cmdLine.set_flag_boolean("emit");
                cmdLine.set_flag_int("time" , 0);
                cmdLine.parse( command );
                
                time_span = cmdLine.get_flag_int("time");
                emit = cmdLine.get_flag_bool("emit");
                
                if ( time_span == 0 )
                    factor = 1;
                else
                    factor = ( (double)time_span - 1 ) / (double)time_span;

            }

            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // Parse common key
                if( inputs[0].num_kvs > 0 )
                    key.parse( inputs[0].kvs[0]->key );
                else
                    key.parse( inputs[1].kvs[0]->key );

                // Recover state if any
                
                double total = 0;
                time_t t = time(NULL);
                
                if( inputs[1].num_kvs > 0 )
                {
                    value.parse( inputs[1].kvs[0]->value );
                    
                    if( value.isVector() && (value.get_vector_size() == 2) )
                    {
                        // Recover previous 
                        total = value.get_value_from_vector(0)->getDouble();
                        
                        // Forgetting factor 
                        size_t time_diff = t - value.get_value_from_vector(1)->getDouble();
                        total = total * pow( factor  , time_diff );
                    }
                    
                    double tmp =  value.getDouble();
                    total += tmp;
                }                
                
                for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
                {
                    value.parse( inputs[0].kvs[i]->value );
                    double tmp =  value.getDouble();
                    total += tmp;
                }

                
                // Create the output value as a vector [ total time ]
                value.set_as_vector();
                value.add_value_to_vector()->set_double(total);
                value.add_value_to_vector()->set_double(t);

                // Emit to update the state
                writer->emit( 1 , &key , &value );

                // Emit to output if necessary
                if( emit )
                    writer->emit( 0 , &key , &value );
                
                return;
            }
        };    
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduce_update_last
        //
        //       Emit the average of values ( assuming numbers )
        // -----------------------------------------------------------------------------------------------
        
        
        class ValueReduce_update_last : public ValueReduce
        {
            samson::system::Value key;
            samson::system::Value value;
            
            samson::system::Value output_value;
            
        public:
            
            void run( samson::KVSetStruct* inputs , samson::KVWriter *writer  )
            {
                // If inputs, emit the last input
                if( inputs[1].num_kvs > 0 )
                {
                    size_t num_kvs = inputs[1].num_kvs;
                    key.parse( inputs[1].kvs[num_kvs-1]->key );
                    value.parse( inputs[1].kvs[num_kvs-1]->key );
                    writer->emit( 0 , &key , &value );
                    return;
                }
                
                if( inputs[0].num_kvs > 0 )
                {
                    size_t num_kvs = inputs[0].num_kvs;
                    key.parse( inputs[0].kvs[num_kvs-1]->key );
                    value.parse( inputs[0].kvs[num_kvs-1]->key );
                    writer->emit( 0 , &key , &value );
                    return;
                }
                
            }
        };    
        
        
        
        // -----------------------------------------------------------------------------------------------
        // ValueReduceManager manager
        //
        //     Factory of ValueReduce operations
        // -----------------------------------------------------------------------------------------------
        
        typedef ValueReduce*(*factory_ValueReduce)();        
        
        template<class C>
        ValueReduce* factory_ValueReduce_impl()
        {
            return new C();
        }
        
        class ValueReduceManager
        {
            
            // Map with all available commands
            std::map<std::string, factory_ValueReduce> factories;
            
        public:
            
            ValueReduceManager( std::string type )
            {
                
                if( type == "reduce" )
                {
                    
                    add<ValueReduce_all>("all");
                    add<ValueReduce_unique>("unique");
                    add<ValueReduce_unique_counter>("unique_counter");
                    
                    add<ValueReduce_top>("top");                    // Expect key - value
                    add<ValueReduce_top_concept>("top_concept");    // Expect key - [ value counter ]
                    
                    add<ValueReduce_sum>("sum");
                    add<ValueReduce_average>("average");
                }
                
                if( type == "update" )
                {
                    add<ValueReduce_update_last>("last");        // Keep the last value
                    add<ValueReduce_update_sum>("sum");          // Keep the total sum of seen values
                }
                
            }
            
            template<class C>
            void add( std::string name )
            {
                factories.insert( std::pair<std::string, factory_ValueReduce>(name, factory_ValueReduce_impl<C>));
            }
            
            ValueReduce * getInstance( std::string command )
            {
                // Discover the main command
                au::CommandLine cmdLine;
                cmdLine.parse( command );
                
                if( cmdLine.get_num_arguments() == 0 )
                    return  NULL;
                
                // Main command
                std::string name = cmdLine.get_argument(0);
                
                std::map<std::string, factory_ValueReduce>::iterator it_factories;
                it_factories = factories.find( name );
                
                if( it_factories == factories.end() )
                    return NULL;
                else
                {
                    ValueReduce* value_reduce =  it_factories->second();
                    value_reduce->init( command );
                    return value_reduce;
                }
                
            }
            
            std::string getListOfCommands()
            {
                std::ostringstream output;
                
                std::map<std::string, factory_ValueReduce>::iterator it_factories;
                for( it_factories = factories.begin() ; it_factories != factories.end() ; it_factories++ )
                    output << it_factories->first << " ";
                return output.str();
            }
            
        };
    }
}

#endif
