#ifndef __NET_H
#define	__NET_H

#include "stm32f10x.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "enc28j60.h"	
void InitNet(void);	
void UipPro(void);
void eth_poll(void);

#endif /* __NET_H */
