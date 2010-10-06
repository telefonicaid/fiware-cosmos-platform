#ifndef SAMSON_SOCKETS_H
#define SAMSON_SOCKETS_H

// Definition of the ClientSocket class

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <fstream>



namespace au {

	const int MAXHOSTNAME = 200;
	const int MAXCONNECTIONS = 5;

	class SocketException
	{
	public:
		SocketException ( std::string s ) : m_s ( s ) {};
		~SocketException (){};
		
		std::string description() { return m_s; }
		
	private:
		std::string m_s;
	};


	class Socket
	{
		
	private:
		int m_sock;
		sockaddr_in m_addr;
		
	 public:
	  Socket();
	  virtual ~Socket();

		int getSocket()
		{
			return m_sock;
		}
		
	  // Server initialization
	  bool create();
	  bool bind ( const int port );
	  bool listen() const;
	  bool accept ( Socket * ) const;

		// Close the socket
	  void close();	
		
	  // Client initialization
	  bool connect ( const std::string host, const int port );

	  void set_non_blocking ( const bool );
	  bool is_valid() const { return m_sock != -1; }

	// Data Transimission
	ssize_t recv (  char *buffer , size_t max_length ) const;
	ssize_t recv_waiting ( char *buffer , size_t max_length ) const;
	ssize_t send ( const char *buffer , size_t length ) const;
		
	const Socket& operator << ( const std::string& s ) const;
	const Socket& operator >> ( std::string& s ) const;
		
		
	};

	class ClientSocket : public Socket
	{
	public:
		ClientSocket ( std::string host, int port );
		virtual ~ClientSocket(){};
	};

	class ServerSocket : public Socket
	{
	public:
		ServerSocket (int port);
		ServerSocket() { };
		virtual ~ServerSocket();
		
		void accept(ServerSocket*);
	};

}

#endif
