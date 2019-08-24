#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "smartconfig.h"

/* 声明回调 */
void ICACHE_FLASH_ATTR smartconfig_done(sc_status status, void	*pdata);

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

/* 用户函数 */
void ICACHE_FLASH_ATTR
user_init(void)
{
	os_printf("ESP8266 SDK By_lxz\r\n");
	smartconfig_stop();			//关闭smartconfig
	wifi_station_disconnect();	//断开wifi连接

	smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS);		//设置连接方式SC_TYPE_ESPTOUCH/ SC_TYPE_AIRKISS/ SC_TYPE_ESPTOUCH_AIRKISS
	smartconfig_start(smartconfig_done);		//打开smartconfig
}

/* 回调函数 */
void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void	*pdata)
{
	switch(status)
	{
		case SC_STATUS_WAIT:
			os_printf("SC_STATUS_WAIT\n");
		break;
		case SC_STATUS_FIND_CHANNEL:
			os_printf("SC_STATUS_FIND_CHANNEL\n");
		break;
		case SC_STATUS_GETTING_SSID_PSWD:
			os_printf("SC_STATUS_GETTING_SSID_PSWD\n");
			sc_type	*type	=	pdata;
			if	(*type	==	SC_TYPE_ESPTOUCH)
				{
					os_printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
				}else{
						os_printf("SC_TYPE:SC_TYPE_AIRKISS\n");}
		break;
		case SC_STATUS_LINK:
			os_printf("SC_STATUS_LINK\n");
			struct station_config *sta_conf	= pdata;
			wifi_station_set_config(sta_conf);
			wifi_station_disconnect();
			wifi_station_connect();
		break;
		case SC_STATUS_LINK_OVER:
			os_printf("SC_STATUS_LINK_OVER\n");
			if (pdata != NULL)
			{
				uint8 phone_ip[4] =	{0};
				memcpy(phone_ip, (uint8*)pdata,	4);
				os_printf("Phone ip: %d.%d.%d. %d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);}
				smartconfig_stop();
		break;
	}
}

