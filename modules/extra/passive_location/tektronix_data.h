#ifndef TEKTROKIX_DATA_H
#define TEKTROKIX_DATA_H

struct struct_tek_record
{
    int typeDR;
    uint64_t timestamp;
    uint64_t imsi;
    uint64_t imei;
    uint64_t tmsi;
    uint64_t last_tmsi;
    uint64_t msisdn;
    int probeId;
    int LAC;
    int cellID;
    int callType;
    uint32_t callNumber;
    int DTAPCause;
    int BSSMAPCause;
    int ALCAPCause;
    int CCCause;
    int MMCause;
    int RANAPCause;
};

void init_tek_record(struct struct_tek_record *tek_record);
bool parse_OHDR_header(unsigned char **p_ohdr, uint32_t *sizeOHDR, int *numDRs, int *typeMsg);
//bool parse_DR(unsigned char **p_dr, uint32_t *sizeDR, int *typeDR, uint64_t *timestamp, uint64_t *imsi, uint64_t *imei, uint64_t *msisdn, int *probeId);
//bool parse_DR(unsigned char **p_dr, uint32_t *sizeDR, int *typeDR, uint64_t *timestamp, uint64_t *imsi, uint64_t *imei, uint64_t *msisdn, int *probeId, int *LAC, int *cellID, int *callType, uint32_t *callNumber, int *DTAPCause, int *BSSMAPCause, char **CCCause, char **MMCause, char **RANAPCause, char **ALCAPCause );
bool parse_DR(unsigned char **p_dr, uint32_t *sizeDR, struct struct_tek_record *tek_record);
#endif //de TEKTROKIX_DATA_H
