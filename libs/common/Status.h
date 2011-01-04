#ifndef _H_AU_STATUS
#define _H_AU_STATUS

#include <set>
#include <sstream>		// std::ostringstream

namespace au {
	
	/**
	 Class to monitorize run-time status of objetcs
	 */
	
	class Status
	{
		
	public:

		std::string status_title;		// Full text title of this object
		std::string status_nick;		// Nick name of this status object ( for path search )
		
		// Children status elements
		std::set<Status*> status_childrens;

		/**
		 Full function to get status of an object
		 */
		
		std::string getStatus( std::string path )
		{
			std::ostringstream output;
			getStatus( output , path , "");	// Status without any prefix per line
			return output.str();
		}
		
		void setStatusTile( const std::string& _status_title , const std::string&  _status_nick)
		{
			status_title = _status_title;
			status_nick = _status_nick;
		}

		
		void getStatus( std::ostream &output , std::string path , std::string prefix_per_line )
		{
			output << prefix_per_line << status_title;

			if( status_nick.length() > 0)
				output << " (" << status_nick << ")";
			
			output << ":";
			getStatus( output , prefix_per_line + "\t" );

			std::set<Status*>::iterator iter;
			for (iter = status_childrens.begin() ; iter != status_childrens.end() ;iter++)
				(*iter)->getStatus( output , path , prefix_per_line + "\t" );
		}
		
		
		void addChildrenStatus( Status * s )
		{
			status_childrens.insert( s );
		}

		void removeChildrenStatus( Status * s )
		{
			status_childrens.erase( s );
		}
		
		/**
		 Virtual function defined at each object ot get the status
		 */
		
		virtual void getStatus( std::ostream &output , std::string prefix_per_line )
		{
			output<< "\n";
		}
		
		/**
		 Utility function to get the output form a set of elements in a map
		 */
		 
		template< typename K, typename V>
        void getStatusFromMap( std::ostream &output, std::map<K,V*>& m , std::string prefix_per_line )
        {
			typename std::map<K, V* >::iterator iter;
			for ( iter = m.begin() ; iter != m.end() ; iter++ )
			{
				output << prefix_per_line;
				iter->second->getStatus( output , prefix_per_line + "\t\t" );
			}
        }
		
		
		
		
	};
}


#endif
