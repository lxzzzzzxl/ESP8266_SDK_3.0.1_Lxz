#ifndef __TCP_H__
#define __TCP_H__
#include "c_types.h"
#include "ip_addr.h"

#define GET "GET /user2.bin HTTP/1.1\r\nHost: 192.168.10.154\r\nConnection: keep-alive\r\n\r\n"

void tcp_client_init(uint16 port);
void tcp_recv_cb(void *arg, char *pdata, unsigned short len);
void tcp_reconn_cb(void *arg, sint8 err);
void tcp_disconn_cb(void *arg);
void tcp_conn_cb(void *arg);


#endif
