#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

/* Defines */

/* Typedefs */
/*
struct rule
{
    int listenPort;
    int sendPort;
    struct rule *next;
};
typedef struct rule rule;
*/
typedef struct
{
    char nic[6];
    char ip[16];
    int rawSocket;
} info;

/* Function Prototypes */
void systemFatal(const char* message);
unsigned int addRuleToMaps(unsigned int clientIp, unsigned int clientPort);
void deleteRuleFromMap(unsigned int clientIp, unsigned int clientPort, unsigned int serverPort);
int cliFind(unsigned int clientIp, unsigned int clientPort, unsigned int *srvPort);
int srvFind(unsigned int serverPort, unsigned int *clientIp, unsigned int *clientPort);
int rlFind(unsigned int clientDestPort, unsigned int *serverDestPort, unsigned int *serverDestIp);
int rlAdd(unsigned int clientDestPort, unsigned int serverDestPort, unsigned int serverDestIp);
char* rlToStr();


#endif
