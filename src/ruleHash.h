#ifndef RULEHASH_H
#define RULEHASH_H

#include "uthash.h"

typedef struct {
    unsigned short clientDestPort;
    unsigned short serverDestPort;
    unsigned int serverDestIp;
    UT_hash_handle hh;
} RULE, *PRULE;

void ruleAdd(unsigned short clientDestPort, unsigned short serverDestPort, unsigned int serverDestIp);
PRULE ruleFind(short clientDestPort);
void ruleDelete(short clientDestPort);
void ruleDeleteAll();
char* rulePrint();
int ruleSort(PRULE a, PRULE b);
void ruleSortById();

#endif
