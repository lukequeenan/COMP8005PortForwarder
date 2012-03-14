#include "mapHash.h"
#include <stdio.h>

int main() {
	PMAP map = malloc(sizeof(MAP));
    printf("a\n");
    map->clientPort = 2;
    mapAdd(map);
    printf("b\n");
    PMAP b = mapFind(2);
    printf("address %d\n",b);
//	printf("map port %d\n", b->clientPort);
	return 0;
}
