#ifndef SHARED_LIBRARY_H
#define SHARED_LIBRARY_H

/* Defines */
struct list
{
    int externPort;
    char ip[16];
    int internPort;
    struct list* next;
};

typedef struct list rule;

/* Function Prototypes */
void systemFatal(const char* message);

#endif
