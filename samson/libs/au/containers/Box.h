
/* ****************************************************************************
 *
 * FILE            Box
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Box of instances of a partiuclar type using shared poitners
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef AU_CONTAINERS_BOX_H_
#define AU_CONTAINERS_BOX_H_

#include <list>		// std::list
#include "logMsg/logMsg.h"					 // LM_M()
#include "au/containers/SharedPointer.h"

namespace au {
  
  template <class V>
  class Box
  {
    
  public:
    
    void Insert( const SharedPointer<V>& shared_pointer_v )
    {
      V* v = shared_pointer_v.shared_object();
      
      if( Contains( shared_pointer_v ) )
        return; // Already included in the box
      
      items_.insert( std::pair<V*, au::SharedPointer<V> >( v , shared_pointer_v ) );
    }

    // Handy function to inser direct objects
    void Insert( V* v )
    {
      SharedPointer<V> tmp_v(v);
      Insert(tmp_v);
    }

    
    void Erase( const SharedPointer<V>& shared_pointer_v )
    {
      V* v = shared_pointer_v.shared_object();
      typename std::map<V*, au::SharedPointer<V> >::iterator iter = items_.find( v );
      
      if( iter == items_.end() )
        return; // Not really included in the box
      else
        items_.erase( iter ); // Remove entry
    }
    
    bool Contains( const SharedPointer<V>& shared_pointer_v )
    {
      V* v = shared_pointer_v.shared_object();
      return  ( items_.find( v ) != items_.end() );
    }

    size_t size()
    {
      return items_.size();
    }
    
    std::vector< SharedPointer<V> > items()
    {
      std::vector< SharedPointer<V> > vector;
      typename std::map< V*, SharedPointer<V> >::iterator iter;
      for ( iter = items_.begin() ; iter != items_.end() ; iter++ )
        vector.push_back( iter->second );
      return vector;
    }
    
  private:

    // List of elements included in the box
    std::map< V*, SharedPointer<V> > items_;

    
  };
  
}

#endif
