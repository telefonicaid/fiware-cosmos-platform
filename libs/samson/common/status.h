#ifndef _H_SAMSON_STATUS
#define _H_SAMSON_STATUS

namesapace samson
{
  typedef enum Status
  {
    OK,
    Error,
    
  }

  const char* status( Status s )
  {
      switch( s )
      {
          case OK:      return "OK";
          case Error:   return "Error";
      }
  }

}


#endif
