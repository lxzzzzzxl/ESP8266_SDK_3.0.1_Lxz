#ifndef __HTTP_RE_H__
#define __HTTP_RE_H__

#define GET "GET /v3/weather/now.json?key=smtq3n0ixdggurox&location=beijing&language=zh-Hans&unit=c HTTP/1.1\r\nHost:api.seniverse.com\r\nAccept-Language:zh-cn\r\nConnection:keep-alive\r\n\r\n"


void http_visit_web(const char *url);
void find_dns(const char *name, ip_addr_t *ipaddr, void *callback_arg);

#endif
