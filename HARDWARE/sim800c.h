#ifndef __SIM800C_H
#define __SIM800C_H

#include "stm32f10x.h"

u8 sim800c_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
void gprs_init(void);



#endif


