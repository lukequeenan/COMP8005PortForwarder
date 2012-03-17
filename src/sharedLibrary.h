#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

/* Defines */


/* Function Prototypes */
void systemFatal(const char* message);
unsigned int randomSourcePort();
unsigned int addRuleToMaps(unsigned int clientIp, unsigned int clientPort);
void deleteRuleFromMap(unsigned int clientIp, unsigned int clientPort, unsigned int serverPort);
int cliFind(unsigned int clientIp, unsigned int clientPort, unsigned int *srvPort);
int srvFind(unsigned int serverPort, unsigned int *clientIp, unsigned int *clientPort);

#endif
