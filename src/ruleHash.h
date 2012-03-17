#ifndef RULEHASH_H
#define RULEHASH_H

#include "uthash.h"

typedef struct {
    int clientDestPort;
    unsigned int serverDestPort;
    unsigned int serverDestIp;
    UT_hash_handle hh;
} RULE, *PRULE;

void ruleAdd(unsigned int clientDestPort, unsigned int serverDestPort, unsigned int serverDestIp);
PRULE ruleFind(int clientDestPort);
void ruleDelete(int clientDestPort);
void ruleDeleteAll();
char* rulePrint();
int ruleSort(PRULE a, PRULE b);
void ruleSortById();

#endif
