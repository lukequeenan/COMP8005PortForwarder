#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

/* Defines */


/* Function Prototypes */
void systemFatal(const char* message);
unsigned int randomSourcePort();
unsigned int addRuleToMaps(unsigned int clientIp, unsigned int clientPort, unsigned int serverIp);
void deleteRuleFromMap(unsigned int clientIp, unsigned int clientPort, unsigned int serverPort);

#endif
