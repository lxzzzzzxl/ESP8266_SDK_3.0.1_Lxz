#include "user_interface.h"
#include "osapi.h"

#include "espconn.h"
#include "mem.h"
#include "c_types.h"
#include "tcp_server.h"

struct espconn tcpclientConf;		//��������ṹ��

void ICACHE_FLASH_ATTR
tcp_server_init(uint16 port)		//��ʼ������
{
	//uint8 remote_ip[4] = {};		//��Ϊ����ˣ�����Զ��ip���ȴ��ͻ������Ӽ���

	tcpclientConf.type = ESPCONN_TCP;			//UDP/TCP
	tcpclientConf.state = ESPCONN_NONE;			//Ŀǰ״̬��none

	tcpclientConf.proto.tcp = (esp_tcp*)os_zalloc(sizeof(esp_tcp));		//����ռ�

	tcpclientConf.proto.tcp->remote_port = port;		//Զ��ip
	tcpclientConf.proto.tcp->local_port = port;			//����ip

	tcpclientConf.proto.tcp->reconnect_callback = tcp_reconn_cb;	//�����������ӻص�
	tcpclientConf.proto.tcp->disconnect_callback = tcp_disconn_cb;	//�Ͽ����ӻص�
	tcpclientConf.proto.tcp->connect_callback = tcp_conn_cb;		//���ӻص�

	tcpclientConf.recv_callback = tcp_recv_cb;		//���ջص�

	espconn_accept(&tcpclientConf);	//����TCP����
}


/* �ص�����  */
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

