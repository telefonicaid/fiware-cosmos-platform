/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include <iostream>
#include <list>
#include <sstream>

#include "samson/modules/hit/HitsState.h"

namespace au
{

class TopElement
{
    
public:
    
    size_t hits;
    std::string concept;
    
    TopElement( size_t _hits , std::string _concept )
    {
        hits = _hits;
        concept = _concept;
    }
  
    
    
};

class TopElementList
{



    std::list< TopElement > elements; 
    size_t max_num_elements;

public:
    
    TopElementList( int _max_num_elements )
    {
        max_num_elements = _max_num_elements;
    }

    int size()
    {
    	return elements.size();
    }

    void fill( unsigned long time ,  samson::hit::HitsState* state )
    {
      std::list< TopElement >::iterator i;
      for ( i = elements.begin() ; i != elements.end() ; i++)
      {
	::samson::hit::Hit*hit =  state->hitsAdd();

	hit->time.value = time;
	hit->concept.value = i->concept;
	hit->hits.value = i->hits;

      }

    }

    void insert( size_t hits , std::string & concept )
    {
        // If not enough elements, just insert where
        if( elements.size() == 0 )
        {
            TopElement _new_element( hits , concept );
            elements.insert( elements.begin() ,  _new_element );
            return;
        }
        
        if( elements.front().hits < hits )
        {
            // Top element
            TopElement _new_element( hits , concept );
            elements.insert( elements.begin() , _new_element );            
        }
        else if ( elements.back().hits > hits )
        {
            if( elements.size() < max_num_elements )
            {
                // Add at the end of the vector
                TopElement _new_element( hits , concept );
                elements.insert( elements.end() , _new_element );            
                return;
            }
            
            return;
            
        }
        else
        {
            TopElement _new_element( hits , concept );
            elements.insert( find_pos(hits) ,_new_element );
            
        }
        
        
        while( elements.size() > max_num_elements )
            elements.pop_back();
        
    }
    
    void clear()
    {
        elements.clear();
    }

    void clearAndCopyFrom( TopElementList& other )
    {
        clear();
        for ( std::list< TopElement >::iterator i = other.elements.begin() ; i != other.elements.end() ; i ++)
            insert( i->hits , i->concept );
    }
    
    
    std::list< TopElement >::iterator find_pos( size_t hits )
    {
        std::list< TopElement >::iterator iter = elements.begin();
        
        while( iter->hits > hits )
            iter++;
        
        return iter;
    }

    std::string str()
    {
        std::ostringstream output;
        
        std::list< TopElement >::iterator i;
        
        for ( i = elements.begin() ; i != elements.end() ; i++)
            output << i->concept << " [" << i->hits << "]\n";
        
        return output.str();
    }
    
    
    
};


class ContinuousTopList
{
    unsigned long time;     // Time we are currently working

    public:
    
    TopElementList current_top_list;
    TopElementList top_list;        
    
    ContinuousTopList(  ) : current_top_list( 10 ) , top_list( 10 )
    {
        time = 0;
    }

    
    void insert( unsigned long _time , size_t hits , std::string &concept )
    {
        if ( _time == time )
        {
            current_top_list.insert( hits , concept );
            return;
        }
        
        if( _time > time )
        {
            if ( _time == ( time+1 ) )
            {
                top_list.clearAndCopyFrom( current_top_list );
            }
            else
                top_list.clear();

            // Set the new time reference
            time = _time;
            
            // Clear the current top list
            current_top_list.clear();
            
            // Init the new list with this element
            current_top_list.insert( hits , concept);
            
        }
        
        
    }

    void fill( samson::hit::HitsState* state )
    {
      current_top_list.fill(  time , state );
      top_list.fill(  time - 1 , state );
    }

    std::string str()
    {
        std::ostringstream output;
        
        output << "------------------\n";
        output << "Current top list\n";
        output << "------------------\n";
        
        output << current_top_list.str();

        output << "------------------\n";
        output << "Top list\n";
        output << "------------------\n";

        output << top_list.str();

        return output.str();
    }    
    
};


/*
int main (int argc, const char * argv[])
{

    ContinuousTopList top(10);
    
    for ( int i = 0 ; i < 1000000 ; i++)
    {
        int hits = rand()%100000;
        std::stringstream concept;
        concept << "name_" << i;
        
        unsigned long time = i/100;
        std::string _concept = concept.str();

        top.insert( time , hits , _concept );
        
        std::cout << "\n";
        std::cout << "Adding " << _concept << " " << hits << "\n";
        std::cout << "\n";
        std::cout << top.str();
        std::cout << "\n";
        std::cout << "\n";
        std::cout << "\n";
        std::cout << "\n";
        
        
        usleep(100000);
        
    }
    
    // insert code here...
    std::cout << top.str();
    return 0;
}

*/


} // Namespace au
