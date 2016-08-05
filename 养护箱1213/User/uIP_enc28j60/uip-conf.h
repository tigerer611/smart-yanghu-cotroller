/****************************************************************************
* Copyright (C), 2011 奋斗嵌入式工作室 www.ourstm.net
*	           
* QQ: 9191274, 旺旺：sun68, Email: sun68@163.com 
* 淘宝店铺：ourstm.taobao.com  
*
* 文件名: uip_conf.h
* 内容简述:
*       uip配置
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.1    2011-8-23 sun68  修改该文件
*
*/

#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

#include <inttypes.h>


typedef uint8_t u8_t;

typedef uint16_t u16_t;	

typedef unsigned short uip_stats_t;


/* 最大TCP连接数 */
#define UIP_CONF_MAX_CONNECTIONS 10

/* 最大端口监听数 */
#define UIP_CONF_MAX_LISTENPORTS 10	  

/* uIP 缓存大小*/
#define UIP_CONF_BUFFER_SIZE     4096

/* CPU字节顺序 */ 
#define UIP_CONF_BYTE_ORDER  UIP_LITTLE_ENDIAN

/* 日志开关	 */			
#define UIP_CONF_LOGGING         1

/* UDP支持开关*/
#define UIP_CONF_UDP             1

/*UDP校验和开关 */
#define UIP_CONF_UDP_CHECKSUMS   1

/* uIP统计开关 */
#define UIP_CONF_STATISTICS      1


#include "tcp_demo.h"
#endif
