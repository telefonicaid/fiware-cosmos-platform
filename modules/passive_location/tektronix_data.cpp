#include <arpa/inet.h> // ntohl(), ntohs()
#include <stdint.h>  // uint32_t, uint16_t...
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#include "logMsg/logMsg.h"

bool parse_OHDR_header(unsigned char **p_ohdr, uint32_t *sizeOHDR, int *numDRs, int *typeMsg)
{
    *numDRs = 0;
    *typeMsg = 0;

    for (int i = 0; (i < 232); i++)
    {
        //OLM_M(("ohdr[%d](0x%0x) = 0x%0x", i, *p_ohdr+i, int((*p_ohdr)[i])));
    }


#define OHDR_HEADER_SIZE 12
    *sizeOHDR = ntohl(*((uint32_t *)*p_ohdr)) + sizeof(uint32_t);
    //OLM_M(("tras ntohl() sizeOHDR:%u", *sizeOHDR));

    *typeMsg = int(*(*p_ohdr+sizeof(uint32_t)));
    if (*typeMsg != 130)
    {
        //LM_M(("OHDR is not a data message type (%d != expected 130)", int(*(*p_ohdr+sizeof(uint32_t)))));

        *p_ohdr += *sizeOHDR;
        return false;
    }
    else
    {
        //OLM_M(("OHDR IS a data message type (%d == expected 130)", int(*(*p_ohdr+sizeof(uint32_t)))));
    }
    *numDRs = int(*(*p_ohdr+ 2*sizeof(uint32_t)));
    //OLM_M(("numDRs:%u", *numDRs));
    *p_ohdr += OHDR_HEADER_SIZE;
    return true;
}


bool parse_DR(unsigned char **p_dr, uint32_t *sizeDR, int *typeDR, uint64_t *timestamp, uint64_t *imsi, uint64_t *imei, uint64_t *msisdn, int *probeId)
{

    *typeDR = 0;
    *timestamp = 0;
    *imsi = 0;
    *imei = 0;
    *msisdn = 0;
    *probeId = 0;

    for (int i = 0; (i < 232); i++)
    {
        //OLM_M(("dr[%d](0x%0x) = 0x%0x", i, *p_dr+i, int((*p_dr)[i])));
    }

#define DR_HEADER_SIZE 4
    *sizeDR = ntohs(*((uint16_t *)*p_dr));
    //OLM_M(("tras ntohs() sizeDR:%u", *sizeDR));

    int total_lengh_elementIDs = int(*(*p_dr+3));
    //OLM_M(("total_lengh_elementIDs:%d", total_lengh_elementIDs));

    unsigned char bitmask = *(*p_dr+sizeof(uint16_t));
    //OLM_M(("bitmask:0x%0x", int(bitmask)));
    if (bitmask == 0)
    {
        *p_dr += *sizeDR;
        return false;
    }
#define TYPE_gsmA 2
#define TYPE_UMTS 5
    *typeDR = int(bitmask & 0x07);

#define DR_WORDSECTION_SIZE_gsmA 44
#define DR_WORDSECTION_SIZE_UMTS 52

#define DR_SHORTSECTION_SIZE_gsmA 28
#define DR_SHORTSECTION_SIZE_UMTS 24

    int num_elementIDs = 0;
    int fixed_length_word_section = 0;
    int fixed_length_short_section = 0;
    int desp_probeId = 0;
    int bit_imsi = 0;
    int bit_imei = 0;
    int bit_msisdn = 0;

    int varSize = 0;

    if (*typeDR == TYPE_gsmA)
    {
        //OLM_M(("typeDR=gsm-A"));
        //OLM_M(("bitmask >> 3: 0x%0x", int(bitmask >> 3)));
        total_lengh_elementIDs = total_lengh_elementIDs - 1;

        num_elementIDs = int(bitmask >> 3) & 0x07;
        fixed_length_word_section = DR_WORDSECTION_SIZE_gsmA;
        fixed_length_short_section = DR_SHORTSECTION_SIZE_gsmA;
        desp_probeId = 0;
        bit_imsi = 0;
        bit_msisdn = 1;
        bit_imei = 2;
    }
    else if (*typeDR == TYPE_UMTS)
    {
        //OLM_M(("typeDR=UMTS"));
        // If UMTS, size came in words, not bytes
        *sizeDR = *sizeDR * sizeof(uint32_t);
        total_lengh_elementIDs = total_lengh_elementIDs * sizeof(uint32_t);

        num_elementIDs = int((bitmask >> 3) & 0x03);
        fixed_length_word_section = DR_WORDSECTION_SIZE_UMTS;
        fixed_length_short_section = DR_SHORTSECTION_SIZE_UMTS;
        desp_probeId = 1;
        bit_imsi = 0;
        bit_imei = 8;
        bit_msisdn = 10;
    }
    else
    {
        //OLM_M(("Error, unknown type %d, (gsmA:%d, UMTS:%d)", *typeDR, TYPE_gsmA, TYPE_UMTS));
        *p_dr += *sizeDR;
        return false;
    }
    //OLM_M(("tras typeDR sizeDR:%u", *sizeDR));
    //LM_M(("tras typeDR total_lengh_elementIDs:%d", total_lengh_elementIDs));

    //LM_M(("num_elementIDs:%d", num_elementIDs));

    unsigned char *p_elementsID_mask = *p_dr + DR_HEADER_SIZE;

    for (int i = 0; (i < num_elementIDs); i++)
    {
        uint32_t elementMask = ntohl(*((uint32_t *)p_elementsID_mask));
        //OLM_M(("elementMask:0x%0x", elementMask));
        if (((elementMask >> 29) & 0x00000007) == 0)
        {
            //OLM_M(("Fixed Length Word section"));
            if ((elementMask >> 1) & 0x00000001)
            {
                *timestamp = ntohl(*((uint32_t *)(p_elementsID_mask + 2*sizeof(uint32_t))));
                //LM_M(("Fixed: timestamp:0x%0x", *timestamp));
            }

            p_elementsID_mask += fixed_length_word_section;
        }
        else if (((elementMask >> 29) & 0x00000007) == 1)
        {
            //OLM_M(("Fixed Length Short section"));
            if ((elementMask >> desp_probeId) & 0x00000001)
            {
                *probeId = ntohs(*((uint16_t *)(p_elementsID_mask + (sizeof(uint32_t) + sizeof(uint32_t)*desp_probeId))));
                //OLM_M(("probeId:0x%0x", *probeId));
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

                    if ((j == bit_imsi) || (j == bit_imei) || (j == bit_msisdn))
                    {
                        char *bufferVarSize = (char *)malloc(varSize+1);
                        char *p_buffer = bufferVarSize;
                        for (int k = 0; (k < varSize); k++)
                        {
                            *p_buffer++ = *p_ini_varElements++;
                        }
                        *p_buffer = '\0';

                        //LM_M(("bufferVarSize:'%s'", bufferVarSize));
                        uint64_t bufferLong = atoll(bufferVarSize);
                        //OLM_M(("bufferLong:%lu", bufferLong));

                        if (j == bit_imsi)
                        {
                            *imsi = bufferLong;
                            //LM_M(("imsi:0x%0x", *imsi));
                        }
                        else if (j == bit_msisdn)
                        {
                            *msisdn = bufferLong;
                            //LM_M(("msisdn:0x%0x", *msisdn));
                        }
                        else
                        {
                            *imei = bufferLong;
                            //LM_M(("imei:0x%0x", *imei));
                        }

                        free(bufferVarSize);
                    }
                    else
                    {
                        char *bufferVarSize = (char *)malloc(varSize+1);
                        char *p_buffer = bufferVarSize;
                        for (int k = 0; (k < varSize); k++)
                        {
                            *p_buffer++ = *p_ini_varElements++;
                        }
                        *p_buffer = '\0';

                        //LM_M(("bufferVarSize:'%s'", bufferVarSize));
                        free(bufferVarSize);
                        //p_ini_varElements += varSize;
                    }

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
