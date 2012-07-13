/* valsort.c - Sort output data validator
 *
 * Version 1.1  6 Sep 2009  Chris Nyberg <chris.nyberg@ordinal.com>
 *
 * Copyright (C) 2009
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of Version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>   /* use crc32() function */
#include "rand16.h"
#define NO_EXTRA_THREADS  (-1)

#define REC_SIZE 100
#define SUM_SIZE (sizeof(struct summary))

/* Comparison routine, either memcmp() or strcasecmp() */
int     (*Compare)(const char *a, const char *b, size_t n);   
int     Read_summary;    /* Read a file of partition summaries, not records */
int     Quiet;           /* Quiet mode, don't output diagnostic information */

/* struct used to summarize a partition of sort output
 */
struct summary
{
    u16         first_unordered;     /* index of first unordered record,
                                      * or 0 if no unordered records */
    u16         unordered_count;     /* total number of unordered records */
    u16         rec_count;           /* total number of records */
    u16         dup_count;           /* total number of duplicate keys */
    u16         checksum;            /* checksum of all records */
    char        first_rec[REC_SIZE]; /* first record */
    char        last_rec[REC_SIZE];  /* last record */
};

struct summary Summary;


/* next_rec - get the next record to be validated
 *
 */
char *next_rec(void *in, char *rec_buf, struct summary *sum)
{
    int                 read_size;
    char                *rec = NULL;
    u16                 temp16 = {0LL, 0LL};

    read_size = fread(rec_buf, 1, REC_SIZE, in);
    rec = rec_buf;
    
    if (read_size == REC_SIZE)
    {
        temp16.lo8 = crc32(0, rec, REC_SIZE);
        sum->checksum = add16(sum->checksum, temp16);
    }
    else if (read_size == 0)
        return (NULL);
    else if (read_size < 0)
    {
        fprintf(stderr, "record read error\n");
        exit(1);
    }
    else
    {
        fprintf(stderr, "partial record found at end\n");
        exit(1);
    }
    return (rec);
}


/* summarize_records - summarize the validity of a sequence of records.
 */
int summarize_records(void *in, void *unused)
{
    struct summary      *sum;
    int                 diff;
    u16                 one = {0LL, 1LL};
    unsigned char       *rec;
    unsigned char       rec_buf[REC_SIZE];
    unsigned char       prev[REC_SIZE];
    char                sumbuf[U16_ASCII_BUF_SIZE];
    struct summary      local_summary;

    sum = &Summary;

    if ((rec = next_rec(in, rec_buf, sum)) == NULL)
    {
        fprintf(stderr, "there must be at least one record to be validated\n");
        exit(1);
    }
    bcopy(rec, sum->first_rec, REC_SIZE);
    bcopy(rec, prev, REC_SIZE);
    sum->rec_count = add16(sum->rec_count, one);
    
    while ((rec = next_rec(in, rec_buf, sum)) != NULL)
    {
        /* make sure the record key is equal to or more than the
         * previous key
         */
        diff = (*Compare)(prev, rec, 10);
        if (diff == 0)
            sum->dup_count = add16(sum->dup_count, one);
        else if (diff > 0)
        {
            if (sum->first_unordered.hi8 == 0 &&
                sum->first_unordered.lo8 == 0)
            {
                sum->first_unordered = sum->rec_count;
                if (!Quiet)
                    fprintf(stderr,
                            "First unordered record is record %s\n",
                            u16_to_dec(sum->first_unordered, sumbuf));
            }
            sum->unordered_count = add16(sum->unordered_count, one);
        }

        sum->rec_count = add16(sum->rec_count, one);
        bcopy(rec, prev, REC_SIZE);
    }
    bcopy(prev, sum->last_rec, REC_SIZE);  /* set last record for summary */

    return (0);
}


/* next_sum - get the next partition summary
 */
int next_sum(void *in, struct summary *sum)
{
    int                 ret;
    u16                 temp16 = {0LL, 0LL};

    ret = fread(sum, 1, SUM_SIZE, in);           /* get from file */
    
    if (ret == 0)
        return (0);
    else if (ret < 0)
    {
        fprintf(stderr, "summary read error\n");
        exit(1);
    }
    else if (ret != SUM_SIZE)
    {
        fprintf(stderr, "partial partition summary found at end\n");
        exit(1);
    }
    return (ret);
}


/* sum_summaries - validate a sequence of partition summaries
 */
void sum_summaries(void *in)
{
    struct summary      *sum;
    int                 diff;
    u16                 one = {0LL, 1LL};
    char                rec_buf[REC_SIZE];
    char                prev[REC_SIZE];
    char                sumbuf[U16_ASCII_BUF_SIZE];
    struct summary      local_sum;

    if (next_sum(in, &local_sum) == 0)
    {
        fprintf(stderr, "there must be at least one record to be validated\n");
        exit(1);
    }
    bcopy(&local_sum, &Summary, SUM_SIZE);
    bcopy(Summary.last_rec, prev, REC_SIZE);
    
    while (next_sum(in, &local_sum))
    {
        /* make sure the record key is equal to or more than the
         * previous key
         */
        diff = (*Compare)(prev, local_sum.first_rec, 10);
        if (diff == 0)
            Summary.dup_count = add16(Summary.dup_count, one);
        else if (diff > 0)
        {
            if (Summary.first_unordered.hi8 == 0 &&
                Summary.first_unordered.lo8 == 0)
            {
                if (!Quiet)
                    fprintf(stderr, "First unordered record is record %s\n",
                            u16_to_dec(Summary.rec_count, sumbuf));
                Summary.first_unordered = Summary.rec_count;
            }
            Summary.unordered_count = add16(Summary.unordered_count, one);
        }

        if ((Summary.first_unordered.hi8 == 0 &&
             Summary.first_unordered.lo8 == 0) &&
            !(local_sum.first_unordered.hi8 == 0 &&
              local_sum.first_unordered.lo8 == 0))
        {
            Summary.first_unordered =
                add16(Summary.rec_count, local_sum.first_unordered);
            if (!Quiet)
                fprintf(stderr, "First unordered record is record %s\n",
                        u16_to_dec(Summary.first_unordered, sumbuf));
        }

        Summary.unordered_count =
            add16(Summary.unordered_count, local_sum.unordered_count);
        Summary.rec_count = add16(Summary.rec_count, local_sum.rec_count);
        Summary.dup_count = add16(Summary.dup_count, local_sum.dup_count);
        Summary.checksum = add16(Summary.checksum, local_sum.checksum);
        bcopy(local_sum.last_rec, prev, REC_SIZE);
    }
    bcopy(prev, Summary.last_rec, REC_SIZE); /* get last rec of last summary */
}


/* get_input_fp - get input file pointer by opening input file for reading
 */
FILE *get_input_fp(char *filename)
{
    FILE        *in;
    
    if ((in = fopen(filename, "r")) == NULL)
    {
        perror(filename);
        exit(1);
    }
    return (in);
}


static char usage_str[] =
    "valsort sort output validator, $Revision: 1.2 $\n"
    "\n"
    "usage: valsort [-i] [-q] "
    "[-o SUMMARY_FILE] [-s] FILE_NAME\n"
    "-i        Use case insensitive ascii comparisons (optional for PennySort).\n"
    "          Case sensitive ascii or binary keys are assumed by default.\n"
    "-q        Quiet mode, don't output diagnostic text.\n"
    "-o SUMMARY_FILE  Output a summary of the validated records. This method\n"
    "          can be used to validate partitioned sort outputs separately.\n"
    "          The contents of the separate summary files can then be\n"
    "          concatenated into a single file that can be checked using\n"
    "          the valsort program with the -s flag.\n"
    "-s        The file to validate contains partition summaries instead of\n"
    "          sorted records.\n"
    "FILE_NAME The name of the sort output file or the partition summaries\n"
    "          file to validate.\n"
    "\n"
    "Example 1 - to validate the sorted order of a single sort output file:\n"
    "    valsort sortoutputfile\n"
    "\n"
    "Example 2 - to validate the sorted order of output that has been\n"
    "partitioned into 4 output files: out0.dat, out1.dat, out2.dat and out3.dat:\n"
    "    valsort -o out0.sum out0.dat\n"
    "    valsort -o out1.sum out1.dat\n"
    "    valsort -o out2.sum out2.dat\n"
    "    valsort -o out3.sum out3.dat\n"
    "    cat out0.sum out1.sum out2.sum out3.sum > all.sum\n"
    "    valsort -s all.sum\n"
    ;

void usage(void)
{
    fprintf(stderr, usage_str);
    exit(1);
}


int main(int argc, char *argv[])
{
    int                 diff;
    FILE                *in;
    char                sumbuf[U16_ASCII_BUF_SIZE];
    FILE                *out = NULL;
    
    Compare = (int (*)(const char *a, const char *b, size_t n))memcmp;
    
    while (argc > 1 && argv[1][0] == '-')
    {
        if (argv[1][1] == 'i')
            Compare =
                (int (*)(const char *a, const char *b, size_t n))strcasecmp;
        else if (argv[1][1] == 'o')
        {
            if (argc < 4 || argv[2][0] == '-')
                usage();
            if ((out = fopen(argv[2], "w")) == NULL)
            {
                perror(argv[2]);
                exit(1);
            }
            argc--;
            argv++;
        }
        else if (argv[1][1] == 'q')
            Quiet = 1;
        else if (argv[1][1] == 's')
            Read_summary = 1;
        else
            usage();
        argc--;
        argv++;
    }
    if (argc != 2)
        usage();

    /* if we are validating output partition summaries
     */
    if (Read_summary)
    {
        in = get_input_fp(argv[1]);
        sum_summaries(in);
    }
    /* else we are validating a file with records in sorted order */
    else
    {
        /* else validate the order of records with using a chaingang */
        in = get_input_fp(argv[1]);
        summarize_records(in, NULL);
    }

    /* if requested ("-o"), output a summary file */
    if (out != NULL)
        fwrite(&Summary, SUM_SIZE, 1, out);

    if (!Quiet)
    {
        fprintf(stderr, "Records: %s\n",
                u16_to_dec(Summary.rec_count, sumbuf));
        fprintf(stderr, "Checksum: %s\n",
                u16_to_hex(Summary.checksum, sumbuf));
        if (Summary.unordered_count.hi8 | Summary.unordered_count.lo8)
        {
            fprintf(stderr, "ERROR - there are %s unordered records\n",
                    u16_to_dec(Summary.unordered_count, sumbuf));
        }
        else
        {
            fprintf(stderr, "Duplicate keys: %s\n",
                    u16_to_dec(Summary.dup_count, sumbuf));
            fprintf(stderr, "SUCCESS - all records are in order\n");
        }
    }
    
    /* return non-zero if there are any unordered records */
    return (Summary.unordered_count.hi8 | Summary.unordered_count.lo8) ? 1 : 0;
}
