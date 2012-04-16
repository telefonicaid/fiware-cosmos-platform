

#ifndef _H_AU_LOG_TO_SERVER
#define _H_AU_LOG_TO_SERVER


namespace au
{
    
    /* ****************************************************************************
     *
     * start_log_to_server - 
     */
    
    void start_log_to_server( std::string log_host , int log_port , std::string local_log_file );
    
    /* ****************************************************************************
     *
     * logToLogServer - 
     */
    
    
    void logToLogServer(void* vP, char* text, char type, time_t secondsNow, int timezone, int dst, const char* file, int lineNo, const char* fName, int tLev, const char* stre);
    
}

#endif
