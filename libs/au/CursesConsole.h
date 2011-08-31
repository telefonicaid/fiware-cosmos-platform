
/* ****************************************************************************
 *
 * FILE            CursesConsole.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au lib
 *
 * DATE            7/16/11
 *
 * DESCRIPTION
 *
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_Samson_CursesConsole
#define _H_Samson_CursesConsole


#include <ncurses.h>        // ncurses library
#include <string>           // std::string
#include <set>              // std::set


namespace au
{
    
    class CommandOptions
    {

    public:
        
        std::vector<std::string> command_options;  // Set of options to complete the command
      
        void clear()
        {
            command_options.clear();
        }
     
        // Remove options that do not match "command"
        void filter( std::string command )
        {
            // Remove the options that do not match with the current command
            if( command.length() > 0)
            {
                std::set<std::string> unique_command_options;
                
                for ( size_t i = 0 ; i < command_options.size() ; i++ )
                {
                    if( command_options[i].substr(0, command.length()) == command )
                        unique_command_options.insert( command_options[i]  );
                }
                
                // Remove options
                command_options.clear();
                
                // Add the unique filtered elements
                std::set<std::string>::iterator u;
                for ( u = unique_command_options.begin() ; u != unique_command_options.end() ; u++ )
                    command_options.push_back(*u);
                
            }
            
        }
        
        
        std::string auto_complete( std::string command )
        {
            // Fulter using the current command
            filter( command );
            
            // Unique option
            if( command_options.size() == 1 )
            {
                std::string future_word = *command_options.begin();
                clear();
                return select_auto_complete( command , future_word  );
            }
            
            // No auto-complete if no options
            if( command_options.size() == 0)
                return "";

            // Select the min common length to all the options
            size_t min_common_length = command_options[0].length();
            for ( size_t i = 1 ; i < command_options.size() ; i++ )
                min_common_length = std::min( min_common_length , common_chars( command_options[0]  , command_options[i] ) );

            
            if ( min_common_length == command.length() )
            {
                // Autocomplete is not possible
                return "";
            }
            else
            {
                std::string future_command = command_options[0].substr( 0 ,  min_common_length );
                clear();
                return select_auto_complete( command , future_command );
            }
            
        }
        
        size_t common_chars( std::string word, std::string word2 )
        {
            size_t l =  std::min( word.length() , word2.length() );
            for ( size_t i = 0 ; i < l ; i++ )
                if( word[i] != word2[i] )
                    return i;
            return l;
        }
        
        std::string select_auto_complete( std::string current_command , std::string future_command )
        {
            if( future_command.length() <= current_command.length() )
                return "";
            
            return future_command.substr( current_command.length() , future_command.length() - current_command.length() );
        }
        
        std::string str( size_t length )
        {
            std::ostringstream txt;
            
            txt << "Auto-complete ( ";
            
            for ( size_t i = 0 ; i < command_options.size() ; i++ )
            {
                if( (txt.str().length() + command_options[i].length() + 3) < length )
                    txt << command_options[i] << " ";
                else
                {
                    txt << "...";
                    break;
                }
            }
            
            txt << ") ";
            
            return txt.str();
        }
        
        
    };
    
    
    class CursesConsole
    {   
        
        int current_row;
        int rows,cols;
        
    protected:
        
        std::string command;                    // Command typed into console
        
        CommandOptions options;                 // Informationa bout auto-completion options
            
        bool quitConsole;                       // Flag to quit the console
    
	public:
        
        // Default constructor
        CursesConsole();

        // Main function to start console
        void run();
        
    private:
        
        void refresh();        
        void clearScreen();
        void clearLine( int r );

    private:
        
        // Function executed with tab is pressed ( auto-complete )
        void pressTab();
        
    protected:

        virtual std::string getHeaderLeft(){ return "Header Left"; };
        virtual std::string getHeaderRight(){ return "Header right"; };
        virtual std::string getPrompt(){ return " > "; };
        virtual void printContent()=0;
        
        virtual void auto_complete( std::vector<std::string>& previous_words , std::string& current_word ,std::vector<std::string>& command_options ){}
        
        virtual void evalComamnd( ){};
        virtual void evalRealTimeComamnd( ){};
        
        // Functions to be used int the virtual print() function
        void printLines( std::string txt );
        void print( std::string line );
        void print( std::string left_line ,std::string rigth_line  );
        void printLine();
        
        int getCols()
        {
            return cols;
        }
        int getRows()
        {
            return rows;
        }
        
    private:
        
        // Intern function to print at particular row
        void printLine( int r );
        void print( int r , std::string left_line ,std::string rigth_line );

        
        
    };
    
    
}

#endif
