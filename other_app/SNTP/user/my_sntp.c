#include "sntp.h"
#include "osapi.h"
#include "my_sntp.h"

void ICACHE_FLASH_ATTR
enable_sntp(void)
{
	sntp_setservername(0,"ntp1.aliyun.com");
	sntp_setservername(1,"s1b.time.edu.cn");
	sntp_init();
}

void ICACHE_FLASH_ATTR
enable_time(void)
{
	os_timer_t sntp_timer;
	os_timer_disarm(&sntp_timer);
	os_timer_setfn(&sntp_timer, (os_timer_func_t*)user_check_sntp_stamp, NULL);
	os_timer_arm(&sntp_timer, 500, 1);
}

void ICACHE_FLASH_ATTR
user_check_sntp_stamp(void)
{
	uint32 current_stamp;
	current_stamp = sntp_get_current_timestamp();
	os_printf("sntp: %d,%s", current_stamp, sntp_get_real_time(current_stamp));
}
