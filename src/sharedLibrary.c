#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sharedLibrary.h"
#include "clientHash.h"
#include "serverHash.h"



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
unsigned int addRuleToMaps(unsigned int clientIp, unsigned int clientPort, unsigned int serverIp) {
    unsigned int serverPort = randomSourcePort();
    clientAdd(clientIp,clientPort, serverIp, serverPort);
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
void deleteRuleFromMap(unsigned int clientIp, unsigned int clientPort, unsigned int serverPort) {
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
unsigned int randomSourcePort() {
    unsigned int iseed = (unsigned int)time(NULL);
    srand (iseed);
    unsigned int port;
    do {
        port = htons(rand() % 45000 + 20000);
    } while(serverFind(port) != 0);
    return port;
}
