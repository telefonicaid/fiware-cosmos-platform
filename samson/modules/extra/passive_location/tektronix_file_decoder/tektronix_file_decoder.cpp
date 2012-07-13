#include <stdint.h>  // uint32_t, uint16_t...
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


#include "../tektronix_data.h"

int main( int argc, const char *argv[])
{
    int inputFd = -1;
    FILE *outputTxtFile = NULL;

#define BLOCK_SIZE (64*1024*1024)
    // If globalBuffer is a static array, its size is limited by the stack,
    // and the larger, the more efficient, so we move to a globalBuffer in heap
    //char globalBuffer[2 * BLOCK_SIZE];
    char *globalBuffer;

    if ((globalBuffer = (char *)malloc(2 * BLOCK_SIZE)) == NULL)
    {
	fprintf(stderr, "Error getting memory (%d bytes) for the globalBuffer\n", 2 * BLOCK_SIZE);
	perror("globalBuffer");
	exit(-1);
    }

    uint64_t DR_counter = 0;

    if (argc != 3)
    {
        fprintf(stderr, "%s: Wrong number of arguments: argc:%d, expected:2\n", argv[0], argc-1);
        fprintf(stderr, "Usage: %s bin_input txt_output\n", argv[0]);
        exit(-1);
    }

    if (strcmp(argv[1],"-") == 0)
    {
        inputFd = 0;
    }
    else
    {
        if ((inputFd = open(argv[1], O_RDONLY)) < 0)
        {
            fprintf(stderr, "Error opening file %s\n", argv[1]);
            perror(argv[1]);
            exit(-2);
        }
    }

    if (strcmp(argv[2], "-") == 0)
    {
        outputTxtFile = stdout;
    }
    else
    {
        if ((outputTxtFile = fopen(argv[2], "w")) == NULL)
        {
            fprintf(stderr, "Error opening file %s\n", argv[2]);
            perror(argv[2]);
            exit(-3);
        }
    }
    fprintf(outputTxtFile, "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n", "Type", "callType", "IMSI", "TMSI first", "TMSI last", "IMEI", "LAC", "cellID", "time", "DTAPCause", "BSSMAPCause", "ALCAPCause", "CCCause", "MMCause", "RANAPCause");

    ssize_t lengthToRead = BLOCK_SIZE;
    char *inData = globalBuffer;
    ssize_t n_read;
    bool cont = true;
    //int first_in_chunk = 3;
    while (cont)
    {
        n_read = read(inputFd, inData, lengthToRead);
        //fprintf(stderr, "read: %lld bytes, at DR_counter:%llu\n", (long long int)n_read, (long long unsigned)DR_counter);
        if (n_read != lengthToRead)
        {
            //fprintf(stderr, "Error, read:%lld bytes, asked:%lld\n", (long long int)n_read, (long long int)lengthToRead);
            if (n_read < 0)
            {
                perror("Input read:");
                cont = false;
            }
            else if (n_read == 0)
            {
                //fprintf(stderr, "n_read == 0, end of file\n");
                cont = false;
            }
        }
			//first_in_chunk = 3;

        unsigned char *p_blob = (unsigned char *)globalBuffer;
        unsigned char *p_endRead = (unsigned char *)inData + n_read;
        unsigned char *p_init_ohdr = p_blob;
        unsigned char *p_init_ohdr_checked = p_blob;
        unsigned char *p_end_ohdr;

	//fprintf(stderr, "Start a loop to consume read data: p_blob:%p, p_endRead:%p\n", p_blob, p_endRead);
        while( p_blob < p_endRead )
        {
            unsigned int sizeOHDR = 0;
            int typeMsg = 0;
            int numDRs = 0;

            unsigned int sizeDR = 0;
            struct struct_tek_record tek_record;

	    bool OK_OHDR_header = true;

            p_init_ohdr = p_blob;
	    //fprintf(stderr, "Read one OHDR with at p_blob:%p\n", p_blob);
            if ((OK_OHDR_header = parse_OHDR_header(&p_blob, &sizeOHDR, &numDRs, &typeMsg)) && ((p_init_ohdr + sizeOHDR) <= p_endRead))
            {
		//fprintf(stderr, "OK decoded OHDR with typeMsg:%d, updated p_blob:%p, p_endRead:%p\n", typeMsg, p_blob, p_endRead);
		p_init_ohdr_checked = p_init_ohdr;
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

                            strftime(timestampStr, MAX_TIME_LENGTH, "%Y%m%d%H%M%S", &st_time);

                            fprintf(outputTxtFile, "%d|%d|%llu|%llu|%llu|%llu|%d|%d|%s|%d|%d|%d|%d|%d|%d\n", tek_record.typeDR, tek_record.callType, (long long unsigned int) tek_record.imsi, (long long unsigned int) tek_record.tmsi, (long long unsigned int) tek_record.last_tmsi, (long long unsigned int) tek_record.imei, tek_record.LAC, (tek_record.cellID % 10000), timestampStr, tek_record.DTAPCause, tek_record.BSSMAPCause, tek_record.ALCAPCause, tek_record.CCCause, tek_record.MMCause, tek_record.RANAPCause);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Error parsing DR, %d of %d numDRs, DR_counter:%llu\n", i, numDRs, (long long unsigned)DR_counter);
                        char timestampStr[MAX_TIME_LENGTH];

                        struct tm st_time;
                        time_t time = tek_record.timestamp;

                        localtime_r( &time, &st_time);

                        strftime(timestampStr, MAX_TIME_LENGTH, "%Y%m%d%H%M%S", &st_time);

                        fprintf(stderr, "DRERROR:%d|%d|%llu|%llu|%llu|%llu|%d|%d|%s|%d|%d|%d|%d|%d|%d\n", tek_record.typeDR, tek_record.callType, (long long unsigned int) tek_record.imsi, (long long unsigned int) tek_record.tmsi, (long long unsigned int) tek_record.last_tmsi, (long long unsigned int) tek_record.imei, tek_record.LAC, (tek_record.cellID % 10000), timestampStr, tek_record.DTAPCause, tek_record.BSSMAPCause, tek_record.ALCAPCause, tek_record.CCCause, tek_record.MMCause, tek_record.RANAPCause);

                    }
//		    if (first_in_chunk)
//		    {
//                       char timestampStr[MAX_TIME_LENGTH];
//
//                       struct tm st_time;
//                      time_t time = tek_record.timestamp;
//
//                       localtime_r( &time, &st_time);
//
//                       strftime(timestampStr, MAX_TIME_LENGTH, "%Y%m%d%H%M%S", &st_time);
//
//			fprintf(stderr, "At pos:pblob-globalBuffer:%lu\n", p_blob-(unsigned char *)globalBuffer);
//                      fprintf(stderr, "FIRSTINCHUNK:%d|%d|%llu|%llu|%llu|%llu|%d|%d|%s|%d|%d|%d|%d|%d|%d\n", tek_record.typeDR, tek_record.callType, (long long unsigned int) tek_record.imsi, (long long unsigned int) tek_record.tmsi, (long long unsigned int) tek_record.last_tmsi, (long long unsigned int) tek_record.imei, tek_record.LAC, (tek_record.cellID % 10000), timestampStr, tek_record.DTAPCause, tek_record.BSSMAPCause, tek_record.ALCAPCause, tek_record.CCCause, tek_record.MMCause, tek_record.RANAPCause);
//
//			first_in_chunk--;
//
//                   }
		    DR_counter++;
                }
                if (p_blob != p_end_ohdr)
                {
                    fprintf(stderr, "Alignment failed in a OHDR of %d DRs, DR_counter=%llu\n", numDRs, (long long unsigned)DR_counter);
                }
		p_init_ohdr_checked = p_end_ohdr;
            }
            else if (OK_OHDR_header == false)
            {
		    fprintf(stderr, "WRONG decoded OHDR, DR_counter=%llu, with typeMsg:%d, updated p_blob:%p, p_endRead:%p\n", (long long unsigned)DR_counter, typeMsg, p_blob, p_endRead);
		    if ((p_init_ohdr + sizeOHDR) > p_endRead)
		    {
			    //fprintf(stderr, "Probably, OHDR ends further than the end of the chunk with p_init_ohdr:%p + sizeOHDR:%u > p_endRead:%p\n", p_init_ohdr, sizeOHDR, p_endRead);
		    }
                    fprintf(stderr, "Trying to find a OHDR begin in sync.\n");
		    p_blob = p_init_ohdr+1;
            }
	    else
	    {
		    //fprintf(stderr, "OHDR ends further than the end of the chunk with typeMsg:%d, updated p_blob:%p, p_endRead:%p. Let to be completed with next chunk\n", typeMsg, p_blob, p_endRead);
		    break;
	    }
        }
        if (p_init_ohdr_checked < p_endRead)
        {
            int pendingBytes = p_endRead - p_init_ohdr_checked;
            //fprintf(stderr, "End of a block with %d pendingBytes\n", pendingBytes);
            memcpy(globalBuffer, p_init_ohdr_checked, pendingBytes);
            inData = globalBuffer + pendingBytes;
        }
        else if (p_init_ohdr_checked == p_endRead)
        {
            //fprintf(stderr, "End of a block with %d pendingBytes\n", 0);
            inData = globalBuffer;
        }
    }
}
