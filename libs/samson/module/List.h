#ifndef SAMSON_LIST_H
#define SAMSON_LIST_H


#include <list>							// std::map
#include <sstream>
#include <samson/module/DataInstance.h>		// samson::Environment


/**
    
    List    Class used in SAMSON modules in combination with samsonModulesParser command
            It contains a collection of otherDataType elements
 
make            It focused to obtimice the reuse of elements
 
 **/

namespace samson {
    
    
    template < class T >
    class List 
    {
        std::list<T*> reuse_elements;
        
    public:

        std::list<T*> elements;
        
        ~List()
        {
            // Delete all elements in the reuse queue
            typename std::list<T*>::iterator it_elements;
            for (it_elements = reuse_elements.begin() ; it_elements != reuse_elements.end() ; it_elements++ )
                delete (*it_elements);
            reuse_elements.clear();
        }
        
        // Iterate thougth the entire list
        typename std::list<T*>::iterator it;
        
        void clear_iterator()
        {
            it = elements.begin();
        }
        
        T* getNext( )
        {
            if( it == elements.end() )
                return NULL;
            else
            {
                T* t = *it;
                it++;
                return t;
            }
        }
        
        // Add an element at both sides of the list
        
        T* add_back()
        {
            T* t = get_new_element();
            elements.push_back( t );
            return t;
        }
    
        T* add_front()
        {
            T* t = get_new_element();
            elements.push_front( t );
            return t;
        }
        
        T* add( typename std::list<T*>::iterator it )
        {
            T* t = get_new_element();
            elements.insert( it , t );
            return t;
        }

        void add_back(T* t)
        {
            elements.push_back( t );
        }
        
        void add_front(T* t)
        {
            elements.push_front( t );
        }
        
        T* get_front()
        {
            if( elements.size() == 0 )
                return NULL;
            return *elements.front();
        }

        T* get_back()
        {
            if( elements.size() == 0 )
                return NULL;
            return *elements.back();
        }

        T* extract_front()
        {
            if( elements.size() == 0 )
                return NULL;
            T*t =  elements.front();
            elements.pop_front();
            return t;
        }
        
        T* extract_back()
        {
            if( elements.size() == 0 )
                return NULL;
            T*t =  elements.back();
            elements.pop_back();
            return t;
        }
        
        void remove_front()
        {
            T*t = extract_front();
            if( t )
                push_to_reuse( t );
        }
        
        void remove_back()
        {
            T*t = extract_back();
            if( t )
                push_to_reuse( t );
        }
        
        size_t count()
        {
            return elements.size();
        }
        
        // DataType necessary operations
        
        int parse( char *data )
        {
            
            clear();    // Prepare this element to parse again content
            
            int offset = 0;
            
            size_t _length; // Number of elements
            offset += samson::staticVarIntParse( data+offset , &_length );
            
            for (int i = 0 ; i < (int)_length ; i++)                
            { 
                T* t = add_back();
                offset += t->parse(data+offset);
            }
            
            return offset;
        }
        
        int serialize( char *data )
        {
            
            int offset = 0;
            offset += samson::staticVarIntSerialize( data+offset , elements.size() );
            
            typename std::list<T*>::iterator it_elements;
            for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
                offset += (*it_elements)->serialize(data+offset);
            
            printf("Serializing list of %d elements with %d bytes" , (int) elements.size() , offset );

            return offset;
        }

	void setFromString(const char *_value_data)
	{
		typename std::list<T*>::iterator it_elements;
		for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
		{
			(*it_elements)->setFromString(_value_data);
		}
	}
        
        void clear()
        {
            // Put all elements to be reused
            typename std::list<T*>::iterator it_elements;
            for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
                push_to_reuse(*it_elements);
            elements.clear();
        }
        
        DataInstance * getDataInstanceFromPath(const int *dataPathIntP)
        {
            // Not implemented
            return NULL;
        }
        
        void copyFrom( List<T>* other )
        {
            // Copy content from another list 
        }
        
        std::string str()
        {
            std::ostringstream output;
            output << "[ ";
            
            typename std::list<T*>::iterator it_elements;
            for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
                output << (*it_elements)->str() << " ";
            output << "]";
            return output.str();
        }
        
        std::string strJSON(std::string name)
        {
            std::ostringstream output;
            output << "[ ";
            
            typename std::list<T*>::iterator it_elements;
            for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
                output << (*it_elements)->str() << " ";
            output << "]";
            return output.str();
        }
        
        std::string strJSONInternal(std::string name, bool _vectorMember)
        {
            std::ostringstream output;
            output << "[ ";
            
            typename std::list<T*>::iterator it_elements;
            for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
                output << (*it_elements)->str() << " ";
            output << "]";
            return output.str();
        }
        
        std::string strXML(std::string name)
        {
            std::ostringstream output;
	    output << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
            typename std::list<T*>::iterator it_elements;
            for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
	    {
	        output << (*it_elements)->strXMLInternal(name);
            }
            return output.str();
        }
        
        std::string strXMLInternal(std::string name)
        {
            std::ostringstream output;
            typename std::list<T*>::iterator it_elements;
            for (it_elements = elements.begin() ; it_elements != elements.end() ; it_elements++ )
	    {
	        output << (*it_elements)->strXMLInternal(name);
            }
            return output.str();
        }

        int hash( int max_num )
        {
            if( elements.size() == 0)
                return 0;
            
            return elements.front()->hash( max_num );
            
        }
        
        static size_t size( char * data )
        {
            size_t _length;
            size_t offset = 0;
            
            // Get the length of elements
            offset += samson::staticVarIntParse( data+offset , &_length );
            
            T _tmp;
            for (int i = 0 ; i < (int)_length ; i++)
            { //Sizing v
                offset += _tmp.parse(data+offset);
            }            
            
            return offset;
        }
                           
      
        /**
         Reuse mechanism
         **/
        
        // Get a new element ( or a reused one if possible )
        T* get_new_element()
        {
            if( reuse_elements.size() == 0 )
                return new T();
            
            T* t = reuse_elements.front();
            reuse_elements.pop_front();
            
            return t;
        }        
        
        // Push an element to be reused
        void push_to_reuse( T* t )
        {
            reuse_elements.push_back( t );
        }
        
        
    };
    
}

#endif
