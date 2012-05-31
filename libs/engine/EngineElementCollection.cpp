
#include "au/tables/Table.h"

#include "engine/EngineElement.h"
#include "engine/EngineElementCollection.h" // Own interface


namespace engine 
{
    EngineElementCollection::EngineElementCollection() : token("EngineElementCollection")
    {
    }
    
    EngineElementCollection::~EngineElementCollection()
    {
        au::TokenTaker tt(&token);
        
        // Remove pending elements in Engine
        repeated_elements.clearList();
        normal_elements.clearList();
        extra_elements.clearVector();
    }

    
    void EngineElementCollection::add( EngineElement* element )
    {
        au::TokenTaker tt(&token);
        
        // Insert an element in the rigth queue
        LM_T( LmtEngine, ("Adding Engineelement: %s", element->str().c_str() ));
        
        if( element->isRepeated() )
            repeated_elements.insert( _find_pos_in_repeated_elements( element ) ,  element );
        else if( element->isExtra() )
            extra_elements.push_back( element );
        else
            normal_elements.push_back( element );
    }
    
    
    bool EngineElementCollection::isEmpty()
    {
        au::TokenTaker tt(&token);
        
        if ( repeated_elements.size() > 0 )
            return false;
        if ( normal_elements.size() > 0 )
            return false;
        if ( extra_elements.size() > 0 )
            return false;
        
        return true;
    }
    
    EngineElement* EngineElementCollection::getNextRepeatedEngineElement()
    {
        au::TokenTaker tt(&token);
        
        // Check first repeated elements
        if( repeated_elements.size() == 0 )
            return NULL;
        
        double t_sleep = repeated_elements.front()->getTimeToTrigger();
        
        // If ready to be executed....
        if( t_sleep < 0.01 )
        {
            EngineElement * element = repeated_elements.front();
            repeated_elements.pop_front();
            return element;
        }
        else
            return NULL;
    }
    
    EngineElement*  EngineElementCollection::getNextNormalEngineElement()
    {
        au::TokenTaker tt(&token);
        
        if( normal_elements.size() == 0 )
            return NULL;
        
        EngineElement * element = normal_elements.front();
        normal_elements.pop_front();
        
        return element;
    }
    
    double EngineElementCollection::getTimeForNextRepeatedEngineElement()
    {
        au::TokenTaker tt(&token);
        
        // Check first repeated elements
        if( repeated_elements.size() == 0 )
            return 0;
        
        double t_sleep = repeated_elements.front()->getTimeToTrigger();
        
        // If ready to be executed....
        if( t_sleep < 0 )
            return 0;
        
        return t_sleep;
    }
    
    
    size_t EngineElementCollection::getNumEngineElements()
    {
        size_t total = 0;
        total += repeated_elements.size();
        total += normal_elements.size();
        total += extra_elements.size();
        
        return total;
    }
    
    size_t EngineElementCollection::getNumNormalEngineElements()
    {
        return normal_elements.size();
    }
    
    std::string EngineElementCollection::getTableOfEngineElements()
    {
        au::TokenTaker tt(&token);
        
        au::tables::Table table("Type|Waiting,f=double|Period,f=double|Description,left");
        
        au::list<EngineElement>::iterator it_elements;
        for( it_elements = repeated_elements.begin() ; it_elements != repeated_elements.end() ; it_elements++ )
        {
            EngineElement* element = *it_elements;
            
            au::StringVector values;
            values.push_back("Repeat");
            values.push_back(au::str( "%.12f" , element->getWaitingTime() ));
            values.push_back(au::str( "%.12f" , element->getPeriod() ));
            //values.push_back(element->getName());
            values.push_back(element->getDescription());
            table.addRow( values );
        }
        
        for( it_elements = normal_elements.begin() ; it_elements != normal_elements.end() ; it_elements++ )
        {
            EngineElement* element = *it_elements;
            
            au::StringVector values;
            values.push_back("Normal");
            values.push_back(au::str( "%.12f" , element->getWaitingTime() ));
            values.push_back("X");
            //values.push_back(element->getName());
            values.push_back(element->getDescription());
            table.addRow( values );
        }
        
        for ( size_t i = 0 ; i < extra_elements.size() ; i++ )
        {
            EngineElement* element = extra_elements[i];
            
            au::StringVector values;
            values.push_back("Extra");
            values.push_back(au::str( "%.12f" , element->getWaitingTime() ) );
            values.push_back("X");
            //values.push_back(element->getName());
            values.push_back(element->getDescription());
            table.addRow( values );
        }        
        
        return table.str();
    }

    
    void EngineElementCollection::print_elements()
    {
        au::TokenTaker tt(&token);
        
        // Print entire engine items...
        
        au::list<EngineElement>::iterator it_elements;
        for( it_elements = repeated_elements.begin() ; it_elements != repeated_elements.end() ; it_elements++ )
        {
            EngineElement* element = *it_elements;
            LM_M(("ENGINE REPEATED ELEMENT: %s",element->str().c_str()));
        }
        
        for( it_elements = normal_elements.begin() ; it_elements != normal_elements.end() ; it_elements++ )
        {
            EngineElement* element = *it_elements;
            LM_M(("ENGINE NORMAL ELEMENT: %s",element->str().c_str()));
        }
        
        for ( size_t i = 0 ; i < extra_elements.size() ; i++ )
        {
            EngineElement* element = extra_elements[i];
            LM_M(("ENGINE EXTRA ELEMENT: %s",element->str().c_str()));
        }
        
    }
    
    std::vector<EngineElement*> EngineElementCollection::getExtraElements()
    {
        au::TokenTaker tt(&token);
        
        std::vector<EngineElement*> tmp;
        for ( size_t i = 0 ; i < extra_elements.size() ; i++ )
        {
            EngineElement* element = extra_elements[i];
            tmp.push_back( element );
        }
        return tmp;
    }
    
    
    size_t EngineElementCollection::getMaxWaitingTimeInEngineStack()
    {
        au::TokenTaker tt(&token);
        
        if( normal_elements.size() == 0 )
            return 0;
        
        EngineElement* last_element =  normal_elements.back();
        return last_element->getWaitingTime();
    }
    
    

}
