#include "user_interface.h"		//wifi�ӿڶ���
#include "wifi_link.h"			//�Զ��庯������ͷ�ļ�
#include "osapi.h"				//ϵͳapi

/*Ϊ����������*/
void ICACHE_FLASH_ATTR
user_set_station_config(void)
{
	char ssid[32] = SSID;				//ssid
	char password[64] = PASSWORD;		//password

	struct station_config stationConf;	//wifi���ýṹ�壬һ��������������

	stationConf.bssid_set = 0;							//�Ƿ���MAC��ַ��Ĭ��Ϊ0
	os_memcpy(&stationConf.ssid, ssid, 32);				//��������ڴ濽��������ֱ�Ӹ�ֵ
	os_memcpy(&stationConf.password, password, 64);		//ͬ��

	if(wifi_station_set_config_current(&stationConf) == true)	//���ú���
	{
		os_printf("connect OK\n");
	};

	wifi_set_event_handler_cb(wifi_event);		//ע��wifi�¼��ص�
}


/*�ص�����*/
void ICACHE_FLASH_ATTR
wifi_event(System_Event_t *event)
{
	switch (event->event)
	{
		case EVENT_STAMODE_CONNECTED:		//���ӳɹ�
			os_printf("connect to ssid %s, channel %d\n",
					event->event_info.connected.ssid,
					event->event_info.connected.channel);
			break;
		case EVENT_STAMODE_DISCONNECTED:	//�Ͽ�����
			os_printf("disconnect	from	ssid	%s,	reason	%d\n",
					event->event_info.disconnected.ssid,
					event->event_info.disconnected.reason);
			break;
		case EVENT_STAMODE_GOT_IP:			//��ȡ��IP
			os_printf("ip:"	IPSTR	",mask:"	IPSTR	",gw:"	IPSTR,
						IP2STR(&event->event_info.got_ip.ip),
						IP2STR(&event->event_info.got_ip.mask),
						IP2STR(&event->event_info.got_ip.gw));
			os_printf("\n");
			break;
		default:
			break;
		}
}
