#include "sntp.h"
#include "osapi.h"
#include "my_sntp.h"
#include "user_interface.h"
#include "ets_sys.h"

/* 使能sntp */
void ICACHE_FLASH_ATTR
enable_sntp(void)
{
	sntp_setservername(0,"ntp1.aliyun.com");	//设置sntp服务器，0为主，最多支持3个
	sntp_setservername(1,"s1b.time.edu.cn");
	sntp_init();			//初始化
}

/* 使能软件定时器  */
void ICACHE_FLASH_ATTR
enable_time(void)
{
	os_timer_t sntp_timer;	//定时器结构
	os_timer_disarm(&sntp_timer);	//取消定时器定时
	os_timer_setfn(&sntp_timer, (os_timer_func_t*)user_check_sntp_stamp, NULL);	//设置定时器回调函数。使用定时器，必须设置回调函数。

	os_timer_arm(&sntp_timer, 500, 1);	//使能毫秒级定时器，不重复
}


/* 回调函数 */
void ICACHE_FLASH_ATTR
user_check_sntp_stamp(void)
{
	uint32 current_stamp;
	current_stamp = sntp_get_current_timestamp(); //查询当前距离基准时间 (1970.01.01 00：00：00 GMT + 8) 的时间戳
	os_printf("sntp: %d,%s", current_stamp, sntp_get_real_time(current_stamp));	//输出实际时间
}
