#pragma once
#include <stdint.h>
#include "numicro_8051.h"

// We are using the external clock value here so that we delay for the correct amount of time
#ifndef EXT_CLK
#define EXT_CLK F_CPU
#endif

#define _DIV_ROUND_CLOSEST_POS(n, d) (((n) + (d) / 2) / (d))
#define get_delay_timer_value(delay_us, clk_div, cpu_hz) (65536 - (_DIV_ROUND_CLOSEST_POS((delay_us * _DIV_ROUND_CLOSEST_POS(cpu_hz/ 1000000UL)), clk_div)))
// Only use this for constant expressions
#define constexpr_get_delay_timer_value(delay_us, clk_div, cpu_hz) (65536ULL - (_DIV_ROUND_CLOSEST_POS((delay_us * cpu_hz / clk_div), 1000000ULL)))

// Because these are all constant expressions, SDCC should optimize all usages of these into constant values
#define TIMER_DIV12_VALUE_10us	   constexpr_get_delay_timer_value(10ULL, 12ULL, EXT_CLK)
#define TIMER_DIV12_VALUE_100us	   constexpr_get_delay_timer_value(100ULL, 12ULL, EXT_CLK)
#define TIMER_DIV12_VALUE_1ms	   constexpr_get_delay_timer_value(1000ULL, 12ULL, EXT_CLK)
#define	TIMER_DIV12_VALUE_10ms	   constexpr_get_delay_timer_value(10000ULL, 12ULL, EXT_CLK)
#define TIMER_DIV12_VALUE_40ms	   constexpr_get_delay_timer_value(40000ULL, 12ULL, EXT_CLK)
#define TIMER_DIV4_VALUE_10us	   constexpr_get_delay_timer_value(10ULL, 4ULL, EXT_CLK)
#define TIMER_DIV4_VALUE_100us	   constexpr_get_delay_timer_value(100ULL, 4ULL, EXT_CLK)
#define TIMER_DIV4_VALUE_200us	   constexpr_get_delay_timer_value(200ULL, 4ULL, EXT_CLK)
#define TIMER_DIV4_VALUE_500us	   constexpr_get_delay_timer_value(500ULL, 4ULL, EXT_CLK)
#define TIMER_DIV4_VALUE_1ms       constexpr_get_delay_timer_value(1000ULL, 4ULL, EXT_CLK)
#define TIMER_DIV16_VALUE_10ms	   constexpr_get_delay_timer_value(10000ULL, 16ULL, EXT_CLK)
#define TIMER_DIV64_VALUE_30ms	   constexpr_get_delay_timer_value(30000ULL, 64ULL, EXT_CLK)
#define	TIMER_DIV128_VALUE_100ms   constexpr_get_delay_timer_value(100000ULL, 128ULL, EXT_CLK)
#define	TIMER_DIV128_VALUE_200ms   constexpr_get_delay_timer_value(200000ULL, 128ULL, EXT_CLK)
#define TIMER_DIV256_VALUE_500ms   constexpr_get_delay_timer_value(500000ULL, 256ULL, EXT_CLK)
#define	TIMER_DIV512_VALUE_1s      constexpr_get_delay_timer_value(1000000ULL, 512ULL, EXT_CLK)

void Timer0_Delay100us(UINT32 u32CNT);
void Timer0_Delay1ms(UINT32 u32CNT);
void Timer1_Delay10ms(UINT32 u32CNT);
void Timer2_Delay500us(UINT32 u32CNT);
void Timer3_Delay100ms(UINT32 u32CNT);

void Timer0_Delay40ms(UINT32 u32CNT);
void Timer3_Delay10us(UINT32 u32CNT);