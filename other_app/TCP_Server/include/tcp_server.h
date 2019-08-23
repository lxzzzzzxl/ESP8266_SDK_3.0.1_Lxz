#ifndef __TCP_H__
#define __TCP_H__
#include "c_types.h"
#include "ip_addr.h"


void tcp_server_init(uint16 port);
void tcp_recv_cb(void *arg, char *pdata, unsigned short len);
void tcp_reconn_cb(void *arg, sint8 err);
void tcp_disconn_cb(void *arg);
void tcp_conn_cb(void *arg);


#endif
