#ifndef SAMSON_NETWORK_MANAGER_H
#define SAMSON_NETWORK_MANAGER_H

#include <assert.h>
#include <set>
#include <sstream>               /* ostringstream                            */

#include "samsonLogMsg.h"        /* LOG_ERROR                                */
#include "KVSetBuffer.h"         /* KVSetBuffer                              */
#include "Sockets.h"             /* au::Socket                               */
#include "Packet.h"              /* Packet                                   */
#include "Lock.h"                /* Lock                                     */
#include "NetworkConnection.h"   /* NetworkConnection                        */



/**
 SAMSON Network Manager
*/


#define SAMSON_PORT			9999
#define SAMSON_SIMPLE_PORT	9998


namespace ss {
	
	template <class T>
	void* thread_task(void* p)
	{
		((T*)p)->run_thread();
		return NULL;
	}	



	
	
	/**
	 Manager of the IO for network
	 */
	
	class NetworkManager
	{
		std::set<NetworkConnection*> connections;
		
		au::Lock lock;	
		pthread_t thread;

		NetworkManager(  ) 
		{
			// Create the thread
			int ans = pthread_create(&thread, NULL, thread_task<NetworkManager>, this);	

			if (ans != 0)
			{
				LOG_ERROR(("pthread_create error"));
				assert(0);
			}
		}
		
	public:
		
		static NetworkManager* shared();
		
		void close()
		{
			lock.lock();
			
			for (std::set<NetworkConnection*>::iterator i = connections.begin() ; i != connections.end() ; i++)
			{
				(*i)->close();
				delete *i;
			}
			
			lock.unlock();
			
		}

		void purgeConnections()
		{
			lock.lock();
			
			std::set<NetworkConnection*> remove_connections;
			for (std::set<NetworkConnection*>::iterator i = connections.begin() ; i != connections.end() ; i++)
			{
				if( (*i)->finished )
					remove_connections.insert( *i );
			}

			for (std::set<NetworkConnection*>::iterator i = remove_connections.begin() ; i != remove_connections.end() ; i++)
			{
				connections.erase( *i );
				delete	(*i);
			}
			
			
			lock.unlock();
			
		}
		
		std::string showConnections()
		{
			std::ostringstream o;
			
			lock.lock();

			o << "Connections:" << std::endl;
			o << "--------------------------------" << std::endl;
			
			for (std::set<NetworkConnection*>::iterator i = connections.begin() ; i != connections.end() ; i++)
				o << (*i)->str() << std::endl;
			
			lock.unlock();
			
			return o.str();
		}
		
		
		void run_thread()
		{
			
			try
			{
				// Create the listening socket
				au::ServerSocket server ( SAMSON_PORT );
				
				while ( true )
				{    
					//create the conversational socket
					NetworkConnection *connection = new NetworkConnection();
					
					server.accept ( connection );

					lock.lock();
					connections.insert( connection );
					lock.unlock();
					
					int ans = pthread_create(&connection->thread, NULL, thread_task<NetworkConnection>, connection);	

					if (ans != 0)
					{
						LOG_ERROR(("pthread_create error"));
						assert(0);
					}
				}
			}
			catch ( au::SocketException& e )
			{
				std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
			}
			
		}
		
	};
	
	
#pragma mark Network manager for simple interface
	
	/**
	 Incoming connection
	 */
	
	class SimpleNetworkConnection : public au::ServerSocket
	{
		
	public:
		pthread_t thread;			//!< thread that is working with this connection
		bool finished;				//!< Flag to indicate that it is not finished
		
		SimpleNetworkConnection()
		{
			finished = false;
			set_non_blocking(true);	// non bloquing threads to be able to see if new messages are comming without bloquing
		}
		
		void run_thread();
		
		std::string str( )
		{
			std::ostringstream o;
			
			o << "Connection ";
			if( finished )
				o << "(F)";
			
			return o.str();
		}
		
	};
	
	
	
	class SimpleNetworkManager
	{
		au::Lock lock;	
		std::set<SimpleNetworkConnection*> connections;
		pthread_t thread;
		
		SimpleNetworkManager(  ) 
		{
			// Create the thread
			int ans = pthread_create(&thread, NULL, thread_task<SimpleNetworkManager>, this);	

			if (ans != 0)
			{
				LOG_ERROR(("pthread_create error"));
				assert(0);
			}
		}
		
	public:
		
		static SimpleNetworkManager* shared(); 
		
		
		void close()
		{
			lock.lock();
			
			for (std::set<SimpleNetworkConnection*>::iterator i = connections.begin() ; i != connections.end() ; i++)
			{
				(*i)->close();
				delete *i;
			}
			
			lock.unlock();
		}
		
		void purgeConnections()
		{
			lock.lock();
			
			std::set<SimpleNetworkConnection*> remove_connections;
			for (std::set<SimpleNetworkConnection*>::iterator i = connections.begin() ; i != connections.end() ; i++)
			{
				if( (*i)->finished )
					remove_connections.insert( *i );
			}
			
			for (std::set<SimpleNetworkConnection*>::iterator i = remove_connections.begin() ; i != remove_connections.end() ; i++)
			{
				connections.erase( *i );
				delete	(*i);
			}
			
			
			lock.unlock();

		}
		
		std::string showConnections()
		{
			std::ostringstream o;
			
			lock.lock();
			
			o << "Connections:" << std::endl;
			o << "--------------------------------" << std::endl;
			
			for (std::set<SimpleNetworkConnection*>::iterator i = connections.begin() ; i != connections.end() ; i++)
				o << (*i)->str() << std::endl;
			
			lock.unlock();
			
			return o.str();
		}
		
		
		void run_thread()
		{
			
			try
			{
				// Create the listening socket
				au::ServerSocket server ( SAMSON_SIMPLE_PORT );
				
				while ( true )
				{    
					//create the conversational socket
					SimpleNetworkConnection *connection = new SimpleNetworkConnection();
					
					server.accept ( connection );
					
					lock.lock();
					connections.insert( connection );
					lock.unlock();
					
					int ans = pthread_create(&connection->thread, NULL, thread_task<SimpleNetworkConnection>, connection);	

					if (ans != 0)
					{
						LOG_ERROR(("pthread_create error"));
						assert(0);
					}
				}
			}
			catch ( au::SocketException& e )
			{
				std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
			}
			
		}
		
	};
}

#endif
