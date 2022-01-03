/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef _DT_BINDINGS_NPCM845_CLOCK_H_
#define _DT_BINDINGS_NPCM845_CLOCK_H_

#define CLK_AHB		0
#define CLK_APB1	1
#define CLK_APB2	2
#define CLK_APB3	3
#define CLK_APB4	4
#define CLK_APB5	5
#define CLK_UART1	6
#define CLK_UART2	7
#define CLK_SDHC	8
#define CLK_PLL0	9
#define CLK_PLL1	10
#define CLK_PLL2	11
#define CLK_PLL2DIV2	12
#define CLK_REFCLK	13

#define CLK_COUNT	(CLK_REFCLK + 1)

#endif
