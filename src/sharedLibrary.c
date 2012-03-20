#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "sharedLibrary.h"
#include "clientHash.h"
#include "serverHash.h"
#include "ruleHash.h"

unsigned short randomSourcePort();

/*
 -- FUNCTION: systemFatal
 --
 -- DATE: March 12, 2011
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Aman Abdulla
 --
 -- PROGRAMMER: Luke Queenan
 --
 -- INTERFACE: static void systemFatal(const char* message);
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- This function displays an error message and shuts down the program.
 */
void systemFatal(const char* message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

/*
 -- FUNCTION: rlToStr
 --
 -- DATE: March 17, 2011
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: char* rlToStr()
 --
 -- RETURNS: 0 on failure or a string useable by pcap to filter by ports eg "port 22 or port 23"
 --
 -- NOTES:
 -- wrapper for to string method from ruleHash
 */
char* rlToStr() {
    return rulePrint();
}

/*
 -- FUNCTION: rlAdd
 --
 -- DATE: March 17, 2011
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int rlAdd(unsigned int clientDestPort, unsigned int serverDestPort, unsigned int serverDestIp)
 --
 -- RETURNS: 0 on failure 1 on success
 --
 -- NOTES:
 -- adds an entry to ruleHash useable for parsing client packets and giving the appropriate
 -- service port and service ip
 */
int rlAdd(unsigned short clientDestPort, unsigned short serverDestPort, unsigned int serverDestIp) {
    if (ruleFind(clientDestPort) != 0) {
        return 0;
    }
    ruleAdd(clientDestPort, serverDestPort, serverDestIp);
    return 1;
}

/*
 -- FUNCTION: rlAdd
 --
 -- DATE: March 17, 2011
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int rlFind(unsigned int clientDestPort, unsigned int *serverDestPort, unsigned int *serverDestIp)
 --
 -- RETURNS: 0 if no rule match 1 on match
 --
 -- NOTES:
 -- retrieves the server ip and port from the clientDestPort parsed from incoming packet
 */
int rlFind(unsigned short clientDestPort, unsigned short *serverDestPort, unsigned int *serverDestIp) {
    PRULE pRule = ruleFind(clientDestPort);
    if (pRule == 0) {
        return 0;
    }
    *serverDestPort = pRule->serverDestPort;
    *serverDestIp = pRule->serverDestIp;
    return 1;
}

/*
 -- FUNCTION: srvFind
 --
 -- DATE: March 17, 2011
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int srvFind(unsigned int serverPort, unsigned int *clientIp, unsigned int *clientPort)
 --
 -- RETURNS: 0 on failure 1 on success
 --
 -- NOTES:
 -- This function retrieves the client ip and client port data from the hashmap
 */
unsigned short srvFind(unsigned short serverPort, unsigned int *clientIp, unsigned short *clientPort) {
    PSERVER srv = serverFind(serverPort);
    if (srv == 0) {
        return 0;
    }
    *clientIp = srv->clientIp;
    *clientPort = srv->clientPort;
    return 1;
}

/*
 -- FUNCTION: cliFind
 --
 -- DATE: March 17, 2011
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int cliFind(unsigned int clientIp, unsigned int clientPort, unsigned int *srvPort)
 --
 -- RETURNS: 0 on failure 1 on success
 --
 -- NOTES:
 -- This function retrieves the server source port
 */
int cliFind(unsigned int clientIp, unsigned short clientPort, unsigned short *srvPort) {
    PCLIENT cli = clientFind(clientIp, clientPort);
    if (cli == 0) {
        return 0;
    }
    *srvPort = cli->serverPort;
    return 1;
}

/*
 -- FUNCTION: addRuleToMaps
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: unsigned int addRuleToMaps(unsigned int clientIp, unsigned int clientPort, unsigned int serverIp)
 --
 -- RETURNS: the new port in network byte order between 20000 and 65000 (i like 0's)
 --
 -- NOTES:
 -- This function takes takes the client information ip and port.  The info must be in network byte order
 -- So, remember a straight copy from the ip and tcp headers from the new client connection
 -- and inet_addr on the configuration server ip.
 */
unsigned short addRuleToMaps(unsigned int clientIp, unsigned short clientPort) {
    unsigned short serverPort = randomSourcePort();
    clientAdd(clientIp, clientPort, serverPort);
    serverAdd(serverPort, clientIp, clientPort);
    return serverPort;
}

/*
 -- FUNCTION: deleteRuleFromMap
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void deleteRulefromMap(unsigned int clientIp, unsigned int clientPort, unsigned int serverPort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- This function takes either a pair of clientIp and clientPort fileds with a null server port
 -- or it takes a null clientIp and null clientPort.  With either of these choices it will find
 -- the rule pair in the hashmaps and delete them both.
 -- Thus this function is usuable from either a client disconnection or a server disconnection.
 */
void deleteRuleFromMap(unsigned int clientIp, unsigned short clientPort, unsigned short serverPort) {
    if (serverPort == 0) {
        PCLIENT cli = clientFind(clientIp, clientPort);
        serverDelete(cli->serverPort);
        clientDelete(clientIp,clientPort);
    } else {
        PSERVER srv = serverFind(serverPort);
        clientDelete(srv->clientIp, srv->clientPort);
        serverDelete(serverPort);
    }
}

/*
 -- FUNCTION: RandomSourcePort
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: unsigned int RandomSourcePort()
 --
 -- RETURNS: the new port in network byte order between 20000 and 65000 (i like 0's)
 --
 -- NOTES:
 -- This function generates a random number between 20000 and 65000 it then queries
 -- the serverHashmap file to ensure it isn't already in use.
 */
unsigned short randomSourcePort() {
    unsigned int iseed = (unsigned int)time(NULL);
    srand (iseed);
    unsigned short port;
    do {
        port = htons(rand() % 45000 + 20000);
    } while(serverFind(port) != 0);
    return port;
}
