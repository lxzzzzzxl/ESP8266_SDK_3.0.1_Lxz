#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "espnow.h"
#include "simple_pair.h"

//#define AS_STA		//代码块宏定义
#define AS_AP

/* 函数声明 */
void sp_status(u8 *sa, u8 status);
void init_done(void);
void show_key(u8 *buf, u8 len);
static void scan_done(void *arg, STATUS status);


static u8 tmpkey[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};	//临时密钥

#ifdef AS_STA    //sta的密钥，将从ap获取
	static u8 ex_key[16] = {0x00};
#endif

#ifdef AS_AP	//ap的密钥，将发送给sta
	static u8 ex_key[16] = {0xff, 0xee, 0xdd, 0xcc, 0xbb, 0xaa, 0x99, 0x88,    0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00};
#endif


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

	system_init_done_cb(init_done);		//注册系统初始化完成的回调函数
}

void ICACHE_FLASH_ATTR
init_done(void)
{
	int ret;
#ifdef AS_STA
	wifi_set_opmode(STATION_MODE);

	ret = simple_pair_init();		//初始化simple-pair功能
	if(ret)
	{
		os_printf("Simple Pair: init error, %d\n", ret);
		return;
	}

	ret = register_simple_pair_status_cb(sp_status);	//注册simple-pair状态回调函数
	if(ret)
	{
		 os_printf("Simple Pair: register status cb error, %d\n", ret);
		 return;
	}
	os_printf("Simple Pair: STA Enter Scan Mode ...\n");

	ret = simple_pair_sta_enter_scan_mode();	// Station端进入 scan模式
	if(ret)
	{
		os_printf("Simple Pair: STA Enter Scan Mode Error, %d\n", ret);
		return;
	 }
	os_printf("Simple Pair: STA Scan AP ...\n");

	wifi_station_scan(NULL,scan_done);		//获取ap信息
#endif

#ifdef AS_AP
	wifi_set_opmode(SOFTAP_MODE);

	ret = simple_pair_init();
	if (ret)
	{
		os_printf("Simple Pair: init error, %d\n", ret);
		return;
	}

	ret = register_simple_pair_status_cb(sp_status);
	if (ret)
	{
		os_printf("Simple Pair: register status cb error, %d\n", ret);
		return;
	}
	 os_printf("Simple Pair: AP Enter Announce Mode ...\n");

	 ret = simple_pair_ap_enter_announce_mode();		//AP端进入 announce模式
	 if (ret)
	 {
		 os_printf("Simple Pair: AP Enter Announce Mode Error, %d \n", ret);
		 return;
	 }
	#endif
}

/* 回调函数 */
void ICACHE_FLASH_ATTR
sp_status(u8 *sa, u8 status)
{
#ifdef AS_STA
	switch (status)
	{
		case  SP_ST_STA_FINISH:
			simple_pair_get_peer_ref(NULL, NULL, ex_key);	//获取ap的密钥
			os_printf("Simple Pair: STA FINISH, Ex_key ");
			show_key(ex_key, 16);
			simple_pair_deinit();
			break;
		case SP_ST_STA_AP_REFUSE_NEG:
			os_printf("Simple Pair: Recv AP Refuse\n");
			simple_pair_state_reset();
			simple_pair_sta_enter_scan_mode();
			wifi_station_scan(NULL, scan_done);
			break;
		case SP_ST_WAIT_TIMEOUT:
			os_printf("Simple Pair: Neg Timeout\n");
			simple_pair_state_reset();
			simple_pair_sta_enter_scan_mode();
			wifi_station_scan(NULL, scan_done);
			break;
		case SP_ST_SEND_ERROR:
			os_printf("Simple Pair: Send Error\n");
			break;
		case SP_ST_KEY_INSTALL_ERR:
			os_printf("Simple Pair: Key Install Error\n");
			break;
		case SP_ST_KEY_OVERLAP_ERR:
			os_printf("Simple Pair: Key Overlap Error\n");
			break;
		case SP_ST_OP_ERROR:
			os_printf("Simple Pair: Operation Order Error\n");
			break;
		default:
			os_printf("Simple Pair: Unknown Error\n");
			break;
	}
#endif /* AS_STA */

#ifdef AS_AP
	switch (status)
	{
		case  SP_ST_AP_FINISH:
			simple_pair_get_peer_ref(NULL, NULL, ex_key);
			os_printf("Simple Pair: AP FINISH\n");
			simple_pair_deinit();
			break;
		case SP_ST_AP_RECV_NEG:
			os_printf("Simple Pair: Recv STA Negotiate Request\n");
			simple_pair_set_peer_ref(sa, tmpkey, ex_key);
			simple_pair_ap_start_negotiate();
			break;
		case SP_ST_WAIT_TIMEOUT:
			os_printf("Simple Pair: Neg Timeout\n");
			simple_pair_state_reset();
			simple_pair_ap_enter_announce_mode();
			break;
		case SP_ST_SEND_ERROR:
			os_printf("Simple Pair: Send Error\n");
			break;
		case SP_ST_KEY_INSTALL_ERR:
			os_printf("Simple Pair: Key Install Error\n");
			break;
		case SP_ST_KEY_OVERLAP_ERR:
			break;
		case SP_ST_OP_ERROR:
			os_printf("Simple Pair: Operation Order Error\n");
			break;
		default:
			os_printf("Simple Pair: Unknown Error\n");
			break;
	}
#endif /* AS_AP */
}

#ifdef AS_STA
static void ICACHE_FLASH_ATTR
scan_done(void *arg, STATUS status)
{
	int ret;
    if (status == OK)
    {
        struct bss_info *bss_link = (struct bss_info *)arg;
        while (bss_link != NULL)
        {
        	if (bss_link->simple_pair)
        	{
        		os_printf("Simple Pair: bssid %02x:%02x:%02x:%02x: %02x:%02x Ready!\n",
        				bss_link->bssid[0], bss_link->bssid[1], bss_link->bssid[2],
						bss_link->bssid[3], bss_link->bssid[4], bss_link->bssid[5]);
        		simple_pair_set_peer_ref(bss_link->bssid, tmpkey, NULL);
        		ret = simple_pair_sta_start_negotiate();
        		if (ret)
        			os_printf("Simple Pair: STA start NEG Failed\n");
        		else
        			os_printf("Simple Pair: STA start NEG OK\n");
        		break;
        	}
        	bss_link = bss_link->next.stqe_next;
        }
    } else
    {
    	os_printf("err, scan status %d\n", status);
    }
}
#endif

void ICACHE_FLASH_ATTR
show_key(u8 *buf, u8 len)
{
	u8 i;
	for (i = 0; i < len; i++)
		os_printf("%02x,%s", buf[i], (i%16 == 15?"\n":" "));
}


