#ifndef MAPHASH_H
#define MAPHASH_H

#include "uthash.h"

//still needs to be fleshed out
typedef struct {
    int port;
    int ip;
} MAP, *PMAP;

void mapAdd(PMAP pmap);
PMAP mapFind(int port);
void mapDelete(PMAP pmap);
void mapDeleteAll();
void mapPrint();
int mapSort(PMAP a, PMAP b);
void mapSortById();

#endif
