#ifndef HOST_H
#define HOST_H

/* ****************************************************************************
*
* FILE                     Host.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 16 2011
*
*/





/* ****************************************************************************
*
* Host - 
*/
class Host
{
    std::set<std::string> alias;
    
    public:

    Host()
    {
        
    }

    Host( std::string host_name )
    {
        alias.insert(host_name);
    }
    
    bool match( std::string host_name  )
    {
        std::set<std::string>::iterator it_alias;
        for (it_alias = alias.begin() ; it_alias != alias.end() ; it_alias++ )
            if( *it_alias == host_name )
                return true;
        return false;
    }
    
    void addAlias( std::string new_alias )
    {
        alias.insert(new_alias);
    }
    
    size_t getNumAliases()
    {
        return alias.size();
    }

    std::vector<std::string> getAllAliases()
    {
        std::vector<std::string> aliases;
        std::set<std::string>::iterator it_alias;
        for (it_alias = alias.begin() ; it_alias != alias.end() ; it_alias++ )
            aliases.push_back( *it_alias );
        return aliases;
    }

    std::string str()
    {
        std::ostringstream output;
        std::set<std::string>::iterator it_alias;
        for (it_alias = alias.begin() ; it_alias != alias.end() ; )
        {
            output << *it_alias;
            it_alias++;
            if ( it_alias != alias.end() )
                output << ",";
            
        }
        return output.str();
    }
    
    std::string getAlternative( std::string host_name )
    {
        if( !match(host_name) )
            return host_name;

        std::set<std::string>::iterator it_alias;
        for (it_alias = alias.begin() ; it_alias != alias.end() ; )
        {
            if( host_name != *it_alias )
                return *it_alias;
        }
        
        return host_name;
    }
    
    static bool onlyDigitsAndDots( std::string host_name )
    {
        if( host_name == "" )
            return false;
        
        for (size_t ix = 0; ix < host_name.length() ; ix++)
        {
            
            if (host_name[ix] == '.')
                continue;
            
            if ((host_name[ix] >= '0') && (host_name[ix] <= '9'))
                continue;
            
            return false;
        }
        
        return true;
    }
    
    
} ;

#endif
