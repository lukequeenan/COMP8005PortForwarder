#include "ruleHash.h"
#include <stdio.h>

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
 -- INTERFACE: void ruleAdd(unsigned int rulePort, unsigned int clientIp, unsigned int clientPort)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for uthash add macro
 -- IP and Port should be normalized to network byte order
 */
void ruleAdd(unsigned short clientDestPort, unsigned short serverDestPort, unsigned int serverDestIp)
{
    PRULE pRule = malloc(sizeof(RULE));
    pRule->clientDestPort = clientDestPort;
    pRule->serverDestPort = serverDestPort;
    pRule->serverDestIp = serverDestIp;
    HASH_ADD_INT(hashRule, clientDestPort, pRule);

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
    HASH_FIND_INT(hashRule, &clientDestPort, rule);	/* s: output pointer */
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
 -- testing function
 */
char* rulePrint()
{
    PRULE s;
    char *str = malloc(1024);
    s = hashRule;
    if (hashRule == 0) {
        free(str);
        return 0;
    }

    snprintf(str, 11, "dst port %d", ntohs(s->clientDestPort));
    for (s = s->hh.next; s != NULL; s = s->hh.next) {
        snprintf(&str[strlen(str)], 15, " or port %d", ntohs(s->clientDestPort));
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
