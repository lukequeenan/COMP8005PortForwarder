#include "ruleHash.h"
#include <stdio.h>
#include <arpa/inet.h>

PRULE hashRule = NULL;

/*
 -- FUNCTION: ruleAdd
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- void ruleAdd(unsigned short clientDestPort, unsigned short serverDestPort, unsigned int serverDestIp, char* serverIp, char* forwarderIp)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash add macro
 -- Stores all the information from the settings.conf file in information that can be used for forwarding data.
 -- The two pointers serverIp and forwarderIp are used for creating of the filter rules that are applied to libpcap
 -- IP and Port should be normalized to network byte order
 */
void ruleAdd(unsigned short clientDestPort, unsigned short serverDestPort, unsigned int serverDestIp, char* serverIp, char* forwarderIp)
{
    PRULE pRule = malloc(sizeof(RULE));
    pRule->clientDestPort = clientDestPort;
    pRule->serverDestPort = serverDestPort;
    pRule->serverDestIp = serverDestIp;
    pRule->serverIp = serverIp;
    pRule->forwarderIp = forwarderIp;
    HASH_ADD_SHORT(hashRule, clientDestPort, pRule);

}

/*
 -- FUNCTION: ruleFind
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: PRULE ruleFind(int rulePort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash find macro
 -- IP and Port should be normalized to network byte order
 */
PRULE ruleFind(short clientDestPort)
{
    PRULE rule;
    HASH_FIND_SHORT(hashRule, &clientDestPort, rule);	/* s: output pointer */
    return rule;
}

/*
 -- FUNCTION: ruleDelete
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void ruleDelete(int rulePort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash delete macro
 */
void ruleDelete(short clientDestPort)
{
    PRULE pRule = ruleFind(clientDestPort);
    if (pRule) {
        HASH_DEL(hashRule, pRule);	/* user: pointer to deletee */
        free(pRule);
    }
}

/*
 -- FUNCTION: ruleDeleteAll
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void ruleDeleteAll()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash delete macro and iterates through whole hashmap
 */
void ruleDeleteAll()
{
    PRULE current_rule, tmp;
    
    HASH_ITER(hh, hashRule, current_rule, tmp) {
        HASH_DEL(hashRule, current_rule);	/* delete it (metrics advances to next) */
        free(current_rule);	/* free it */
    }
}

/*
 -- FUNCTION: rulePrint
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void rulePrint()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- Builds the string that is passed to libpcap for reading packets from client computer.
 -- This is used to create a filter that will only accept packets that are destined to be filter by the
 -- forwarder as set by the setting.conf
 */
char* rulePrint()
{
    PRULE s;
    char * str = malloc(MAXBUFSIZE);
    s = hashRule;

    if (hashRule == 0) {
        free(str);
        return 0;
    }
    snprintf(str, 51, "(dst host %s and dst port %d)", s->forwarderIp, ntohs(s->clientDestPort));
    for (s = s->hh.next; s != NULL; s = s->hh.next) {
        snprintf(&str[strlen(str)], 55, " or (dst host %s and dst port %d)", s->forwarderIp, ntohs(s->clientDestPort));
    }
    return str;
}

/*
 -- FUNCTION: internRulePrint
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void rulePrint()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- Builds the string that is passed to libpcap for reading packets from server computer.
 -- This is used to create a filter that will only accept packets that are destined to be forwarderd
 -- as determined from the settings.conf file
 */
char* internRulePrint() {
    PRULE s;
    char * str = malloc(MAXBUFSIZE);
    s = hashRule;

    if (hashRule == 0) {
        free(str);
        return 0;
    }
    snprintf(str, 25, "src host %s", s->serverIp);
    for (s = s->hh.next; s != NULL; s = s->hh.next) {
        snprintf(&str[strlen(str)], 29, " or src host %s", s->serverIp);
    }
    return str;
}

/*
 -- FUNCTION: ruleSort
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int ruleSort(PRULE a, PRULE b)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- sorting
 */
int ruleSort(PRULE a, PRULE b)
{
    return (a->clientDestPort - b->clientDestPort);
}

/*
 -- FUNCTION: ruleSortById
 --
 -- DATE: March 16, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: ruleSortById()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- sorting
 */
void ruleSortById()
{
    HASH_SORT(hashRule, ruleSort);
}


