

#include "au/string.h"              
#include "au/CommandLine.h"

#include "CursesConsole.h"			// Own interface

namespace au 
{
    
    void ncurses_atexit(void) 
    {                     
        endwin();
    }  
    
    
    CursesConsole::CursesConsole()
    {
        // Default constructor
        quitConsole = false;
    }
    
#pragma mark Main functions
    
    void CursesConsole::run()
    {
        // Add the nscurses all at the exit
        atexit(ncurses_atexit);
        
        
        WINDOW *w = initscr(); /* Start curses mode   */
        cbreak();
        nodelay(w, TRUE);        
        
        keypad(stdscr, TRUE);		/* We get F1, F2 etc..              */
        noecho();                   /* Don't echo() while we do getch   */
        //raw();                    /* Line buffering disabled          */
        
        
        while( true )
        {
            if( quitConsole )
                return;
            
            refresh();
            
            // Sleep a bit
            usleep( 10000 );
            
            // Eval commands in real time
            evalRealTimeComamnd( );
            
            // Get a char
            int ch = getch();
            if( ch != ERR )
            {
                // Clear the options
                options.clear();
                
                // Path to discover new keys
                //command.append( au::str("<%d>" , ch) );
                
                switch (ch) {
                        
                    case 127:
                    {
                        if ( command.length() > 0 )
                        {
                            command.erase( command.length()-1 , 1 );
                        }
                        break;
                    }
                        
                    case '\t':
                        // Do something with the tab
                        pressTab();
                        break;
                        
                    case '\n':
                        // Do something with the tab
                        evalComamnd( );
                        command="";
                        break;
                        
                    default:
                        // append the letter to the command
                        command += (char)ch;
                        break;
                }
                
            }
        }
        
    }
    
    
    void CursesConsole::refresh()
    {
        // Get the size of the screen
        getmaxyx(stdscr,rows,cols);
        
        // Clear screen
        clearScreen();
        
        // Print header
        printLine();
        print( getHeaderLeft() , getHeaderRight() );
        printLine();
        
        // Print the user content
        printContent();
        
        if( options.command_options.size() == 0 )
        {
            // Clean the bottom part of the screen
            clearLine( rows - 2 );
            clearLine( rows - 1 );
            
            printLine( rows - 2 );
            
            move(rows-1, 0);
            printw( getPrompt().c_str() );
            printw( command.c_str() );
        }
        else
        {
            // Clean the bottom part of the screen
            clearLine( rows - 3 );
            clearLine( rows - 2 );
            clearLine( rows - 1 );
            
            printLine( rows - 3 );
            print( rows - 2 , options.str( cols ) , "" );
            
            move(rows-1, 0);
            printw( getPrompt().c_str() );
            printw( command.c_str() );
            
        }
        
    }
    
    void CursesConsole::clearLine( int r )
    {
        std::string white_line;
        for (int i = 0 ; i < cols ; i++)
            white_line.append(" ");
        print(r,white_line,"");
        
    }
    
    void CursesConsole::clearScreen()
    {
        std::string white_line;
        for (int i = 0 ; i < cols ; i++)
            white_line.append(" ");
        
        for (int i = 0 ; i < rows ; i++)
        {
            move( i , 0 );
            printw(  white_line.c_str() );
        }
        
        
        current_row = 0;
        
    }    
    
#pragma mark Internal functions used in the print()
    
    void CursesConsole::printLines( std::string txt )
    {
        std::vector<std::string> lines;
        au::split(txt, '\n', lines);
        
        for ( size_t i = 0 ; i < lines.size() ; i++ )
            print(lines[i]);
    }
    
    void CursesConsole::print( std::string line )
    {
        if( current_row > (rows-1) )
            return;
        
        print( current_row++ , line, "" );
    }
    
    void CursesConsole::print(  std::string left_line ,std::string rigth_line  )
    {
        if( current_row > (rows-1) )
            return;
        
        print( current_row++ , left_line, rigth_line );
    }
    
    void CursesConsole::printLine()
    {
        if( current_row > (rows-1) )
            return;
        
        printLine( current_row++ );
    }
    
#pragma mark Internal functions to print lines
    
    void CursesConsole::printLine( int r )
    {
        move(r,0);
        for ( int i = 0 ; i < cols-1 ; i++)
            printw("-");
    }
    
    void CursesConsole::print( int r , std::string left_line ,std::string rigth_line )
    {
        if( r >= rows )
            return;
        
        move( r , 0 );
        printw("%s",left_line.c_str());
        
        move( r , cols-1 - strlen( rigth_line.c_str() ) );
        printw("%s",rigth_line.c_str());
    }    
    
#pragma marg Autocomplete
    
    void CursesConsole::pressTab()
    {
        au::CommandLine cmdLine;
        cmdLine.parse( command );
        
        // Vector containing all the previous words introduced
        std::vector<std::string> previous_words;
        
        // Current word
        std::string current_word;
        
        if( ((command.length() > 0) && command[command.length()-1] == ' ' ))
        {
            for (int i = 0 ; i < cmdLine.get_num_arguments() ; i++ )
                previous_words.push_back( cmdLine.get_argument(i) );
        }
        else
        {
            for (int i = 0 ; i < (cmdLine.get_num_arguments() - 1) ; i++ )
                previous_words.push_back( cmdLine.get_argument(i) );
            
            if( cmdLine.get_num_arguments() > 0 )
                current_word = cmdLine.get_argument( cmdLine.get_num_arguments() - 1 );
        }
        
        LM_M(("Auto-complete %lu previous words & current_command '%s'" , previous_words.size() , current_word.c_str() ));
        
        // Check the autocompletion options
        options.clear();
        auto_complete( previous_words ,current_word , options.command_options );
        
        LM_M(("Options: %lu" , options.command_options.size() ));
        
        // Get the auto-complete string and use it with the current command line
        std::string auto_complete_command = options.auto_complete( current_word );
        command.append(auto_complete_command);
        
        
        
    }
    
    
}
