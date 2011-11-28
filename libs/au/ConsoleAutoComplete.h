#ifndef _AU_CONSOLE_AUTO_COMPLETE
#define _AU_CONSOLE_AUTO_COMPLETE

#include <string>
#include <vector>

namespace au {
    
    class ConsoleAutoComplete
    {
        
        // Alternatives for the last word        
        std::vector<std::string> last_word_alternatives;
        
        std::string help_message;
        
    protected:
        
        // Current message introduced so far
        std::string current_command;
        
        // Preivous words ( before the last one )
        std::vector<std::string> previous_words;
        std::string last_word;
        
        
    public:
        
        ConsoleAutoComplete( std::string command );

        // Check what is the current situation in auto-completion
        bool completingFirstWord();
        bool completingSecondWord();
        bool completingThirdWord();
        bool completingSecondWord( std::string first_word );
        bool completingThirdWord( std::string first_word , std::string second_word );

        // Add an alternative for the last word
        void add( std::string command );

        // Set some help message
        void setHelpMessage( std::string _help_message );

        
        // Get the string to add to the current comman
        std::string stringToAppend();
        std::string getHelpMessage();
            
        // Auxiliar functions
        int common_chars_in_last_word_alternative();
        bool necessary_print_last_words_alternatives();
        void print_last_words_alternatives();
        
    };
    

    
}

#endif