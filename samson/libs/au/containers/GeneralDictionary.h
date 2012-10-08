
/* ****************************************************************************
 *
 * FILE            GlobalDictionary
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2012
 *
 * DESCRIPTION
 *
 *      General ditionary of objects ( retained by shared pointers )
 *      Any object ( whatever class ) can be stored and retrieved
 *
 * COPYRIGTH       Copyright 2012 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef AU_CONTAINERS_GLOBAL_DICTIONARY_H_
#define AU_CONTAINERS_GLOBAL_DICTIONARY_H_

#include <iostream>
#include <map>             // std::map
#include <string.h>

#include "au/containers/SharedPointer.h"
#include "au/containers/Dictionary.h"

#include "logMsg/logMsg.h"  // LM_M()

namespace au {
  
  class GeneralDictionaryWrapper
  {
  public:
    virtual ~GeneralDictionaryWrapper(){};
  };
  
  template<class C>
  class SpetializedGeneralDictionaryWrapper : public GeneralDictionaryWrapper
  {
  public:
    
    SpetializedGeneralDictionaryWrapper( au::SharedPointer<C> c )
    {
      c_=c;
    }
    au::SharedPointer<C> internal_object()
    {
      return c_;
    }
  private:
    au::SharedPointer<C> c_;
  };
  
  class GeneralDictionary {
    
  public:
    
    template< class C>
    void Set( const std::string& name , au::SharedPointer<C> c )
    {
      au::SharedPointer<SpetializedGeneralDictionaryWrapper<C> > wrapped_c( new SpetializedGeneralDictionaryWrapper<C>( c ));
      au::SharedPointer<GeneralDictionaryWrapper> wrapped_object;
      wrapped_object = wrapped_c.template static_pointer_cast<GeneralDictionaryWrapper>();
      dictionary_.Set( name , wrapped_object );
    }
    
    template<class C>
    au::SharedPointer<C> Get( const std::string& name ) const
    {
      au::SharedPointer<GeneralDictionaryWrapper> wrapped_object = dictionary_.Get(name);
      
      // If no object at all, just return NULL
      if( wrapped_object == NULL )
        return au::SharedPointer<C>(NULL);

      // If we have an object, try to cast to provided class
      au::SharedPointer<SpetializedGeneralDictionaryWrapper<C> > wrapped_c;
      wrapped_c = wrapped_object.dynamic_pointer_cast<SpetializedGeneralDictionaryWrapper<C> >();
      if( wrapped_c == NULL )
        return au::SharedPointer<C>(NULL);
      
      return wrapped_c->internal_object();
    }

  private:
    
    Dictionary<std::string,GeneralDictionaryWrapper> dictionary_;
    
  };

}

#endif