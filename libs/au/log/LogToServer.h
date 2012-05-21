

#ifndef _H_AU_LOG_TO_SERVER
#define _H_AU_LOG_TO_SERVER


namespace au
{
    
    /* ****************************************************************************
     *
     * start and stop connection with log server
     */
    
    void start_log_to_server( std::string log_host , int log_port , std::string local_log_file );
    void stop_log_to_server( );
    
    /* ****************************************************************************
     *
     * logToLogServer - 
     */
    
    void logToLogServer(void* vP, char* text, char type, time_t secondsNow, int timezone, int dst, const char* file, int lineNo, const char* fName, int tLev, const char* stre);

    // Get fd for logServer connection ( it is used to not be closed in isolated processes )
    int getLogServerConnectionFd();
    
}

#endif
