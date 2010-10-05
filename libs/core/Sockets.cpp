// Implementation of the ClientSocket class

#include <cerrno>
#include <fcntl.h>

#include "Sockets.h"



namespace au {

#pragma mark Socket -----
	
	
	Socket::Socket() :
	m_sock ( -1 )
	{
		memset ( &m_addr,
				0,
				sizeof ( m_addr ) );
	}
	
	Socket::~Socket()
	{
		close();
	}
	
	bool Socket::create()
	{
		m_sock = socket ( AF_INET,
						 SOCK_STREAM,
						 0 );
		
		if ( ! is_valid() )
			return false;
		
		
		// TIME_WAIT - argh
		int on = 1;
		if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
			return false;
		
		
		return true;
		
	}
	
	
	
	bool Socket::bind ( const int port )
	{
		
		if ( ! is_valid() )
		{
			return false;
		}
		
		m_addr.sin_family = AF_INET;
		m_addr.sin_addr.s_addr = INADDR_ANY;
		m_addr.sin_port = htons ( port );
		
		int bind_return = ::bind ( m_sock,
								  ( struct sockaddr * ) &m_addr,
								  sizeof ( m_addr ) );
		
		
		if ( bind_return == -1 )
		{
			return false;
		}
		
		return true;
	}
	
	
	bool Socket::listen() const
	{
		if ( ! is_valid() )
		{
			return false;
		}
		
		int listen_return = ::listen ( m_sock, MAXCONNECTIONS );
		
		
		if ( listen_return == -1 )
		{
			return false;
		}
		
		return true;
	}
	
	
	bool Socket::accept ( Socket *new_socket ) const
	{
		int addr_length = sizeof ( m_addr );
		new_socket->m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );
		
		if ( new_socket->m_sock <= 0 )
			return false;
		else
			return true;
	}
		
	ssize_t Socket::send ( const char *buffer , size_t length ) const
	{
		int status = ::send ( m_sock, (void*)buffer, length, 0 );
		if ( status == -1 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	

	ssize_t Socket::recv_waiting ( char *buffer , size_t max_length ) const
	{
		return ::recv ( m_sock, (void*)buffer, max_length, MSG_WAITALL );
	}
	
	ssize_t Socket::recv ( char *buffer , size_t max_length ) const
	{
		return ::recv ( m_sock, (void*)buffer, max_length, 0 );
	}
	
	
	
	bool Socket::connect ( const std::string host, const int port )
	{
		if ( ! is_valid() ) return false;
		
		m_addr.sin_family = AF_INET;
		m_addr.sin_port = htons ( port );
		
		int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );
		
		if ( errno == EAFNOSUPPORT ) return false;
		
		status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );
		
		if ( status == 0 )
			return true;
		else
			return false;
	}
	
	void Socket::close()
	{
		if ( is_valid() )
		{
			::close ( m_sock );
			m_sock = -1;
		}
	}
	
	void Socket::set_non_blocking ( const bool b )
	{
		
		int opts;
		
		opts = fcntl ( m_sock,
					  F_GETFL );
		
		if ( opts < 0 )
		{
			return;
		}
		
		if ( b )
			opts = ( opts | O_NONBLOCK );
		else
			opts = ( opts & ~O_NONBLOCK );
		
		fcntl ( m_sock,
			   F_SETFL,opts );
		
	}

#pragma mark ClientSocket
	
	ClientSocket::ClientSocket ( std::string host, int port )
	{
	  if ( ! Socket::create() )
		{
		  throw SocketException ( "Could not create client socket." );
		}

	  if ( ! Socket::connect ( host, port ) )
		{
		  throw SocketException ( "Could not connect to host." );
		}
	}

	const Socket& Socket::operator << ( const std::string& s ) const
	{
	  if ( ! send ( (char*)s.c_str() , s.length() ) )
		{
		  throw SocketException ( "Could not write to socket." );
		}

	  return *this;

	}


	const Socket& Socket::operator >> ( std::string& s ) const
	{
		char buffer[1000];
		ssize_t length = recv( buffer , 1000 );
		
		buffer[length]='\0';
		s = buffer;
		
		if ( length == -1 )
		{
		  throw SocketException ( "Could not read from socket." );
		}

	  return *this;
	}
	
#pragma mark ServerSocket
	

	ServerSocket::ServerSocket ( int port )
	{
	  if ( ! Socket::create() )
		{
		  throw SocketException ( "Could not create server socket." );
		}

	  if ( ! Socket::bind ( port ) )
		{
		   char errorText[256];

		   snprintf(errorText, sizeof(errorText), "error binding to port %d: %s", port, strerror(errno));
		   throw SocketException(errorText);
		}

	  if ( ! Socket::listen() )
		{
		  throw SocketException ( "Could not listen to socket." );
		}

	}

	ServerSocket::~ServerSocket()
	{
	}

	void ServerSocket::accept ( ServerSocket *sock )
	{
	  if ( ! Socket::accept ( sock ) )
		{
		  throw SocketException ( "Could not accept socket." );
		}
	}



	
}
