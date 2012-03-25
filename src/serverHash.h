#ifndef SERVERHASH_H
#define SERVERHASH_H

#include "uthash.h"

typedef struct {
    short serverPort;
    unsigned int clientIp;
    unsigned short clientPort;
    unsigned short clientSrcPort;
    UT_hash_handle hh;
} SERVER, *PSERVER;

void serverAdd(unsigned short serverPort, unsigned int clientIp, unsigned short clientPort, unsigned short clientSrcPort);
PSERVER serverFind(unsigned short serverPort);
void serverDelete(unsigned short serverPort);
void serverDeleteAll();
void serverPrint();
int serverSort(PSERVER a, PSERVER b);
void serverSortById();

#endif
