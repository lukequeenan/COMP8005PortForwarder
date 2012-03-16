#include "clientHash.h"
#include <stdio.h>
#include <string.h>

PCLIENT hashClient = NULL;




//this works in theory not well tested
unsigned long combineIpPort(unsigned int ip, unsigned int port) {
    unsigned long x;
    unsigned long a = ip;
    unsigned long b = port;

    x = a << (sizeof(unsigned int) * 8);
    x = x | b;
    return x;
}

void clientAdd(unsigned int clientIp, unsigned int clientPort, unsigned int serverIp, unsigned int serverPort)
{
    PCLIENT cli = malloc(sizeof(CLIENT));
    //#define HASH_ADD(hh,head,fieldname,keylen_in,add)

    cli->clientIpPort = combineIpPort(clientIp, clientPort);
    cli->serverIp = serverIp;
    cli->serverPort = serverPort;
    HASH_ADD_LONG(hashClient, clientIpPort, cli);
}

PCLIENT clientFind(unsigned int clientIp, unsigned int clientPort)
{
    PCLIENT cli;
    unsigned long clientIpPort = combineIpPort(clientIp, clientPort);
    HASH_FIND_LONG(hashClient, &clientIpPort, cli);	/* s: output pointer */
    return cli;
}

void clientDelete(unsigned int clientIp, unsigned int clientPort)
{
    PCLIENT cli = clientFind(clientIp, clientPort);
    if (cli) {
        HASH_DEL(hashClient, cli);	/* user: pointer to deletee */
        free(cli);
    }
}

void clientDeleteAll()
{
    PCLIENT current_client, tmp;
    
    HASH_ITER(hh, hashClient, current_client, tmp) {
        HASH_DEL(hashClient, current_client);	/* delete it (metrics advances to next) */
        free(current_client);	/* free it */
    }
}

void clientPrint()
{
    PCLIENT s;
    int i = 0;
    for (s = hashClient; s != NULL; s = s->hh.next) {
        printf("client num: %dn", i++);
    }
}

int clientSort(PCLIENT a, PCLIENT b)
{
    return (a->clientIpPort - b->clientIpPort);
}

void clientSortById()
{
    HASH_SORT(hashClient, clientSort);
}
