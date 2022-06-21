#include "kernel/types.h"
#include "kernel/net.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "sntp.h"

#define GT +2

// see https://datatracker.ietf.org/doc/html/rfc1361
char *month[] = {"Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dez"};
int daysOfMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void
printDate(uint32 sec)
{
    uint32 min = sec / 60;
    uint32 h   = min / 60;
    uint32 d   = h   / 24;
    uint32 y   = 1900;
    int i = 0;

    uint32 comp_val = ((y+1) % 4 != 0) ? 365 : 366;
    while (d >= comp_val) {
        d -= comp_val;
        y++;
        comp_val = ((y+1) % 4 != 0) ? 365 : 366;
    }
    d++; // d=0 -> 1 Jan
    while (d > daysOfMonth[i] || (i == 1 && y % 4 == 0 && d > daysOfMonth[i])) {
        d -= daysOfMonth[i];
        if (i == 1 && y % 4 == 0)
            d--;
        i++;
    }

    printf("Date: %l %s. %l, %l:%l:%l\n", d, month[i], y, (h%24) + GT, min%60, sec%60);
}


void
sntp(void) 
{
    
    int fd;
    uint32 dst = (192 << 24) | (53 << 16) | (103 << 8) | (108 << 0);

    struct sntp* packet = malloc(sizeof(struct sntp));

    packet->li_vn_mode = 0b11100011; //leap indicator, version, mode
    packet->stratum = 0;
    packet->poll = 0;
    packet->precision = 0;
    packet->root_delay = 0;
    packet->root_dispersion = 0;
    packet->reference_id = 0;
    packet->reference_timestamp = 0;
    packet->originate_timestamp = 0;
    packet->receive_timestamp = 0;
    packet->transmit_timestamp = 0;

    if((fd = connect(dst, 2001, 123)) < 0) {
        fprintf(2, "sntp: connect() failed\n");
        exit(1);
    }

    if(write(fd, packet, sizeof(struct sntp)) < 0) {
        fprintf(2, "sntp: write() failed\n");
        exit(1);
    }

    if(read(fd, packet, sizeof(struct sntp)) < 0) {
        fprintf(2, "sntp: read() failed\n");
        exit(1);
    }

    printDate(ntohl(packet->transmit_timestamp));

    close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 1){
    fprintf(2, "Usage: sntp");
    exit(1);
  }
  sntp();
  exit(0);
}