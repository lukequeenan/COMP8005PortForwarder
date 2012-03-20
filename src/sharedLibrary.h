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
    int externFilter;
} info;

/* Function Prototypes */
void systemFatal(const char* message);
unsigned short addRuleToMaps(unsigned int clientIp, unsigned short clientPort);
void deleteRuleFromMap(unsigned int clientIp, unsigned short clientPort, unsigned short serverPort);
int cliFind(unsigned int clientIp, unsigned short clientPort, unsigned short *srvPort);
unsigned short srvFind(unsigned short serverPort, unsigned int *clientIp, unsigned short *clientPort);
int rlFind(unsigned short clientDestPort, unsigned short *serverDestPort, unsigned int *serverDestIp);
int rlAdd(unsigned short clientDestPort, unsigned short serverDestPort, unsigned int serverDestIp);
char* rlToStr();


#endif
