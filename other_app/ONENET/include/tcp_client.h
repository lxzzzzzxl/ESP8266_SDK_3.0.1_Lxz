#ifndef __TCP_H__
#define __TCP_H__
#include "c_types.h"
#include "ip_addr.h"


#define POST "POST /devices/539213828/datapoints?type=3 HTTP/1.1\n\
api-key:=8OHLD7NqQWiYxbH3yp6pibc9b4=\n\
host:api.heclouds.com\n\
content-length:18\n\n\
{\"t1\":66, \"t2\":88}"

void tcp_client_init(uint16 port);
void tcp_recv_cb(void *arg, char *pdata, unsigned short len);
void tcp_reconn_cb(void *arg, sint8 err);
void tcp_disconn_cb(void *arg);
void tcp_conn_cb(void *arg);


#endif
