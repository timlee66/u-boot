// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/*
 * Cache support: switch on or off, get status
 */
#include <common.h>
#include <command.h>
#include <linux/compiler.h>
#include <asm/arch/clock.h>



#define _BIT(reg,bit)        (1 << bit)
#define HW_BYTE(add) (*(( volatile unsigned char *)(uintptr_t)(add)))
#define HW_WORD(add) (*(( volatile unsigned short *)(uintptr_t)(add)))
#define HW_DWORD(add) (*(( volatile unsigned int *)(uintptr_t)(add)))
#define _READ_REG(RegName) (RegName)
#define _WRITE_REG(RegName,data) (RegName=data)
#define _SET_BIT_REG(RegName, Bit) (RegName|=(1<<Bit))
#define _CLEAR_BIT_REG(RegName, Bit) (RegName&=(~(1<<Bit)))
#define _READ_BIT_REG(RegName, Bit) ((RegName>>Bit)&1)

#define READ_REG(RegisterName) _READ_REG(RegisterName)
#define WRITE_REG(RegisterName, Value) _WRITE_REG(RegisterName, Value)
#define SET_BIT_REG(RegisterName, BitNum) _SET_BIT_REG(RegisterName, BitNum)
#define CLEAR_BIT_REG(RegisterName, BitNum) _CLEAR_BIT_REG(RegisterName, BitNum)
#define READ_BIT_REG(RegisterName, BitNum) _READ_BIT_REG(RegisterName, BitNum)

#define CLK_BA				0xF0801000
#define SECCNT				HW_DWORD(CLK_BA+0x68)			// Seconds Counter
#define CNTR25M				HW_DWORD(CLK_BA+0x6C)			// Mili Seconds Counter

volatile u32 memtest_src_addr = 0x600000;


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetTimeStamp                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         Current time stamp                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void  CLK_GetTimeStamp(u32 time_quad[2])
{
	u32 Seconds;
	u32 RefClocks;

	do
	{
		Seconds = READ_REG(SECCNT);
		RefClocks = (READ_REG(CNTR25M)) & 0x01FFFFFF;
	} while (READ_REG(SECCNT) != Seconds);

	time_quad[0] = RefClocks;
	time_quad[1] = Seconds;
}


void my_memwrite(u32 source, u32 size)
{
	int i;
	u32 *ptr = (u32 *)(uintptr_t)source;

	for (i = 0; i < size/4; i++)
	    *(ptr + i) = source + i;
}

void my_memread(u32 source, u32 size)
{
	int i;
	volatile u32 *ptr = (u32 *)(uintptr_t)source;
	u32 sigma = 0;

	/* since 'sigma' will not be use but 'ptr' is volatile, the assembly 
	 * will do only reads into a register w/o adding to sigma
	 * we also do loop unrolling to minimize instruction not related to 
	 * loading from memory, so we get one assembly line per read
	 * it is recommended to verify this in disassembly
	 */

	for (i = 0, sigma = 0; i < size/4; i+=64){
 	    sigma += *(ptr + i +  0);
 	    sigma += *(ptr + i +  1);
 	    sigma += *(ptr + i +  2);
 	    sigma += *(ptr + i +  3);
 	    sigma += *(ptr + i +  4);
 	    sigma += *(ptr + i +  5);
 	    sigma += *(ptr + i +  6);
 	    sigma += *(ptr + i +  7);
 	    sigma += *(ptr + i +  8);
 	    sigma += *(ptr + i +  9);
	    sigma += *(ptr + i + 10);
	    sigma += *(ptr + i + 11);
	    sigma += *(ptr + i + 12);
	    sigma += *(ptr + i + 13);
	    sigma += *(ptr + i + 14);
	    sigma += *(ptr + i + 15);
	    sigma += *(ptr + i + 16);
	    sigma += *(ptr + i + 17);
	    sigma += *(ptr + i + 18);
	    sigma += *(ptr + i + 19);
	    sigma += *(ptr + i + 20);
	    sigma += *(ptr + i + 21);
	    sigma += *(ptr + i + 22);
	    sigma += *(ptr + i + 23);
	    sigma += *(ptr + i + 24);
	    sigma += *(ptr + i + 25);
	    sigma += *(ptr + i + 26);
	    sigma += *(ptr + i + 27);
	    sigma += *(ptr + i + 28);
	    sigma += *(ptr + i + 29);
	    sigma += *(ptr + i + 30);
	    sigma += *(ptr + i + 31);
	    sigma += *(ptr + i + 32);
	    sigma += *(ptr + i + 33);
	    sigma += *(ptr + i + 34);
	    sigma += *(ptr + i + 35);
	    sigma += *(ptr + i + 36);
	    sigma += *(ptr + i + 37);
	    sigma += *(ptr + i + 38);
	    sigma += *(ptr + i + 39);
	    sigma += *(ptr + i + 40);
	    sigma += *(ptr + i + 41);
	    sigma += *(ptr + i + 42);
	    sigma += *(ptr + i + 43);
	    sigma += *(ptr + i + 44);
	    sigma += *(ptr + i + 45);
	    sigma += *(ptr + i + 46);
	    sigma += *(ptr + i + 47);
	    sigma += *(ptr + i + 48);
	    sigma += *(ptr + i + 49);
	    sigma += *(ptr + i + 50);
	    sigma += *(ptr + i + 51);
	    sigma += *(ptr + i + 52);
	    sigma += *(ptr + i + 53);
	    sigma += *(ptr + i + 54);
	    sigma += *(ptr + i + 55);
	    sigma += *(ptr + i + 56);
	    sigma += *(ptr + i + 57);
	    sigma += *(ptr + i + 58);
	    sigma += *(ptr + i + 59);
	    sigma += *(ptr + i + 60);
	    sigma += *(ptr + i + 61);
	    sigma += *(ptr + i + 62);
	    sigma += *(ptr + i + 63);
	}
}

void test_basic_performance(void)
{
	u32 i,j,chunk_size = 4096;
	u32 loops = 4096;
	u32 iUsCnt_start[2], iUsCnt_end[2];
	u32 ticks;
	u32 src_addr = memtest_src_addr;

	for (j=0; j<10; j++)
	{
		chunk_size <<= 1;
		loops >>=1;
		/* Setup the buffer */
		/* fill the memory and cache in case of allocating on write */
		my_memwrite(src_addr, chunk_size);
		
		/* read the memory once before measuring time in order to fill
		 * the cacche in case of allocate on read
		 */
		my_memread(src_addr, chunk_size);

		CLK_GetTimeStamp(iUsCnt_start);
		
		for (i = 0; i < loops; i++)
			my_memread(src_addr, chunk_size);

		CLK_GetTimeStamp(iUsCnt_end);
		ticks = ((EXT_CLOCK_FREQUENCY_MHZ * _1MHz_ * (iUsCnt_end[1] - iUsCnt_start[1])) + iUsCnt_end[0] - iUsCnt_start[0])/EXT_CLOCK_FREQUENCY_MHZ;
		printf("Duration = %7d usec loops= %4d ",ticks, loops );
		printf("chunk_size = %4d KB: Performance %7d KBps \n", chunk_size/1024, chunk_size*loops*100/ticks*10);
		src_addr = memtest_src_addr + chunk_size; // this is done in order not to use a used location
	}
}

int do_cache_perf_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	test_basic_performance();

	return 0;
}

U_BOOT_CMD(
	cache_perf_test,   1,   1,     do_cache_perf_test,
	"Cache performance test according to L1 and L2 cache sizes",
	"\n"
	" If data and instruction cache are enabled:\n"
	"   performance should be fastest when      chunk_size <= L1\n"
	"   performance should be fast    when L1 < chunk_size <= L2\n"
	"   performance should be slow    when L2 < chunk_size\n"
	" If data cache is disabled:\n"
	"   performance should be slowest on all chunk_size cases"
);
