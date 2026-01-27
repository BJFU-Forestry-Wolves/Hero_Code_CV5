/*
 *  Project      :  
 *  
 *  FilePath     : util_debug.c
 *  Description  : 
 *  LastEditors  : Mr.Lee
 *  Date         : 2024年10月3日23:18:23
 *  LastEditTime : 
 */
 
#include "util_debug.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


#define 			Debug_Uart				huart1


void debug_init()
{



}


 void usart_printf(const char *fmt,...) 
{ 
	static uint8_t tx_buf[256] = {0}; 
	static va_list ap; 
	static uint16_t len; 
	va_start(ap, fmt); 
	
	//return length of string 
	//返回字符串长度 
	len = vsprintf((char *)tx_buf, fmt, ap); 
	va_end(ap); 
	HAL_UART_Transmit_IT(&Debug_Uart,tx_buf,len);
}
