#include "user_interface.h"
#include "osapi.h"

#include "espconn.h"
#include "mem.h"
#include "c_types.h"
#include "tcp_client.h"
#include "spi_flash.h"
#include "upgrade.h"

struct espconn tcpclientConf;		//定义网络结构体
bool flag = 0;						//定义擦除标志位
uint32 flash_addr = 0;				//flash地址

void ICACHE_FLASH_ATTR
tcp_client_init(uint16 port)		//初始化函数
{
	uint8 remote_ip[4] = {192,168,10,154};		//远端ip，PC测试工具的话就是电脑ip

	tcpclientConf.type = ESPCONN_TCP;			//UDP/TCP
	tcpclientConf.state = ESPCONN_NONE;			//目前状态，none

	tcpclientConf.proto.tcp = (esp_tcp*)os_zalloc(sizeof(esp_tcp));		//分配空间

	tcpclientConf.proto.tcp->remote_port = port;		//远端ip

	tcpclientConf.proto.tcp->reconnect_callback = tcp_reconn_cb;	//设置重新连接回调
	tcpclientConf.proto.tcp->disconnect_callback = tcp_disconn_cb;	//断开连接回调
	tcpclientConf.proto.tcp->connect_callback = tcp_conn_cb;		//连接回调

	os_memcpy(tcpclientConf.proto.tcp->remote_ip, remote_ip, 4);	//远端ip，内存拷贝

	tcpclientConf.recv_callback = tcp_recv_cb;		//接收回调

	int result = espconn_connect(&tcpclientConf);	//建立TCP连接
	if(result == 0){
		os_printf("tcp test success\r\n");
	}else {
		os_printf("tcp test fault\r\n");
	}
}


/* 回调函数  */
void ICACHE_FLASH_ATTR
tcp_recv_cb(void *arg, char *pdata, unsigned short len)
{
	uint16 i, j, k;
	uint32 temp;
	uint32 file_size = 0;		//bin文件大小
	uint32 sector_size = 0;		//flash扇区大小
	uint32 sector_num = 0;		//扇区编号

	os_printf("get data: %s\r\n", pdata);	//打印接收数据

	if(flag == 0)
	{
		flag =1;
		os_printf("目前是：%s\r\n",(system_upgrade_userbin_check() == 0)?"user1":"user2");

		for(i=len-5, j = 0; pdata[i] != ' '; j++, i--)
		{
			temp = pdata[i] - '0';
			for(k=0; k<j; k++)
			{
				temp *= 10;
			}
			file_size += temp;
		}

		if(file_size % 4096 == 0)
		{
			sector_size = file_size/4096;
		}else{
			sector_size = file_size/4096 + 1;
		}

		os_printf("file_size = %d\n", file_size);
		os_printf("sector_size = %d\n", sector_size);

		if(system_upgrade_userbin_check() == 0)
				{
					for(i=0; i<sector_size; i++)
					{
						os_printf("user2 eraseing....\n");
						if(spi_flash_erase_sector(0x101000/4096 + i) == SPI_FLASH_RESULT_OK)
						{
							os_printf("sector %d erase ok\n", (0x101000/4096) + i);
						}
					}
					flash_addr = 0x101000;
					sector_num = 0x101000/4096;
				} else
				{
					for(i=0; i<sector_size; i++)
					{
						os_printf("user1 eraseing....\n");
						if(spi_flash_erase_sector(0x01000/4096 + i) == SPI_FLASH_RESULT_OK)
						{
							os_printf("sector %d erase ok\n", (0x01000/4096) + i);
						}
					}
					flash_addr = 0x01000;
					sector_num = 0x01000/4096;
				}

	} else
	{
		if(((sector_num+1)*4096 - flash_addr) > len)
		{
			spi_flash_write(flash_addr, (uint32*)pdata, len);
			flash_addr += len;
		}
		else
		{
			spi_flash_write(flash_addr, (uint32*)pdata, (sector_num+1)*4096 - flash_addr);
			flash_addr = (sector_num+1)*4096;

			spi_flash_write(flash_addr,
					(uint32*)pdata + (sector_num+1)*4096 - flash_addr,
					len - ((sector_num+1)*4096 - flash_addr));
			flash_addr += (len - ((sector_num+1)*4096 - flash_addr));
			sector_num++;
		}
	}
}

void ICACHE_FLASH_ATTR
tcp_reconn_cb(void *arg, sint8 err)
{
	os_printf("reconnect\r\n");
}

/* 烧写完新固件后会自动断开连接
 * 设置升级状态：升级完成
 * 重新启动*/
void ICACHE_FLASH_ATTR
tcp_disconn_cb(void *arg)
{
	os_printf("disconnect\r\n");
	flag = 0;

	system_upgrade_flag_set(UPGRADE_FLAG_FINISH);
	system_upgrade_reboot();
}

/* 连接到服务器后发送GET请求，获取新固件 */
void ICACHE_FLASH_ATTR
tcp_conn_cb(void *arg)
{
	os_printf("connect success\r\n");
	espconn_send(&tcpclientConf, GET, os_strlen(GET));
}

