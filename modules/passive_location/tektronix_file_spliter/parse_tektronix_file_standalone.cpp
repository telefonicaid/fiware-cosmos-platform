#include <stdint.h>  // uint32_t, uint16_t...
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <au/CommandLine.h>

#include "../tektronix_data.h"

int main( int argc, const char *argv[])
{

    au::CommandLine cmd;
    cmd.set_flag_uint64( "size" , 50*1024*1024 );
    cmd.parse( argc , argv );


    size_t size = 50*1024*1024;
    char *inputFile;
    char *outputFile_base;
    char outputFile[1024];

    struct stat pstat;
    int inputFd;
    int outputFd;
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
    int typeDR = 0;
    uint64_t timestamp = 0;
    uint64_t imsi = 0;
    uint64_t imei = 0;
    uint64_t msisdn = 0;
    int probeId = 0;


    if (cmd.get_num_arguments() < 2)
    {
        fprintf(stderr, "%s input output_base [-size max_size] \n", argv[0]);
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

    for (int i = 0; (i < 232); i++)
    {
        //OLM_M(("data[%d](0x%0x) = 0x%0x", i, p_blob+i, int(p_blob[i])));
    }

    while( p_blob < p_end_blob )
    {
        p_init_ohdr = p_blob;
        if (parse_OHDR_header(&p_blob, &sizeOHDR, &numDRs, &typeMsg))
        {
            p_end_ohdr = p_init_ohdr + sizeOHDR;
            for (int i = 0; ((i < numDRs) && (p_blob < p_end_ohdr)); i++)
            {
                if (parse_DR(&p_blob, &sizeDR, &typeDR, &timestamp, &imsi, &imei, &msisdn, &probeId))
                {
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
                }
            }
            if (p_blob != p_end_ohdr)
            {
                fprintf(stderr, "Alignment failed in a OHDR of %d DRs", numDRs);
            }
        }
        else
        {
            frprintf(stderr, "OHDR ignored because not valid header, with typeMsg=%d", typeMsg);
        }
        //OLM_M(("p_end_blob - p_blob=%lu (length(%lu))", p_end_blob - p_blob, length));
        if ((p_blob - p_chunk) > size)
        {
            fprintf(stdout, "New chunk of %lu bytes (size:%lu)\n", (p_blob - p_chunk), size);
            sprintf(outputFile, "%s_%d.bin", outputFile_base, numChunk);
            if ((outputFd = open(outputFile, O_CREAT|O_WRONLY, 0666)) < 0)
            {
                fprintf(stderr, "Error al hacer open del fichero %s\n", outputFile);
                perror(outputFile);
                exit(-4);
            }

            n_write = write(outputFd, p_chunk, (p_blob - p_chunk));
            if (n_write != (p_blob - p_chunk))
            {
                fprintf(stderr, "Error, escritos %lu bytes de %lu pedidos\n", n_read, (p_blob - p_chunk));
            }
            close(outputFd);
            numChunk++;
            p_chunk = p_blob;
        }
    }
}
