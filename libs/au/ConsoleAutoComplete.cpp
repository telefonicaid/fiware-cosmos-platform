

#include "au/string.h"
#include "au/utils.h"

#include "au/ConsoleAutoComplete.h" // Own interface


namespace au {
    
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
    
    void ConsoleAutoComplete::add( std::string command )
    {
        if ( command.length() < last_word.length() )
            return; // Not valid candidate
        
        if( strings_begin_equal(command, last_word) )
            last_word_alternatives.push_back( command );            
    }
    
    void ConsoleAutoComplete::setHelpMessage( std::string _help_message )
    {
        help_message = _help_message;
    }
    
    int ConsoleAutoComplete::common_chars_in_last_word_alternative()
    {
        if (last_word_alternatives.size() == 0)
            return 0;
        
        int common_chars = last_word_alternatives[0].length();
        for (size_t i=1;i<last_word_alternatives.size();i++)
            replaceIfLower( common_chars , getCommonChars( last_word_alternatives[i] , last_word_alternatives[i-1]) );
        
        return common_chars;
    }
    
    std::string ConsoleAutoComplete::stringToAppend()
    {
        // If no options, return the previous one...
        if ( last_word_alternatives.size() == 0 )
            return "";
        
        int last_word_length = last_word.length();
        int common_chars = common_chars_in_last_word_alternative();
        
        std::string complete_text = last_word_alternatives[0].substr( last_word_length, common_chars - last_word_length );
        
        //printf("Complete %s", complete_text.c_str());
        if( last_word_alternatives.size() == 1 )
            return  complete_text + " ";
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
        
        if (help_message.length()>0) 
            printf("help: %s\n" , help_message.c_str());
        
        if( !necessary_print_last_words_alternatives() )
            return;
        
        int columns = getColumns();
        int max_length = 0;
        for (size_t i=0;i<last_word_alternatives.size();i++)
            replaceIfHiger(max_length, last_word_alternatives[i].length() );
        
        int num_words_per_row = columns / ( max_length + 1 );
        
        for (size_t i=0;i<last_word_alternatives.size();i++)
        {
            printf(  au::str("%%%d-s ", max_length ).c_str() , last_word_alternatives[i].c_str() );
            
            if( (i%num_words_per_row) == (size_t)(num_words_per_row-1) )
                printf("\n");
        }
        
        if( ((last_word_alternatives.size()-1)%num_words_per_row)!=(size_t)(num_words_per_row-1) )
            printf("\n");
    }
}