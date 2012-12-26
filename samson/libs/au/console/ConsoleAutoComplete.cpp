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


#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "au/string.h"
#include "au/utils.h"
#include "au/file.h"

#include "au/console/ConsoleAutoComplete.h" // Own interface


NAMESPACE_BEGIN(au)


char get_last_char( std::string txt )
{
    if( txt.length() == 0 )
        return 0;
    else
        return txt[ txt.length() -1 ];
}


ConsoleAutoComplete::ConsoleAutoComplete( std::string command )
{
    current_command = command;
    
    // Get all the words so far and the last word
    au::split(command, ' ', previous_words);
    if( command.length() > 0 )
        if( command[command.length()-1] != ' ')
        {
            last_word = previous_words.back();
            previous_words.pop_back();
        }
    
    add_space_if_unique_alternative = true;
    
}

int ConsoleAutoComplete::completingWord()
{
    return previous_words.size();
}

std::string ConsoleAutoComplete::firstWord()
{
    if( previous_words.size() < 1 )
        return "";
    return previous_words[0];
}

std::string ConsoleAutoComplete::secondWord()
{
    if( previous_words.size() < 2 )
        return "";
    return previous_words[1];
    
}


bool ConsoleAutoComplete::completingSecondWord( std::string first_word )
{
    if ( previous_words.size() == 1 )
        if ( previous_words[0] == first_word )
            return true;
    return false;
}

bool ConsoleAutoComplete::completingFirstWord()
{
    return (previous_words.size() == 0);
}

bool ConsoleAutoComplete::completingSecondWord()
{
    return (previous_words.size() == 1);
}

bool ConsoleAutoComplete::completingThirdWord()
{
    return (previous_words.size() == 2);
}

bool ConsoleAutoComplete::completingThirdWord( std::string first_word , std::string second_word )
{
    if ( previous_words.size() != 2 )
        return false;
    
    if ( ( first_word != "*" ) && ( previous_words[0] != first_word ) )
        return false;
    if ( ( second_word != "*" ) && ( previous_words[1] != second_word ) )
        return false;
    
    return true;
}

std::string ConsoleAutoComplete::getCurrentCommand()
{
    return current_command;
}


void ConsoleAutoComplete::add( std::string command )
{
    add( ConsoleAutoCompleteAlternative( command ) );
}

void ConsoleAutoComplete::add( std::vector<std::string> commands )
{
    for ( size_t i = 0 ; i <  commands.size() ; i++ )
        add( commands[i] );
}

void ConsoleAutoComplete::add( std::string label , std::string command , bool add_space )
{
    add( ConsoleAutoCompleteAlternative( label, command , add_space ) );
}

void ConsoleAutoComplete::add( ConsoleAutoCompleteAlternative alternative )
{
    if ( alternative.command.length() < last_word.length() )
        return; // Not valid candidate

    // Check if it was previously included...
    for( size_t i = 0 ; i < last_word_alternatives.size() ; i++ )
        if( last_word_alternatives[i].command == alternative.command )
            return;
    
    if( strings_begin_equal(alternative.command, last_word) )
        last_word_alternatives.push_back( alternative );            
    
}

void ConsoleAutoComplete::auto_complete_files( std::string file_selector )
{
    std::string directory = get_directory_from_path( last_word );  // By default, take the last work as the directory to go
    std::string base = path_remove_last_component( last_word );
    
    if( ( base.length() > 0 ) && ( base != "/" )) 
        base.append("/");
    
    //printf("Last word '%s' dir='%s' base='%s'\n", last_word.c_str() , directory.c_str() , base.c_str() );
    
    // Try to open directory
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir( directory.c_str() )) == NULL) 
        return; // Nothing else to do...
    
    while ((dirp = readdir(dp)) != NULL) 
    {
        std::string fileName = dirp->d_name;
        
        // Skip ".files"
        if( fileName.length()>0 )
            if ( fileName[0] == '.' )
                continue;
        
        
        
        // Full path of the file
        std::string path = path_from_directory( directory , dirp->d_name );
        
        struct ::stat info;
        stat(path.c_str(), &info);
        
        if( S_ISREG(info.st_mode) )
        {
            if ( string_ends( path , file_selector ) )
            {
                // Final string to show
                size_t size = info.st_size;

                
                add(
                    au::str("%s (%s)" ,fileName.c_str() , au::str( size ,"B" ).c_str() ) 
                    , base + fileName 
                    , true 
                    );
            }
        }
        else if ( S_ISDIR(info.st_mode) )
        {
            add( fileName + "/" , base + fileName + "/" , false );
        }
        
    }
    
    closedir(dp);
    
    
    
}


void ConsoleAutoComplete::setHelpMessage( std::string _help_message )
{
    help_message = _help_message;
}

int ConsoleAutoComplete::common_chars_in_last_word_alternative()
{
    if (last_word_alternatives.size() == 0)
        return 0;
    
    int common_chars = last_word_alternatives[0].command.length();
    for (size_t i=1;i<last_word_alternatives.size();i++)
        replaceIfLower( common_chars , getCommonChars( last_word_alternatives[i].command , last_word_alternatives[i-1].command ) );
    
    return common_chars;
}

std::string ConsoleAutoComplete::stringToAppend()
{
    // If no options, return the previous one...
    if ( last_word_alternatives.size() == 0 )
        return "";
    
    int last_word_length = last_word.length();
    int common_chars = common_chars_in_last_word_alternative();
    
    std::string complete_text = last_word_alternatives[0].command.substr( last_word_length, common_chars - last_word_length );
    
    //printf("Complete %s", complete_text.c_str());
    if( last_word_alternatives.size() == 1 )
    {
        if( last_word_alternatives[0].add_space_if_unique )
            return  complete_text + " ";
        else
            return  complete_text;
    }
    else
        return  complete_text;
}

std::string ConsoleAutoComplete::getHelpMessage()
{
    return help_message;
}


bool ConsoleAutoComplete::necessary_print_last_words_alternatives()
{

    if ( last_word_alternatives.size() == 0 )
        return false;
    
    if ( last_word_alternatives.size() == 1 )
        return false;
    
    
    int last_word_length = last_word.length();
    int common_chars = common_chars_in_last_word_alternative();
    
    return (last_word_length == common_chars);
    
}

void ConsoleAutoComplete::print_last_words_alternatives()
{
    
    if( !necessary_print_last_words_alternatives() )
        return;


    int columns = getColumns();
    int max_length = 0;
    for (size_t i=0;i<last_word_alternatives.size();i++)
        replaceIfHiger(max_length, last_word_alternatives[i].label.length() );
    
    int num_words_per_row = columns / ( max_length + 1 );
    
    for (size_t i=0;i<last_word_alternatives.size();i++)
    {
        std::string format = std::string("%-") + au::str("%d",max_length) + std::string("s ");
        printf(  format.c_str()  , last_word_alternatives[i].label.c_str() );
        
        if( (i%num_words_per_row) == (size_t)(num_words_per_row-1) )
            printf("\n");
    }
    
    if( ((last_word_alternatives.size()-1)%num_words_per_row)!=(size_t)(num_words_per_row-1) )
        printf("\n");
}

NAMESPACE_END
