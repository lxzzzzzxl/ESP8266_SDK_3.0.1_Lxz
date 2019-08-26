#include "ets_sys.h"
#include "user_interface.h"
#include "sntp.h"
#include "osapi.h"
#include "my_sntp.h"
#include "mem.h"

os_timer_t sntp_timer;

/* 手册上说最多可设置3个sntp服务器
 * 但是实践发现没设置够3个会连接不上wifi
 * 所以要设置满3个sntp服务器 */
void ICACHE_FLASH_ATTR
enable_sntp(void)
{
	//ip_addr_t *addr = (ip_addr_t *)os_zalloc(sizeof(ip_addr_t));
	sntp_setservername(2,"ntp1.aliyun.com");
	sntp_setservername(1,"s1b.time.edu.cn");
	sntp_setservername(0,"0.cn.pool.ntp.org");
	//ipaddr_aton("210.72.145.44", addr);
	//sntp_setserver(2, addr);	//set server 2 by IP address
	sntp_init();
	//os_free(addr);
}

void ICACHE_FLASH_ATTR
enable_time(void)
{
	os_timer_disarm(&sntp_timer);
	os_timer_setfn(&sntp_timer, (os_timer_func_t*)user_check_sntp_stamp, NULL);
	os_timer_arm(&sntp_timer, 5000, 1);
}

void ICACHE_FLASH_ATTR
user_check_sntp_stamp(void)
{
	uint32 current_stamp;
	current_stamp = sntp_get_current_timestamp();
	os_printf("sntp: %d,%s", current_stamp, sntp_get_real_time(current_stamp));
}
