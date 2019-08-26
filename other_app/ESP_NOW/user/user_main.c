#include "osapi.h"
#include "user_interface.h"

#include "espnow.h"

#define ESP_NOW_CONTROLLER		//���Ʒ�����궨��
//#define ESP_NOW_SLAVER		//�����Ʒ�����궨��

/* �������� */
void espnow_recv_cb(u8 *mac_addr, u8 *data, u8 len);
void timer_intterupt(void *timer_arg);

os_timer_t timer; //��ʱ���ṹ

u8 key[16]= {0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44};	//ͨ����Կ
u8 mac1[6] = {0x2C, 0xF4, 0x32, 0x71, 0x59, 0xD0};		//Station Controller �����ַ
u8 mac2[6] = {0xA6, 0xCF, 0x12, 0xB9, 0x2C, 0x80};		//AP Slaver �����ַ

#if ((SPI_FLASH_SIZE_MAP == 0) || (SPI_FLASH_SIZE_MAP == 1))
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 2)
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 3)
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 4)
#error "The flash map is not supported"
#elif (SPI_FLASH_SIZE_MAP == 5)
#define SYSTEM_PARTITION_OTA_SIZE							0xE0000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x1fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x1fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x1fd000
#define SYSTEM_PARTITION_AT_PARAMETER_ADDR					0xfd000
#define SYSTEM_PARTITION_SSL_CLIENT_CERT_PRIVKEY_ADDR		0xfc000
#define SYSTEM_PARTITION_SSL_CLIENT_CA_ADDR					0xfb000
#define SYSTEM_PARTITION_WPA2_ENTERPRISE_CERT_PRIVKEY_ADDR	0xfa000
#define SYSTEM_PARTITION_WPA2_ENTERPRISE_CA_ADDR			0xf9000
#elif (SPI_FLASH_SIZE_MAP == 6)
#define SYSTEM_PARTITION_OTA_SIZE							0xE0000
#define SYSTEM_PARTITION_OTA_2_ADDR							0x101000
#define SYSTEM_PARTITION_RF_CAL_ADDR						0x3fb000
#define SYSTEM_PARTITION_PHY_DATA_ADDR						0x3fc000
#define SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR				0x3fd000
#define SYSTEM_PARTITION_AT_PARAMETER_ADDR					0xfd000
#define SYSTEM_PARTITION_SSL_CLIENT_CERT_PRIVKEY_ADDR		0xfc000
#define SYSTEM_PARTITION_SSL_CLIENT_CA_ADDR					0xfb000
#define SYSTEM_PARTITION_WPA2_ENTERPRISE_CERT_PRIVKEY_ADDR	0xfa000
#define SYSTEM_PARTITION_WPA2_ENTERPRISE_CA_ADDR			0xf9000
#else
#error "The flash map is not supported"
#endif

#ifdef CONFIG_ENABLE_IRAM_MEMORY
uint32 user_iram_memory_is_enabled(void)
{
    return CONFIG_ENABLE_IRAM_MEMORY;
}
#endif

static const partition_item_t at_partition_table[] = {
    { SYSTEM_PARTITION_BOOTLOADER, 						0x0, 												0x1000},
    { SYSTEM_PARTITION_OTA_1,   						0x1000, 											SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_OTA_2,   						SYSTEM_PARTITION_OTA_2_ADDR, 						SYSTEM_PARTITION_OTA_SIZE},
    { SYSTEM_PARTITION_RF_CAL,  						SYSTEM_PARTITION_RF_CAL_ADDR, 						0x1000},
    { SYSTEM_PARTITION_PHY_DATA, 						SYSTEM_PARTITION_PHY_DATA_ADDR, 					0x1000},
    { SYSTEM_PARTITION_SYSTEM_PARAMETER, 				SYSTEM_PARTITION_SYSTEM_PARAMETER_ADDR, 			0x3000},
    { SYSTEM_PARTITION_AT_PARAMETER, 					SYSTEM_PARTITION_AT_PARAMETER_ADDR, 				0x3000},
	{ SYSTEM_PARTITION_SSL_CLIENT_CERT_PRIVKEY, 		SYSTEM_PARTITION_SSL_CLIENT_CERT_PRIVKEY_ADDR, 		0x1000},
	{ SYSTEM_PARTITION_SSL_CLIENT_CA, 					SYSTEM_PARTITION_SSL_CLIENT_CA_ADDR, 				0x1000},
#ifdef CONFIG_AT_WPA2_ENTERPRISE_COMMAND_ENABLE
	{ SYSTEM_PARTITION_WPA2_ENTERPRISE_CERT_PRIVKEY, 	SYSTEM_PARTITION_WPA2_ENTERPRISE_CERT_PRIVKEY_ADDR,	0x1000},
    { SYSTEM_PARTITION_WPA2_ENTERPRISE_CA, 				SYSTEM_PARTITION_WPA2_ENTERPRISE_CA_ADDR, 			0x1000},
#endif
};

void ICACHE_FLASH_ATTR user_pre_init(void)
{
    if(!system_partition_table_regist(at_partition_table, sizeof(at_partition_table)/sizeof(at_partition_table[0]),SPI_FLASH_SIZE_MAP)) {
		os_printf("system_partition_table_regist fail\r\n");
		while(1);
	}
}

void ICACHE_FLASH_ATTR
user_init(void)
{
	os_printf("ESP8266 SDK By_lxz\r\n");

	if(esp_now_init() == 0)
	{
		os_printf("esp now init OK\r\n");

		esp_now_register_recv_cb(espnow_recv_cb);	//ע��ص�����

/* Controller ���� */
#ifdef ESP_NOW_CONTROLLER
		wifi_set_opmode(STATION_MODE);		//����ΪSTAģʽ
		esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
		esp_now_add_peer(mac2, ESP_NOW_ROLE_SLAVE, 1, key, 16);

		os_timer_disarm(&timer);
		os_timer_setfn(&timer,  (os_timer_func_t *)timer_intterupt, NULL);
		os_timer_arm(&timer, 1000, 1);		//ÿһ�뷢��һ��hello��Slaver
#endif

/* Slaver ���� */
#ifdef ESP_NOW_SLAVER
		wifi_set_opmode(SOFTAP_MODE);		//����ΪAPģʽ
		esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
		esp_now_add_peer(mac1, ESP_NOW_ROLE_CONTROLLER, 1, key, 16);
#endif

	} else {
		os_printf("esp_now init failed\n");
	}
}


/* ���ջص�
 * Slaver���յ����ݺ�
 * �����ͷ���MAC��ַ�����ݴ�ӡ���� */
void ICACHE_FLASH_ATTR
espnow_recv_cb(u8 *mac_addr, u8 *data, u8 len)
{
	uint8 i;
	os_printf("MAC:");

	for(i=0; i<6; i++)
	{
		os_printf("%x ", mac_addr[i]);
	}
	os_printf("data: %s\r\n", data);
}

/* �������ݸ�Slaver */
void ICACHE_FLASH_ATTR
timer_intterupt(void *timer_arg)
{
	esp_now_send(mac2, "hello", 5);
}
