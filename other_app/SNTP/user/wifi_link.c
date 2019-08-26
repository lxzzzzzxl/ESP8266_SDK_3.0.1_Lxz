#include "user_interface.h"			//wifi接口定义
#include "wifi_link.h"				//自定义函数声明头文件
#include "osapi.h"					//系统api
#include "sntp.h"
#include "ets_sys.h"
#include "my_sntp.h"


/*为主函数调用*/
void ICACHE_FLASH_ATTR
user_set_station_config(void)
{
	char ssid[32] = SSID;				//ssid
	char password[64] = PASSWORD;		//password

	struct station_config stationConf;	//wifi配置结构体，一般配置以下三项

	stationConf.bssid_set = 0;						//是否检查MAC地址，默认为0
	os_memcpy(&stationConf.ssid, ssid, 32);			//数组采用内存拷贝，不能直接赋值
	os_memcpy(&stationConf.password, password, 64);	//同上

	if(wifi_station_set_config_current(&stationConf) == true)	//配置函数
	{
		os_printf("connect OK\n");
	};

	wifi_set_event_handler_cb(wifi_event);				//注册wifi事件回调
}


/*回调函数*/
void ICACHE_FLASH_ATTR
wifi_event(System_Event_t *event)
{
	switch (event->event)
	{
		case EVENT_STAMODE_CONNECTED:
			os_printf("connect to ssid %s, channel %d\n",
					event->event_info.connected.ssid,
					event->event_info.connected.channel);
			break;
		case EVENT_STAMODE_DISCONNECTED:
			os_printf("disconnect	from	ssid	%s,	reason	%d\n",
					event->event_info.disconnected.ssid,
					event->event_info.disconnected.reason);
			break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			os_printf("mode:	%d	->	%d\n",
					event->event_info.auth_change.old_mode,
					event->event_info.auth_change.new_mode);
			break;
		case EVENT_STAMODE_GOT_IP:
			os_printf("ip:"	IPSTR	",mask:"	IPSTR	",gw:"	IPSTR,
						IP2STR(&event->event_info.got_ip.ip),
						IP2STR(&event->event_info.got_ip.mask),
						IP2STR(&event->event_info.got_ip.gw));
			enable_time();
			os_printf("\n");
			break;
		case EVENT_SOFTAPMODE_STACONNECTED:
			os_printf("station:	"	MACSTR	"join,	AID	=	%d\n",
						MAC2STR(event->event_info.sta_connected.mac),
						event->event_info.sta_connected.aid);
			break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			os_printf("station:	"	MACSTR	"leave,	AID	=	%d\n",
						MAC2STR(event->event_info.sta_disconnected.mac),
						event->event_info.sta_disconnected.aid);
			break;
		default:
			break;
		}
}
