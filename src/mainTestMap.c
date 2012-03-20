#include <stdio.h>
#include "clientHash.h"
#include "serverHash.h"
#include "ruleHash.h"
#include "sharedLibrary.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main() {
    printf("----Testing adding hash entry to both tables----\n");
    unsigned int a = randomSourcePort();
    printf("serverPort: %d\n", a);
    clientAdd(1,1,a);
    serverAdd(a,1,1);
    PCLIENT b = clientFind(1,1);
    printf("clientFind should return serverPort %d: %d\n", a, b->serverPort);
    PSERVER c = serverFind(b->serverPort);
    printf("serverFind should return 1,1: %d,%d\n", c->clientIp, c->clientPort);
    printf("----Deleting hash entries from both tables-----\n");
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
    unsigned int z = inet_addr("192.168.0.2");
    unsigned int r = htons(22);
    unsigned int s = addRuleToMaps(y,r);
    printf("server port: %d\n", s);
    b = clientFind(y,r);
    printf("clientFind should return serverPort %d: %d\n", s, b->serverPort);
    c = serverFind(s);
    /*printf("serverFind should return %d,%d: %d,%d\n", y, r, c->clientIp, c->clientPort);
    printf("----Deleting hash entries from both tables using deleteRuleFromMaps and just serverPort-----\n");
    deleteRuleFromMap(0,0,s);
    printf("findServer should return value 0 result: %d\n", serverFind(s));
    printf("findclient should return value 0 result: %d\n", clientFind(y, r));*/
    unsigned int clientIp;
    unsigned short clientPort;
    if (srvFind(s, &clientIp, &clientPort)) {
        printf("Client IP: %d, client Port %d\n", clientIp, clientPort);
    } else {
        printf("Server Rule Not found\n");

    }
    unsigned short serverPort;
    if (cliFind(y, r, &serverPort)) {
        printf("server port: %d\n", serverPort);
    } else {
        printf("client Rule not Found\n");
    }

    unsigned int serverDestIp = z;
    unsigned int serverDestPort = htons(9999);
    unsigned int clientDestPort = htons(22222);
    rlAdd(clientDestPort, serverDestPort, serverDestIp);
    unsigned int ddd;
    unsigned short eee;
    rlFind(clientDestPort, &eee, &ddd);
    printf("rlFind should return value %d: %d\n",serverDestPort, eee);

    printf("%s\n", rlToStr());
    rlAdd(htons(22223), serverDestPort, serverDestIp);
    rlAdd(htons(22224), serverDestPort, serverDestIp);
    rlAdd(htons(22225), serverDestPort, serverDestIp);
    printf("%s\n", rlToStr());





    return 0;

}
