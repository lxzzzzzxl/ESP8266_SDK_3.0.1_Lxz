#include "user_interface.h"
#include "osapi.h"

#include "espconn.h"
#include "mem.h"
#include "c_types.h"
#include "udp.h"

struct espconn udpConf;		//定义网络结构体


/* UDP初始化函数  */
void ICACHE_FLASH_ATTR
udp_init(uint16 port)
{
	uint8 remote_ip[4] = {255,255,255,255};

	udpConf.type = ESPCONN_UDP;		//UDP/TCP
	udpConf.state = ESPCONN_NONE;	//目前状态，none

	udpConf.proto.udp = (esp_udp*)os_zalloc(sizeof(esp_udp));	//分配空间
	udpConf.proto.udp->remote_port = port;						//远程端口
	udpConf.proto.udp->local_port = port;						//本地端口

	os_memcpy(udpConf.proto.udp->remote_ip, &remote_ip, 4);		//远程ip，内存拷贝

	udpConf.recv_callback = udp_recv_cb;			//接收回调

	espconn_create(&udpConf);				//创建UDP连接
	os_printf("udp test success\r\n");
}

/* 回调函数 */
void ICACHE_FLASH_ATTR
udp_recv_cb(void *arg, char *pdata, unsigned short len)
{
	os_printf("get data: %s\r\n", pdata);		//打印接收的内容
	if(espconn_sendto(&udpConf, "hello", sizeof("hello")) != 0)
		{
			os_printf("result: fault \r\n");
		} else{
			os_printf("result: OK \r\n");
		};

}

