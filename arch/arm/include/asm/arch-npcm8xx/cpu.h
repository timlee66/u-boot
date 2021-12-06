/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#ifndef _NPCM_CPU_H_
#define _NPCM_CPU_H_

#define NPCM_GCR_BA			0xF0800000
#define NPCM_CLK_BA			0xF0801000
#define NPCM_GPIO_BA			0xF0010000

#define NPCM_UART0_BA		0xF0000000
#define NPCM_UART1_BA		0xF0001000
#define NPCM_UART2_BA		0xF0002000
#define NPCM_UART3_BA		0xF0003000

#define NPCM_BASE(device, base) \
static inline unsigned long __attribute__((no_instrument_function)) \
	npcm_get_base_##device(void) \
{ \
	return NPCM_##base; \
}

NPCM_BASE(gcr, GCR_BA)
NPCM_BASE(clk, CLK_BA)
NPCM_BASE(gpio, GPIO_BA)
NPCM_BASE(uart, UART0_BA)

#endif
