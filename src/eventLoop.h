#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <libnet.h>
#include <pcap.h>
#include <pthread.h>
#include <unistd.h>

#include "sharedLibrary.h"
#include "forward.h"

#define SNAP_LEN 1518
#define FILTER_BUFFER 1024

void *pcapLoop(void *data);

#endif
