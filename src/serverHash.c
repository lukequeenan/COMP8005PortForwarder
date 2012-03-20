#include "serverHash.h"
#include <stdio.h>

PSERVER hashServer = NULL;

/*
 -- FUNCTION: serverAdd
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void serverAdd(unsigned int serverPort, unsigned int clientIp, unsigned int clientPort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash add macro
 -- IP and Port should be normalized to network byte order
 */
void serverAdd(unsigned short serverPort, unsigned int clientIp, unsigned short clientPort)
{
    PSERVER srv = malloc(sizeof(SERVER));
    srv->serverPort = serverPort;
    srv->clientIp = clientIp;
    srv->clientPort = clientPort;
    HASH_ADD_INT(hashServer, serverPort, srv);
}

/*
 -- FUNCTION: serverFind
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: PSERVER serverFind(int serverPort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash find macro
 -- IP and Port should be normalized to network byte order
 */
PSERVER serverFind(unsigned short serverPort)
{
    PSERVER server;
    HASH_FIND_INT(hashServer, &serverPort, server);	/* s: output pointer */
    return server;
}

/*
 -- FUNCTION: serverDelete
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void serverDelete(int serverPort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash delete macro
 */
void serverDelete(unsigned short serverPort)
{
    PSERVER srv = serverFind(serverPort);
    if (srv) {
        HASH_DEL(hashServer, srv);	/* user: pointer to deletee */
        free(srv);
    }
}

/*
 -- FUNCTION: serverDeleteAll
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void serverDeleteAll()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash delete macro and iterates through whole hashmap
 */
void serverDeleteAll()
{
    PSERVER current_server, tmp;
    
    HASH_ITER(hh, hashServer, current_server, tmp) {
        HASH_DEL(hashServer, current_server);	/* delete it (metrics advances to next) */
        free(current_server);	/* free it */
    }
}

/*
 -- FUNCTION: serverPrint
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void serverPrint()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- testing function
 */
void serverPrint()
{
    PSERVER s;
    int i = 0;
    for (s = hashServer; s != NULL; s = s->hh.next) {
        //print ipetId, addr in and addr out maybe more.
        //printf("user id %d: name %sn", s->id, s->name);
        printf("server num: %dn", i++);
    }
}

/*
 -- FUNCTION: serverSort
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int serverSort(PSERVER a, PSERVER b)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- sorting
 */
int serverSort(PSERVER a, PSERVER b)
{
    return (a->serverPort - b->serverPort);
}

/*
 -- FUNCTION: serverSortById
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: serverSortById()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- sorting
 */
void serverSortById()
{
    HASH_SORT(hashServer, serverSort);
}
