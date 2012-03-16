#include "serverHash.h"
#include <stdio.h>

PSERVER hashServer = NULL;

void serverAdd(unsigned int serverPort, unsigned int clientIp, unsigned int clientPort)
{
    PSERVER srv = malloc(sizeof(SERVER));
    srv->serverID = serverPort;
    srv->clientIp = clientIp;
    srv->clientPort = clientPort;
    HASH_ADD_INT(hashServer, serverID, srv);
}

PSERVER serverFind(int serverPort)
{
    PSERVER server;
    HASH_FIND_INT(hashServer, &serverPort, server);	/* s: output pointer */
    return server;
}

void serverDelete(int serverPort)
{
    PSERVER srv = serverFind(serverPort);
    if (srv) {
        HASH_DEL(hashServer, srv);	/* user: pointer to deletee */
        free(srv);
    }
}

void serverDeleteAll()
{
    PSERVER current_server, tmp;
    
    HASH_ITER(hh, hashServer, current_server, tmp) {
        HASH_DEL(hashServer, current_server);	/* delete it (metrics advances to next) */
        free(current_server);	/* free it */
    }
}

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

int serverSort(PSERVER a, PSERVER b)
{
    return (a->serverID - b->serverID);
}

void serverSortById()
{
    HASH_SORT(hashServer, serverSort);
}
