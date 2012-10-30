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
#include <arpa/inet.h> // ntohl(), ntohs()
#include <stdint.h>  // uint32_t, uint16_t...
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tektronix_data.h"

//#include "logMsg/logMsg.h"

void init_tek_record(struct struct_tek_record *tek_record)
{
    tek_record->typeDR = 0;
    tek_record->timestamp = 0;
    tek_record->imsi = 0;
    tek_record->tmsi = 0;
    tek_record->last_tmsi = 0;
    tek_record->imei = 0;
    tek_record->msisdn = 0;
    tek_record->probeId = 0;
    tek_record->LAC = 0;
    tek_record->cellID = 0;
    tek_record->callType = 0;
    tek_record->callNumber = 0;
    tek_record->DTAPCause = 0;
    tek_record->BSSMAPCause = 0;
    tek_record->ALCAPCause = 0;
    tek_record->CCCause = 0;
    tek_record->MMCause = 0;
    tek_record->RANAPCause = 0;
}

bool parse_OHDR_header(unsigned char **p_ohdr, uint32_t *sizeOHDR, int *numDRs, int *typeMsg)
{
    *numDRs = 0;
    *typeMsg = 0;

    for (int i = 0; (i < 232); i++)
    {
        //LM_M(("ohdr[%d](0x%0x) = 0x%0x", i, *p_ohdr+i, int((*p_ohdr)[i])));
    }


#define OHDR_HEADER_SIZE 12
    *sizeOHDR = ntohl(*((uint32_t *)*p_ohdr)) + sizeof(uint32_t);
    //LM_M(("tras ntohl() sizeOHDR:%u", *sizeOHDR));

    *typeMsg = int(*(*p_ohdr+sizeof(uint32_t)));
    if (*typeMsg != 130)
    {
        //LM_W(("OHDR is not a data message type (%d != expected 130)", int(*(*p_ohdr+sizeof(uint32_t)))));
        //fprintf(stderr, "OHDR is not a data message type (%d != expected 130)", int(*(*p_ohdr+sizeof(uint32_t))));

        *p_ohdr += *sizeOHDR;
        return false;
    }
    else
    {
        //LM_M(("OHDR IS a data message type (%d == expected 130)", int(*(*p_ohdr+sizeof(uint32_t)))));
    }
    *numDRs = int(*(*p_ohdr+ 2*sizeof(uint32_t)));
    //LM_M(("numDRs:%u", *numDRs));
    //fprintf(stderr, "numDRs:%u", *numDRs);
    *p_ohdr += OHDR_HEADER_SIZE;
    return true;
}


bool parse_DR(unsigned char **p_dr, uint32_t *sizeDR, struct struct_tek_record *tek_record)
{

init_tek_record(tek_record);


    for (int i = 0; (i < 232); i++)
    {
        //LM_M(("dr[%d](0x%0x) = 0x%0x", i, *p_dr+i, int((*p_dr)[i])));
    }

#define DR_HEADER_SIZE 4
    *sizeDR = ntohs(*((uint16_t *)*p_dr));
    //LM_M(("tras ntohs() sizeDR:%u", *sizeDR));
    //fprintf(stderr, "tras ntohs() sizeDR:%u\n", *sizeDR);


    int total_lengh_elementIDs = int(*(*p_dr+3));
    //LM_M(("total_lengh_elementIDs:%d", total_lengh_elementIDs));
    //fprintf(stderr, "total_lengh_elementIDs:%d\n", total_lengh_elementIDs);


    unsigned char bitmask = *(*p_dr+sizeof(uint16_t));
    //LM_M(("bitmask:0x%0x", int(bitmask)));
    //fprintf(stderr, "bitmask:0x%0x\n", int(bitmask));
    if (bitmask == 0)
    {
        *p_dr += *sizeDR;
        return false;
    }
#define TYPE_gsmA 2
#define TYPE_UMTS 5
    tek_record->typeDR = int(bitmask & 0x07);

#define DR_WORDSECTION_SIZE_gsmA 44
#define DR_WORDSECTION_SIZE_UMTS 52

#define DR_SHORTSECTION_SIZE_gsmA 28
#define DR_SHORTSECTION_SIZE_UMTS 24

    int num_elementIDs = -1;
    int fixed_length_word_section = -1;
    int fixed_length_short_section = -1;
    int bit_WF_callNumber = -1;
    int bit_WF_timestamp = -1;
    int bit_SF_probeId = -1;
    int bit_SF_lac = -1;;
    int bit_SF_cellID = -1;
    int bit_SF_callType = -1;
    int bit_SF_DTAPCause = -1;
    int bit_SF_BSSMAPCause = -1;
    int bit_SF_ALCAPCause = -1;
    int bit_VF_imsi = -1;
    int bit_VF_imei = -1;
    int bit_VF_first_tmsi = -1;
    int bit_VF_last_tmsi = -1;
    int bit_VF_msisdn = -1;
    int bit_VF_CCCause = -1;
    int bit_VF_MMCause = -1;
    int bit_VF_RANAPCause = -1;

    int varSize = 0;

    if (tek_record->typeDR == TYPE_gsmA)
    {
        //LM_M(("typeDR=gsm-A"));
        //LM_M(("bitmask >> 3: 0x%0x", int(bitmask >> 3)));
        total_lengh_elementIDs = total_lengh_elementIDs - 1;

        num_elementIDs = int((bitmask >> 3) & 0x07);

        int interfaceType = int((bitmask >> 6) & 0x03);

        if (interfaceType != 0)
        {
            //LM_E(("Error, wrong interfaceType:%d for gsm-A:0", interfaceType));
            //fprintf(stderr, "Error, wrong interfaceType:%d for gsm-A:0", interfaceType);
            *p_dr += *sizeDR;
            return false;
        }

        bit_WF_callNumber = 0;
        bit_WF_timestamp = 1;

        bit_SF_probeId = 0;
        bit_SF_lac = 4;
        bit_SF_cellID = 5;
        bit_SF_BSSMAPCause = 6;
        bit_SF_DTAPCause = 7;
        bit_SF_callType = 10;

        bit_VF_imsi = 0;
        bit_VF_msisdn = 1;
        bit_VF_imei = 2;
        bit_VF_last_tmsi = 5;
        bit_VF_first_tmsi = 6;
    }
    else if (tek_record->typeDR == TYPE_UMTS)
    {
        //LM_M(("typeDR=UMTS"));
        // If UMTS, size came in words, not bytes
        *sizeDR = *sizeDR * sizeof(uint32_t);
        total_lengh_elementIDs = total_lengh_elementIDs * sizeof(uint32_t);

        num_elementIDs = int((bitmask >> 3) & 0x03);

        int interfaceType = int((bitmask >> 5) & 0x07);

        if (interfaceType != 1)
        {
            //LM_E(("Error, wrong interfaceType:%d for UMTS:1", interfaceType));
            //fprintf(stderr, "Error, wrong interfaceType:%d for UMTS:1", interfaceType);
            *p_dr += *sizeDR;
            return false;
        }

        bit_WF_callNumber = 0;
        bit_WF_timestamp = 1;

        bit_SF_probeId = 1;
        bit_SF_callType = 3;
        bit_SF_lac = 5;
        bit_SF_cellID = 7;
        bit_SF_ALCAPCause = 4;

        bit_VF_imsi = 0;
        bit_VF_first_tmsi = 1;
        bit_VF_last_tmsi = 2;
        bit_VF_RANAPCause = 5;
        bit_VF_CCCause = 6;
        bit_VF_MMCause = 7;
        bit_VF_imei = 8;
        bit_VF_msisdn = 10;
    }
    else
    {
        //LM_E(("Error, unknown type %d, (gsmA:%d, UMTS:%d)", tek_record->typeDR, TYPE_gsmA, TYPE_UMTS));
        *p_dr += *sizeDR;
        return false;
    }
    //LM_M(("tras typeDR sizeDR:%u", *sizeDR));
    //LM_M(("tras typeDR total_lengh_elementIDs:%d", total_lengh_elementIDs));

    //LM_M(("num_elementIDs:%d", num_elementIDs));

    unsigned char *p_elementsID_mask = *p_dr + DR_HEADER_SIZE;

    for (int i = 0; (i < num_elementIDs); i++)
    {
        uint32_t elementMask = ntohl(*((uint32_t *)p_elementsID_mask));
        //LM_M(("elementMask:0x%0x", elementMask));

        uint32_t bitMask = elementMask;
        int numFields = 0;
        for (int j = 0; (j < 28); j++)
        {
            if (bitMask & 0x00000001)
            {
                numFields++;
            }
            bitMask >>= 1;
        }

        if (((elementMask >> 29) & 0x00000007) == 0)
        {
            //LM_M(("Fixed Length Word section"));
            fixed_length_word_section = sizeof(uint32_t) + numFields*sizeof(uint32_t);

            unsigned char *p_ini_WFElements = p_elementsID_mask + sizeof(uint32_t);
#define NUM_4BYTELENGTH_FIELDS 10
            for (int j = 0; (j < NUM_4BYTELENGTH_FIELDS); j++)
            {
                if ((elementMask >> j) & 0x00000001)
                {
                    if (j == bit_WF_callNumber)
                    {
                        tek_record->callNumber = ntohl(*((uint32_t *)(p_ini_WFElements)));
                        //LM_M(("callNumber:0x%0x", tek_record->callNumber));
                    }
                    else if (j == bit_WF_timestamp)
                    {
                        tek_record->timestamp = ntohl(*((uint32_t *)(p_ini_WFElements)));
                        //LM_M(("timestamp:0x%0x", tek_record->timestamp));
                    }
                    p_ini_WFElements += sizeof(uint32_t);
                }
            }
            p_elementsID_mask += fixed_length_word_section;
        }
        else if (((elementMask >> 29) & 0x00000007) == 1)
        {
            fixed_length_short_section = sizeof(uint32_t) + numFields*sizeof(uint16_t);

            unsigned char *p_ini_SFElements = p_elementsID_mask + sizeof(uint32_t);
#define NUM_2BYTELENGTH_FIELDS 12
            for (int j = 0; (j < NUM_2BYTELENGTH_FIELDS); j++)
            {
                if ((elementMask >> j) & 0x00000001)
                {
                    if (j == bit_SF_probeId)
                    {
                        tek_record->probeId = ntohs(*((uint16_t *)(p_ini_SFElements)));
                        //LM_M(("probeId:0x%0x", tek_record->probeId));
                    }
                    else if (j == bit_SF_lac)
                    {
                        tek_record->LAC = ntohs(*((uint16_t *)(p_ini_SFElements)));
                        //LM_M(("LAC:0x%0x", tek_record->LAC));
                    }
                    else if (j == bit_SF_cellID)
                    {
                        tek_record->cellID = ntohs(*((uint16_t *)(p_ini_SFElements)));
                        //LM_M(("cellID:0x%0x", tek_record->cellID));
                    }
                    else if (j == bit_SF_callType)
                    {
                        tek_record->callType = ntohs(*((uint16_t *)(p_ini_SFElements)));
                        //LM_M(("callType:0x%0x", tek_record->callType));
                    }
                    else if (j == bit_SF_DTAPCause)
                    {
                        tek_record->DTAPCause = ntohs(*((uint16_t *)(p_ini_SFElements)));
                        //LM_M(("DTAPCause:0x%0x", tek_record->DTAPCause));
                    }
                    else if (j == bit_SF_BSSMAPCause)
                    {
                        tek_record->BSSMAPCause = ntohs(*((uint16_t *)(p_ini_SFElements)));
                        //LM_M(("BSSMAPCause:0x%0x", tek_record->BSSMAPCause));
                    }
                    else if (j == bit_SF_ALCAPCause)
                    {
                        tek_record->ALCAPCause = ntohs(*((uint16_t *)(p_ini_SFElements)));
                        //LM_M(("ALCAPCause:0x%0x", tek_record->ALCAPCause));
                    }

                    p_ini_SFElements += sizeof(uint16_t);
                }
            }

            p_elementsID_mask += fixed_length_short_section;
        }
        else if (((elementMask >> 29) & 0x00000007) == 2)
        {
            //LM_M(("Variable Length section"));
            unsigned char *p_ini_varElements = p_elementsID_mask + sizeof(uint32_t);
#define NUM_VARLENGTH_FIELDS 28
            for (int j = 0; (j < NUM_VARLENGTH_FIELDS); j++)
            {
                if ((elementMask >> j) & 0x00000001)
                {
                    varSize = int(*p_ini_varElements);
                    p_ini_varElements++;
                    //LM_M(("Detected element:%d with varSize:%d", j, varSize));

                    char *bufferVarSize = (char *)malloc(varSize+1);
                    char *p_buffer = bufferVarSize;
                    for (int k = 0; (k < varSize); k++)
                    {
                        *p_buffer++ = *p_ini_varElements++;
                    }
                    *p_buffer = '\0';


                    if (j == bit_VF_imsi)
                    {
                        uint64_t bufferLong = atoll(bufferVarSize);
                        tek_record->imsi = bufferLong;
                        //LM_M(("imsi:0x%0x", tek_record->imsi));
                    }
                    if (j == bit_VF_first_tmsi)
                    {
                        uint64_t bufferLong = atoll(bufferVarSize);
                        tek_record->tmsi = bufferLong;
                        //LM_M(("tmsi:0x%0x", tek_record->tmsi));
                    }
                    if (j == bit_VF_last_tmsi)
                    {
                        uint64_t bufferLong = atoll(bufferVarSize);
                        tek_record->last_tmsi = bufferLong;
                        //LM_M(("last_tmsi:0x%0x", tek_record->last_tmsi));
                    }
                    else if (j == bit_VF_imei)
                    {
                        uint64_t bufferLong = atoll(bufferVarSize);
                        tek_record->imei = bufferLong;
                        //LM_M(("imei:0x%0x", tek_record->imei));
                    }
                    else if (j == bit_VF_msisdn)
                    {
                        uint64_t bufferLong = atoll(bufferVarSize);
                        tek_record->msisdn = bufferLong;
                        //LM_M(("msisdn:0x%0x", tek_record->msisdn));
                    }
                    else if (j == bit_VF_CCCause)
                    {
                        int bufferInt = atol(bufferVarSize);
                        tek_record->CCCause = bufferInt;
                        //tek_record->CCCause = strdup(bufferVarSize);
                        //fprintf(stdout, "CCCause varSize:%d\n", varSize);
                        //LM_M(("CCCause:%s", tek_record->CCCause));
                    }
                    else if (j == bit_VF_MMCause)
                    {
                        int bufferInt = atol(bufferVarSize);
                        tek_record->MMCause = bufferInt;
                        //tek_record->MMCause = strdup(bufferVarSize);
                        //fprintf(stdout, "MMCause varSize:%d\n", varSize);
                        //LM_M(("MMCause:%s", tek_record->MMCause));
                    }
                    else if (j == bit_VF_RANAPCause)
                    {
                        int bufferInt = atol(bufferVarSize);
                        tek_record->RANAPCause = bufferInt;
                        //tek_record->RANAPCause = strdup(bufferVarSize);
                        //fprintf(stdout, "RANAPCause varSize:%d\n", varSize);
                        //LM_M(("RANAPCause:%s", tek_record->RANAPCause));
                    }

                    free(bufferVarSize);
                }
            }
        }
        else
        {
            //LM_W(("Error in elementMask for fixed sections, 0x%0x should be 0, 1 or 2, in bits 32-30", elementMask));
            *p_dr += *sizeDR;
            return false;
        }
    }

//    unsigned char *p_var_section = *p_dr + DR_HEADER_SIZE + total_lengh_elementIDs;
//
//    int lengthVarFields = ntohs(*((uint16_t *)p_var_section));
//    LM_M(("lengthVarFields:%d", lengthVarFields));
//    int numVarFields = ntohs(*((uint16_t *)(p_var_section + sizeof(uint16_t))));
//    LM_M(("numVarFields:%d", numVarFields));
//    int formatID = ntohs(*((uint16_t *)(p_var_section + 2*sizeof(uint16_t))));
//    LM_M(("formatID:%d", formatID));
//
//
//    p_var_section += 3*sizeof(uint16_t);
//
//    for (int i = 0; (i < numVarFields); i++)
//    {
//	LM_M(("Varable DataID:%d", ntohs(uint16_t(*(p_var_section)))));
//        varSize = int(*(p_var_section + sizeof(uint16_t) + 1));
//        if ((*(p_var_section + sizeof(uint16_t))) & 0x00000001)
//        {
//            *timestamp = ntohl(*(uint32_t *)(p_var_section + 1 + varSize));
//	    LM_M(("Variable: timestamp:0x%0x", *timestamp));
//	    varSize += sizeof(uint32_t);
//            break;
//        }
//        if ((*(p_var_section + sizeof(uint16_t))) & 0x00000002)
//        {
//	    varSize += sizeof(uint32_t);
//        }
//        p_var_section += sizeof(uint16_t) + 1 + varSize;
//    }

    *p_dr += *sizeDR;
    return true;

}
