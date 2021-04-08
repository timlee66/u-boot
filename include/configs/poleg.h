/*
 * Copyright (c) 2016 Nuvoton Technology Corp.
 *
 * Configuration settings for the NUVOTON POLEG board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_POLEG_H
#define __CONFIG_POLEG_H

#undef  CONFIG_USE_IRQ

#define CONFIG_SKIP_LOWLEVEL_INIT

#define CONFIG_MACH_TYPE		MACH_TYPE_NPCMX50

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG

#ifdef  CONFIG_SYS_PROMPT
#undef  CONFIG_SYS_PROMPT
#define CONFIG_SYS_PROMPT               "U-Boot>"
#endif

#define CONFIG_ENV_SIZE                  0x10000
#define CONFIG_ENV_OFFSET               (0x100000)
#define CONFIG_ENV_ADDR                 (0x80000000 + CONFIG_ENV_OFFSET)
#define CONFIG_ENV_SECT_SIZE            0x1000

#ifndef CONFIG_SYS_L2CACHE_OFF
#define CONFIG_SYS_L2_PL310		1
#define CONFIG_SYS_PL310_BASE	0xF03FC000       /* L2 - Cache Regs Base (4k Space)*/
#endif

#define CONFIG_SYS_MALLOC_LEN           (CONFIG_ENV_SIZE + 2048*1024)

#define CONFIG_SYS_MAXARGS              32
#define CONFIG_SYS_CBSIZE               256
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_PROMPT_HUSH_PS2	    "> "

#define CONFIG_SYS_BOOTMAPSZ            (0x30 << 20)
#define CONFIG_SYS_LOAD_ADDR            0x8000
#define CONFIG_SYS_SDRAM_BASE           0x0
#define CONFIG_SYS_SRAM_BASE            0xfffd0000
#define CONFIG_SYS_SRAM_SIZE            (128 << 10)
#define CONFIG_SYS_INIT_SP_ADDR         (0x00008000 - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_MONITOR_LEN          (256 << 10) /* Reserve 256 kB for Monitor   */
#define CONFIG_SYS_MONITOR_BASE	        CONFIG_SYS_TEXT_BASE

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x08000000)

#define CONFIG_STANDALONE_LOAD_ADDR     0x10000000

#define SPI_FLASH_BASE_ADDR				0x80000000
#define SPI_FLASH_REGION_SIZE			0x08000000	/* 128MB */
#define SPI0_BASE_ADDR					0x80000000
#define SPI0_END_ADDR					0x9FFFFFFF
#define SPI3_BASE_ADDR					0xA0000000
#define SPI3_END_ADDR					0xBFFFFFFF

#define PHYS_SDRAM_1			        CONFIG_SYS_SDRAM_BASE

#define CONFIG_BAUDRATE                 115200
#define CONFIG_SYS_BAUDRATE_TABLE       {115200, 57600, 38400}

#define CONFIG_SYS_HZ                   1000

#define CONFIG_USB_OHCI_NEW
#define CONFIG_SYS_USB_OHCI_CPU_INIT
#define CONFIG_SYS_USB_OHCI_MAX_ROOT_PORTS	1


/* Default environemnt variables */
#ifdef CONFIG_NPCMX50_CORE0  
/* CORE 0  memory=512M */
#define CONFIG_BOOTCOMMAND "run clear_results; tftp 0x8200 multi_core_poleg/u-boot-core1.bin; cpu 1 release 0x8200; while itest.l *fffd000C -eq 0x0; do sleep 0.2; done; run core0_gmac_emmc_mtest_stress"
#endif

#ifdef CONFIG_NPCMX50_CORE1
/* CORE 1  memory=256M */
#define CONFIG_BOOTCOMMAND "run core1_usbhd_stress"
#endif

#if defined (CONFIG_NPCMX50_CORE0) || defined (CONFIG_NPCMX50_CORE1)
#define CONFIG_SERVERIP                 10.191.20.62
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
        "ethact=gmac0\0"   \
        "autostart=no\0"   \
        "ethaddr=00:00:F7:A0:00:FC\0"    \
        "eth1addr=00:00:F7:A0:00:FD\0"   \
        "eth2addr=00:00:F7:A0:00:FE\0"    \
        "eth3addr=00:00:F7:A0:00:FF\0"    \
        "gatewayip=10.191.20.254\0"    \
        "dhcp_cmd=dhcp\0"    \
        "uboot=setenv autostart no; ${dhcp_cmd};tftp 20000 ./user/ofer/nuvoton/BMC_UBOOT_DEV_NPCMX50/deliverables/PolegSVB/u-boot.bin;mw 10000 ${filesize};cp 10000 8000F144 1;cp.b 20000 8000F200 ${filesize}\0"    \
        "gpio0_passcount=0x0\0"    \
        "gpio0_failcount=0x0\0"    \
        "flash0_passcount=0x0\0"    \
        "flash0_failcount=0x0\0"    \
        "flash3_passcount=0x0\0"    \
        "flash3_failcount=0x0\0"    \
        "flashx_passcount=0x0\0"    \
        "flashx_failcount=0x0\0"    \
        "mtest_passcount=0x0\0"    \
        "mtest_failcount=0x0\0"    \
        "gmac0_passcount=0x0\0"    \
        "gmac0_failcount=0x0\0"    \
        "gmac1_passcount=0x0\0"    \
        "gmac1_failcount=0x0\0"    \
        "usbh_passcount=0x0\0"    \
        "usbh_failcount=0x0\0"    \
        "emmc_passcount=0x0\0"    \
        "emmc_failcount=0x0\0"    \
        "count=0x0\0"    \
        "gpio_in=22\0"    \
        "gpio_out=23\0"    \
        "flash0_cs=0\0"    \
        "flash3_cs=0\0"    \
        "flashx_cs=0\0"    \
        "gpio0_check=if gpio input ${gpio_in}; then setenv gpioin_val 0; else setenv gpioin_val 1; fi; if test ${gpioin_val} = ${gpioout_val}; then setexpr gpio0_passcount ${gpio0_passcount} + 0x1; mw.l 0xfffd0090 ${gpio0_passcount}; else setexpr gpio0_failcount ${gpio0_failcount} + 0x1; mw.l 0xfffd0094 ${gpio0_failcount}; fi\0"    \
        "gpio0_stress=setexpr gpio0_count 0x0; while test 400 -ne $gpio0_count; do gpio toggle ${gpio_out}; setexpr gpioout_val 0x1 - ${gpioout_val}; run gpio0_check; setexpr gpio0_count ${gpio0_count} + 0x1; done\0"    \
        "gpio0_result=if itest.l *fffd0094 != 0; then echo ***GPIO0-FAIL!***; else echo ***GPIO0-PASS!***; fi; echo GPIO0 pass count!; md.l 0xfffd0090 1; echo GPIO0 fail count!; md.l 0xfffd0094 1\0"    \
        "flash0_clr_mem=mw.b 0xBB0000 0x00 0x20000; sf erase 0xC00000 0x10000;\0"    \
        "flash0_check_blocks=if cmp.b ${flash0_addr} 0xBC0000 0x8000; then if itest.l *${flash0_addr} == 0xAAAAAAAA; then setexpr flash0_passcount ${flash0_passcount} + 0x1; mw.l 0xfffd0080 ${flash0_passcount}; else setexpr flash0_failcount ${flash0_failcount} + 0x1; mw.l 0xfffd0084 ${flash0_failcount}; fi; else setexpr flash0_failcount ${flash0_failcount} + 0x1; mw.l 0xfffd0084 ${flash0_failcount}; fi; run flash0_clr_mem\0"    \
        "flash0_stress=setexpr flash0_count 0x0; setexpr flash0_offset ${flash0_cs} * 0x8000000; setexpr flash0_addr 0x80C00000 + ${flash0_offset}; while test 20 -ne $flash0_count; do mw.b 0xBB0000 0xAA 0x8000; sf write 0xBB0000 0xC00000 0x8000; sf read 0xBC0000 0xC00000 0x8000; run flash0_check_blocks; setexpr flash0_count ${flash0_count} + 0x1; done\0"    \
        "flash0_result=if itest.l *fffd0084 != 0; then echo ***FLASH0:${flash0_cs}-FAIL!***; else echo ***FLASH0:${flash0_cs}-PASS!***; fi; echo FLASH0:${flash0_cs} pass count!; md.l 0xfffd0080 1; echo FLASH0:${flash0_cs} fail count!; md.l 0xfffd0084 1\0"    \
        "flash3_clr_mem=mw.b 0xB70000 0x00 0x20000; sf erase 0x0 0x10000;\0"    \
        "flash3_check_blocks=if cmp.b ${flash3_addr} 0xB80000 0x8000; then if itest.l *${flash3_addr} == 0xAAAAAAAA; then setexpr flash3_passcount ${flash3_passcount} + 0x1; mw.l 0xfffd0060 ${flash3_passcount}; else setexpr flash3_failcount ${flash3_failcount} + 0x1; mw.l 0xfffd0064 ${flash3_failcount}; fi; else setexpr flash3_failcount ${flash3_failcount} + 0x1; mw.l 0xfffd0064 ${flash3_failcount}; fi; run flash3_clr_mem\0"    \
        "flash3_stress=setexpr flash3_count 0x0; setexpr flash3_offset ${flash3_cs} * 0x8000000; setexpr flash3_addr 0xA0000000 + ${flash3_offset}; while test 20 -ne $flash3_count; do mw.b 0xB70000 0xAA 0x8000; sf write 0xB70000 0x0 0x8000; sf read 0xB80000 0x0 0x8000; run flash3_check_blocks; setexpr flash3_count ${flash3_count} + 0x1; done\0"    \
        "flash3_result=if itest.l *fffd0064 != 0; then echo ***FLASH3:${flash3_cs}-FAIL!***; else echo ***FLASH3:${flash3_cs}-PASS!***; fi; echo FLASH3:${flash3_cs} pass count!; md.l 0xfffd0060 1; echo FLASH3:${flash3_cs} fail count!; md.l 0xfffd0064 1\0"    \
        "flashx_clr_mem=mw.b 0xB50000 0x00 0x20000; sf erase 0x0 0x10000;\0"    \
        "flashx_check_blocks=if cmp.b ${flashx_addr} 0xB60000 0x8000; then if itest.l *${flashx_addr} == 0xAAAAAAAA; then setexpr flashx_passcount ${flashx_passcount} + 0x1; mw.l 0xfffd0050 ${flashx_passcount}; else setexpr flashx_failcount ${flashx_failcount} + 0x1; mw.l 0xfffd0054 ${flashx_failcount}; fi; else setexpr flashx_failcount ${flashx_failcount} + 0x1; mw.l 0xfffd0054 ${flashx_failcount}; fi; run flashx_clr_mem\0"    \
        "flashx_stress=setexpr flashx_count 0x0; setexpr flashx_offset ${flashx_cs} * 0x1000000; setexpr flashx_addr 0xF8000000 + ${flashx_offset}; while test 20 -ne $flashx_count; do mw.b 0xB50000 0xAA 0x8000; sf write 0xB50000 0x0 0x8000; sf read 0xB60000 0x0 0x8000; run flashx_check_blocks; setexpr flashx_count ${flashx_count} + 0x1; done\0"    \
        "flashx_result=if itest.l *fffd0054 != 0; then echo ***FLASHX:${flashx_cs}-FAIL!***; else echo ***FLASHX:${flashx_cs}-PASS!***; fi; echo FLASHX:${flashx_cs} pass count!; md.l 0xfffd0050 1; echo FLASHX:${flashx_cs} fail count!; md.l 0xfffd0054 1\0"    \
        "mtest_clr_mem=mw.b 0xB00000 0x00 0x10000\0"    \
        "mtest_check=if mtest 0xB00000 0xB10000 0x55663322 0x3; then setexpr mtest_passcount ${mtest_passcount} + 0x1; mw.l 0xfffd0040 ${mtest_passcount}; else setexpr mtest_failcount ${mtest_failcount} + 0x1; mw.l 0xfffd0044 ${mtest_failcount}; fi\0"    \
        "mtest_stress=setexpr mtest_count 0x0; while test 10 -ne $mtest_count; do run mtest_check; run mtest_clr_mem; setexpr mtest_count ${mtest_count} + 0x1; done\0"    \
        "mtest_result=if itest.l *fffd0044 != 0; then echo ***MTEST-FAIL!***; else echo ***MTEST-PASS!***; fi; echo MTEST pass count!; md.l 0xfffd0040 1; echo MTEST fail count!; md.l 0xfffd0044 1\0"    \
        "gmac1_clr_mem=mw.b 0x900000 0x00 ${filesize}; mw.b 0xA00000 0xFF ${filesize}\0"    \
        "gmac1_check_image=if cmp.b 0x900000 0xA00000 ${filesize}; then setexpr gmac1_passcount ${gmac1_passcount} + 0x1; mw.l 0xfffd0030 ${gmac1_passcount}; else setexpr gmac1_failcount ${gmac1_failcount} + 0x1; mw.l 0xfffd0034 ${gmac1_failcount}; fi; run gmac1_clr_mem\0"    \
        "gmac1_stress=setexpr gmac1_count 0x0; while test 20 -ne $gmac1_count; do tftp 0x900000 u-boot.bin; tftp 0xA00000 u-boot.bin; run gmac1_check_image; setexpr gmac1_count ${gmac1_count} + 1; done\0"    \
        "gmac1_result=if itest.l *fffd0034 != 0; then echo ***GMAC1-FAIL!***; else echo ***GMAC1-PASS!***; fi; echo GMAC1 pass count!; md.l 0xfffd0030 1; echo GMAC1 fail count!; md.l 0xfffd0034 1\0"    \
        "gmac0_clr_mem=mw.b 0x700000 0x00 ${filesize}; mw.b 0x800000 0xFF ${filesize}\0"    \
        "gmac0_check_image=if cmp.b 0x700000 0x800000 ${filesize}; then setexpr gmac0_passcount ${gmac0_passcount} + 0x1; mw.l 0xfffd0020 ${gmac0_passcount}; else setexpr gmac0_failcount ${gmac0_failcount} + 0x1; mw.l 0xfffd0024 ${gmac0_failcount}; fi; run gmac0_clr_mem\0"    \
        "gmac0_stress=setexpr gmac0_count 0x0; while test 20 -ne $gmac0_count; do tftp 0x700000 u-boot.bin; tftp 0x800000 u-boot.bin; run gmac0_check_image; setexpr gmac0_count ${gmac0_count} + 1; done\0"    \
        "gmac0_result=if itest.l *fffd0024 != 0; then echo ***GMAC0-FAIL!***; else echo ***GMAC0-PASS!***; fi; echo GMAC0 pass count!; md.l 0xfffd0020 1; echo GMAC0 fail count!; md.l 0xfffd0024 1\0"    \
        "usbh_clr_mem=mw.b 0x500000 0x00 0x2000\0"    \
        "usbh_check_blocks=if cmp.b 0x500000 0x600000 0x2000; then setexpr usbh_passcount ${usbh_passcount} + 0x1; mw.l 0xfffd0010 ${usbh_passcount}; else setexpr usbh_failcount ${usbh_failcount} + 0x1; mw.l 0xfffd0014 ${usbh_failcount}; fi; run usbh_clr_mem\0"    \
        "usbh_stress=setexpr usbh_count 0x0; while test 400 -ne $usbh_count; do mw.b 0x500000 0xaa 0x2000; usb write 0x500000 0x50 0x10; usb read 0x600000 0x50 0x10; run usbh_check_blocks; setexpr usbh_count ${usbh_count} + 0x1; done\0"    \
        "usbh_result=if itest.l *fffd0014 != 0; then echo ***USBH-FAIL!***; else echo ***USBH-PASS!***; fi; echo USBH pass count!; md.l 0xfffd0010 1; echo USBH fail count!; md.l 0xfffd0014 1\0"    \
        "emmc_clr_mem=mw.b 0x300000 0x00 0x2000\0"    \
        "emmc_check_blocks=if cmp.b 0x300000 0x400000 0x2000; then setexpr emmc_passcount ${emmc_passcount} + 0x1; mw.l 0xfffd0000 ${emmc_passcount}; else setexpr emmc_failcount ${emmc_failcount} + 0x1; mw.l 0xfffd0004 ${emmc_failcount}; fi; run emmc_clr_mem\0"    \
        "emmc_stress=setexpr emmc_count 0x0; while test 200 -ne $emmc_count; do mw.b 0x300000 0xaa 0x2000; mmc write 0x300000 0x50 0x10; mmc read 0x400000 0x50 0x10; run emmc_check_blocks; setexpr emmc_count ${emmc_count} + 0x1; done\0"    \
        "emmc_result=if itest.l *fffd0004 != 0; then echo ***eMMC-FAIL!***; else echo ***eMMC-PASS!***; fi; echo eMMC pass count!; md.l 0xfffd0000 1; echo eMMC fail count!; md.l 0xfffd0004 1\0"    \
        "clear_results=setenv count 0; mw.b 0xfffd0000 0x00 0x1000; setexpr flash0_passcount 0x0; setexpr flash0_failcount 0x0; setexpr flash3_passcount 0x0; setexpr flash3_failcount 0x0; setexpr flashx_passcount 0x0; setexpr flashx_failcount 0x0; setexpr mtest_passcount 0x0; setexpr mtest_failcount 0x0; setexpr gmac0_passcount 0x0; setexpr gmac0_failcount 0x0; setexpr gmac1_passcount 0x0; setexpr gmac1_failcount 0x0;  setexpr usbh_passcount 0x0; setexpr usbh_failcount 0x0;  setexpr emmc_passcount 0x0; setexpr emmc_failcount 0x0\0"    \
        "get_results=run emmc_result; run usbh_result; run gmac0_result; run gmac1_result; run mtest_result; run flash0_result; run flash3_result; run flashx_result; run gpio0_result;\0"    \
        "core0_gmac_emmc_mtest_stress=mmc dev 1; while test 10 -ne $count; do run gmac0_stress; run emmc_stress; run mtest_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core1_emmc_mtest_stress=mmc dev 1; while test 10 -ne $count; do run emmc_stress; run mtest_stress; setexpr count ${count} + 0x1; done\0"    \
        "core1_usbhd_stress=usb start; ums USB_controller usb 0:0\0"    \
        "core0_flash0_stress=run clear_results; sf probe 0:${flash0_cs} 20000000 0; while test 10 -ne $count; do run flash0_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core0_flash3_stress=run clear_results; sf probe 3:${flash3_cs} 20000000 0; while test 10 -ne $count; do run flash3_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core0_flashx_stress=run clear_results; sf probe 4:${flashx_cs} 20000000 0; while test 10 -ne $count; do run flashx_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core0_gpio0_stress=run clear_results; gpio input ${gpio_in}; gpio set ${gpio_out}; gpio clear ${gpio_out}; setexpr gpioout_val 0; while test 10 -ne $count; do run gpio0_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
		"\0"
#else
#define CONFIG_EXTRA_ENV_SETTINGS  \
        "stdin=serial\0"   \
        "stdout=serial\0"   \
        "stderr=serial\0"    \
        "ethact=gmac0\0"   \
        "autostart=yes\0"   \
        "gatewayip=10.191.20.254\0"    \
        "dhcp_cmd=dhcp\0"    \
        "uboot=setenv autostart no; ${dhcp_cmd};tftp 20000 ./user/ofer/nuvoton/BMC_UBOOT_DEV_NPCMX50/deliverables/PolegSVB/u-boot.bin;mw 10000 ${filesize};cp 10000 8000F144 1;cp.b 20000 8000F200 ${filesize}\0"    \
        "gpio0_passcount=0x0\0"    \
        "gpio0_failcount=0x0\0"    \
        "flash0_passcount=0x0\0"    \
        "flash0_failcount=0x0\0"    \
        "flash3_passcount=0x0\0"    \
        "flash3_failcount=0x0\0"    \
        "flashx_passcount=0x0\0"    \
        "flashx_failcount=0x0\0"    \
        "mtest_passcount=0x0\0"    \
        "mtest_failcount=0x0\0"    \
        "gmac0_passcount=0x0\0"    \
        "gmac0_failcount=0x0\0"    \
        "gmac1_passcount=0x0\0"    \
        "gmac1_failcount=0x0\0"    \
        "usbh_passcount=0x0\0"    \
        "usbh_failcount=0x0\0"    \
        "emmc_passcount=0x0\0"    \
        "emmc_failcount=0x0\0"    \
        "count=0x0\0"    \
        "gpio_in=22\0"    \
        "gpio_out=23\0"    \
        "flash0_cs=0\0"    \
        "flash3_cs=0\0"    \
        "flashx_cs=0\0"    \
        "gpio0_check=if gpio input ${gpio_in}; then setenv gpioin_val 0; else setenv gpioin_val 1; fi; if test ${gpioin_val} = ${gpioout_val}; then setexpr gpio0_passcount ${gpio0_passcount} + 0x1; mw.l 0xfffd0090 ${gpio0_passcount}; else setexpr gpio0_failcount ${gpio0_failcount} + 0x1; mw.l 0xfffd0094 ${gpio0_failcount}; fi\0"    \
        "gpio0_stress=setexpr gpio0_count 0x0; while test 400 -ne $gpio0_count; do gpio toggle ${gpio_out}; setexpr gpioout_val 0x1 - ${gpioout_val}; run gpio0_check; setexpr gpio0_count ${gpio0_count} + 0x1; done\0"    \
        "gpio0_result=if itest.l *fffd0094 != 0; then echo ***GPIO0-FAIL!***; else echo ***GPIO0-PASS!***; fi; echo GPIO0 pass count!; md.l 0xfffd0090 1; echo GPIO0 fail count!; md.l 0xfffd0094 1\0"    \
        "flash0_clr_mem=mw.b 0xBB0000 0x00 0x20000; sf erase 0xC00000 0x10000;\0"    \
        "flash0_check_blocks=if cmp.b ${flash0_addr} 0xBC0000 0x8000; then if itest.l *${flash0_addr} == 0xAAAAAAAA; then setexpr flash0_passcount ${flash0_passcount} + 0x1; mw.l 0xfffd0080 ${flash0_passcount}; else setexpr flash0_failcount ${flash0_failcount} + 0x1; mw.l 0xfffd0084 ${flash0_failcount}; fi; else setexpr flash0_failcount ${flash0_failcount} + 0x1; mw.l 0xfffd0084 ${flash0_failcount}; fi; run flash0_clr_mem\0"    \
        "flash0_stress=setexpr flash0_count 0x0; setexpr flash0_offset ${flash0_cs} * 0x8000000; setexpr flash0_addr 0x80C00000 + ${flash0_offset}; while test 20 -ne $flash0_count; do mw.b 0xBB0000 0xAA 0x8000; sf write 0xBB0000 0xC00000 0x8000; sf read 0xBC0000 0xC00000 0x8000; run flash0_check_blocks; setexpr flash0_count ${flash0_count} + 0x1; done\0"    \
        "flash0_result=if itest.l *fffd0084 != 0; then echo ***FLASH0:${flash0_cs}-FAIL!***; else echo ***FLASH0:${flash0_cs}-PASS!***; fi; echo FLASH0:${flash0_cs} pass count!; md.l 0xfffd0080 1; echo FLASH0:${flash0_cs} fail count!; md.l 0xfffd0084 1\0"    \
        "flash3_clr_mem=mw.b 0xB70000 0x00 0x20000; sf erase 0x0 0x10000;\0"    \
        "flash3_check_blocks=if cmp.b ${flash3_addr} 0xB80000 0x8000; then if itest.l *${flash3_addr} == 0xAAAAAAAA; then setexpr flash3_passcount ${flash3_passcount} + 0x1; mw.l 0xfffd0060 ${flash3_passcount}; else setexpr flash3_failcount ${flash3_failcount} + 0x1; mw.l 0xfffd0064 ${flash3_failcount}; fi; else setexpr flash3_failcount ${flash3_failcount} + 0x1; mw.l 0xfffd0064 ${flash3_failcount}; fi; run flash3_clr_mem\0"    \
        "flash3_stress=setexpr flash3_count 0x0; setexpr flash3_offset ${flash3_cs} * 0x8000000; setexpr flash3_addr 0xA0000000 + ${flash3_offset}; while test 20 -ne $flash3_count; do mw.b 0xB70000 0xAA 0x8000; sf write 0xB70000 0x0 0x8000; sf read 0xB80000 0x0 0x8000; run flash3_check_blocks; setexpr flash3_count ${flash3_count} + 0x1; done\0"    \
        "flash3_result=if itest.l *fffd0064 != 0; then echo ***FLASH3:${flash3_cs}-FAIL!***; else echo ***FLASH3:${flash3_cs}-PASS!***; fi; echo FLASH3:${flash3_cs} pass count!; md.l 0xfffd0060 1; echo FLASH3:${flash3_cs} fail count!; md.l 0xfffd0064 1\0"    \
        "flashx_clr_mem=mw.b 0xB50000 0x00 0x20000; sf erase 0x0 0x10000;\0"    \
        "flashx_check_blocks=if cmp.b ${flashx_addr} 0xB60000 0x8000; then if itest.l *${flashx_addr} == 0xAAAAAAAA; then setexpr flashx_passcount ${flashx_passcount} + 0x1; mw.l 0xfffd0050 ${flashx_passcount}; else setexpr flashx_failcount ${flashx_failcount} + 0x1; mw.l 0xfffd0054 ${flashx_failcount}; fi; else setexpr flashx_failcount ${flashx_failcount} + 0x1; mw.l 0xfffd0054 ${flashx_failcount}; fi; run flashx_clr_mem\0"    \
        "flashx_stress=setexpr flashx_count 0x0; setexpr flashx_offset ${flashx_cs} * 0x1000000; setexpr flashx_addr 0xF8000000 + ${flashx_offset}; while test 20 -ne $flashx_count; do mw.b 0xB50000 0xAA 0x8000; sf write 0xB50000 0x0 0x8000; sf read 0xB60000 0x0 0x8000; run flashx_check_blocks; setexpr flashx_count ${flashx_count} + 0x1; done\0"    \
        "flashx_result=if itest.l *fffd0054 != 0; then echo ***FLASHX:${flashx_cs}-FAIL!***; else echo ***FLASHX:${flashx_cs}-PASS!***; fi; echo FLASHX:${flashx_cs} pass count!; md.l 0xfffd0050 1; echo FLASHX:${flashx_cs} fail count!; md.l 0xfffd0054 1\0"    \
        "mtest_clr_mem=mw.b 0xB00000 0x00 0x10000\0"    \
        "mtest_check=if mtest 0xB00000 0xB10000 0x55663322 0x3; then setexpr mtest_passcount ${mtest_passcount} + 0x1; mw.l 0xfffd0040 ${mtest_passcount}; else setexpr mtest_failcount ${mtest_failcount} + 0x1; mw.l 0xfffd0044 ${mtest_failcount}; fi\0"    \
        "mtest_stress=setexpr mtest_count 0x0; while test 10 -ne $mtest_count; do run mtest_check; run mtest_clr_mem; setexpr mtest_count ${mtest_count} + 0x1; done\0"    \
        "mtest_result=if itest.l *fffd0044 != 0; then echo ***MTEST-FAIL!***; else echo ***MTEST-PASS!***; fi; echo MTEST pass count!; md.l 0xfffd0040 1; echo MTEST fail count!; md.l 0xfffd0044 1\0"    \
        "gmac1_clr_mem=mw.b 0x900000 0x00 ${filesize}; mw.b 0xA00000 0xFF ${filesize}\0"    \
        "gmac1_check_image=if cmp.b 0x900000 0xA00000 ${filesize}; then setexpr gmac1_passcount ${gmac1_passcount} + 0x1; mw.l 0xfffd0030 ${gmac1_passcount}; else setexpr gmac1_failcount ${gmac1_failcount} + 0x1; mw.l 0xfffd0034 ${gmac1_failcount}; fi; run gmac1_clr_mem\0"    \
        "gmac1_stress=setexpr gmac1_count 0x0; while test 20 -ne $gmac1_count; do tftp 0x900000 u-boot.bin; tftp 0xA00000 u-boot.bin; run gmac1_check_image; setexpr gmac1_count ${gmac1_count} + 1; done\0"    \
        "gmac1_result=if itest.l *fffd0034 != 0; then echo ***GMAC1-FAIL!***; else echo ***GMAC1-PASS!***; fi; echo GMAC1 pass count!; md.l 0xfffd0030 1; echo GMAC1 fail count!; md.l 0xfffd0034 1\0"    \
        "gmac0_clr_mem=mw.b 0x700000 0x00 ${filesize}; mw.b 0x800000 0xFF ${filesize}\0"    \
        "gmac0_check_image=if cmp.b 0x700000 0x800000 ${filesize}; then setexpr gmac0_passcount ${gmac0_passcount} + 0x1; mw.l 0xfffd0020 ${gmac0_passcount}; else setexpr gmac0_failcount ${gmac0_failcount} + 0x1; mw.l 0xfffd0024 ${gmac0_failcount}; fi; run gmac0_clr_mem\0"    \
        "gmac0_stress=setexpr gmac0_count 0x0; while test 20 -ne $gmac0_count; do tftp 0x700000 u-boot.bin; tftp 0x800000 u-boot.bin; run gmac0_check_image; setexpr gmac0_count ${gmac0_count} + 1; done\0"    \
        "gmac0_result=if itest.l *fffd0024 != 0; then echo ***GMAC0-FAIL!***; else echo ***GMAC0-PASS!***; fi; echo GMAC0 pass count!; md.l 0xfffd0020 1; echo GMAC0 fail count!; md.l 0xfffd0024 1\0"    \
        "usbh_clr_mem=mw.b 0x500000 0x00 0x2000\0"    \
        "usbh_check_blocks=if cmp.b 0x500000 0x600000 0x2000; then setexpr usbh_passcount ${usbh_passcount} + 0x1; mw.l 0xfffd0010 ${usbh_passcount}; else setexpr usbh_failcount ${usbh_failcount} + 0x1; mw.l 0xfffd0014 ${usbh_failcount}; fi; run usbh_clr_mem\0"    \
        "usbh_stress=setexpr usbh_count 0x0; while test 400 -ne $usbh_count; do mw.b 0x500000 0xaa 0x2000; usb write 0x500000 0x50 0x10; usb read 0x600000 0x50 0x10; run usbh_check_blocks; setexpr usbh_count ${usbh_count} + 0x1; done\0"    \
        "usbh_result=if itest.l *fffd0014 != 0; then echo ***USBH-FAIL!***; else echo ***USBH-PASS!***; fi; echo USBH pass count!; md.l 0xfffd0010 1; echo USBH fail count!; md.l 0xfffd0014 1\0"    \
        "emmc_clr_mem=mw.b 0x300000 0x00 0x2000\0"    \
        "emmc_check_blocks=if cmp.b 0x300000 0x400000 0x2000; then setexpr emmc_passcount ${emmc_passcount} + 0x1; mw.l 0xfffd0000 ${emmc_passcount}; else setexpr emmc_failcount ${emmc_failcount} + 0x1; mw.l 0xfffd0004 ${emmc_failcount}; fi; run emmc_clr_mem\0"    \
        "emmc_stress=setexpr emmc_count 0x0; while test 200 -ne $emmc_count; do mw.b 0x300000 0xaa 0x2000; mmc write 0x300000 0x50 0x10; mmc read 0x400000 0x50 0x10; run emmc_check_blocks; setexpr emmc_count ${emmc_count} + 0x1; done\0"    \
        "emmc_result=if itest.l *fffd0004 != 0; then echo ***eMMC-FAIL!***; else echo ***eMMC-PASS!***; fi; echo eMMC pass count!; md.l 0xfffd0000 1; echo eMMC fail count!; md.l 0xfffd0004 1\0"    \
        "clear_results=setenv count 0; mw.b 0xfffd0000 0x00 0x1000; setexpr flash0_passcount 0x0; setexpr flash0_failcount 0x0; setexpr flash3_passcount 0x0; setexpr flash3_failcount 0x0; setexpr flashx_passcount 0x0; setexpr flashx_failcount 0x0; setexpr mtest_passcount 0x0; setexpr mtest_failcount 0x0; setexpr gmac0_passcount 0x0; setexpr gmac0_failcount 0x0; setexpr gmac1_passcount 0x0; setexpr gmac1_failcount 0x0;  setexpr usbh_passcount 0x0; setexpr usbh_failcount 0x0;  setexpr emmc_passcount 0x0; setexpr emmc_failcount 0x0\0"    \
        "get_results=run emmc_result; run usbh_result; run gmac0_result; run gmac1_result; run mtest_result; run flash0_result; run flash3_result; run flashx_result; run gpio0_result;\0"    \
        "core0_gmac_emmc_mtest_stress=mmc dev 1; while test 10 -ne $count; do run gmac0_stress; run emmc_stress; run mtest_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core1_emmc_mtest_stress=mmc dev 1; while test 10 -ne $count; do run emmc_stress; run mtest_stress; setexpr count ${count} + 0x1; done\0"    \
        "core1_usbhd_stress=usb start; ums USB_controller usb 0:0\0"    \
        "core0_flash0_stress=run clear_results; sf probe 0:${flash0_cs} 20000000 0; while test 10 -ne $count; do run flash0_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core0_flash3_stress=run clear_results; sf probe 3:${flash3_cs} 20000000 0; while test 10 -ne $count; do run flash3_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core0_flashx_stress=run clear_results; sf probe 4:${flashx_cs} 20000000 0; while test 10 -ne $count; do run flashx_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
        "core0_gpio0_stress=run clear_results; gpio input ${gpio_in}; gpio set ${gpio_out}; gpio clear ${gpio_out}; setexpr gpioout_val 0; while test 10 -ne $count; do run gpio0_stress; setexpr count ${count} + 0x1; run get_results; done\0"    \
		"\0"
#endif

#endif
