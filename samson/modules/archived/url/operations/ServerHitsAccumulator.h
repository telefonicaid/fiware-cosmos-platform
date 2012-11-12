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
#ifndef _H_ServerHitsAccumulator
#define _H_ServerHitsAccumulator



class ServerHitsAccumulator
{
 public:

  // Local list of top 100 servers                                               
  int size;
  size_t *hits;
  std::string *servers;

  ServerHitsAccumulator( )
  {
    size = 0;
    hits = NULL;
    servers = NULL;
  }

  ~ServerHitsAccumulator()
  {
    destroy();
  }

  void destroy()
  {
    if ( hits )
      free( hits );
    if ( servers )
      delete[] servers;

    hits = NULL;
    servers = NULL;
  }
  
  void init( int _size )
  {
    if( size != _size )
    {
      // Destroy if previously ini with different size
      destroy();

      size = _size;
      hits = (size_t*) malloc( sizeof( size_t ) * size );
      servers = new std::string[ size ];    
    }

    // Init the hits vector
    for (int i = 0 ; i < size ; i++ )
      hits[i] = 0;
  }

  void add( std::string &server , size_t local_hits )
  {
    for (int pos = 0 ; pos < size ; pos++)
      {
	if ( local_hits > hits[pos] )
	  {
	    // Move the rest of top elements                                         
	    for (int pos_move = size-1 ; pos_move > pos ; pos_move--)
	      {
		hits[pos_move] = hits[pos_move-1];
		servers[pos_move] = servers[pos_move-1];
	      }

	    // Save the results for this server                                      
	    hits[pos] = local_hits;
	    servers[pos] = server;

	    return; // End for loop                                                   
	  }
      }
    
  } 


};


#endif
