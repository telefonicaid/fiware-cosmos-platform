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
#include <stdint.h>  // uint32_t, uint16_t...
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include <au/CommandLine.h>

#include "../tektronix_data.h"

int main( int argc, const char *argv[])
{

    au::CommandLine cmd;
    cmd.set_flag_uint64( "size" , 50*1024*1024 );
    cmd.set_flag_boolean( "txt" );
    cmd.parse( argc , argv );


    size_t size =  cmd.get_flag_uint64("size");
    bool txt = cmd.get_flag_bool("txt");
    bool newFile = true;

    char *inputFile;
    char *outputFile_base;
    char outputFileName[1024];
    char outputTxtFileName[1024];

    struct stat pstat;
    int inputFd = -1;
    int outputFd = -1;
    FILE *outputTxtFile = NULL;
    size_t length;
    char *data;
    size_t n_read;
    size_t n_write;


    size_t offset = 0;

    //OLM_M(("length:%lu", length));

    unsigned char *p_blob;
    unsigned char *p_end_blob;

    unsigned char *p_chunk;
    int numChunk = 0;

    unsigned char *p_init_ohdr;
    unsigned char *p_end_ohdr;

    unsigned int sizeOHDR = 0;
    int typeMsg = 0;
    int numDRs = 0;

    unsigned int sizeDR = 0;
    struct struct_tek_record tek_record;



    if (cmd.get_num_arguments() < 2)
    {
        fprintf(stderr, "%s input output_base [-size max_size] [-txt] \n", argv[0]);
        exit(2);
    }

    inputFile = strdup(cmd.get_argument(1).c_str());
    outputFile_base = strdup(cmd.get_argument(2).c_str());

    if (stat(inputFile, &pstat) < 0)
    {
        fprintf(stderr, "Error al hacer stat del fichero %s\n", inputFile);
        exit(-1);
    }


    if ((inputFd = open(inputFile, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Error al hacer open del fichero %s\n", inputFile);
        perror(inputFile);
        exit(-2);
    }

    length = pstat.st_size;
    if ((data = (char *)malloc(length)) == NULL)
    {
        fprintf(stderr, "Error, no hay memoria para data de length:%lu\n", length);
        exit(-3);
    }
    n_read = read(inputFd, data, length);
    if (n_read != pstat.st_size)
    {
        fprintf(stderr, "Error, leidos %lu bytes de %lu pedidos\n", n_read, length);
    }
    close(inputFd);




    //OLM_M(("length:%lu", length));


    p_blob = (unsigned char *)data;
    p_end_blob = (unsigned char *)data + length;
    p_chunk = p_blob;

//    for (int i = 0; (i < 232); i++)
//    {
//        OLM_M(("data[%d](0x%0x) = 0x%0x", i, p_blob+i, int(p_blob[i])));
//    }



    while( p_blob < p_end_blob )
    {
        if (txt && newFile)
        {
            if (outputTxtFile)
            {
                fclose(outputTxtFile);
            }

            sprintf(outputTxtFileName, "%s_%d.txt", outputFile_base, numChunk);
            if ((outputTxtFile = fopen(outputTxtFileName, "w")) == NULL)
            {
                fprintf(stderr, "Error al hacer open del fichero %s\n", outputTxtFileName);
                perror(outputTxtFileName);
                exit(-4);
            }

            newFile = false;
        }

        p_init_ohdr = p_blob;
        if (parse_OHDR_header(&p_blob, &sizeOHDR, &numDRs, &typeMsg))
        {
            p_end_ohdr = p_init_ohdr + sizeOHDR;
            for (int i = 0; ((i < numDRs) && (p_blob < p_end_ohdr)); i++)
            {
                init_tek_record(&tek_record);
                if (parse_DR(&p_blob, &sizeDR, &tek_record))
                {

                    if (txt)
                    {
#define MAX_TIME_LENGTH 81
                        char timestampStr[MAX_TIME_LENGTH];

                        struct tm st_time;
                        time_t time = tek_record.timestamp;

                        localtime_r( &time, &st_time);

                        strftime(timestampStr, MAX_TIME_LENGTH, "%Y%m%d%H%M%s", &st_time);


                        fprintf(outputTxtFile, "%d|%d|%lu|%lu|%lu|%lu|%d|%d|%s|%d|%d|%s|%s|%s|%s\n", tek_record.typeDR, tek_record.callType, tek_record.imsi, tek_record.tmsi, tek_record.last_tmsi, tek_record.imei, tek_record.LAC, tek_record.cellID, timestampStr, tek_record.DTAPCause, tek_record.BSSMAPCause, tek_record.CCCause, tek_record.MMCause, tek_record.RANAPCause, tek_record.ALCAPCause);
                    }

                    //                        equip_id.value = probeId;
                    //
                    //                        record.imsi.value = imsi;
                    //                        record.imei.value = imei;
                    //                        record.timestamp.value = timestamp;
                    //                        record.cell_id.value = probeId;
                    //                        //LM_M(("Ready to emit typeDR:%d for msisdn:%lu at probeId:%d at %lu(%s)", typeDR, msisdn, probeId, record.timestamp.value, record.timestamp.str().c_str()));
                    //
                    //                        // Emit the record at the output
                    //                        writer->emit(0, &equip_id, &record);
                    free(tek_record.CCCause);
                    free(tek_record.MMCause);
                    free(tek_record.RANAPCause);
                    free(tek_record.ALCAPCause);

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
        //OLM_M(("p_end_blob - p_blob=%lu (length(%lu))", p_end_blob - p_blob, length));
        if ((p_blob - p_chunk) > size)
        {
            fprintf(stdout, "New chunk of %lu bytes (size:%lu)\n", (p_blob - p_chunk), size);
            sprintf(outputFileName, "%s_%d.bin", outputFile_base, numChunk);
            if ((outputFd = open(outputFileName, O_CREAT|O_WRONLY, 0666)) < 0)
            {
                fprintf(stderr, "Error al hacer open del fichero %s\n", outputFileName);
                perror(outputFileName);
                exit(-4);
            }

            n_write = write(outputFd, p_chunk, (p_blob - p_chunk));
            if (n_write != (p_blob - p_chunk))
            {
                fprintf(stderr, "Error, escritos %lu bytes de %lu pedidos\n", n_read, (p_blob - p_chunk));
            }
            close(outputFd);
            numChunk++;
            newFile = true;
            p_chunk = p_blob;
        }
    }
}
