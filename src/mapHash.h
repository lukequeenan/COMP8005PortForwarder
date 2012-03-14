#ifndef MAPHASH_H
#define MAPHASH_H

#include "uthash.h"

//still needs to be fleshed out
typedef struct {
    int clientPort;
    char clientIp[16];
    int serverPort;
    char serverIp[16];
    UT_hash_handle hh;
} MAP, *PMAP;

void mapAdd(PMAP pmap);
PMAP mapFind(int clientPort);
void mapDelete(PMAP pmap);
void mapDeleteAll();
void mapPrint();
int mapSort(PMAP a, PMAP b);
void mapSortById();

#endif
