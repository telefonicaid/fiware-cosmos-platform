/* gensort.c - Generator program for sort benchmarks.
 *
 * Version 1.3  16 Oct 2009  Chris Nyberg <chris.nyberg@ordinal.com>
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
#include "rand16.h"
#include <zlib.h>   /* use crc32() function in zlib */

#define REC_SIZE 100
#define HEX_DIGIT(x) ((x) >= 10 ? 'A' + (x) - 10 : '0' + (x))


/* Structure for a 10-deep queue of random numbers.  This queue is used
 * to cheaply create records where every byte is psuedo random, while
 * only creating one 128-bit number per record.  The 10-byte keys that
 * begin each record are generated using the top 10 bytes of a random
 * number (this is exactly the same as was done in the original gensort
 * program).  The next 90 bytes of each record are broken into 9 10-byte
 * parts.  Each part is generated using subsequent random numbers in the
 * queue xor'ed with a constant that is particular to that part. 
 */
#define QUEUE_SIZE      10
#define get_queue_rand(rq, index) (rq->rand[(index + rq->head_index) % QUEUE_SIZE])
typedef struct
{
    int         head_index;             /* index of head of queue in rand[] */
    u16         curr_rec_number;        /* current record number */
    u16         rand[QUEUE_SIZE];       /* circular queue of random numbers */
} rand_queue;
#define RQ(rq, i) (rq->rand[rq->head_index + i - (rq->head_index + i >= QUEUE_SIZE ? QUEUE_SIZE : 0)])
#define ASSIGN_10_BYTES(rec_buf, rand) \
    (rec_buf)[0] = (rand.hi8 >> 56) & 0xFF; \
    (rec_buf)[1] = (rand.hi8 >> 48) & 0xFF; \
    (rec_buf)[2] = (rand.hi8 >> 40) & 0xFF; \
    (rec_buf)[3] = (rand.hi8 >> 32) & 0xFF; \
    (rec_buf)[4] = (rand.hi8 >> 24) & 0xFF; \
    (rec_buf)[5] = (rand.hi8 >> 16) & 0xFF; \
    (rec_buf)[6] = (rand.hi8 >>  8) & 0xFF; \
    (rec_buf)[7] = (rand.hi8 >>  0) & 0xFF; \
    (rec_buf)[8] = (rand.lo8 >> 56) & 0xFF; \
    (rec_buf)[9] = (rand.lo8 >> 48) & 0xFF;


int         Print_checksum;     /* boolean to produce record checksum */
u16         Sum16;              /* record checksum */
void        (*Gen)(unsigned char *buf, rand_queue *rq); /* ptr to generator */
int         Skip_output;        /* boolean to skip output */


/* init_rand_queue - initialize a queue of random numbers 
 */
void init_rand_queue(rand_queue *rq, u16 starting_rec_number)
{
    int         i;
    
    rq->head_index = 0;
    rq->curr_rec_number = starting_rec_number;
    rq->rand[0] = next_rand(skip_ahead_rand(rq->curr_rec_number));
    for (i = 1; i < QUEUE_SIZE; i++)
        rq->rand[i] = next_rand(rq->rand[i - 1]);
}


/* bump_queue - progress random queue to next random number and record number.
 */
void bump_queue(rand_queue *rq)
{
    int tail_index;

    /* head_index is the head of the queue.  find the tail index. */
    tail_index = rq->head_index - 1;
    if (tail_index < 0)
        tail_index = QUEUE_SIZE - 1;

    /* make a new tail entry where the current head is */
    rq->rand[rq->head_index] = next_rand(rq->rand[tail_index]);

    /* bump the head_index to make a new head of the queue */
    if (++rq->head_index == QUEUE_SIZE)
        rq->head_index = 0;

    /* bump the current record number */
    if (++rq->curr_rec_number.lo8 == 0)
        ++rq->curr_rec_number.hi8;
}


/* gen_rec - generate a "binary" record suitable for all sort
 *              benchmarks *except* PennySort.
 */
void gen_rec(unsigned char *rec_buf, rand_queue *rq)
{
    int         i;
    u16         rand;
    u16         rec_number = rq->curr_rec_number;
    
    /* generate the 10-byte key using the high 10 bytes of the 1st 128-bit
     * random number
     */
    rand = RQ(rq, 0);
    ASSIGN_10_BYTES(rec_buf + 0, rand);

    /* generate next 10 bytes using 2nd random number, xor with a constant
     * that is specific to the 2nd ten bytes.  This could allow a
     * a rogue entrant to compress the sort input 10 : 1 using these
     * same xor constants, but this would violate the sort contest rules.
     * The psuedo-random records should make ineffective any inadvertent
     * compression at the hardware or lower levels of the network protocols.
     */
    rand = RQ(rq, 1);
    rand.hi8 ^= 0xF0E8E4E2E1D8D4D2;     rand.lo8 ^= 0xD1CC000000000000;
    ASSIGN_10_BYTES(rec_buf + 10, rand);

    /* get next 10 bytes using 3rd random number, xor with specific constant
     */
    rand = RQ(rq, 2);
    rand.hi8 ^= 0xCAC9C6C5C3B8B4B2;     rand.lo8 ^= 0xB1AC000000000000;
    ASSIGN_10_BYTES(rec_buf + 20, rand);

    /* get next 10 bytes using 4th random number, xor with specific constant
     */
    rand = RQ(rq, 3);
    rand.hi8 ^= 0xAAA9A6A5A39C9A99;     rand.lo8 ^= 0x9695000000000000;
    ASSIGN_10_BYTES(rec_buf + 30, rand);

    /* get next 10 bytes using 5th random number, xor with specific constant
     */
    rand = RQ(rq, 4);
    rand.hi8 ^= 0x938E8D8B87787472;     rand.lo8 ^= 0x716C000000000000;
    ASSIGN_10_BYTES(rec_buf + 40, rand);

    /* get next 10 bytes using 6th random number, xor with specific constant
     */
    rand = RQ(rq, 5);
    rand.hi8 ^= 0x6A696665635C5A59;     rand.lo8 ^= 0x5655000000000000;
    ASSIGN_10_BYTES(rec_buf + 50, rand);

    /* get next 10 bytes using 7th random number, xor with specific constant
     */
    rand = RQ(rq, 6);
    rand.hi8 ^= 0x534E4D4B473C3A39;     rand.lo8 ^= 0x3635000000000000;
    ASSIGN_10_BYTES(rec_buf + 60, rand);

    /* get next 10 bytes using 8th random number, xor with specific constant
     */
    rand = RQ(rq, 7);
    rand.hi8 ^= 0x332E2D2B271E1D1B;     rand.lo8 ^= 0x170F000000000000;
    ASSIGN_10_BYTES(rec_buf + 70, rand);

    /* get next 10 bytes using 9th random number, xor with specific constant
     */
    rand = RQ(rq, 8);
    rand.hi8 ^= 0xC8C4C2C198949291;     rand.lo8 ^= 0x8CE0000000000000;
    ASSIGN_10_BYTES(rec_buf + 80, rand);

    /* get last 10 bytes using 10th random number, xor with specific constant
     */
    rand = RQ(rq, 9);
    rand.hi8 ^= 0x170F332E2D2B271E;     rand.lo8 ^= 0x1D1B000000000000;
    ASSIGN_10_BYTES(rec_buf + 90, rand);
}


/* gen_ascii_rec = generate an ascii record suitable for all sort
 *              benchmarks including PennySort.
 */
void gen_ascii_rec(unsigned char *rec_buf, rand_queue *rq)
{
    int         i;
    u16         rand = rq->rand[rq->head_index];
    u16         rec_number = rq->curr_rec_number;
    u8          temp;
    
    /* generate the 10-byte ascii key using mostly the high 64 bits.
     */
    temp = rand.hi8;
    rec_buf[0] = ' ' + (temp % 95);
    temp /= 95;
    rec_buf[1] = ' ' + (temp % 95);
    temp /= 95;
    rec_buf[2] = ' ' + (temp % 95);
    temp /= 95;
    rec_buf[3] = ' ' + (temp % 95);
    temp /= 95;
    rec_buf[4] = ' ' + (temp % 95);
    temp /= 95;
    rec_buf[5] = ' ' + (temp % 95);
    temp /= 95;
    rec_buf[6] = ' ' + (temp % 95);
    temp /= 95;
    rec_buf[7] = ' ' + (temp % 95);
    temp = rand.lo8;
    rec_buf[8] = ' ' + (temp % 95);
    temp /= 95;
    rec_buf[9] = ' ' + (temp % 95);
    temp /= 95;

    /* add 2 bytes of "break" */
    rec_buf[10] = ' ';
    rec_buf[11] = ' ';
    
    /* convert the 128-bit record number to 32 bits of ascii hexadecimal
     * as the next 32 bytes of the record.
     */
    for (i = 0; i < 16; i++)
        rec_buf[12 + i] = HEX_DIGIT((rec_number.hi8 >> (60 - 4 * i)) & 0xF);
    for (i = 0; i < 16; i++)
        rec_buf[28 + i] = HEX_DIGIT((rec_number.lo8 >> (60 - 4 * i)) & 0xF);

    /* add 2 bytes of "break" data */
    rec_buf[44] = ' ';
    rec_buf[45] = ' ';

    /* add 52 bytes of filler based on low 48 bits of random number */
    rec_buf[46] = rec_buf[47] = rec_buf[48] = rec_buf[49] = 
        HEX_DIGIT((rand.lo8 >> 48) & 0xF);
    rec_buf[50] = rec_buf[51] = rec_buf[52] = rec_buf[53] = 
        HEX_DIGIT((rand.lo8 >> 44) & 0xF);
    rec_buf[54] = rec_buf[55] = rec_buf[56] = rec_buf[57] = 
        HEX_DIGIT((rand.lo8 >> 40) & 0xF);
    rec_buf[58] = rec_buf[59] = rec_buf[60] = rec_buf[61] = 
        HEX_DIGIT((rand.lo8 >> 36) & 0xF);
    rec_buf[62] = rec_buf[63] = rec_buf[64] = rec_buf[65] = 
        HEX_DIGIT((rand.lo8 >> 32) & 0xF);
    rec_buf[66] = rec_buf[67] = rec_buf[68] = rec_buf[69] = 
        HEX_DIGIT((rand.lo8 >> 28) & 0xF);
    rec_buf[70] = rec_buf[71] = rec_buf[72] = rec_buf[73] = 
        HEX_DIGIT((rand.lo8 >> 24) & 0xF);
    rec_buf[74] = rec_buf[75] = rec_buf[76] = rec_buf[77] = 
        HEX_DIGIT((rand.lo8 >> 20) & 0xF);
    rec_buf[78] = rec_buf[79] = rec_buf[80] = rec_buf[81] = 
        HEX_DIGIT((rand.lo8 >> 16) & 0xF);
    rec_buf[82] = rec_buf[83] = rec_buf[84] = rec_buf[85] = 
        HEX_DIGIT((rand.lo8 >> 12) & 0xF);
    rec_buf[86] = rec_buf[87] = rec_buf[88] = rec_buf[89] = 
        HEX_DIGIT((rand.lo8 >>  8) & 0xF);
    rec_buf[90] = rec_buf[91] = rec_buf[92] = rec_buf[93] = 
        HEX_DIGIT((rand.lo8 >>  4) & 0xF);
    rec_buf[94] = rec_buf[95] = rec_buf[96] = rec_buf[97] = 
        HEX_DIGIT((rand.lo8 >>  0) & 0xF);

    /* add 2 bytes of "break" data */
    rec_buf[98] = '\r'; /* nice for Windows */
    rec_buf[99] = '\n';
}



static char usage_str[] =
    "gensort sort input generator, $Revision: 1.2 $\n"
    "\n"
    "usage: gensort [-a] [-c] [-bSTARTING_REC_NUM] "
    "NUM_RECS FILE_NAME\n"
    "-a        Generate ascii records required for PennySort or JouleSort.\n"
    "          These records are also an alternative input for the other\n"
    "          sort benchmarks.  Without this flag, binary records will be\n"
    "          generated that contain the highest density of randomness in\n"
    "          the 10-byte key.\n"
    "-c        Calculate the sum of the crc32 checksums of each of the\n"
    "          generated records and send it to standard error.\n"
    "-bN       Set the beginning record generated to N. By default the\n"
    "          first record generated is record 0.\n"
    "NUM_RECS  The number of sequential records to generate.\n"
    "FILE_NAME The name of the file to write the records to.\n"
    "\n"
    "Example 1 - to generate 1000000 ascii records starting at record 0 to\n"
    "the file named \"pennyinput\":\n"
    "    gensort -a 1000000 pennyinput\n"
    "\n"
    "Example 2 - to generate 1000 binary records beginning with record 2000\n"
    "to the file named \"partition2\":\n"
    "    gensort -b2000 1000 partition2\n";

void usage(void)
{
    fprintf(stderr, usage_str);
    exit(1);
}


int main(int argc, char *argv[])
{
    u8                  j;                      /* should be a u16 someday */
    u16                 starting_rec_number;
    u16                 num_recs;
    unsigned char       rec_buf[REC_SIZE];
    FILE                *out;
    u16                 temp16 = {0LL, 0LL};
    char                sumbuf[U16_ASCII_BUF_SIZE];
    char                prntbuf[U16_ASCII_BUF_SIZE];
    rand_queue          rq;

    starting_rec_number.hi8 = 0;
    starting_rec_number.lo8 = 0;
    Gen = gen_rec;

    while (argc > 1 && argv[1][0] == '-')
    {
        if (argv[1][1] == 'a')
            Gen = gen_ascii_rec;
        else if (argv[1][1] == 'b')
            starting_rec_number = dec_to_u16(argv[1] + 2);
        else if (argv[1][1] == 'c')
            Print_checksum = 1;
        else
            usage();
        argc--;
        argv++;
    }
    if (argc != 3)
        usage();
    num_recs = dec_to_u16(argv[1]);
    Skip_output = (strcmp(argv[2], "/dev/null") == 0);

    if ((out = fopen(argv[2], "w")) == NULL)
    {
        perror(argv[2]);
        exit(1);
    }
        
    init_rand_queue(&rq, starting_rec_number);
    
    for (j = 0; j < num_recs.lo8; j++)
    {
        (*Gen)(rec_buf, &rq);
        if (Print_checksum)
        {
            temp16.lo8 = crc32(0, rec_buf, REC_SIZE);
            Sum16 = add16(Sum16, temp16);
        }
        if (!Skip_output)
            fwrite(rec_buf, REC_SIZE, 1, out);
        bump_queue(&rq);
    }

    if (Print_checksum)
        fprintf(stderr, "%s\n", u16_to_hex(Sum16, sumbuf));
    return (0);
}
