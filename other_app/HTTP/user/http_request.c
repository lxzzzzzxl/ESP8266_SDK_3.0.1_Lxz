#include "tcp_client.h"
#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "mem.h"
#include "c_types.h"
#include "http_request.h"

extern struct espconn tcpclientConf;

void ICACHE_FLASH_ATTR
http_visit_web(const char *url)		//通过域名获取服务器ip地址
{
	ip_addr_t server_ip;
	espconn_gethostbyname(&tcpclientConf, url, &server_ip, find_dns);
}

void ICACHE_FLASH_ATTR
find_dns(const char *name, ip_addr_t *ipaddr, void *callback_arg)
{
	if (ipaddr != NULL){		//打印服务器ip地址
		os_printf("user_esp_platform_dns_found %d.%d.%d.%d/n", *((uint8	*)&ipaddr->addr), *((uint8	*)&ipaddr->addr	+	1), *((uint8	*)&ipaddr->addr	+	2),	*((uint8	*)&ipaddr->addr	+	3));
	}

	tcp_client_init(ipaddr, 80);	//连接服务器
}
