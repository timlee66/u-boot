/*
 * Copyright (c) 2016 Nuvoton Technology Corp.
 *
 * Configuration settings for the NUVOTON POLEG board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ARBEL_H
#define __CONFIG_ARBEL_H

#undef  CONFIG_USE_IRQ

//#define DEBUG

//#define _NPCM850_EXT_SD_  1

#define GMAC2_RGMII


#define CONFIG_GICV2
#define GICD_BASE			(0xDFFF9000)
#define GICC_BASE			(0xDFFFA000)


#define CONFIG_MACH_TYPE		MACH_TYPE_NPCMX50

#define CPU_RELEASE_ADDR		secondary_boot_func

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG

#ifdef  CONFIG_SYS_PROMPT
#undef  CONFIG_SYS_PROMPT
#define CONFIG_SYS_PROMPT               "U-Boot>"
#endif

#define CONFIG_ENV_SIZE                  0x10000
#define CONFIG_ENV_OFFSET               (0x100000)
#define CONFIG_ENV_SECT_SIZE            0x1000

#define CONFIG_SF_DEFAULT_BUS		0
#define CONFIG_SF_DEFAULT_CS		0
#define CONFIG_ENV_ADDR                 (0x80000000 + CONFIG_ENV_OFFSET)

#define CONFIG_SYS_MALLOC_LEN           (CONFIG_ENV_SIZE + 2048*1024)

#define CONFIG_SYS_MAXARGS              32
#define CONFIG_SYS_CBSIZE               256
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_PROMPT_HUSH_PS2	    "> "

#define CONFIG_SYS_BOOTM_LEN            (20 << 20)
#define CONFIG_SYS_BOOTMAPSZ            (20 << 20)
#define CONFIG_SYS_LOAD_ADDR            0x8000
#define CONFIG_SYS_SDRAM_BASE           0x0
#define CONFIG_SYS_SRAM_BASE            0xC0008000  /* RAM3*/
#define CONFIG_SYS_SRAM_SIZE            (16 << 10)
#define CONFIG_SYS_INIT_SP_ADDR         (0x00008000 - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_MONITOR_LEN          (256 << 10) /* Reserve 256 kB for Monitor   */
#define CONFIG_SYS_MONITOR_BASE	        CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x08000000)

#define CONFIG_STANDALONE_LOAD_ADDR     0x10000000

#define SPI_FLASH_BASE_ADDR				0x80000000
#define SPI_FLASH_REGION_SIZE			0x08000000	/* 128MB */
#define SPI1_FLASH_REGION_SIZE			0x01000000	/* 16MB */
#define SPI0_BASE_ADDR					0x80000000
#define SPI0_END_ADDR					0x8FFFFFFF
#define SPI1_BASE_ADDR					0x90000000
#define SPI1_END_ADDR					0x93FFFFFF
#define SPI3_BASE_ADDR					0xA0000000
#define SPI3_END_ADDR					0xBFFFFFFF

#define PHYS_SDRAM_1			        CONFIG_SYS_SDRAM_BASE

#define CONFIG_BAUDRATE                 115200
#define CONFIG_SYS_BAUDRATE_TABLE       {115200, 57600, 38400}

#define CONFIG_SYS_HZ                   1000

#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_CPU_INIT
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	1

#define PCB_VER_ID0   78
#define PCB_VER_ID1   79

/* Default environemnt variables */
#if 0 //def CONFIG_NPCMX50_CORE0
/* CORE 0  */
/*tftp 0x8000 multi_core_arbel/u-boot-core0.bin; mw.l 0xF0800E7C 0x0; mw.l 0xF0800E78 0x8000; */
#define CONFIG_BOOTCOMMAND	\
	"run clear_results; setenv ipaddr 10.191.20.46; setenv serverip 10.191.20.47; setenv ethact gmac2;"	\
	"tftp 0x8000 multi_core_arbel/u-boot-core1.bin; cpu 1 release 0x8000 15; while itest.l *fffb000C -eq 0x0; do sleep 0.2; done;"	\
	"tftp 0x8000 multi_core_arbel/u-boot-core2.bin; cpu 2 release 0x8000 15; while itest.l *fffb000C -eq 0x1; do sleep 0.2; done;"	\
	"tftp 0x8000 multi_core_arbel/u-boot-core3.bin; cpu 3 release 0x8000 15; while itest.l *fffb000C -eq 0x2; do sleep 0.2; done;"	\
	"run core1_rgmii_stress"
#endif

#ifdef CONFIG_NPCMX50_CORE0
#define CONFIG_BOOTCOMMAND	\
	"run clear_results; usb start; "	\
	"fatload usb 0:1 0x8000 multi_core_arbel/u-boot-core1.bin; cpu 1 release 0x8000 15; while itest.l *fffb000C -eq 0x0; do sleep 10; done;"	\
	"fatload usb 0:1 0x8000 multi_core_arbel/u-boot-core2.bin; cpu 2 release 0x8000 15; while itest.l *fffb000C -eq 0x1; do sleep 10; done;"	\
	"fatload usb 0:1 0x8000 multi_core_arbel/u-boot-core3.bin; cpu 3 release 0x8000 15; while itest.l *fffb000C -eq 0x2; do sleep 10; done;"	\
	"run core0_mtest0_stress"
#endif

#ifdef CONFIG_NPCMX50_CORE1
/* CORE 1   */
//#define CONFIG_BOOTCOMMAND "while itest.l *fffb000C -ne 0x3; do sleep 0.2; done; run core0_sgmii_stress"
//#define CONFIG_BOOTCOMMAND "while itest.l *fffb000C -ne 0x3; do sleep 30; done; run core1_emmc_stress"
#define CONFIG_BOOTCOMMAND "while itest.l *fffb000C -ne 0x3; do sleep 30; done; run core1_mtest1_stress"
//#define CONFIG_BOOTCOMMAND "echo core1 done"
#endif

#ifdef CONFIG_NPCMX50_CORE2
/* CORE 2   */
//#define CONFIG_BOOTCOMMAND "while itest.l *fffb000C -ne 0x3; do sleep 30; done; run core2_flash3_stress"
#define CONFIG_BOOTCOMMAND "while itest.l *fffb000C -ne 0x3; do sleep 30; done; run core2_mtest2_stress"
//#define CONFIG_BOOTCOMMAND "echo core2 done"
#endif

#ifdef CONFIG_NPCMX50_CORE3
/* CORE 3   */
//#define CONFIG_BOOTCOMMAND "run core3_usbhd_stress"
//#define CONFIG_BOOTCOMMAND "run core3_usbh1_stress"
//#define CONFIG_BOOTCOMMAND "run core3_usbh2_stress"
//#define CONFIG_BOOTCOMMAND "setenv gpio_in 121; setenv gpio_in 122; run core3_gpio0_stress"
#define CONFIG_BOOTCOMMAND "run core3_mtest3_stress"
//#define CONFIG_BOOTCOMMAND "echo core3 done"
#endif

#if !defined (CONFIG_NPCMX50_CORE0) && !defined (CONFIG_NPCMX50_CORE1) && !defined (CONFIG_NPCMX50_CORE2) && !defined (CONFIG_NPCMX50_CORE3)
#define CONFIG_BOOTCOMMAND "run clear_results"
#endif

#define CONFIG_BITBANGMII_MULTI
#define CONFIG_SERVERIP                 10.191.20.49
#define CONFIG_IPADDR                   10.191.20.48
#define CONFIG_NETMASK                  255.255.255.0
#define CONFIG_ETHADDR                  "00:00:F7:A0:FF:FC"
#define CONFIG_HAS_ETH1
#define CONFIG_ETH1ADDR                 "00:00:F7:A0:FF:FD"
#define CONFIG_HAS_ETH2
#define CONFIG_ETH2ADDR                 "00:00:F7:A0:FF:FE"
#define CONFIG_HAS_ETH3
#define CONFIG_ETH3ADDR                 "00:00:F7:A0:FF:FF"

#define CONFIG_EXTRA_ENV_SETTINGS  \
		"stdin=serial\0"   \
		"stdout=serial\0"   \
		"stderr=serial\0"    \
		"ethact=gmac1\0"   \
		"autostart=no\0"   \
		"ethaddr=00:00:F7:A0:00:FC\0"    \
		"eth1addr=00:00:F7:A0:00:FD\0"   \
		"eth2addr=00:00:F7:A0:00:FE\0"    \
		"eth3addr=00:00:F7:A0:00:FF\0"    \
		"gatewayip=10.191.20.254\0"    \
		"serverip=10.191.20.49\0"    \
		"ipaddr=10.191.20.48\0"    \
		"physpeed=1000\0"    \
		"gmacloopb=0\0"    \
        "gpio0_passcount=0x0\0"    \
        "gpio0_failcount=0x0\0"    \
        "flash0_passcount=0x0\0"    \
        "flash0_failcount=0x0\0"    \
        "flash1_passcount=0x0\0"    \
        "flash1_failcount=0x0\0"    \
        "flash3_passcount=0x0\0"    \
        "flash3_failcount=0x0\0"    \
        "flashx_passcount=0x0\0"    \
        "flashx_failcount=0x0\0"    \
        "mtest0_passcount=0x0\0"    \
        "mtest0_failcount=0x0\0"    \
        "mtest1_passcount=0x0\0"    \
        "mtest1_failcount=0x0\0"    \
        "mtest2_passcount=0x0\0"    \
        "mtest2_failcount=0x0\0"    \
        "mtest3_passcount=0x0\0"    \
        "mtest3_failcount=0x0\0"    \
        "sgmii_passcount=0x0\0"    \
        "sgmii_failcount=0x0\0"    \
        "rgmii_passcount=0x0\0"    \
        "rgmii_failcount=0x0\0"    \
        "usbh1_passcount=0x0\0"    \
        "usbh1_failcount=0x0\0"    \
        "usbh2_passcount=0x0\0"    \
        "usbh2_failcount=0x0\0"    \
        "emmc_passcount=0x0\0"    \
        "emmc_failcount=0x0\0"    \
        "count=0x0\0"    \
        "gpio_in=28\0"    \
        "gpio_out=29\0"    \
        "flash0_cs=0\0"    \
        "flash1_cs=0\0"    \
        "flash3_cs=0\0"    \
        "flashx_cs=0\0"    \
        "mtest3_clr_mem=mw.b 0x1000000 0x00 0x800000\0"    \
        "mtest3_check=if mtest 0x4000000 0x4800000 0x89abcdef 0x100; then setexpr mtest3_passcount ${mtest3_passcount} + 0x1; mw.l 0xC0008130 ${mtest3_passcount}; else setexpr mtest3_failcount ${mtest3_failcount} + 0x1; mw.l 0xC0008134 ${mtest3_failcount}; fi\0"    \
        "mtest3_stress=setexpr mtest3_count 0x0; while test 10 -ne $mtest3_count; do run mtest3_check; run mtest3_clr_mem; setexpr mtest3_count ${mtest3_count} + 0x1; done\0"    \
        "mtest3_result=if itest.l *fffb0134 != 0; then echo ***MTEST3-FAIL!***; else echo ***MTEST3-PASS!***; fi; echo MTEST3 pass count!; md.l 0xC0008130 1; echo MTEST3 fail count!; md.l 0xC0008134 1\0"    \
        "mtest2_clr_mem=mw.b 0x3000000 0x00 0x800000\0"    \
        "mtest2_check=if mtest 0x3000000 0x3800000 0xfedcba98 0x100; then setexpr mtest2_passcount ${mtest2_passcount} + 0x1; mw.l 0xC0008120 ${mtest2_passcount}; else setexpr mtest2_failcount ${mtest2_failcount} + 0x1; mw.l 0xC0008124 ${mtest2_failcount}; fi\0"    \
        "mtest2_stress=setexpr mtest2_count 0x0; while test 10 -ne $mtest2_count; do run mtest2_check; run mtest2_clr_mem; setexpr mtest2_count ${mtest2_count} + 0x1; done\0"    \
        "mtest2_result=if itest.l *fffb0124 != 0; then echo ***MTEST2-FAIL!***; else echo ***MTEST2-PASS!***; fi; echo MTEST2 pass count!; md.l 0xC0008120 1; echo MTEST2 fail count!; md.l 0xC0008124 1\0"    \
        "mtest1_clr_mem=mw.b 0x2000000 0x00 0x800000\0"    \
        "mtest1_check=if mtest 0x2000000 0x2800000 0x76543210 0x100; then setexpr mtest1_passcount ${mtest1_passcount} + 0x1; mw.l 0xC0008110 ${mtest1_passcount}; else setexpr mtest1_failcount ${mtest1_failcount} + 0x1; mw.l 0xC0008114 ${mtest1_failcount}; fi\0"    \
        "mtest1_stress=setexpr mtest1_count 0x0; while test 10 -ne $mtest1_count; do run mtest1_check; run mtest1_clr_mem; setexpr mtest1_count ${mtest1_count} + 0x1; done\0"    \
        "mtest1_result=if itest.l *fffb0114 != 0; then echo ***MTEST1-FAIL!***; else echo ***MTEST1-PASS!***; fi; echo MTEST1 pass count!; md.l 0xC0008110 1; echo MTEST1 fail count!; md.l 0xC0008114 1\0"    \
        "mtest0_clr_mem=mw.b 0x1000000 0x00 0x800000\0"    \
        "mtest0_check=if mtest 0x1000000 0x1800000 0x01234567 0x100; then setexpr mtest0_passcount ${mtest0_passcount} + 0x1; mw.l 0xC0008100 ${mtest0_passcount}; else setexpr mtest0_failcount ${mtest0_failcount} + 0x1; mw.l 0xC0008104 ${mtest0_failcount}; fi\0"    \
        "mtest0_stress=setexpr mtest0_count 0x0; while test 10 -ne $mtest0_count; do run mtest0_check; run mtest0_clr_mem; setexpr mtest0_count ${mtest0_count} + 0x1; done\0"    \
        "mtest0_result=if itest.l *fffb0104 != 0; then echo ***MTEST0-FAIL!***; else echo ***MTEST0-PASS!***; fi; echo MTEST0 pass count!; md.l 0xC0008100 1; echo MTEST0 fail count!; md.l 0xC0008104 1\0"    \
        "gpio0_check=if gpio input ${gpio_in}; then setenv gpioin_val 0; else setenv gpioin_val 1; fi; if test ${gpioin_val} = ${gpioout_val}; then setexpr gpio0_passcount ${gpio0_passcount} + 0x1; mw.l 0xC0008090 ${gpio0_passcount}; else setexpr gpio0_failcount ${gpio0_failcount} + 0x1; mw.l 0xC0008094 ${gpio0_failcount}; fi\0"    \
        "gpio0_stress=setexpr gpio0_count 0x0; while test 400 -ne $gpio0_count; do gpio toggle ${gpio_out}; setexpr gpioout_val 0x1 - ${gpioout_val}; run gpio0_check; setexpr gpio0_count ${gpio0_count} + 0x1; done\0"    \
        "gpio0_result=if itest.l *fffd0094 != 0; then echo ***GPIO0-FAIL!***; else echo ***GPIO0-PASS!***; fi; echo GPIO0 pass count!; md.l 0xC0008090 1; echo GPIO0 fail count!; md.l 0xC0008094 1\0"    \
        "flash0_clr_mem=mw.b 0xBB0000 0x00 0x20000; sf erase 0x300000 0x10000;\0"    \
        "flash0_check_blocks=if cmp.b ${flash0_addr} 0xBC0000 0x8000; then if itest.l *${flash0_addr} == 0xAAAAAAAA; then setexpr flash0_passcount ${flash0_passcount} + 0x1; mw.l 0xC0008080 ${flash0_passcount}; else setexpr flash0_failcount ${flash0_failcount} + 0x1; mw.l 0xC0008084 ${flash0_failcount}; fi; else setexpr flash0_failcount ${flash0_failcount} + 0x1; mw.l 0xC0008084 ${flash0_failcount}; fi; run flash0_clr_mem\0"    \
        "flash0_stress=setexpr flash0_count 0x0; setexpr flash0_offset ${flash0_cs} * 0x8000000; setexpr flash0_addr 0x80300000 + ${flash0_offset}; while test 20 -ne $flash0_count; do mw.b 0xBB0000 0xAA 0x8000; sf write 0xBB0000 0x300000 0x8000; sf read 0xBC0000 0x300000 0x8000; run flash0_check_blocks; setexpr flash0_count ${flash0_count} + 0x1; done\0"    \
        "flash0_result=if itest.l *fffb0084 != 0; then echo ***FLASH0:${flash0_cs}-FAIL!***; else echo ***FLASH0:${flash0_cs}-PASS!***; fi; echo FLASH0:${flash0_cs} pass count!; md.l 0xC0008080 1; echo FLASH0:${flash0_cs} fail count!; md.l 0xC0008084 1\0"    \
        "flash1_clr_mem=mw.b 0xB90000 0x00 0x20000; sf erase 0x0 0x10000;\0"    \
        "flash1_check_blocks=if cmp.b ${flash1_addr} 0xBA0000 0x8000; then if itest.l *${flash1_addr} == 0xAAAAAAAA; then setexpr flash1_passcount ${flash1_passcount} + 0x1; mw.l 0xC0008070 ${flash1_passcount}; else setexpr flash1_failcount ${flash1_failcount} + 0x1; mw.l 0xC0008074 ${flash1_failcount}; fi; else setexpr flash1_failcount ${flash1_failcount} + 0x1; mw.l 0xC0008074 ${flash1_failcount}; fi; run flash1_clr_mem\0"    \
        "flash1_stress=setexpr flash1_count 0x0; setexpr flash1_offset ${flash1_cs} * 0x1000000; setexpr flash1_addr 0x90000000 + ${flash1_offset}; while test 20 -ne $flash1_count; do mw.b 0xB90000 0xAA 0x8000; sf write 0xB90000 0x0 0x8000; sf read 0xBA0000 0x0 0x8000; run flash1_check_blocks; setexpr flash1_count ${flash1_count} + 0x1; done\0"    \
        "flash1_result=if itest.l *fffb0074 != 0; then echo ***FLASH1:${flash1_cs}-FAIL!***; else echo ***FLASH1:${flash1_cs}-PASS!***; fi; echo FLASH1:${flash1_cs} pass count!; md.l 0xC0008070 1; echo FLASH1:${flash1_cs} fail count!; md.l 0xC0008074 1\0"    \
        "flash3_clr_mem=mw.b 0xB70000 0x00 0x20000; sf erase 0x0 0x10000;\0"    \
        "flash3_check_blocks=if cmp.b ${flash3_addr} 0xB80000 0x8000; then if itest.l *${flash3_addr} == 0xAAAAAAAA; then setexpr flash3_passcount ${flash3_passcount} + 0x1; mw.l 0xC0008060 ${flash3_passcount}; else setexpr flash3_failcount ${flash3_failcount} + 0x1; mw.l 0xC0008064 ${flash3_failcount}; fi; else setexpr flash3_failcount ${flash3_failcount} + 0x1; mw.l 0xC0008064 ${flash3_failcount}; fi; run flash3_clr_mem\0"    \
        "flash3_stress=setexpr flash3_count 0x0; setexpr flash3_offset ${flash3_cs} * 0x8000000; setexpr flash3_addr 0xA0000000 + ${flash3_offset}; while test 20 -ne $flash3_count; do mw.b 0xB70000 0xAA 0x8000; sf write 0xB70000 0x0 0x8000; sf read 0xB80000 0x0 0x8000; run flash3_check_blocks; setexpr flash3_count ${flash3_count} + 0x1; done\0"    \
        "flash3_result=if itest.l *fffb0064 != 0; then echo ***FLASH3:${flash3_cs}-FAIL!***; else echo ***FLASH3:${flash3_cs}-PASS!***; fi; echo FLASH3:${flash3_cs} pass count!; md.l 0xC0008060 1; echo FLASH3:${flash3_cs} fail count!; md.l 0xC0008064 1\0"    \
        "flashx_clr_mem=mw.b 0xB50000 0x00 0x20000; sf erase 0x0 0x10000;\0"    \
        "flashx_check_blocks=if cmp.b ${flashx_addr} 0xB60000 0x8000; then if itest.l *${flashx_addr} == 0xAAAAAAAA; then setexpr flashx_passcount ${flashx_passcount} + 0x1; mw.l 0xC0008050 ${flashx_passcount}; else setexpr flashx_failcount ${flashx_failcount} + 0x1; mw.l 0xC0008054 ${flashx_failcount}; fi; else setexpr flashx_failcount ${flashx_failcount} + 0x1; mw.l 0xC0008054 ${flashx_failcount}; fi; run flashx_clr_mem\0"    \
        "flashx_stress=setexpr flashx_count 0x0; setexpr flashx_offset ${flashx_cs} * 0x1000000; setexpr flashx_addr 0xF8000000 + ${flashx_offset}; while test 20 -ne $flashx_count; do mw.b 0xB50000 0xAA 0x8000; sf write 0xB50000 0x0 0x8000; sf read 0xB60000 0x0 0x8000; run flashx_check_blocks; setexpr flashx_count ${flashx_count} + 0x1; done\0"    \
        "flashx_result=if itest.l *fffb0054 != 0; then echo ***FLASHX:${flashx_cs}-FAIL!***; else echo ***FLASHX:${flashx_cs}-PASS!***; fi; echo FLASHX:${flashx_cs} pass count!; md.l 0xC0008050 1; echo FLASHX:${flashx_cs} fail count!; md.l 0xC0008054 1\0"    \
        "rgmii_clr_mem=mw.b 0x900000 0x00 ${filesize}; mw.b 0xA00000 0xFF ${filesize}\0"    \
        "rgmii_check_image=if cmp.b 0x900000 0xA00000 ${filesize}; then setexpr rgmii_passcount ${rgmii_passcount} + 0x1; mw.l 0xC0008030 ${rgmii_passcount}; else setexpr rgmii_failcount ${rgmii_failcount} + 0x1; mw.l 0xC0008034 ${rgmii_failcount}; fi; run rgmii_clr_mem\0"    \
        "rgmii_stress=setexpr rgmii_count 0x0; while test 20 -ne $rgmii_count; do tftp 0x900000 u-boot.bin; tftp 0xA00000 u-boot.bin; run rgmii_check_image; setexpr rgmii_count ${rgmii_count} + 1; done\0"    \
        "rgmii_result=if itest.l *fffb0034 != 0; then echo ***RGMII-FAIL!***; else echo ***RGMII-PASS!***; fi; echo RGMII pass count!; md.l 0xC0008030 1; echo RGMII fail count!; md.l 0xC0008034 1\0"    \
        "sgmii_clr_mem=mw.b 0x700000 0x00 ${filesize}; mw.b 0x800000 0xFF ${filesize}\0"    \
        "sgmii_check_image=if cmp.b 0x700000 0x800000 ${filesize}; then setexpr sgmii_passcount ${sgmii_passcount} + 0x1; mw.l 0xC0008020 ${sgmii_passcount}; else setexpr sgmii_failcount ${sgmii_failcount} + 0x1; mw.l 0xC0008024 ${sgmii_failcount}; fi; run sgmii_clr_mem\0"    \
        "sgmii_stress=setexpr sgmii_count 0x0; while test 20 -ne $sgmii_count; do tftp 0x700000 u-boot.bin; tftp 0x800000 u-boot.bin; run sgmii_check_image; setexpr sgmii_count ${sgmii_count} + 1; done\0"    \
        "sgmii_result=if itest.l *fffb0024 != 0; then echo ***SGMII-FAIL!***; else echo ***SGMII-PASS!***; fi; echo SGMII pass count!; md.l 0xC0008020 1; echo SGMII fail count!; md.l 0xC0008024 1\0"    \
        "usbh1_clr_mem=mw.b 0x500000 0x00 0x2000\0"    \
        "usbh1_check_blocks=if cmp.b 0x500000 0x600000 0x2000; then setexpr usbh1_passcount ${usbh1_passcount} + 0x1; mw.l 0xC0008010 ${usbh1_passcount}; else setexpr usbh1_failcount ${usbh1_failcount} + 0x1; mw.l 0xC0008014 ${usbh1_failcount}; fi; run usbh1_clr_mem\0"    \
        "usbh1_stress=setexpr usbh1_count 0x0; while test 100 -ne $usbh1_count; do mw.b 0x500000 0xaa 0x2000; usb write 0x500000 0x50 0x10; usb read 0x600000 0x50 0x10; run usbh1_check_blocks; setexpr usbh1_count ${usbh1_count} + 0x1; done\0"    \
        "usbh1_result=if itest.l *fffb0014 != 0; then echo ***USBH1-FAIL!***; else echo ***USBH1-PASS!***; fi; echo USBH1 pass count!; md.l 0xC0008010 1; echo USBH1 fail count!; md.l 0xC0008014 1\0"    \
        "usbh2_clr_mem=mw.b 0xc00000 0x00 0x2000\0"    \
        "usbh2_check_blocks=if cmp.b 0xc00000 0xd00000 0x2000; then setexpr usbh2_passcount ${usbh2_passcount} + 0x1; mw.l 0xC0008140 ${usbh2_passcount}; else setexpr usbh2_failcount ${usbh2_failcount} + 0x1; mw.l 0xC0008144 ${usbh2_failcount}; fi; run usbh2_clr_mem\0"    \
        "usbh2_stress=setexpr usbh2_count 0x0; while test 100 -ne $usbh2_count; do mw.b 0xc00000 0xaa 0x2000; usb write 0xc00000 0x50 0x10; usb read 0xd00000 0x50 0x10; run usbh2_check_blocks; setexpr usbh2_count ${usbh2_count} + 0x1; done\0"    \
        "usbh2_result=if itest.l *fffb0144 != 0; then echo ***USBH2-FAIL!***; else echo ***USBH2-PASS!***; fi; echo USBH2 pass count!; md.l 0xC0008140 1; echo USBH2 fail count!; md.l 0xC0008144 1\0"    \
        "emmc_clr_mem=mw.b 0x300000 0x00 0x2000\0"    \
        "emmc_check_blocks=if cmp.b 0x300000 0x400000 0x2000; then setexpr emmc_passcount ${emmc_passcount} + 0x1; mw.l 0xC0008000 ${emmc_passcount}; else setexpr emmc_failcount ${emmc_failcount} + 0x1; mw.l 0xC0008004 ${emmc_failcount}; fi; run emmc_clr_mem\0"    \
        "emmc_stress=setexpr emmc_count 0x0; while test 200 -ne $emmc_count; do mw.b 0x300000 0xaa 0x2000; mmc write 0x300000 0x50 0x10; mmc read 0x400000 0x50 0x10; run emmc_check_blocks; setexpr emmc_count ${emmc_count} + 0x1; done\0"    \
        "emmc_result=if itest.l *fffb0004 != 0; then echo ***eMMC-FAIL!***; else echo ***eMMC-PASS!***; fi; echo eMMC pass count!; md.l 0xC0008000 1; echo eMMC fail count!; md.l 0xC0008004 1\0"    \
        "clear_results=setenv count 0; mw.b 0xC0008000 0x00 0x1000; setexpr flash0_passcount 0x0; setexpr flash0_failcount 0x0; setexpr flash1_passcount 0x0; setexpr flash1_failcount 0x0; setexpr flash3_passcount 0x0; setexpr flash3_failcount 0x0; setexpr flashx_passcount 0x0; setexpr flashx_failcount 0x0; setexpr sgmii_passcount 0x0; setexpr sgmii_failcount 0x0; setexpr rgmii_passcount 0x0; setexpr rgmii_failcount 0x0;  setexpr usbh1_passcount 0x0; setexpr usbh2_passcount 0x0; setexpr usbh1_failcount 0x0; setexpr usbh2_failcount 0x0;  setexpr emmc_passcount 0x0; setexpr emmc_failcount 0x0; setexpr mtest0_passcount 0x0; setexpr mtest0_failcount 0x0; setexpr mtest1_passcount 0x0; setexpr mtest1_failcount 0x0; setexpr mtest2_passcount 0x0; setexpr mtest2_failcount 0x0; setexpr mtest3_passcount 0x0; setexpr mtest3_failcount 0x0\0"    \
        "get_results=run emmc_result; run usbh1_result; run usbh2_result; run sgmii_result; run rgmii_result; run flash0_result; run flash1_result; run flash3_result; run flashx_result; run gpio0_result; run mtest0_result; run mtest1_result; run mtest2_result; run mtest3_result\0"    \
        "core0_sgmii_stress=setenv count 0; setenv ethact gmac1; while test 100 -ne $count; do run sgmii_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core1_rgmii_stress=setenv count 0; setenv ethact gmac2; while test 100 -ne $count; do run rgmii_stress; setexpr count ${count} + 0x1; done\0"    \
        "core1_emmc_stress=setenv count 0; mmc dev 0; while test 100 -ne $count; do run emmc_stress; setexpr count ${count} + 0x1; done\0"    \
        "core2_emmc_stress=setenv count 0; mmc dev 0; while test 100 -ne $count; do run emmc_stress; setexpr count ${count} + 0x1; done\0"    \
        "core3_usbh1_stress=setenv count 0; usb start; usb dev 0; while test 100 -ne $count; do run usbh1_stress; setexpr count ${count} + 0x1; done\0"    \
        "core3_usbh2_stress=setenv count 0; usb start; usb dev 1; while test 100 -ne $count; do run usbh2_stress; setexpr count ${count} + 0x1; done\0"    \
        "core3_usbd_stress=setenv count 0; usb start; mmc dev 0; ums USB_controller mmc 0:0\0"    \
        "core3_usbhd_stress=setenv count 0; usb start; ums USB_controller usb 0:0\0"    \
        "core0_mtest0_stress=setenv count 0; while test 100 -ne $count; do run mtest0_stress; setexpr count ${count} + 0x1; done\0"    \
        "core1_mtest1_stress=setenv count 0; while test 100 -ne $count; do run mtest1_stress; setexpr count ${count} + 0x1; done\0"    \
        "core2_mtest2_stress=setenv count 0; while test 100 -ne $count; do run mtest2_stress; setexpr count ${count} + 0x1; done\0"    \
        "core3_mtest3_stress=setenv count 0; while test 100 -ne $count; do run mtest3_stress; setexpr count ${count} + 0x1; done\0"    \
        "core0_flash0_stress=setenv count 0; sf probe 0:${flash0_cs} 20000000 0; while test 10 -ne $count; do run flash0_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core0_flash1_stress=setenv count 0; sf probe 1:${flash1_cs} 20000000 0; while test 10 -ne $count; do run flash1_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core2_flash3_stress=setenv count 0; sf probe 3:${flash3_cs} 20000000 0; while test 10 -ne $count; do run flash3_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core0_flashx_stress=setenv count 0; sf probe 4:${flashx_cs} 20000000 0; while test 10 -ne $count; do run flashx_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core3_gpio0_stress=setenv count 0; gpio input ${gpio_in}; gpio set ${gpio_out}; gpio clear ${gpio_out}; setexpr gpioout_val 0; while test 10 -ne $count; do run gpio0_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
		"\0"
#endif
