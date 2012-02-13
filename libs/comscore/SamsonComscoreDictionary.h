
#include "logMsg/logMsg.h"

#include <string.h>
#include <fnmatch.h>

#include "au/string.h"
#include "au/StructCollection.h"
#include "au/StringCollection.h"

#include "comscore/common.h"    // common definitions
#include "comscore/MapId2Id.h"  // MapId2Id

bool match( const char* s1 , const char* s2 );

namespace samson
{
    namespace comscore
    {
       
        // -----------------------------------------------------------------
        // URL
        // 
        // Utility class to split url in parts
        // 
        // -----------------------------------------------------------------
        
        class URL
        {
            
        public:
            
            std::string domain;
            std::string pre_domain;
            std::string core_domain;
            std::string path;
            
            URL( std::string url );
        };
        
        // -----------------------------------------------------------------
        // DictionaryEntry
        // 
        // Structure for main dictionary element
        // 
        // -----------------------------------------------------------------

        
        typedef struct
        {
            uint id;
            uint domain;
            uint pre_domain_pattern;
            uint path_pattern;
            
        } DictionaryEntry;
        
     
        // -----------------------------------------------------------------
        // Header
        // 
        // HEader used for read or write dictionary to disk
        // 
        // -----------------------------------------------------------------
        
        typedef struct 
        {
            // StringCollection
            // StructCollection<SamsonDictionaryEntry>
            
            size_t size_string_collection;
            size_t size_struct_collection_dictionary_entries;
            size_t size_struct_collection_pattern_to_category;
            size_t size_struct_collection_categories;
            
        } Header;
        
        
        // -----------------------------------------------------------------
        // SamsonComscoreDictionary
        // 
        // Main class for TID implementation of comscore dictionary 
        //
        // -----------------------------------------------------------------
        
        class SamsonComscoreDictionary
        {
            Header header;
            au::StringCollection string_collection;
            au::StructCollection<DictionaryEntry> dictionary_entries;
            MapId2Id pattern_to_category;
            MapId2Id categories;
            
        public:
            
            SamsonComscoreDictionary()
            {
                
            }
            
            void push( OriginalDictionaryEntry& original_dictionary_entry )
            {
                DictionaryEntry entry;
                entry.id = original_dictionary_entry.id;
                entry.domain             = string_collection.add( original_dictionary_entry.domain.c_str() );
                entry.pre_domain_pattern = string_collection.add( original_dictionary_entry.pre_domain_pattern.c_str() );
                entry.path_pattern       = string_collection.add( original_dictionary_entry.path_pattern.c_str() );
                
                dictionary_entries.add( entry );
            }
            
            void push_pattern_to_category( Id2Id entry )
            {
                pattern_to_category.add( entry );
            }
            
            void push_category(uint id , std::string category )
            {
                Id2Id entry;
                entry.first = id;
                entry.second = string_collection.add( category.c_str() );
                
                categories.add( entry );
                
            }
            
            void write( const char* file_name )
            {
                
                FILE* file =fopen( file_name , "w" );
                if( !file )
                    LM_X(1, ("Not possible to open file %s to write a SamsonComscoreDictionary" , file_name ));
                
                // Prepare & write header
                header.size_string_collection = string_collection.getSize();
                header.size_struct_collection_dictionary_entries = dictionary_entries.getSize();
                header.size_struct_collection_pattern_to_category = pattern_to_category.getSize();
                header.size_struct_collection_categories = categories.getSize();
                
                if ( fwrite(&header, sizeof(Header), 1, file) != 1 )
                    LM_X(1, ("Error writin to file %s to create a SamsonComscoreDictionary" , file_name ));
                
                
                LM_M(("Writing file %s with ( String %s ) ( Dictionary %s ) ( Pattern2Category %s ) ( Category2Description %s )" 
                      , file_name
                      , au::str( header.size_string_collection ).c_str()
                      , au::str( header.size_struct_collection_dictionary_entries ).c_str()
                      , au::str( header.size_struct_collection_pattern_to_category ).c_str()
                      , au::str( header.size_struct_collection_categories ).c_str()
                      ));
                
                
                // Write String collection
                string_collection.write(file);
                
                // Write dictionary_entries
                dictionary_entries.write(file);
                
                // Write pattern_to_category
                pattern_to_category.write(file);
                
                // Write category description mapping
                categories.write(file);
                
            }
            
            
            void read( const char* file_name )
            {
                FILE* file =fopen( file_name , "r" );
                if( !file )
                    LM_X(1, ("Not possible to open file %s to recover SamsonComscoreDictionary" , file_name ));
                
                
                if( fread(&header, sizeof( Header ), 1, file ) != 1 )
                    LM_X(1, ("Error reading %s while recovering SamsonComscoreDictionary" , file_name));
                
                // Recover string collection
                string_collection.read(file, header.size_string_collection );
                
                // Recover dictionary_entries
                dictionary_entries.read( file , header.size_struct_collection_dictionary_entries );
                
                // Recover pattern to category
                pattern_to_category.read( file , header.size_struct_collection_pattern_to_category );
                
                
                // Recover cateories
                categories.read( file , header.size_struct_collection_categories );
                
                fclose( file );
                
                LM_M(("Read file %s with ( String %s ) ( Dictionary %s ) ( Pattern2Category %s ) ( Category2Description %s )" 
                      , file_name
                      , au::str( header.size_string_collection ).c_str()
                      , au::str( header.size_struct_collection_dictionary_entries ).c_str()
                      , au::str( header.size_struct_collection_pattern_to_category ).c_str()
                      , au::str( header.size_struct_collection_categories ).c_str()
                      ));
                
            }
            
            
            bool find_pattern_range( const char* core_domain , uint *begin , uint* end )
            {
                uint pos = find_one_pattern( core_domain );
                
                if ( pos == (uint) -1 )
                    return false;
                else
                {
                    *begin = pos;
                    *end = pos;
                    
                    // Move limits while same core domain
                    while( (*begin>1) && (strcmp(core_domain, &string_collection.v[dictionary_entries.v[*begin-1].domain] ) == 0) )
                        *begin = *begin-1;
                    while( 
                          (*end<(uint)dictionary_entries.size-2) 
                          && 
                          (strcmp(core_domain, &string_collection.v[dictionary_entries.v[*end+1].domain] ) == 0) 
                          )
                        *end = *end+1;
                    
                    return true;
                    
                }
            }
            
            uint find_one_pattern( const char* core_domain )
            {
                uint begin = 0;
                uint end = dictionary_entries.size;
                
                if ( strcmp(core_domain, get_domain_for_pattern(begin) ) == 0 )
                    return begin;
                if ( strcmp(core_domain, get_domain_for_pattern(end) ) == 0 )
                    return end;
                
                return find_one_pattern(core_domain , begin , end );
                
            }
            
            
            const char* get_domain_for_pattern( uint pos )
            {
                uint string_pos = dictionary_entries.v[pos].domain;
                const char*res = string_collection.get(string_pos);
                return res;
            }
            
            const char* get_pre_domain_for_pattern( uint pos )
            {
                uint string_pos = dictionary_entries.v[pos].pre_domain_pattern;
                const char*res = string_collection.get(string_pos);
                return res;
            }
            
            const char* get_path_for_pattern( uint pos )
            {
                uint string_pos = dictionary_entries.v[pos].path_pattern;
                const char*res = string_collection.get(string_pos);
                return res;
            }
            
            uint find_one_pattern( const char* core_domain , uint begin , uint end )
            {
                // Last interval
                if( end == begin+1 )
                    return (uint)-1;
                
                uint mid_point = (begin + end) / 2;
                
                int c = strcmp(core_domain, get_domain_for_pattern(mid_point) );
                
                if( c == 0)
                    return mid_point;
                
                if( c < 0 )
                    return find_one_pattern( core_domain , begin , mid_point );
                else
                    return find_one_pattern( core_domain , mid_point , end );
            }
            
            
            bool findURLPattern( const char* _url , uint* pattern )
            {
                
                URL url( _url );
                
                // Find position of the rules...
                uint begin_pattern;
                uint end_pattern;
                
                // Find pattern range to evaluate
                if( find_pattern_range( url.core_domain.c_str() , &begin_pattern , &end_pattern ) )
                {
                    
                    //LM_M(("Domain %s has patterns in the range %d , %d" , url.core_domain.c_str() , begin_pattern, end_pattern ));
                    
                    for ( uint p = begin_pattern ; p <= end_pattern ; p++ )
                    {
                        
                        const char * pre_domain_pattern = get_pre_domain_for_pattern( p );
                        const char * path_pattern = get_path_for_pattern( p );
                        
                        if( match( pre_domain_pattern , url.pre_domain.c_str() ) )
                            if( match( path_pattern , url.path.c_str() ) )
                            {
                                *pattern = dictionary_entries.v[ p ].id;
                                return true;
                            }
                    }
                }
                return false;
            }
            
            std::vector<uint> getCategories( const char* url )
            {
                
                uint pattern;
                if( findURLPattern( url , &pattern ) )
                    return pattern_to_category.find( pattern );
                else
                    return std::vector<uint>(); // Empty vector
            }
            
            const char* getCategoryName( uint id )
            {
                std::vector<uint> s = categories.find( id );
                if( s.size() != 1 )
                    return "Unknown";
                else
                    return string_collection.get(s[0]);
            }
            
        };
        
    }
    
}
