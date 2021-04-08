// SPDX-License-Identifier: GPL-2.0+
/*
 *  DDR ECC test commands
 *
 * Copyright (C) 2012-2017 Texas Instruments Incorporated, <www.ti.com>
 */

#include <asm/io.h>
#include <asm/cache.h>
#include <common.h>
#include <command.h>
#include <console.h>

DECLARE_GLOBAL_DATA_PTR;

#define MEMORY_CONTROL_BASE  0xF0824000UL


#define MC_ECC_ENABLE                (MEMORY_CONTROL_BASE + 0x16C)
#define MC_INT_STATUS_ECC            (MEMORY_CONTROL_BASE + 0x228)
#define MC_INT_STATUS_TIMEOUT        (MEMORY_CONTROL_BASE + 0x224)
#define MC_INT_STATUS_USERIF         (MEMORY_CONTROL_BASE + 0x145c)
#define MC_INT_STATUS_LOWPOWER       (MEMORY_CONTROL_BASE + 0x228)
#define MC_INT_STATUS_TRAINING       (MEMORY_CONTROL_BASE + 0x230)
#define MC_INT_STATUS_BIST           (MEMORY_CONTROL_BASE + 0x234)
#define MC_INT_STATUS_DFI            (MEMORY_CONTROL_BASE + 0x234)
#define MC_INT_STATUS_INIT           (MEMORY_CONTROL_BASE + 0x238)
#define MC_INT_STATUS_MISC           (MEMORY_CONTROL_BASE + 0x238)
#define MC_INT_STATUS_MODE           (MEMORY_CONTROL_BASE + 0x23c)
#define MC_INT_STATUS_PARITY         (MEMORY_CONTROL_BASE + 0x23c)
#define MC_INT_ACK_ECC               (MEMORY_CONTROL_BASE + 0x244)
#define MC_INT_MASK_ECC              (MEMORY_CONTROL_BASE + 0x260)
#define MC_INT_STATUS_MASTER         (MEMORY_CONTROL_BASE + 0x21c)


#define ECC_ENABLE      (BIT(17) | BIT(16))


#define ECC_CORRECTABLE_SCRUB_READ    BIT(8)
#define ECC_TRIGGRED_SCRUB_COMPLETED  BIT(7)
#define ECC_WRITEBACK_FAIL_EVENT      BIT(6)
#define ECC_MLT_UNCORRECTABLE_EVENT   BIT(3)
#define ECC_UNCORRECTABLE_EVENT       BIT(2)
#define ECC_MLT_CORRECTABLE_EVENT     BIT(1)
#define ECC_CORRECTABLE_EVENT         BIT(0)

#define DDR_TEST_BURST_SIZE	1024

static inline bool is_ecc_enabled(void)
{
	u32 ecc_ctrl = __raw_readl(MC_ECC_ENABLE);

	return (ecc_ctrl && ECC_ENABLE) ? true : false;
}

static int ddr_memory_test(u32 start_address, u32 end_address, int quick)
{
	u32 index_start, value, index;

	index_start = start_address;

	while (1) {
		/* Write a pattern */
		for (index = index_start;
				index < index_start + DDR_TEST_BURST_SIZE;
				index += 4)
			__raw_writel(index, (uintptr_t)index);

		/* Read and check the pattern */
		for (index = index_start;
				index < index_start + DDR_TEST_BURST_SIZE;
				index += 4) {
			value = __raw_readl((uintptr_t)index);
			if (value != index) {
				printf("ddr_memory_test: Failed at address index = 0x%x value = 0x%x *(index) = 0x%x\n",
				       index, value, __raw_readl((uintptr_t)index));

				return -1;
			}
		}

		index_start += DDR_TEST_BURST_SIZE;
		if (index_start >= end_address)
			break;

		if (quick)
			continue;

		/* Write a pattern for complementary values */
		for (index = index_start;
		     index < index_start + DDR_TEST_BURST_SIZE;
		     index += 4)
			__raw_writel((u32)~index, (uintptr_t)index);

		/* Read and check the pattern */
		for (index = index_start;
		     index < index_start + DDR_TEST_BURST_SIZE;
		     index += 4) {
			value = __raw_readl((uintptr_t)index);
			if (value != ~index) {
				printf("ddr_memory_test: Failed at address index = 0x%x value = 0x%x *(index) = 0x%x\n",
				       index, value, __raw_readl((uintptr_t)index));

				return -1;
			}
		}

		index_start += DDR_TEST_BURST_SIZE;
		if (index_start >= end_address)
			break;

		/* Write a pattern */
		for (index = index_start;
		     index < index_start + DDR_TEST_BURST_SIZE;
		     index += 2)
			__raw_writew((u16)index, (uintptr_t)index);

		/* Read and check the pattern */
		for (index = index_start;
		     index < index_start + DDR_TEST_BURST_SIZE;
		     index += 2) {
			value = __raw_readw((uintptr_t)index);
			if (value != (u16)index) {
				printf("ddr_memory_test: Failed at address index = 0x%x value = 0x%x *(index) = 0x%x\n",
				       index, value, __raw_readw((uintptr_t)index));

				return -1;
			}
		}

		index_start += DDR_TEST_BURST_SIZE;
		if (index_start >= end_address)
			break;

		/* Write a pattern */
		for (index = index_start;
		     index < index_start + DDR_TEST_BURST_SIZE;
		     index += 1)
			__raw_writeb((u8)index, (uintptr_t)index);

		/* Read and check the pattern */
		for (index = index_start;
		     index < index_start + DDR_TEST_BURST_SIZE;
		     index += 1) {
			value = __raw_readb((uintptr_t)index);
			if (value != (u8)index) {
				printf("ddr_memory_test: Failed at address index = 0x%x value = 0x%x *(index) = 0x%x\n",
				       index, value, __raw_readb((uintptr_t)index));

				return -1;
			}
		}

		index_start += DDR_TEST_BURST_SIZE;
		if (index_start >= end_address)
			break;
	}

	puts("ddr memory test PASSED!\n");
	return 0;
}

static int ddr_memory_compare(u32 address1, u32 address2, u32 size)
{
	u32 index, value, index2, value2;

	for (index = address1, index2 = address2;
	     index < address1 + size;
	     index += 4, index2 += 4) {
		value = __raw_readl((uintptr_t)index);
		value2 = __raw_readl((uintptr_t)index2);

		if (value != value2) {
			printf("ddr_memory_test: Compare failed at address = 0x%x value = 0x%x, address2 = 0x%x value2 = 0x%x\n",
			       index, value, index2, value2);

			return -1;
		}
	}

	puts("ddr memory compare PASSED!\n");
	return 0;
}

static inline void ddr_check_status_ecc(void)
{
	u32 int_status_ecc = 0;
	
	if (!is_ecc_enabled())
		puts("\nECC not enabled. Please Enable ECC and try again\n");

	int_status_ecc = __raw_readl(MC_INT_STATUS_ECC);

	if (!(int_status_ecc & 0xFFFF))
		puts("\nint_status_ecc is cleared - no errors found.\n");		

	if (int_status_ecc & ECC_CORRECTABLE_EVENT)
		puts("\nint_status_ecc_bit0: A correctable ECC event has been detected.\n");

	if (int_status_ecc & ECC_MLT_CORRECTABLE_EVENT)
		puts("\nint_status_ecc_bit1: Multiple correctable ECC events have been detected.\n");

	if (int_status_ecc & ECC_UNCORRECTABLE_EVENT)
		puts("\nint_status_ecc_bit2: A uncorrectable ECC event has been detected.\n");

	if (int_status_ecc & ECC_MLT_UNCORRECTABLE_EVENT)
		puts("\nint_status_ecc_bit3: Multiple uncorrectable ECC events have been detected.\n");

	if (int_status_ecc & ECC_WRITEBACK_FAIL_EVENT)
		puts("\nint_status_ecc_bit6: One or more ECC writeback commands could not be executed.\n");

	if (int_status_ecc & ECC_TRIGGRED_SCRUB_COMPLETED)
		puts("\nint_status_ecc_bit7: The scrub operation triggered by setting param ecc scrub start has completed.\n");
		
	if (int_status_ecc & ECC_CORRECTABLE_SCRUB_READ)
		puts("\nint_status_ecc_bit8: An ECC correctable error has been detected in a scrubbing read operation.\n");
		
	printf("int_status_ecc = 0x%x\n", int_status_ecc);
}

static inline void ddr_check_status_master(void)
{
	u32 int_status_master = 0;
	
	int_status_master = __raw_readl(MC_INT_STATUS_MASTER);

	if (!(int_status_master & 0xFFFF))
		puts("\nint_status_master is cleared - no errors found.\n");
			
	printf("int_status_master = 0x%x\n", int_status_master);
	
	if (int_status_master & BIT(0))
	{
		puts("\nint_status_master_bit0: Timeout group event.\n");
		printf("\tMC_INT_STATUS_TIMEOUT: 0x%x\n", (__raw_readl(MC_INT_STATUS_TIMEOUT)));
	}

	if (int_status_master & BIT(1))
	{
		puts("\nint_status_master_bit1: User Interface group event.\n");
		printf("\tMC_INT_STATUS_USERIF: 0x%x\n", __raw_readl(MC_INT_STATUS_USERIF));
	}

	if (int_status_master & BIT(2))
	{
		puts("\nint_status_master_bit2: ECC group event.\n");
		printf("\tMC_INT_STATUS_ECC: 0x%x\n", __raw_readl(MC_INT_STATUS_ECC));
	}

	if (int_status_master & BIT(3))
	{
		puts("\nint_status_master_bit3: Low Power group event.\n");
		printf("\tMC_INT_STATUS_LOWPOWER: 0x%x\n", __raw_readl(MC_INT_STATUS_LOWPOWER));
	}

	if (int_status_master & BIT(4))
	{
		puts("\nint_status_master_bit4: Port Timeout group event.\n");
	}

	if (int_status_master & BIT(5))
	{
		puts("\nint_status_master_bit5: Read FIFO Timeout group event.\n");
	}

	if (int_status_master & BIT(6))
	{
		puts("\nint_status_master_bit6: Training group event.\n");
		printf("\tMC_INT_STATUS_TRAINING: 0x%x\n", __raw_readl(MC_INT_STATUS_TRAINING));
	}

	if (int_status_master & BIT(7))
	{
		puts("\nint_status_master_bit7: BIST group event.\n");
		printf("\tMC_INT_STATUS_BIST: 0x%x\n", __raw_readl(MC_INT_STATUS_BIST));
	}

	if (int_status_master & BIT(8))
	{
		puts("\nint_status_master_bit8: CRC group event.\n");
	}

	if (int_status_master & BIT(9))
	{
		puts("\nint_status_master_bit9: DFI group event.\n");
		printf("\tMC_INT_STATUS_DFI: 0x%x\n", __raw_readl(MC_INT_STATUS_DFI));
	}

	if (int_status_master & BIT(10))
	{
		puts("\nint_status_master_bit10: DIMM group event.\n");
	}

	if (int_status_master & BIT(11))
	{
		puts("\nint_status_master_bit11: Freq group event.\n");
	}

	if (int_status_master & BIT(12))
	{
		puts("\nint_status_master_bit12: Init group event.\n");
		printf("\tMC_INT_STATUS_INIT: 0x%x\n", __raw_readl(MC_INT_STATUS_INIT));
	}

	if (int_status_master & BIT(13))
	{
		puts("\nint_status_master_bit13: Misc group event.\n");
		printf("\tMC_INT_STATUS_MISC: 0x%x\n", __raw_readl(MC_INT_STATUS_MISC));
	}

	if (int_status_master & BIT(14))
	{
		puts("\nint_status_master_bit14: Mode group event.\n");
		printf("\tMC_INT_STATUS_MODE: 0x%x\n", __raw_readl(MC_INT_STATUS_MODE));
	}

	if (int_status_master & BIT(15))
	{
		puts("\nint_status_master_bit15: Parity group event.\n");
		printf("\tMC_INT_STATUS_PARITY: 0x%x\n", __raw_readl(MC_INT_STATUS_PARITY));
	}
}

static int ddr_memory_ecc_err(u32 addr, u32 ecc_err)
{
	u32 val1, val2;


	dcache_disable();
	invalidate_dcache_all();

	val1 = __raw_readl((uintptr_t)addr);
	val2 = val1 ^ ecc_err;
	__raw_writel(0 , MC_ECC_ENABLE);	
	__raw_writel(val2, (uintptr_t)addr);
	
	__raw_writel(ECC_ENABLE, MC_ECC_ENABLE);

	val1 = __raw_readl((uintptr_t)addr);

	ddr_check_status_ecc();

	enable_caches();

	return 0;
}

static int is_addr_valid(u32 addr)
{
	return 1;
}



static int do_ddr(cmd_tbl_t *cmdtp,
		       int flag, int argc, char * const argv[])
{
	u32 start_addr, end_addr, size, ecc_err;
	
	if ((argc == 2) && (strncmp(argv[1], "status_ecc", 11) == 0)) {
		ddr_check_status_ecc();
		return 0;
	}
	if ((argc == 2) && (strncmp(argv[1], "status_master", 14) == 0)) {
		ddr_check_status_master();
		return 0;
	}
	if ((argc == 4) && (strncmp(argv[1], "ecc_err", 8) == 0)) {
		if (!is_ecc_enabled()) {
			puts("ECC not enabled. Please Enable ECC any try again\n");
			return CMD_RET_FAILURE;
		}

		start_addr = simple_strtoul(argv[2], NULL, 16);
		ecc_err = simple_strtoul(argv[3], NULL, 16);

		if (!is_addr_valid(start_addr)) {
			puts("Invalid address. Please enter ECC supported address!\n");
			return CMD_RET_FAILURE;
		}

		ddr_memory_ecc_err(start_addr, ecc_err);
		return 0;
	}

	if (!(((argc == 4) && (strncmp(argv[1], "test", 5) == 0)) ||
	      ((argc == 5) && (strncmp(argv[1], "compare", 8) == 0))))
		return cmd_usage(cmdtp);

	start_addr = simple_strtoul(argv[2], NULL, 16);
	end_addr = simple_strtoul(argv[3], NULL, 16);

	if ((start_addr < CONFIG_SYS_SDRAM_BASE) ||
	    (start_addr > (CONFIG_SYS_SDRAM_BASE +
	     get_effective_memsize() - 1)) ||
	    (end_addr < CONFIG_SYS_SDRAM_BASE) ||
	    (end_addr > (CONFIG_SYS_SDRAM_BASE +
	     get_effective_memsize() - 1)) || (start_addr >= end_addr)) {
		puts("Invalid start or end address!\n");
		return cmd_usage(cmdtp);
	}

	puts("Please wait ...\n");
	if (argc == 5) {
		size = simple_strtoul(argv[4], NULL, 16);
		ddr_memory_compare(start_addr, end_addr, size);
	} else {
		ddr_memory_test(start_addr, end_addr, 0);
	}

	return 0;
}

U_BOOT_CMD(ddr,	5, 1, do_ddr,
	   "DDR/ECC test tool",
	   "test <start_addr in hex> <end_addr in hex> - test DDR from start\n"
	   "	address to end address\n"
	   "ddr compare <start_addr in hex> <end_addr in hex> <size in hex> -\n"
	   "	compare DDR data of (size) bytes from start address to end\n"
	   "	address\n"
	   "ddr ecc_err <addr in hex> <bit_err in hex> - generate bit errors\n"
	   "	in DDR data at <addr>, the command will read a 32-bit data\n"
	   "	from <addr>, and write (data ^ bit_err) back to <addr>\n"
	   "ddr status_ecc - read from int_status_ecc register at MC \n"	   
	   "ddr status_master - read from int_status_master register at MC\n"	   
);
