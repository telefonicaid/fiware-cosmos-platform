#include <stdint.h>  // uint32_t, uint16_t...
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "../tektronix_data.h"

int main( int argc, const char *argv[])
{

    bool txt = cmd.get_flag_bool("txt");
    bool newFile = true;

    char *inputFile;
    char *outputFile;

    struct stat pstat;
    int inputFd = -1;
    int outputFd = -1;
    FILE *outputTxtFile = NULL;
    size_t length;
    char *data;
    size_t n_read;
    size_t n_write;


    size_t offset = 0;

    unsigned char *p_blob;
    unsigned char *p_end_blob;

    unsigned char *p_init_ohdr;
    unsigned char *p_end_ohdr;

    unsigned int sizeOHDR = 0;
    int typeMsg = 0;
    int numDRs = 0;

    unsigned int sizeDR = 0;
    struct struct_tek_record tek_record;

   if (size);
   if (txt);
   if (newFile);
   if (outputFd);
   if (n_write);
   if (offset);




    if (argc != 3)
    {
        fprintf(stderr, "%s: Wrong number of arguments: argc:%d, expected:2\n", argv[0], argc-1);
        fprintf(stderr, "Usage: %s bin_input txt_output\n", argv[0]);
        exit(2);
    }

    inputFile = strdup(argv[1]);
    outputFile = strdup(argv[2]);

    if (stat(inputFile, &pstat) < 0)
    {
        fprintf(stderr, "Error at stat file %s\n", inputFile);
        perror(inputFile);
        exit(-1);
    }


    if ((inputFd = open(inputFile, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Error opening file %s\n", inputFile);
        perror(inputFile);
        exit(-2);
    }

    length = pstat.st_size;
    if ((data = (char *)malloc(length)) == NULL)
    {
        fprintf(stderr, "Error, not enough memory for data length:%lu\n", length);
        exit(-3);
    }
    n_read = read(inputFd, data, length);
    if (n_read != (size_t)pstat.st_size)
    {
        fprintf(stderr, "Error, read:%lu bytes, asked:%lu\n", n_read, length);
    }
    close(inputFd);

    if ((outputTxtFile = fopen(outputFile, "w")) == NULL)
    {
        fprintf(stderr, "Error opening file %s\n", outputFile);
        perror(outputFile);
        exit(-4);
    }
    fprintf(outputTxtFile, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", "Type", "callType", "IMSI", "TMSI first", "TMSI last", "IMEI", "LAC", "cellID", "time", "DTAPCause", "BSSMAPCause", "ALCAPCause", "CCCause", "MMCause", "RANAPCause");


    p_blob = (unsigned char *)data;
    p_end_blob = (unsigned char *)data + length;

    while( p_blob < p_end_blob )
    {
        p_init_ohdr = p_blob;
        if (parse_OHDR_header(&p_blob, &sizeOHDR, &numDRs, &typeMsg))
        {
            p_end_ohdr = p_init_ohdr + sizeOHDR;
            for (int i = 0; ((i < numDRs) && (p_blob < p_end_ohdr)); i++)
            {
                init_tek_record(&tek_record);
                if (parse_DR(&p_blob, &sizeDR, &tek_record))
                {
                    {
#define MAX_TIME_LENGTH 81
                        char timestampStr[MAX_TIME_LENGTH];

                        struct tm st_time;
                        time_t time = tek_record.timestamp;

                        localtime_r( &time, &st_time);

                        strftime(timestampStr, MAX_TIME_LENGTH, "%Y%m%d%H%M%s", &st_time);

                        fprintf(outputTxtFile, "%d|%d|%lu|%lu|%lu|%lu|%d|%d|%s|%d|%d|%d|%d|%d|%d\n", tek_record.typeDR, tek_record.callType, tek_record.imsi, tek_record.tmsi, tek_record.last_tmsi, tek_record.imei, tek_record.LAC, tek_record.cellID, timestampStr, tek_record.DTAPCause, tek_record.BSSMAPCause, tek_record.ALCAPCause, tek_record.CCCause, tek_record.MMCause, tek_record.RANAPCause);
                    }
                }
                else
                {
                    fprintf(stderr, "Error parsing DR, %d of %d numDRs", i, numDRs);
                }
            }
            if (p_blob != p_end_ohdr)
            {
                fprintf(stderr, "Alignment failed in a OHDR of %d DRs", numDRs);
            }
        }
        else
        {
            fprintf(stderr, "OHDR ignored because not valid header, with typeMsg=%d", typeMsg);
        }
    }
}
