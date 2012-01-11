#ifndef TEKTROKIX_DATA_H
#define TEKTROKIX_DATA_H
bool parse_OHDR_header(unsigned char **p_ohdr, uint32_t *sizeOHDR, int *numDRs);
bool parse_DR(unsigned char **p_dr, uint32_t *sizeDR, int *typeDR, int *timestamp, int *imsi, int *imei, int *probeId);
#endif //de TEKTROKIX_DATA_H
