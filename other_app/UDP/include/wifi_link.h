#ifndef __WIFI_LINK_H__
#define __WIFI_LINK_H__


#define SSID			"aptest"
#define PASSWORD		"123456789"

void user_set_station_config(void);
void wifi_event(System_Event_t *event);

#endif
