#pragma once
#include <stdint.h>
#include "numicro_8051.h"


#define _DIV_ROUND_CLOSEST_POS(n, d) (((n) + (d) / 2) / (d))
#define get_delay_timer_value(delay_us, clk_div, cpu_hz) (65536 - (_DIV_ROUND_CLOSEST_POS((delay_us * cpu_hz / clk_div), 1000000)))

// Because these are all constant expressions, SDCC should optimize all usages of these into constant values
#define TIMER_DIV12_VALUE_10us	   get_delay_timer_value(10, 12, F_CPU)
#define TIMER_DIV12_VALUE_100us	   get_delay_timer_value(100, 12, F_CPU)
#define TIMER_DIV12_VALUE_1ms	   get_delay_timer_value(1000, 12, F_CPU)
#define	TIMER_DIV12_VALUE_10ms	   get_delay_timer_value(10000, 12, F_CPU)
#define TIMER_DIV12_VALUE_40ms	   get_delay_timer_value(40000, 12, F_CPU)
#define TIMER_DIV4_VALUE_10us	   get_delay_timer_value(10, 4, F_CPU)
#define TIMER_DIV4_VALUE_100us	   get_delay_timer_value(100, 4, F_CPU)
#define TIMER_DIV4_VALUE_200us	   get_delay_timer_value(200, 4, F_CPU)
#define TIMER_DIV4_VALUE_500us	   get_delay_timer_value(500, 4, F_CPU)
#define TIMER_DIV4_VALUE_1ms       get_delay_timer_value(1000, 4, F_CPU)
#define TIMER_DIV16_VALUE_10ms	   get_delay_timer_value(10000, 16, F_CPU)
#define TIMER_DIV64_VALUE_30ms	   get_delay_timer_value(30000, 64, F_CPU)
#define	TIMER_DIV128_VALUE_100ms   get_delay_timer_value(100000, 128, F_CPU)
#define	TIMER_DIV128_VALUE_200ms   get_delay_timer_value(200000, 128, F_CPU)
#define TIMER_DIV256_VALUE_500ms   get_delay_timer_value(500000, 256, F_CPU)
#define	TIMER_DIV512_VALUE_1s      get_delay_timer_value(1000000, 512, F_CPU)

void Timer0_Delay100us(UINT32 u32CNT);
void Timer0_Delay1ms(UINT32 u32CNT);
void Timer1_Delay10ms(UINT32 u32CNT);
void Timer2_Delay500us(UINT32 u32CNT);
void Timer3_Delay100ms(UINT32 u32CNT);

void Timer0_Delay40ms(UINT32 u32CNT);
void Timer3_Delay10us(UINT32 u32CNT);