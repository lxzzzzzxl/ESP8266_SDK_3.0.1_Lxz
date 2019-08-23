#ifndef __UDP_H__
#define __UDP_H__
#include "c_types.h"


void udp_init(uint16 port);
void udp_recv_cb(void *arg, char *pdata, unsigned short len);


#endif
