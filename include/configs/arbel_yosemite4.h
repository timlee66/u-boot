/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2024 Nuvoton Technology Corp.
 */

#ifndef __CONFIG_ARBEL_YOSEMITE4_H
#define __CONFIG_ARBEL_YOSEMITE4_H

#define CFG_SYS_SDRAM_BASE		0x0
#define CFG_SYS_BOOTMAPSZ		(192 << 20)
#define CFG_SYS_BOOTM_LEN		(20 << 20)
#define CFG_SYS_INIT_RAM_ADDR	CFG_SYS_SDRAM_BASE
#define CFG_SYS_INIT_RAM_SIZE	0x8000
#define CFG_SYS_BAUDRATE_TABLE	{ 9600, 14400, 19200, 38400, 57600, 115200, 230400, \
				  380400, 460800, 921600 }

#define SPI_FLASH_REGION_SIZE		0x08000000	/* 128MB */
#define SPI1_FLASH_REGION_SIZE		0x01000000	/* 16MB */
#define SPI0_BASE_ADDR			0x80000000
#define SPI0_END_ADDR			0x8FFFFFFF
#define SPI1_BASE_ADDR			0x90000000
#define SPI1_END_ADDR			0x93FFFFFF
#define SPI3_BASE_ADDR			0xA0000000
#define SPI3_END_ADDR			0xBFFFFFFF

/* Default environemnt variables */
#define CFG_EXTRA_ENV_SETTINGS \
		"romboot=echo Booting Kernel from flash at 0x${uimage_flash_addr}; " \
		"echo Using bootargs: ${bootargs};bootm ${uimage_flash_addr}\0" \
		"earlycon=uart8250,mmio32,0xf0000000\0" \
		"console=ttyS4,57600n8\0" \
		"common_bootargs=setenv bootargs earlycon=${earlycon} root=/dev/ram " \
		"console=${console} ramdisk_size=48000 " \
		"mtdparts=${mtdparts}\0" \
		"getwdc=setexpr.l wdc *0xf0800060 \\\\& 0x00200000\0" \
		"selectbootcs=if itest.l ${wdc} != 0x200000; then setenv uimage_flash_addr 80300000; setenv bootspi 0; setenv backupspi 1; else setenv uimage_flash_addr 88300000; setenv bootspi 1; setenv backupspi 0; fi\0" \
		"selectmtdname=if itest.l ${wdc} != 0x200000; then setenv bootsuffix \"\"; setenv backupsuffix \"2\"; else setenv bootsuffix \"2\"; setenv backupsuffix \"\"; fi\0" \
		"setmtdparts=setenv mtdparts \"spi0.${bootspi}:128M@0(bmc${bootsuffix}),2816K@0(u-boot),256K@0x2C0000(u-boot-env),6M@0x300000(kernel),40M@0x900000(rofs),15M@0x3100000(rwfs);spi0.${backupspi}:128M@0(bmc${backupsuffix})\"\0" \
		"\0"
#endif

