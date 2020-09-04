#ifndef __DEBUG__H_

#define __DEBUG__H_

#include "stdio.h"
#include "usart.h"

#define DEBUG

#ifdef DEBUG
#define debug(...){\
		int len = 0;\
		len = snprintf(debug_buff,128,__VA_ARGS__);\
		if(len >0){\
			HAL_UART_Transmit(&huart1,(uint8_t *)debug_buff,len,0xff);\
		}\
}
#else
#define debug(...){}
#endif


#endif

