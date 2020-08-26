#ifndef SYSTEM
#define SYSTEM

#include <stdint.h>
#include "xprintf.h"
#include "define.h"
#include "stm8.h"

void delay_ms(uint16_t t);
void delay_us(uint16_t t);
inline void pinInit();
void uartInit(uint32_t baud_rate, uint32_t f_master);
void  uart_print(char *str);
void uart_char(unsigned char c);
uint16_t adc(uint8_t chanel);
void systemInit();

#endif
