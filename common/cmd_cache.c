/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Cache support: switch on or off, get status
 */
#include <common.h>
#include <command.h>
#include <linux/compiler.h>

volatile u32 memtest_pattern = 0x01;

volatile u32 memtest_chunk_size = 1024*1024;
volatile u32 memtest_src_addr = 0x600000;
volatile u32 memtest_dst_addr = 0xA00000;


#if defined (NPCM750)
#define GDMA0_BASE                 0xF0850000      /* GDMA0  */
#define GDMA1_BASE                 0xF0851000      /* GDMA1  */
#define GDMA2_BASE                 0xF0852000      /* GDMA2  */
#define GDMA3_BASE                 0xF0853000      /* GDMA3  */
#endif

void set_memory(u32 start, u32 size, u8 start_pattern)
{
	u8 *src = (u8 *)start;
	int i;
	for (i=0; i<size; i++)
		src[i] = start_pattern++;
}

int validate_memory(u32 start, u32 size, u8 start_pattern)
{
	u8 *src = (u8 *)start;
	int i;
	for (i=0; i<size; i++) {
		if(src[i] != start_pattern++) {
			printf("memory error at %08x:%02x start_pattern=%d \n", start+i, src[i],--start_pattern);
			return 1;
		}
	}
	return 0;
}

void my_memcpy(u32 source, u32 dest, u32 size)
{
#if 0
	memcpy((void *)dest, (void *)source, size);
#else

	u8 *src = (u8 *)source;
	u8 *dst = (u8 *)dest;
	int i;
	for (i=0; i<size; i++)
		dst[i] = src[i];
#endif
}

u32 my_memread(u32 source, u32 size)
{
	int i;
	u32 *ptr = (u32 *) source;
	u32 sigma = 0;

	for (i = 0, sigma = 0; i < size/4; i++)
	    sigma += *(ptr + i);

	return sigma;
}

static void nothing(u32 temp)
{
	return;
}


void test_basic_performance(void)
{
	u32 i,j,chunk_size = 4096;
	u32 loops = 4096;
	u32 temp;
	UINT32 iUsCnt_start[2], iUsCnt_end[2];
	u32 ticks;

	for (j=0; j<10; j++)
	{
		chunk_size <<= 1;
		loops >>=1;
		/* Setup the buffer */
		temp = my_memread(memtest_src_addr, chunk_size);

		CLK_GetTimeStamp(iUsCnt_start);
		for (i = 0; i < loops; i++)
			temp = my_memread(memtest_src_addr, chunk_size);

		CLK_GetTimeStamp(iUsCnt_end);
		ticks = ((EXT_CLOCK_FREQUENCY_MHZ * _1MHz_ * (iUsCnt_end[1] - iUsCnt_start[1])) + iUsCnt_end[0] - iUsCnt_start[0])/EXT_CLOCK_FREQUENCY_MHZ;
		printf("Duration = %8d usec loops= %4d ",ticks, loops );
		printf("Chunk Size = %5d KB: Performance %6d KBps \n", chunk_size/1024, chunk_size*loops*100/ticks*10);
        nothing(temp);
	}
}

void test_basic_copy(void)
{
	set_memory(memtest_src_addr, memtest_chunk_size, memtest_pattern);
	my_memcpy(memtest_src_addr, memtest_dst_addr, memtest_chunk_size);
	if(validate_memory(memtest_dst_addr, memtest_chunk_size, memtest_pattern))
		puts("test_basic_copy failed\n");
	else
		puts("test_basic_copy success\n");
}

void test_inval_d_range(void)
{
	memtest_pattern++;
	set_memory(memtest_src_addr, memtest_chunk_size, memtest_pattern);
	my_memcpy(memtest_src_addr, memtest_dst_addr, memtest_chunk_size);
	invalidate_dcache_range(memtest_dst_addr, memtest_dst_addr+memtest_chunk_size);
	if(validate_memory(memtest_dst_addr, memtest_chunk_size, memtest_pattern))
		puts("test_inval_d_range success\n");
	else
		puts("test_inval_d_range failed\n");
}

void test_flush_d_range(void)
{
	memtest_pattern++;
	set_memory(memtest_src_addr, memtest_chunk_size, memtest_pattern);
	my_memcpy(memtest_src_addr, memtest_dst_addr, memtest_chunk_size);
	flush_dcache_range(memtest_dst_addr, memtest_dst_addr+memtest_chunk_size);
	invalidate_dcache_range(memtest_dst_addr, memtest_dst_addr+memtest_chunk_size);
	if(validate_memory(memtest_dst_addr, memtest_chunk_size, memtest_pattern))
		puts("test_flush_d_range failed\n");
	else
		puts("test_flush_d_range success\n");
}

void test_flush_d_all(void)
{
	memtest_pattern++;
	printf("memtest_pattern %d\n", memtest_pattern);
	flush_dcache_all();
	set_memory(memtest_src_addr, memtest_chunk_size, memtest_pattern);
	my_memcpy(memtest_src_addr, memtest_dst_addr, memtest_chunk_size);
	flush_dcache_all();
	invalidate_dcache_range(memtest_dst_addr, memtest_dst_addr+memtest_chunk_size);
	if(validate_memory(memtest_dst_addr, memtest_chunk_size, memtest_pattern))
		puts("test_flush_d_all failed\n");
	else
		puts("test_flush_d_all success\n");
}


void test_inval_d_all(void)
{
	memtest_pattern++;
	set_memory(memtest_src_addr, memtest_chunk_size, memtest_pattern);
	my_memcpy(memtest_src_addr, memtest_dst_addr, memtest_chunk_size);
	puts("test_inval_d_all - destructive test - expect a crash after this. If there is a crash test is successful!\n");
	invalidate_dcache_all();
	invalidate_dcache_range(memtest_dst_addr, memtest_dst_addr+memtest_chunk_size);
	if(validate_memory(memtest_dst_addr, memtest_chunk_size, memtest_pattern))
		puts("test_inval_d_all failed\n");
	else
		puts("test_inval_d_all success\n");
}


#if defined (NPCM750)               /* Only Poleg */
#define TEST_BUF_SIZE (1024*256)

void l2_cache_test(void)
{
	int i, j;
	u32 *ptr = (u32 *) 0x400000;
	volatile u64 sigma = 0;

	/* Setup the buffer */
	for (i = 0; i < TEST_BUF_SIZE; i++) {
		*(ptr + i) = i;
	}

	flush_dcache_range((u32) ptr, (u32) ptr + (TEST_BUF_SIZE * sizeof(u32)));

	/* Here data at Cache is in sync with SDRAM */
	for (j = 0; j < 5000; j++)
	{
		for (i = 0, sigma = 0; i < TEST_BUF_SIZE; i++)
		{
			sigma += *(ptr + i);
        }

		if (!(j % 500))
		{
			puts(".");
			printf("sigma: 0x%llx\n", sigma);
        }
	}

	printf("total sigma: 0x%llx\n", sigma);
}

#define CP15_PRINT(crn, opc1, crm, opc2) \
	asm volatile("mrc p15, "#opc1", %0, "#crn", "#crm", "#opc2"" : "=r" (reg_val)); \
	printf("mrc p15, "#opc1", "#crn", "#crm", "#opc2" = 0x%08X\n", reg_val)

void cp15_regs(void)
{
	u32 reg_val=0;


	CP15_PRINT(c0, 0, c0, 0);
	CP15_PRINT(c0, 0, c0, 1);
	CP15_PRINT(c0, 0, c0, 2);
	CP15_PRINT(c0, 0, c0, 3);
	CP15_PRINT(c0, 0, c0, 5);
	CP15_PRINT(c0, 0, c0, 6);

	CP15_PRINT(c0, 0, c1, 0);
	CP15_PRINT(c0, 0, c1, 1);
	CP15_PRINT(c0, 0, c1, 2);
	CP15_PRINT(c0, 0, c1, 3);
	CP15_PRINT(c0, 0, c1, 4);
	CP15_PRINT(c0, 0, c1, 5);
	CP15_PRINT(c0, 0, c1, 6);
	CP15_PRINT(c0, 0, c1, 7);

	CP15_PRINT(c0, 0, c2, 0);
	CP15_PRINT(c0, 0, c2, 1);
	CP15_PRINT(c0, 0, c2, 2);
	CP15_PRINT(c0, 0, c2, 3);
	CP15_PRINT(c0, 0, c2, 4);

	CP15_PRINT(c0, 1, c0, 0);
	CP15_PRINT(c0, 1, c0, 1);
	CP15_PRINT(c0, 1, c0, 7);

	CP15_PRINT(c0, 2, c0, 0);

	CP15_PRINT(c1, 0, c0, 0);
	CP15_PRINT(c1, 0, c0, 1);
	CP15_PRINT(c1, 0, c0, 2);
	CP15_PRINT(c1, 0, c1, 0);
	CP15_PRINT(c1, 0, c1, 1);
	CP15_PRINT(c1, 0, c1, 2);
	CP15_PRINT(c1, 0, c1, 3);

	CP15_PRINT(c2, 0, c0, 0);
	CP15_PRINT(c2, 0, c0, 1);
	CP15_PRINT(c2, 0, c0, 2);

	CP15_PRINT(c3, 0, c0, 0);

	CP15_PRINT(c5, 0, c0, 0);
	CP15_PRINT(c5, 0, c0, 1);
	CP15_PRINT(c5, 0, c1, 0);
	CP15_PRINT(c5, 0, c1, 1);

	CP15_PRINT(c6, 0, c0, 0);
	CP15_PRINT(c6, 0, c0, 2);

	CP15_PRINT(c7, 0, c4, 0);

	CP15_PRINT(c10, 0, c0, 0);
	CP15_PRINT(c10, 0, c2, 0);
	CP15_PRINT(c10, 0, c2, 1);

	CP15_PRINT(c11, 0, c0, 0);
	CP15_PRINT(c11, 0, c0, 2);
	CP15_PRINT(c11, 0, c0, 4);
	CP15_PRINT(c11, 0, c1, 0);
	CP15_PRINT(c11, 0, c1, 1);

	CP15_PRINT(c12, 0, c0, 0);
	CP15_PRINT(c12, 0, c0, 2);
	CP15_PRINT(c12, 0, c1, 0);
	CP15_PRINT(c12, 0, c1, 2);

	CP15_PRINT(c15, 0, c0, 0);
	CP15_PRINT(c15, 0, c1, 0);
	CP15_PRINT(c15, 4, c0, 0);
	CP15_PRINT(c15, 5, c5, 2);
	CP15_PRINT(c15, 5, c6, 2);
	CP15_PRINT(c15, 5, c7, 2);
}


#endif

void gdma0_transfer(u32 source, u32 dest, u32 size)
{
	u32 control = 0x2201;   /* Singel Mode 4 byte 4 times  burst of 16 bytes. */

	(*(volatile unsigned int*)(GDMA0_BASE + 0x04) ) = source;
	(*(volatile unsigned int*)(GDMA0_BASE + 0x08) ) = dest;
	(*(volatile unsigned int*)(GDMA0_BASE + 0x0C) ) = size;

	(*(volatile unsigned int*)(GDMA0_BASE + 0x00) ) = control;
	control |= 0x10000;
	(*(volatile unsigned int*)(GDMA0_BASE + 0x00) ) = control;

/*   while ((*(volatile unsigned int*)(GDMA0_BASE + 0x00) ) & (u32)0x10000 ); */

}


void gdma1_transfer(u32 source, u32 dest, u32 size)
{
	u32 control = 0x2201;   /* Singel Mode 4 byte 4 times  burst of 16 bytes. */

	(*(volatile unsigned int*)(GDMA0_BASE + 0x24) ) = source;
	(*(volatile unsigned int*)(GDMA0_BASE + 0x28) ) = dest;
	(*(volatile unsigned int*)(GDMA0_BASE + 0x2C) ) = size;

	(*(volatile unsigned int*)(GDMA0_BASE + 0x20) ) = control;
	control |= 0x10000;
	(*(volatile unsigned int*)(GDMA0_BASE + 0x20) ) = control;

/*   while ((*(volatile unsigned int*)(GDMA0_BASE + 0x20) ) & (u32)0x10000 ); */

}


void gdma_mem_test(u32 src_addr, u32 dst_addr, u32 chunk_size)
{

	memtest_pattern++;
	printf("memtest_pattern %d\n", memtest_pattern);
	printf("src_addr 0x%x\n", src_addr);
	printf("dst_addr 0x%x\n", dst_addr);
	printf("chunk_size 0x%x\n", chunk_size);
	set_memory(src_addr, chunk_size, memtest_pattern);

	memtest_pattern++;
	printf("memtest_pattern %d\n", memtest_pattern);
	printf("src_addr 0x%x\n", src_addr + 0x100000);
	printf("dst_addr 0x%x\n", dst_addr + 0x100000);
	printf("chunk_size 0x%x\n", chunk_size);
	set_memory(src_addr + 0x100000, chunk_size, memtest_pattern);

	memtest_pattern++;
	printf("memtest_pattern %d\n", memtest_pattern);
	printf("src_addr 0x%x\n", src_addr + 0x200000);
	printf("dst_addr 0x%x\n", dst_addr + 0x200000);
	printf("chunk_size 0x%x\n", chunk_size);
	set_memory(src_addr + 0x200000, chunk_size, memtest_pattern);

	gdma0_transfer(src_addr, dst_addr, chunk_size);
	gdma1_transfer(src_addr + 0x100000, dst_addr + 0x100000, chunk_size);
	my_memcpy(src_addr + 0x200000, dst_addr + 0x200000, chunk_size);

	while ((*(volatile unsigned int*)(GDMA0_BASE + 0x00) ) & (u32)0x10000 );   /* GDMA0 */
	while ((*(volatile unsigned int*)(GDMA0_BASE + 0x20) ) & (u32)0x10000 );   /* GDMA1 */

	if(validate_memory(dst_addr + 0x200000, chunk_size, memtest_pattern))
		puts("mem_mem_test failed\n");
	else
		puts("mem_mem_test success\n");

	if(validate_memory(dst_addr + 0x100000, chunk_size, --memtest_pattern))
		puts("gdma1_mem_test failed\n");
	else
		puts("gdma1_mem_test success\n");

	if(validate_memory(dst_addr, chunk_size, --memtest_pattern))
		puts("gdma0_mem_test failed\n");
	else
		puts("gdma0_mem_test success\n");
}


static int parse_argv(const char *);

/* Trego- We use function at arch/arm/cpu/armv7/cache_v7.c */
void __weak invalidate_icache_all(void)
{
	/* please define arch specific invalidate_icache_all */
	puts("No arch specific invalidate_icache_all available!\n");
}

static int do_icache(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	switch (argc) {
	case 2:			/* on / off	*/
		switch (parse_argv(argv[1])) {
		case 0:
			icache_disable();
			break;
		case 1:
			icache_enable();
			break;
		case 2:
			invalidate_icache_all();
			break;
		}
		break;
	case 1:			/* get status */
		printf("Instruction Cache is %s\n",
			icache_status() ? "ON" : "OFF");
		return 0;
	default:
		return CMD_RET_USAGE;
	}
	return 0;
}

/* Trego- We use function at arch/arm/lib/cache.c */
void __weak flush_dcache_all(void)
{
	puts("No arch specific flush_dcache_all available!\n");
	/* please define arch specific flush_dcache_all */
}

static int do_dcache(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	switch (argc) {
	case 2:			/* on / off */
		switch (parse_argv(argv[1])) {
		case 0:
			dcache_disable();
			break;
		case 1:
			dcache_enable();
			break;
		case 2:
			flush_dcache_all();
			break;
		}
		break;
	case 1:			/* get status */
		printf("Data (write-back) Cache is %s\n",
			dcache_status() ? "ON" : "OFF");
		return 0;
	default:
		return CMD_RET_USAGE;
	}
	return 0;
}

static int parse_argv(const char *s)
{
	if (strcmp(s, "flush") == 0)
		return 2;
	else if (strcmp(s, "on") == 0)
		return 1;
	else if (strcmp(s, "off") == 0)
		return 0;

	return -1;
}

int do_icache_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	printf("I-Cache test not implemented \n");

	return 0;
}

int do_dcache_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
		test_basic_copy();
	test_flush_d_range();
	test_inval_d_range();
	test_flush_d_all();
	test_inval_d_all();

	return 0;
}

int do_cache_perf_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	test_basic_performance();

	return 0;
}

int do_gdma_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 src, dst, size;


	if (argc > 1)
		src = simple_strtoul(argv[1], NULL, 16);
	else
		src = memtest_src_addr;

	if (argc > 2)
		dst = simple_strtoul(argv[2], NULL, 16);
	else
		dst = memtest_dst_addr;

	if (argc > 3)
		size = (ulong)simple_strtoul(argv[3], NULL, 16);
	else
		size = memtest_chunk_size;

	gdma_mem_test(src, dst, size);

	return 0;
}

#if defined (NPCM750)               /* Only Poleg */
int do_l2_cache_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	l2_cache_test();

	return 0;
}

int do_cp15_regs(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	cp15_regs();

	return 0;
}
#endif
U_BOOT_CMD(
	icache,   2,   1,     do_icache,
	"enable or disable instruction cache",
	"[on, off, flush]\n"
	"    - enable, disable, or flush instruction cache"
);

U_BOOT_CMD(
	dcache,   2,   1,     do_dcache,
	"enable or disable data cache",
	"[on, off, flush]\n"
	"    - enable, disable, or flush data (writethrough) cache"
);

U_BOOT_CMD(
	dcache_test,   1,   1,     do_dcache_test,
	"D-Cache test",
	""
);

U_BOOT_CMD(
	icache_test,   1,   1,     do_icache_test,
	"I-Cache test",
	""
);

U_BOOT_CMD(
	cache_perf_test,   1,   1,     do_cache_perf_test,
	"Cache performance test",
	""
);

U_BOOT_CMD(
	gdma_test,   4,   1,     do_gdma_test,
	"GDMA test",
	"<source> <destination> <size>"
);
#if defined (NPCM750)               /* Only Poleg */
U_BOOT_CMD(
	l2_cache_test,   1,   1,     do_l2_cache_test,
	"L2-Cache test",
	""
);

U_BOOT_CMD(
	cp15_regs,   1,   1,     do_cp15_regs,
	"CP15-REG's read",
	""
);
#endif