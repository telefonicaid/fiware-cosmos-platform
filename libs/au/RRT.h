
/* ****************************************************************************
 *
 * FILE            RRT
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Class used to mantain samples for monitorization
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/


#ifndef _H_AU_RRT
#define _H_AU_RRT

#include <cstring>
#include <string>    // std::String
#include <time.h>
#include <list>

#include "au/xml.h"

namespace au
{



template <class T,int n>
   class RRT
{
        
public:

   std::list<T> elements;
   
   RRT()
   {
	  while( elements.size() < n )
		 elements.push_back( 0 );
   }
   
   void push( T value )
   {
	  elements.push_back( value );
            
	  while (elements.size() > n)
		 elements.pop_front();
   }

   
   T get( int p )
   {

	  int c = 0;
	  typename std::list<T>::iterator it;
	  for ( it = elements.begin() ; it != elements.end() ; it++ )
	  {
		 if( c == p)
			return *it;
		 c++;
	  }

	  return 0;
   }

   
};   

}

#endif
