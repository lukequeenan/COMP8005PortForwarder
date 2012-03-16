#include "clientHash.h"
#include <stdio.h>
#include <string.h>

PCLIENT hashClient = NULL;

/*
 -- FUNCTION: combineIpPort
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: unsigned long combineIpPort(unsigned int ip, unsigned int port)
 --
 -- RETURNS: the combined port and ip address into one variable for use in the hashing function
 --
 -- NOTES:
 -- Bitshifts the ip into the first 32 bits of the long and ors the
 -- port into the 2nd 32bits of the long giving a unique value
 -- for each client connection.
 */
unsigned long combineIpPort(unsigned int ip, unsigned int port) {
    unsigned long x;
    unsigned long a = ip;
    unsigned long b = port;

    x = a << (sizeof(unsigned int) * 8);
    x = x | b;
    return x;
}

/*
 -- FUNCTION: clientAdd
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void clientAdd(unsigned int clientIp, unsigned int clientPort, unsigned int serverIp, unsigned int serverPort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash add macro
 -- IP and Port should be normalized to network byte order
 */
void clientAdd(unsigned int clientIp, unsigned int clientPort, unsigned int serverIp, unsigned int serverPort)
{
    PCLIENT cli = malloc(sizeof(CLIENT));
    //#define HASH_ADD(hh,head,fieldname,keylen_in,add)

    cli->clientIpPort = combineIpPort(clientIp, clientPort);
    cli->serverIp = serverIp;
    cli->serverPort = serverPort;
    HASH_ADD_LONG(hashClient, clientIpPort, cli);
}

/*
 -- FUNCTION: clientFind
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: PCLIENT clientFind(unsigned int clientIp, unsigned int clientPort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash find macro
 -- IP and Port should be normalized to network byte order
 */
PCLIENT clientFind(unsigned int clientIp, unsigned int clientPort)
{
    PCLIENT cli;
    unsigned long clientIpPort = combineIpPort(clientIp, clientPort);
    HASH_FIND_LONG(hashClient, &clientIpPort, cli);	/* s: output pointer */
    return cli;
}

/*
 -- FUNCTION: clientDelete
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void clientDelete(unsigned int clientIp, unsigned int clientPort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash delete macro
 -- IP and Port should be normalized to network byte order
 */
void clientDelete(unsigned int clientIp, unsigned int clientPort)
{
    PCLIENT cli = clientFind(clientIp, clientPort);
    if (cli) {
        HASH_DEL(hashClient, cli);	/* user: pointer to deletee */
        free(cli);
    }
}

/*
 -- FUNCTION: clientDeleteAll
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void clientDeleteAll()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash delete macro and it iterates through whole hashmap.
 */
void clientDeleteAll()
{
    PCLIENT current_client, tmp;
    
    HASH_ITER(hh, hashClient, current_client, tmp) {
        HASH_DEL(hashClient, current_client);	/* delete it (metrics advances to next) */
        free(current_client);	/* free it */
    }
}

/*
 -- FUNCTION: clientPrint
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void clientPrint()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- used in testing no practical application
 */
void clientPrint()
{
    PCLIENT s;
    int i = 0;
    for (s = hashClient; s != NULL; s = s->hh.next) {
        printf("client num: %dn", i++);
    }
}

/*
 -- FUNCTION: clientSort
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int clientSort(PCLIENT a, PCLIENT b)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- sorting why i don't know leftovers from a long time ago i think
 */
int clientSort(PCLIENT a, PCLIENT b)
{
    return (a->clientIpPort - b->clientIpPort);
}

/*
 -- FUNCTION: clientSortById
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void clientSortById()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- sorting why i don't know leftovers from a long time ago i think
 */
void clientSortById()
{
    HASH_SORT(hashClient, clientSort);
}
