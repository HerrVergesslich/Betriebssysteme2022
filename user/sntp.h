#ifndef SNTP_H
#define SNTP_H

#include "kernel/types.h"

struct sntp {
    uint8 li_vn_mode;
    uint8 stratum;
    uint8 poll;
    uint8 precision;
    uint32 root_delay;
    uint32 root_dispersion;
    uint32 reference_id;
    uint64 reference_timestamp;
    uint64 originate_timestamp;
    uint64 receive_timestamp;
    uint64 transmit_timestamp;
};

#endif