

#ifndef _H_LOGSDATASET
#define _H_LOGSDATASET

#include <time.h>
#include <vector>

/* ****************************************************************************
*
* File -
*/
typedef struct File
{
    char       name[256];
    time_t     date;
    uint64_t   size;
    int        fd;
    bool       already_read;
} File;

void SplitInWords( char *line , std::vector<char*>& words , char separator );
char *fgetsFromFd( char * str, int line_max_size, int fd );
time_t GetTimeUTCFromCalendar(struct tm *tm);
time_t GetTimeFromStrTimeDate_dd_lett_YY_12H_AMPM(const char *strTimeDate);
time_t GetTimeFromStrTimeDate_YYYY_mm_dd_24H(const char *strTimeDate);



class LogsDataSet
{
    char *dir_path_;
    char *extension_;
    int num_files_;
    File *file_vector_;
    int timestamp_position_;
    char separator_;
    int timestamp_type_;
    time_t first_timestamp_;
    char *queue_name_;
    
public:
    LogsDataSet( const char *dir_path, const char *extension, int timestamp_position, int timestamp_type, const char *queue_name);
    ~LogsDataSet();
    
    bool InitDir();
    bool GetLogLineEntry(char **log, time_t *timestamp);
    bool LookAtNextLogLineEntry(char **log, time_t *timestamp);


    time_t GetFirstTimestamp(){ return first_timestamp_;};
    void SetFirstTimestamp(time_t value){ first_timestamp_ = value;};

    const char *GetQueueName(){ return queue_name_;};
};

#endif
