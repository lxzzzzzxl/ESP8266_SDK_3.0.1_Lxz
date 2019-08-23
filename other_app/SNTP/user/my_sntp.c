#include "sntp.h"
#include "osapi.h"
#include "my_sntp.h"
#include "user_interface.h"
#include "ets_sys.h"

/* ʹ��sntp */
void ICACHE_FLASH_ATTR
enable_sntp(void)
{
	sntp_setservername(0,"ntp1.aliyun.com");	//����sntp��������0Ϊ�������֧��3��
	sntp_setservername(1,"s1b.time.edu.cn");
	sntp_init();			//��ʼ��
}

/* ʹ�������ʱ��  */
void ICACHE_FLASH_ATTR
enable_time(void)
{
	os_timer_t sntp_timer;	//��ʱ���ṹ
	os_timer_disarm(&sntp_timer);	//ȡ����ʱ����ʱ
	os_timer_setfn(&sntp_timer, (os_timer_func_t*)user_check_sntp_stamp, NULL);	//���ö�ʱ���ص�������ʹ�ö�ʱ�����������ûص�������

	os_timer_arm(&sntp_timer, 500, 1);	//ʹ�ܺ��뼶��ʱ�������ظ�
}


/* �ص����� */
void ICACHE_FLASH_ATTR
user_check_sntp_stamp(void)
{
	uint32 current_stamp;
	current_stamp = sntp_get_current_timestamp(); //��ѯ��ǰ�����׼ʱ�� (1970.01.01 00��00��00 GMT + 8) ��ʱ���
	os_printf("sntp: %d,%s", current_stamp, sntp_get_real_time(current_stamp));	//���ʵ��ʱ��
}
