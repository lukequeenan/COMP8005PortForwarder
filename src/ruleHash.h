#ifndef RULEHASH_H
#define RULEHASH_H

#include "uthash.h"

#define MAXBUFSIZE 1024

typedef struct {
    unsigned short clientDestPort;
    unsigned short serverDestPort;
    unsigned int serverDestIp;
    char *serverIp;
    char *forwarderIp;
    UT_hash_handle hh;
} RULE, *PRULE;

void ruleAdd(unsigned short clientDestPort, unsigned short serverDestPort, unsigned int serverDestIp,
             char* serverIp, char* forwarderIp);
PRULE ruleFind(short clientDestPort);
void ruleDelete(short clientDestPort);
void ruleDeleteAll();
char* rulePrint();
char* internRulePrint();
int ruleSort(PRULE a, PRULE b);
void ruleSortById();

#endif
