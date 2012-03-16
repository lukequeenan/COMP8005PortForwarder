#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <pcap.h>

#include "sharedLibrary.h"
#include "forward.h"

#define SNAP_LEN 1518

void *monitorSocket(void *data);

#endif
