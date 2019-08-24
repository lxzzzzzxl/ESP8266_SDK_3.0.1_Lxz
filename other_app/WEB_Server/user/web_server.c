#include "web_server.h"
#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "mem.h"
#include "c_types.h"
#include "ets_sys.h"

struct espconn tcpclientConf;

void ICACHE_FLASH_ATTR
web_server_init(uint16 port)
{
	tcpclientConf.type = ESPCONN_TCP;
	tcpclientConf.state = ESPCONN_NONE;
	tcpclientConf.proto.tcp = (esp_tcp*)os_zalloc(sizeof(esp_tcp));

	tcpclientConf.proto.tcp->local_port = port;

	tcpclientConf.proto.tcp->reconnect_callback = tcp_reconn_cb;
	tcpclientConf.proto.tcp->disconnect_callback = tcp_disconn_cb;
	tcpclientConf.proto.tcp->connect_callback = tcp_conn_cb;

	tcpclientConf.recv_callback = tcp_recv_cb;

	espconn_accept(&tcpclientConf);

}

void ICACHE_FLASH_ATTR
tcp_recv_cb(void *arg, char *pdata, unsigned short len)
{
	os_printf("get data: %s\r\n", pdata);
	uint8 http_data[1024] =
			"HTTP/1.1 200 OK\n"
			"Date: Web, 21 Aug 2019 14:43:30 GMT\n"
			"Content-Type: text/html;charset=ISO-8859-1\n"
			"Content-Length: 122\n\n"
			"<html>"
			"<head><title>LXZ-8266</title></head>"
			"<body>welcome to lxz's 8266 server!</body>"
			"</html>";
	espconn_send(&tcpclientConf, http_data, os_strlen(http_data));
}

void ICACHE_FLASH_ATTR
tcp_reconn_cb(void *arg, sint8 err)
{
	os_printf("reconnect\r\n");
}

void ICACHE_FLASH_ATTR
tcp_disconn_cb(void *arg)
{
	os_printf("disconnect\r\n");
}

void ICACHE_FLASH_ATTR
tcp_conn_cb(void *arg)
{
	os_printf("connect success\r\n");
}
