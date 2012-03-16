#include <stdio.h>
#include "clientHash.h"
#include "serverHash.h"
#include "sharedLibrary.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    printf("----Testing adding hash entry to both tables----\n");
    unsigned int a = RandomSourcePort();
    printf("serverPort: %d\n", a);
    clientAdd(1,1,2,a);
    serverAdd(a,1,1);
    PCLIENT b = clientFind(1,1);
    printf("clientFind should return serverPort %d: %d\n", a, b->serverPort);
    PSERVER c = serverFind(b->serverPort);
    printf("serverFind should return 1,1: %d,%d\n", c->clientIp, c->clientPort);
    printf("----Deleiting hash entries from both tables-----\n");
    serverDelete(a); 
    printf("findServer should return value 0 result: %d\n", serverFind(a));
    clientDelete(1,1);
    printf("findclient should return value 0 result: %d\n", clientFind(1,1));
 
    unsigned long x = inet_addr("192.168.0.1");
    unsigned int y = inet_addr("192.168.0.1");
    printf("unsigned int:  %lu should match unsigned long: %d\n", y, x);


    unsigned int p = htons(a);
    unsigned short q = htons(a);

    printf("network ports unsigned int: %d, unsigned short %d\n", p, q);

    return 0;

}
