#ifndef TEKTROKIX_DATA_H
#define TEKTROKIX_DATA_H
bool parse_OHDR_header(unsigned char **p_ohdr, uint32_t *sizeOHDR, int *numDRs, int *typeMsg);
bool parse_DR(unsigned char **p_dr, uint32_t *sizeDR, int *typeDR, uint64_t *timestamp, uint64_t *imsi, uint64_t *imei, uint64_t *msisdn, int *probeId);
#endif //de TEKTROKIX_DATA_H
