#include "user_interface.h"
#include "osapi.h"
#include "http_request.h"
#include "espconn.h"
#include "mem.h"
#include "c_types.h"
#include "tcp_client.h"

struct espconn tcpclientConf;		//定义网络结构体

void ICACHE_FLASH_ATTR
tcp_client_init(ip_addr_t *server_ip, uint16 port)		//初始化函数
{
	tcpclientConf.type = ESPCONN_TCP;			//UDP/TCP
	tcpclientConf.state = ESPCONN_NONE;			//目前状态，none

	tcpclientConf.proto.tcp = (esp_tcp*)os_zalloc(sizeof(esp_tcp));		//分配空间

	tcpclientConf.proto.tcp->remote_port = port;		//远端ip端口号
	os_memcpy(tcpclientConf.proto.tcp->remote_ip, server_ip, 4);	//远端ip，内存拷贝

	tcpclientConf.proto.tcp->reconnect_callback = tcp_reconn_cb;	//设置重新连接回调
	tcpclientConf.proto.tcp->disconnect_callback = tcp_disconn_cb;	//断开连接回调
	tcpclientConf.proto.tcp->connect_callback = tcp_conn_cb;		//连接回调

	tcpclientConf.recv_callback = tcp_recv_cb;		//接收回调

	int result = espconn_connect(&tcpclientConf);	//建立TCP连接
	if(result == 0){
		os_printf("tcp test success\r\n");
	}else {
		os_printf("tcp test fault\r\n");
	}
}


/* 回调函数  */
void ICACHE_FLASH_ATTR
tcp_recv_cb(void *arg, char *pdata, unsigned short len)
{
	os_printf("get data: %s\r\n", pdata);		//打印返回数据
}

void ICACHE_FLASH_ATTR
tcp_reconn_cb(void *arg, sint8 err)
{
	os_printf("reconnect\r\n");
	espconn_connect(&tcpclientConf);
}

void ICACHE_FLASH_ATTR
tcp_disconn_cb(void *arg)
{
	os_printf("disconnect\r\n");
}

void ICACHE_FLASH_ATTR
tcp_conn_cb(void *arg)		//连接后，发送GET请求
{
	os_printf("connect success\r\n");
	espconn_send(&tcpclientConf, GET, os_strlen(GET));
}

