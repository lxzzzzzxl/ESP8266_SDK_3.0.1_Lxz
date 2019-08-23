#include "user_interface.h"
#include "osapi.h"

#include "espconn.h"
#include "mem.h"
#include "c_types.h"
#include "udp.h"

struct espconn udpConf;		//��������ṹ��


/* UDP��ʼ������  */
void ICACHE_FLASH_ATTR
udp_init(uint16 port)
{
	uint8 remote_ip[4] = {255,255,255,255};

	udpConf.type = ESPCONN_UDP;		//UDP/TCP
	udpConf.state = ESPCONN_NONE;	//Ŀǰ״̬��none

	udpConf.proto.udp = (esp_udp*)os_zalloc(sizeof(esp_udp));	//����ռ�
	udpConf.proto.udp->remote_port = port;						//Զ�̶˿�
	udpConf.proto.udp->local_port = port;						//���ض˿�

	os_memcpy(udpConf.proto.udp->remote_ip, &remote_ip, 4);		//Զ��ip���ڴ濽��

	udpConf.recv_callback = udp_recv_cb;			//���ջص�

	espconn_create(&udpConf);				//����UDP����
	os_printf("udp test success\r\n");
}

/* �ص����� */
void ICACHE_FLASH_ATTR
udp_recv_cb(void *arg, char *pdata, unsigned short len)
{
	os_printf("get data: %s\r\n", pdata);		//��ӡ���յ�����
	if(espconn_sendto(&udpConf, "hello", sizeof("hello")) != 0)
		{
			os_printf("result: fault \r\n");
		} else{
			os_printf("result: OK \r\n");
		};

}

