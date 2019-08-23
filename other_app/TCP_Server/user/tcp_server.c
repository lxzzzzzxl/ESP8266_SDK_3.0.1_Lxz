#include "user_interface.h"
#include "osapi.h"

#include "espconn.h"
#include "mem.h"
#include "c_types.h"
#include "tcp_server.h"

struct espconn tcpclientConf;		//定义网络结构体

void ICACHE_FLASH_ATTR
tcp_server_init(uint16 port)		//初始化函数
{
	//uint8 remote_ip[4] = {};		//作为服务端，无需远端ip，等待客户端连接即可

	tcpclientConf.type = ESPCONN_TCP;			//UDP/TCP
	tcpclientConf.state = ESPCONN_NONE;			//目前状态，none

	tcpclientConf.proto.tcp = (esp_tcp*)os_zalloc(sizeof(esp_tcp));		//分配空间

	tcpclientConf.proto.tcp->remote_port = port;		//远端ip
	tcpclientConf.proto.tcp->local_port = port;			//本地ip

	tcpclientConf.proto.tcp->reconnect_callback = tcp_reconn_cb;	//设置重新连接回调
	tcpclientConf.proto.tcp->disconnect_callback = tcp_disconn_cb;	//断开连接回调
	tcpclientConf.proto.tcp->connect_callback = tcp_conn_cb;		//连接回调

	tcpclientConf.recv_callback = tcp_recv_cb;		//接收回调

	espconn_accept(&tcpclientConf);	//建立TCP监听
}


/* 回调函数  */
void ICACHE_FLASH_ATTR
tcp_recv_cb(void *arg, char *pdata, unsigned short len)
{
	os_printf("get data: %s\r\n", pdata);
	espconn_send(&tcpclientConf, "HELLO", os_strlen("HELLO"));
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

