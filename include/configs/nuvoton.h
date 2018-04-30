/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nuvoton.h                                                                                             */
/*            This file contains configurations for the wpcm450 compilation                                */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef __NUVOTON_H
#define __NUVOTON_H

/*---------------------------------------------------------------------------------------------------------*/
/* Including HAL definitions                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#if defined(BOARD_NAME) && !defined(__ASSEMBLY__) && !defined(USE_HOSTCC)
#include "../../board/nuvoton/common/BMC_HAL/Boards/board.h"

#undef MOD
#undef ARRAY_SIZE
#undef DIV_ROUND
#undef MAX
#undef MIN

#endif




/*---------------------------------------------------------------------------------------------------------*/
/* Command line support                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define CONFIG_CMD_ASKENV               /* ask for env variable */
#define CONFIG_SYS_ALT_MEMTEST          /* Allow deeper alternet mtest */
#define CONFIG_CMD_SETGETDCR            /* DCR support on 4xx */
#define CONFIG_CMD_ELF                  /* ELF support */
#define CONFIG_CMD_REGINFO              /* Register dump */
#define CONFIG_CMD_SAVES                /* save S record dump */
#define CONFIG_CMD_UNZIP                /* unzip from memory to memory */

#define CONFIG_CMD_PING                 /* Ping command */
#define CONFIG_CMD_DHCP                 /* DHCP boot */
#define CONFIG_CMD_DIAG                 /* Diagnostic commands */

#define CONFIG_CMD_MII                  /* Network */

#define CONFIG_CMD_MEMTEST
#define CONFIG_CMD_MEMINFO
#define CONFIG_CMD_CACHE

/*#define CONFIG_MMC_TRACE           */
#define CONFIG_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_NPCMX50_SDHCI
#define CONFIG_CMD_MMC
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_FAT
#define CONFIG_FAT_WRITE
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_FS_GENERIC

#define CONFIG_CMDLINE_EDITING
/*#define CONFIG_CMD_SF*/

/* Open Firmware flat tree */
#define CONFIG_OF_LIBFDT


/* Fuses */
#define CONFIG_CMD_FUSE
#define CONFIG_NPCM750_OTP

/*---------------------------------------------------------------------------------------------------------*/
/* High Level Configuration Options                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CORE_TYPE                       Cortex-A9
#define CONFIG_ARMV7	                1	        /* This is an ARM V7 CPU core */
#define CONFIG_IDENT_STRING             " by Nuvoton Technology Corp."
#define CONFIG_NUVOTON                  1

#define CONFIG_DISPLAY_CPUINFO          1           /* display cpu info (and speed) */
#define CONFIG_DISPLAY_BOARDINFO        1           /* display board info */


#define CONFIG_USE_IRQ                  1

#define CONFIG_MISC_INIT_R              1           /* This is used to properly link "nuvoton_eth.c" file
                                                       for more info read the comment in the file
                                                       over misc_init_r function */

#define CONFIG_SETUP_MEMORY_TAGS        1
#define CONFIG_SYS_GENERIC_BOARD        1        /* Trego- UBOOT Relocation old way (for Now) */

/*---------------------------------------------------------------------------------------------------------*/
/* Miscellaneous configurable options                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define CONFIG_SYS_LONGHELP             1                           /* Enable print of long help for commands */

#define CONFIG_SYS_CBSIZE               512                         /* Console I/O Buffer Size */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS              32                          /* max number of command args */

#define CONFIG_SYS_MEMTEST_START        0x00000000                  /* memtest works on (SRAM and DDR) */
#define CONFIG_SYS_MEMTEST_END          0x0A000000                  /* MB in DRAM */
#define CONFIG_SYS_LOAD_ADDR            0x8000                      /* default load address */

/*---------------------------------------------------------------------------------------------------------*/
/* Linux Kernel Parameters                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define CONFIG_CMDLINE_TAG              1       /* enable passing of ATAGs  */
#define CONFIG_SETUP_MEMORY_TAGS        1
#define CONFIG_INITRD_TAG               1       /* Required for ramdisk support */
#define CONFIG_TAGS_LOCATION            0x100

/* FIX_ISSUE_605 */
/* #define CONFIG_TAGS_LOCATION            0x1000100 */

/*---------------------------------------------------------------------------------------------------------*/
/* Hardware drivers                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define CONFIG_SYS_HZ                   1000

/*---------------------------------------------------------------------------------------------------------*/
/* Serial Console Configuration                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define CONFIG_CONS_INDEX               1

/*---------------------------------------------------------------------------------------------------------*/
/* Stack sizes                                                                                             */
/* The stack sizes are set up in start.S using the settings below                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define CONFIG_STACKSIZE                (128*1024)                  /* regular stack */
#define CONFIG_STACKSIZE_IRQ            (4*1024)                    /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ            (4*1024)                    /* FIQ stack */


/*---------------------------------------------------------------------------------------------------------*/
/* Physical Memory Map                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/


#define CONFIG_NR_DRAM_BANKS            2                           /* we have 2 banks: Chip SRAM2 (internal) and SDRAM DDR (external) */
#define PHYS_SDRAM_1                    SRAM_BASE_ADDR
#define PHYS_SDRAM_1_SIZE               SRAM_MEMORY_SIZE
#define PHYS_SDRAM_2                    SDRAM_BASE_ADDR
#define PHYS_SDRAM_2_SIZE               SDRAM_MAPPED_SIZE

#define	CONFIG_SYS_MONITOR_LEN		    (256 << 10)	/* Reserve 256 kB for Monitor	*/
#define CONFIG_SYS_MONITOR_BASE	        CONFIG_SYS_TEXT_BASE

/*#define CONFIG_SYS_INIT_SP_ADDR		    0x00008000                  // Trego (SRAM_BASE_ADDR + SRAM_MEMORY_SIZE)*/
#define CONFIG_SYS_INIT_SP_ADDR		    (0x00008000 - GENERATED_GBL_DATA_SIZE)

/* FIX_ISSUE_605 */
/* #define CONFIG_SYS_INIT_SP_ADDR		    (0x02008000 - GENERATED_GBL_DATA_SIZE) */

#define CONFIG_SYS_SDRAM_BASE           PHYS_SDRAM_2



/*---------------------------------------------------------------------------------------------------------*/
/* Networking                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/

#define CONFIG_MII                      1

#define CONFIG_BOOTP_SERVERIP           1


#define CONFIG_ARP_TIMEOUT              100            /* ARP timeout in ms */
#define CONFIG_NET_RETRY_COUNT          100            /* Number of retries per file transaction */

#define CONFIG_SYS_RX_ETH_BUFFER        16             /* default is 4, set to 16 here. */

/*---------------------------------------------------------------------------------------------------------*/
/* I2C                                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/

#define CONFIG_SYS_I2C_SPEED         100000

#undef USB_INTERRUPT    /* No need USB Dev in UBOOT */

#endif /* __NUVOTON_H */

