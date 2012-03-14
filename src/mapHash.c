#include "mapHash.h"
#include <stdio.h>

PMAP hashMap = NULL;

void mapAdd(PMAP map) {
    HASH_ADD_INT(hashMap, clientPort, map);
}

PMAP mapFind(int clientPort) {
    PMAP map;

    HASH_FIND_INT(hashMap, &clientPort, map);  /* s: output pointer */
    return map;
}

void mapDelete(PMAP map) {
    HASH_DEL(hashMap, map);  /* user: pointer to deletee */
    free(map);
}

void mapDeleteAll() {
  PMAP current_map, tmp;

  HASH_ITER(hh, hashMap, current_map, tmp) {
    HASH_DEL(hashMap, current_map);  /* delete it (metrics advances to next) */
    free(current_map);            /* free it */
  }
}

void mapPrint() {
    PMAP s;
    int i = 0;
    for(s=hashMap; s != NULL; s=s->hh.next) {
        //print ipetId, addr in and addr out maybe more.
        //printf("user id %d: name %s\n", s->id, s->name);
        printf("map num: %d\n",i++);
    }
}

int mapSort(PMAP a, PMAP b) {
    return (a->clientPort - b->clientPort);
}

void mapSortById() {
    HASH_SORT(hashMap, mapSort);
}


