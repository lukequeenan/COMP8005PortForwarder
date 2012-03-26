#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "serverHash.h"
#include "ruleHash.h"
#include "clientHash.h"

#include "sharedLibrary.h"

int main() {

    printf("----Testing adding hash entry to both tables----\n");
    unsigned int a = randomSourcePort();

    clientAdd(1,1,a);
    serverAdd(a,1,1,1);
    PCLIENT b = clientFind(1,1);
    printf("clientFind should return serverPort %d: \t\t%d\n", a, b->serverPort);
    PSERVER c = serverFind(b->serverPort);
    printf("serverFind should return 1,1: \t\t\t\t%d,%d\n", c->clientIp, c->clientPort);
    printf("\n----Deleting hash entries from both tables-----\n");
    serverDelete(a); 
    printf("findServer should return value 0 result: \t\t%d\n", serverFind(a));
    clientDelete(1,1);
    printf("findclient should return value 0 result: \t\t%d\n", clientFind(1,1));

    printf("\n----Testing RuleHash Map-----\n");
    unsigned long x = inet_addr("192.168.0.1");
    unsigned int y = inet_addr("192.168.0.1");
    printf("unsigned int:  %lu should match unsigned long: \t%d\n", y, x);
    unsigned int p = htons(a);
    unsigned short q = htons(a);
    printf("network ports unsigned int: %d, unsigned short \t%d\n", p, q);
    unsigned int z = inet_addr("192.168.0.2");
    unsigned int r = htons(22);
    unsigned int s = addRuleToMaps(y,r,2);
    b = clientFind(y,r);
    printf("clientFind should return serverPort %d: \t\t%d\n", s, b->serverPort);
    c = serverFind(s);
    unsigned int clientIp;
    unsigned short clientPort;
    unsigned short clientSourcePort;
    printf("\n----Testing external functions in sharedLibrary.c-----\n");
    if (srvFind(s, &clientIp, &clientPort, &clientSourcePort)) {
        printf("Client IP: \t%d, \tClient Port: %d\n", clientIp, clientPort);
    } else {
        printf("Server Rule Not found\n");

    }
    unsigned short serverPort;
    if (cliFind(y, r, &serverPort)) {
        printf("server port: \t%d\n", serverPort);
    } else {
        printf("client Rule not Found\n");
    }

    unsigned int serverDestIp = z;
    unsigned int serverDestPort = htons(9999);
    unsigned int clientDestPort = htons(22222);
    char *str1 = malloc(16);
    snprintf(str1, 16, "192.168.0.194");
    char *str2 = malloc(16);
    snprintf(str2, 16, "192.168.0.196");
    rlAdd(clientDestPort, serverDestPort, serverDestIp,str1,str2);
    unsigned int ddd;
    unsigned short eee;
    rlFind(clientDestPort, &eee, &ddd);
    printf("rlFind should return value %d: \t\t\t%d\n",serverDestPort, eee);


    str1 = malloc(16);
    snprintf(str1, 16, "192.168.0.195");
    str2 = malloc(16);
    snprintf(str2, 16, "192.168.0.196");
    rlAdd(htons(22223), serverDestPort, serverDestIp,str1,str2);
    str1 = malloc(16);
    snprintf(str1, 16, "192.168.0.197");
    str2 = malloc(16);
    snprintf(str2, 16, "192.168.0.196");
    rlAdd(htons(22224), serverDestPort, serverDestIp,str1,str2);
    str1 = malloc(16);
    snprintf(str1, 16, "192.168.0.198");
    str2 = malloc(16);
    snprintf(str2, 16, "192.168.0.196");
    rlAdd(htons(22225), serverDestPort, serverDestIp,str1,str2);
    printf("\n----Testing filter generation in toString functions for ruleHash-----\n");
    printf("%s\n", rlExternToStr());
    printf("%s\n", rlInternToStr());
    return 0;

}
