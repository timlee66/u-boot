/*
 *  Copyright (c) 2017 Nuvoton Technology Corp.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __NPCMX50_CPU_H_
#define __NPCMX50_CPU_H_

#define NPCM850_GCR_BA			0xF0800000
#define NPCM850_CLK_BA			0xF0801000
#define NPCM850_GPIO_BA			0xF0010000

#define NPCM850_UART0_BA		0xF0000000
#define NPCM850_UART1_BA		0xF0001000
#define NPCM850_UART2_BA		0xF0002000
#define NPCM850_UART3_BA		0xF0003000



#define NPCM850_BASE(device, base) \
static inline unsigned long __attribute__((no_instrument_function)) \
	npcm850_get_base_##device(void) \
{ \
	return NPCM850_##base; \
}

NPCM850_BASE(gcr, GCR_BA)
NPCM850_BASE(clk, CLK_BA)
NPCM850_BASE(gpio, GPIO_BA)
NPCM850_BASE(uart, UART0_BA)


#endif
