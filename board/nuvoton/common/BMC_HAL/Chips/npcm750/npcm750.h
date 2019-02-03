/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2014 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   npcm750.h                                                                                             */
/*            This file contains chip definitions for the given project                                    */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef __NPCM750_CHIP_H_
#define __NPCM750_CHIP_H_

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                              CHIP                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Chip Name                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define NPCM750
/*#define _PALLADIUM_                       1       // Activate Palladium bypasses */

/*---------------------------------------------------------------------------------------------------------*/
/* On-Chip POLEG NPCM750 VERSIONS                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define  POLEG_Z1                0x00A92750
#define  POLEG_Z2                0x04A92750
#define  POLEG_A1                0x10A92750

#define EXT_CLOCK_FREQUENCY_MHZ 25
#define EXT_CLOCK_FREQUENCY_KHZ (EXT_CLOCK_FREQUENCY_MHZ*_1KHz_)
#define EXT_CLOCK_FREQUENCY_HZ  (EXT_CLOCK_FREQUENCY_MHZ*_1MHz_)

/*---------------------------------------------------------------------------------------------------------*/
/* On-Chip ROM                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define ROM_BASE_ADDR                  0xFFFF0000
#define ROM_MEMORY_SIZE                _64KB_
#define ROMCODE_VER_ADDR               0xFFFF00FC

/*---------------------------------------------------------------------------------------------------------*/
/* External SDRAM Space DDR4                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define SDRAM_BASE_ADDR                 0x00000000
#define SDRAM_MAPPED_SIZE               (_2GB_)

/*---------------------------------------------------------------------------------------------------------*/
/* External Coprocessor (CR16C+) area                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define COPR_BASE_ADDR                  0xF0600000
#define COPR_MAPPED_SIZE               (_2MB_)

/*---------------------------------------------------------------------------------------------------------*/
/* SPI0 Flash                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define SPI0CS0_BASE_ADDR               0x80000000      /* SPI0 direct access CS0  */
#define SPI0CS1_BASE_ADDR               0x88000000      /* SPI0 direct access CS1  */
#define SPI0CS2_BASE_ADDR               0x90000000      /* SPI0 direct access CS2  */
#define SPI0CS3_BASE_ADDR               0x98000000      /* SPI0 direct access CS3  */

#define SPI3CS0_BASE_ADDR               0xA0000000      /* SPI3 direct access CS0  */
#define SPI3CS1_BASE_ADDR               0xA8000000      /* SPI3 direct access CS1  */
#define SPI3CS2_BASE_ADDR               0xB0000000      /* SPI3 direct access CS2  */
#define SPI3CS3_BASE_ADDR               0xB8000000      /* SPI3 direct access CS3  */

#define FLASH_MEMORY_SIZE(device)       _128MB_

#define FLASH_BASE_ADDR(device)         ((device)==0? SPI0CS0_BASE_ADDR: (device)==1? SPI0CS1_BASE_ADDR:(device)==2? SPI3CS0_BASE_ADDR: SPI3CS1_BASE_ADDR)

#define FIU_DEVICES_PER_MODULE           2           /* for SPI3 there are 4 but it is unlikely that someone will use them */

#define SPI0_MEMORY_SIZE                _256MB_
#define SPI3_MEMORY_SIZE                _512MB_
/*---------------------------------------------------------------------------------------------------------*/
/* SPI Expansion Flash                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define SPIXCS0_BASE_ADDR               0xF8000000      /* SPIX direct access CS0  */
#define SPIXCS1_BASE_ADDR               0xF9000000      /* SPIX direct access CS1  */
#define SPIX_MEMORY_SIZE                _64KB_


/*---------------------------------------------------------------------------------------------------------*/
/* XBus                                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define XBUS_BASE_ADDR                  0xC2000000      /* XBUS direct access  */
#define XBUS_MAPPED_SIZE                _32MB_

/*---------------------------------------------------------------------------------------------------------*/
/* Memory Mapped IO                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define IOMEMORY_BASE_ADDR              0xF0000000
#define IOMEMORY_SIZE                   0x0F004000

/*---------------------------------------------------------------------------------------------------------*/
/* RAM3                                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define RAM3_BASE_ADDR                  0xC0008000      /* RAM3  */
#define RAM3_MAPPED_SIZE                _4KB_


/*---------------------------------------------------------------------------------------------------------*/
/* OTP                                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define OTPROM_BASE_ADDR                0xF0189000      /* OTP ROM  */
#define OTP2ROM_BASE_ADDR               0xF018A000      /* OTP2 ROM  */
#define OTPROM_MAPPED_SIZE              _128B_

/*---------------------------------------------------------------------------------------------------------*/
/* On-Chip SRAM     (SRAM2)                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define RAM2_BASE_ADDR                  0xFFFD0000
#define RAM2_MEMORY_SIZE                _128KB_

/*---------------------------------------------------------------------------------------------------------*/
/* SRAM2 lockable  (Lockable in 2x 1KB pieces)                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define SRAM_LOCK_BASE_ADDR                  0xFFFEF800      /* SRAM, lockable  */
#define SRAM_LOCK_MAPPED_SIZE                _2KB_

/*---------------------------------------------------------------------------------------------------------*/
/* first 256 byte in RAM2 mirrored to this address. if RAMV = 1, default 0 (ROM)                           */
/*---------------------------------------------------------------------------------------------------------*/
#define SRAM_VECOTR_BASE_ADDR           0xFFFF0000      /* SRAM, mapped to vector area  */
#define SRAM_VECOTR_MAPPED_SIZE         _256B_


/* Following modules either new to Poleg, or do not have drivers, or empty allocations on the AHB \ APB */
#define MAIN_MEM_BASE_ADDR              0x00000000      /* DDR4 Memory Controller  */
#define AHB1_BASE_ADDR                  0xF0000000      /* AHB1 allocation (Including APB allocations)  */
#define AHB8_BASE_ADDR                  0xF0800000      /* AHB8 allocation  */
#define DVC_BASE_ADDR                   0xF0808000      /* DVC (KVM) registers  */
#define MCR_BASE_ADDR                   0xF0824000      /* Memory Controller Registers  */
#define CRDRDR_BASE_ADDR                0xF0840000      /* SDHC1: Card reader Control registers SD Controller registers  */
#define MMC1_BASE_ADDR                  0xF0842000      /* SDHC2: MMC registers  */
#define GDMA0_BASE_ADDR                 0xF0850000      /* GDMA0  */
#define GDMA1_BASE_ADDR                 0xF0851000      /* GDMA1  */
#define GDMA2_BASE_ADDR                 0xF0852000      /* GDMA2  */
#define GDMA3_BASE_ADDR                 0xF0853000      /* GDMA3  */
#define SECACC_BASE_ADDR                0xF085B000      /* SECACC  */
#define AHB18_BASE_ADDR                 0x80000000      /* AHB18 allocation  */
#define AHB3_BASE_ADDR                  0xA0000000      /* AHB3 allocation  */
#define XBUSR_BASE_ADDR                 0xC0002000      /* XBUS registers  */
#define AHB14_BASE_ADDR                 0xE0000000      /* AHB14 Allocation  */
#define APB14_BASE_ADDR                 0xE0000000      /* APB14 Allocation  */
#define VDMX_BASE_ADDR                  0xE0800000      /* VDMX  */
#define APBS_BASE_ADDR                  0xF0000000      /* APBs Allocation  */
#define APB1_BASE_ADDR                  0xF0000000      /* APB1 Allocation  */
#define RNG_BASE_ADDR                   0xF000B000      /* RNG  */
#define BT_BASE_ADDR                    0xF000D000      /* BT (Block Transfer)  */
#define SHRDIO_BASE_ADDR                0xF001C000      /* 16B Shared I/O (IBM)  */
#define APB2_BASE_ADDR                  0xF0080000      /* APB2 Allocation  */
#define APB2_BASE_ADDR                  0xF0080000      /* APB2 Allocation  */
#define APB3_BASE_ADDR                  0xF0100000      /* APB3 Allocation  */
#define PECI_BASE_ADDR                  0xF0100000      /* PECI registers  */
#define SIOX1_BASE_ADDR                 0xF0101000      /* Serial GPIO Expansion 1  */
#define SIOX2_BASE_ADDR                 0xF0102000      /* Serial GPIO Expansion 2  */
#define APB4_BASE_ADDR                  0xF0180000      /* APB4 Allocation  */
#define THRMS_BASE_ADDR                 0xF0188000      /* Thermal Sensor  */
#define APB2SIB_BASE_ADDR               0xF018C000      /* APB2SIB  */
#define APB5_BASE_ADDR                  0xF0200000      /* APB5 Allocation  */
#define A9_BASE_ADDR                    0xF03FC000      /* Level 2 Cache Registers + A9 Peripherals Registers Allocation  */
#define DAP_BASE_ADDR                   0xF0500000      /* Cortex A9 Debug Access Port + MCPHY Allocation  */
#define AHB19_BASE_ADDR                 0xFFF00000      /* AHB19 allocation  */

#define USBH_EHCI_BASE_ADDR             0xF0806000
#define USBH_OHCI_BASE_ADDR             0xF0807000

#define AHB_PCI_BASE_ADDR               0xF0400000      /* AHB to PCI bridge  */

/*---------------------------------------------------------------------------------------------------------*/
/* Palladium pll0 global register                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#ifdef _PALLADIUM_
#define PALLADIUM_PLL0_ACCESS                      MEM
#define PALLADIUM_PLL0_PHYS_BASE_ADDR              0xffff3fc4

#ifndef DYNAMIC_BASE_ADDRESS
#define PALLADIUM_PLL0_BASE_ADDR                   PALLADIUM_PLL0_PHYS_BASE_ADDR
#else
#define PALLADIUM_PLL0_BASE_ADDR                   PALLADIUM_PLL0_VIRT_BASE_ADDR
#endif
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Multi-Function Timer Module                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define MFT_MODULE_TYPE                 Poleg_IP
#define MFT_ACCESS                      MEM

#define MFT0_BASE_ADDR                  0xF0180000      /* Tachmeter MFT 0  */
#define MFT1_BASE_ADDR                  0xF0181000      /* Tachmeter MFT 1  */
#define MFT2_BASE_ADDR                  0xF0182000      /* Tachmeter MFT 2  */
#define MFT3_BASE_ADDR                  0xF0183000      /* Tachmeter MFT 3  */
#define MFT4_BASE_ADDR                  0xF0184000      /* Tachmeter MFT 4  */
#define MFT5_BASE_ADDR                  0xF0185000      /* Tachmeter MFT 5  */
#define MFT6_BASE_ADDR                  0xF0186000      /* Tachmeter MFT 6  */
#define MFT7_BASE_ADDR                  0xF0187000      /* Tachmeter MFT 7  */

#define MFT_PHYS_BASE_ADDR(module)      (MFT0_BASE_ADDR + ((module) * 0x1000L))
#define MFT_INTERRUPT(module)           (MFT_INTERRUPT_1 + module)

#define MFT_NUM_OF_MODULES              2

#ifndef DYNAMIC_BASE_ADDRESS
#define MFT_BASE_ADDR(module)           MFT_PHYS_BASE_ADDR(module)
#else
#define MFT_BASE_ADDR(module)           MFT_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* General Ethernet configuration                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
/* EMC1/2  ETH0/ETH1 - see BMC_HAL/EMC and npcmX50_eth.c code */
#define CHIP_NUM_OF_EMC_ETH             2
#define CHIP_NUM_OF_ETH                 CHIP_NUM_OF_EMC_ETH
#define CHIP_ETH0_TYPE                  EMC
#define CHIP_ETH1_TYPE                  EMC

/* GMAC1/2  ETH2/ETH3 - see GMAC synopsys code */
#define CHIP_NUM_OF_GMAC_ETH            2
#define GMAC1_ETH2                      2
#define GMAC2_ETH3                      3

/*---------------------------------------------------------------------------------------------------------*/
/* General Module  : System Global Registers                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define GCR_MODULE_TYPE                 Poleg_IP
#define GCR_ACCESS                      MEM
#define GCR_PHYS_BASE_ADDR              0xF0800000

#ifndef DYNAMIC_BASE_ADDRESS
#define GCR_BASE_ADDR                   GCR_PHYS_BASE_ADDR
#else
#define GCR_BASE_ADDR                   GCR_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* UART Module                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define UART_MODULE_TYPE                Hermon_IP
#define UART_ACCESS                     MEM
#define UART0_BASE_ADDR                 0xf0001000      /* UART0 registers  */
#define UART1_BASE_ADDR                 0xf0002000      /* UART1 registers  */
#define UART2_BASE_ADDR                 0xf0003000      /* UART2 registers  */
#define UART3_BASE_ADDR                 0xf0004000      /* UART3 registers  */
#define UART_PHYS_BASE_ADDR(module)     (UART0_BASE_ADDR + ((module) * 0x1000))
#define UART_INTERRUPT(module)          (UART_INTERRUPT_0 + module)
#define UART_NUM_OF_MODULES             4

#ifndef DYNAMIC_BASE_ADDRESS
#define UART_BASE_ADDR(module)          UART_PHYS_BASE_ADDR(module)
#else
#define UART_BASE_ADDR(module)          UART_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PCI MailBox Module                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#define PCIMBX_PHYS_BASE_ADDR           0xF0848000      /* PCI Mailbox direct mapping . Mapped till 0xEFFFFFFF. */

#define PCIMBX_ACCESS                   MEM
#ifndef DYNAMIC_BASE_ADDRESS
#define PCIMBX_BASE_ADDR                PCIMBX_PHYS_BASE_ADDR
#else
#define PCIMBX_BASE_ADDR                PCIMBX_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PCTL (Memory) Module                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define PCTL_ACCESS                     MEM
#define PCTL_PHYS_BASE_ADDR             0xFF001000

#ifndef DYNAMIC_BASE_ADDRESS
#define PCTL_BASE_ADDR                  PCTL_PHYS_BASE_ADDR
#else
#define PCTL_BASE_ADDR                  PCTL_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Timer Module                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define TIMER_MODULE_TYPE               Hermon_IP
#define TIMER_ACCESS                    MEM
#define TMR0_BASE_ADDR                  0xF0008000      /* Timer0  module registers  */
#define TMR1_BASE_ADDR                  0xF0009000      /* Timer1  module registers  */
#define TMR2_BASE_ADDR                  0xF000A000      /* Timer2  module registers  */
#define TIMER_PHYS_BASE_ADDR(module)    (TMR0_BASE_ADDR + ((module)*0x1000))
#define TIMER_INTERRUPT(port)           (TIMER_INTERRUPT_0 + (port))         /* (TIMER_INTERRUPT_0 to TIMER_INTERRUPT_14) */
#define TIMER_GROUP_INTERRUPT(port)     ((((port) >=2) && ((port) <5)) ? (16 + ((port)-2)) : AIC_GROUP_INTERRUPT_NONE)
#define WDG_INTERRUPT(port)             (WDG_INTERRUPT_0 + (port))           /* (WDG_INTERRUPT_0 to WDG_INTERRUPT_2) */

#define TIMER_NUM_OF_MODULES            3
#define TIMER_NUM_OF_PORTS              15

#ifndef DYNAMIC_BASE_ADDRESS
#define TIMER_BASE_ADDR(port)           TIMER_PHYS_BASE_ADDR(port)
#else
#define TIMER_BASE_ADDR(port)           TIMER_VIRT_BASE_ADDR(port)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SMB Module                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define SMB_MODULE_TYPE                 Hermon_IP
#define SMB_ACCESS                      MEM
#define SMB_PHYS_BASE_ADDR(module)      (0xF0080000+((module)*0x1000))

#define SMB_MAX_BUSES                   16
#define SMB_SCL_GPIOS                   114, 116, 118, 31, 29, 27, 171, 173, 128, 130, 132, 134, 220, 222, 23, 21

#ifndef DYNAMIC_BASE_ADDRESS
#define SMB_BASE_ADDR(module)           SMB_PHYS_BASE_ADDR(module)
#else
#define SMB_BASE_ADDR(module)           SMB_VIRT_BASE_ADDR(module)
#endif

#define SMB_INTERRUPT(module)           (SMB_INTERRUPT_0 + module)


/*---------------------------------------------------------------------------------------------------------*/
/* KCS Module                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define KCS_MODULE_TYPE                 Yarkon_IP
#define KCS_ACCESS                      MEM
#define KCS_PHYS_BASE_ADDR              0xF0007000

#define KCS_MAX_CHANNELS                3

#ifndef DYNAMIC_BASE_ADDRESS
#define KCS_BASE_ADDR                   KCS_PHYS_BASE_ADDR
#else
#define KCS_BASE_ADDR                   KCS_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ADC Module                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ADC_MODULE_TYPE                 Yarkon_IP
#define ADC_ACCESS                      MEM
#define ADC_PHYS_BASE_ADDR              0xF000C000

#define ADC_MAX_CHANNELS                8
#define ADC_GROUP_INTERRUPT             8

#ifndef DYNAMIC_BASE_ADDRESS
#define ADC_BASE_ADDR                   ADC_PHYS_BASE_ADDR
#else
#define ADC_BASE_ADDR                   ADC_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PWM Module                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_MODULE_TYPE                 Yarkon_IP
#define PWM_ACCESS                      MEM
#define PWM_PHYS_BASE_ADDR(module)      (0xF0103000 + (0x1000 * (module)))

#define PWM_MAX_MODULES                 2
#define PWM_MAX_PORTS                   8

#ifndef DYNAMIC_BASE_ADDRESS
#define PWM_BASE_ADDR(module)           PWM_PHYS_BASE_ADDR(module)
#else
#define PWM_BASE_ADDR(module)           PWM_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* FIU Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define FIU_MODULE_TYPE                 Poleg_IP
#define FIU_ACCESS                      MEM

#define FIU0_BASE_ADDR                  0xFB000000      /* SPI0 registers  */
#define FIUX_BASE_ADDR                  0xFB001000      /* SPIX registers  */
#define FIU3_BASE_ADDR                  0xC0000000      /* SPI3 registers  */

#define FIU_PHYS_BASE_ADDR(n)           ( ( (n) == 0 ) ? FIU0_BASE_ADDR : ( (n) == 3 ) ? FIU3_BASE_ADDR : FIUX_BASE_ADDR)
#define FIU_NUM_OF_MODULES              3
#ifndef DYNAMIC_BASE_ADDRESS
#define FIU_BASE_ADDR(module)           FIU_PHYS_BASE_ADDR(module)    /* Use this when handle FIU for on Poleg_IP inside HAL */
#else
#define FIU_BASE_ADDR(module)           FIU_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Clock Module                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_MODULE_TYPE                 Poleg_IP
#define CLK_ACCESS                      MEM
#define CLK_PHYS_BASE_ADDR              0xF0801000

#ifndef DYNAMIC_BASE_ADDRESS
#define CLK_BASE_ADDR                   CLK_PHYS_BASE_ADDR
#else
#define CLK_BASE_ADDR                   CLK_VIRT_BASE_ADDR
#endif

#define EXT_CLOCK_FREQUENCY_MHZ 25
#define EXT_CLOCK_FREQUENCY_KHZ (EXT_CLOCK_FREQUENCY_MHZ*_1KHz_)
#define EXT_CLOCK_FREQUENCY_HZ  (EXT_CLOCK_FREQUENCY_MHZ*_1MHz_)

/*---------------------------------------------------------------------------------------------------------*/
/* Shared Memory (SHM) Module                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define SHM_MODULE_TYPE                 Yarkon_IP
#define SHM_ACCESS                      MEM
#define SHM_PHYS_BASE_ADDRESS           0xC0001000
#ifndef DYNAMIC_BASE_ADDRESS
#define SHM_BASE_ADDR                   SHM_PHYS_BASE_ADDRESS
#else
#define SHM_BASE_ADDR                   SHM_VIRT_BASE_ADDRESS
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* AIC/GIC A9 Peripheral Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define AIC_MODULE_TYPE                 Poleg_IP      /* Using AIC API only. */
#define AIC_ACCESS                      MEM
#define AIC_PHYS_BASE_ADDR              0xF03FE000

#define GIC_INTERRUPT_NUM               160           /* GIC (INTC for A9) first 32 interrupts are not used (128 irq's connected) */
#define GIC_CPU_INTERFACE_OFFSET        0x0100
#define GIC_DISTRIBUTOR_OFFSET          0x1000

#ifndef DYNAMIC_BASE_ADDRESS
#define AIC_BASE_ADDR                   AIC_PHYS_BASE_ADDR
#else
#define AIC_BASE_ADDR                   AIC_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* SCU A9 Peripheral Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define SCU_PHYS_BASE_ADDR              0xF03FE000
#ifndef DYNAMIC_BASE_ADDRESS
#define SCU_BASE_ADDR                   SCU_PHYS_BASE_ADDR
#else
#define SCU_BASE_ADDR                   SCU_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* TWD A9 Peripheral Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define TWD_PHYS_BASE_ADDR              0xF03FE600
#ifndef DYNAMIC_BASE_ADDRESS
#define TWD_BASE_ADDR                   TWD_PHYS_BASE_ADDR
#else
#define TWD_BASE_ADDR                   TWD_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Ethernet 100 Mac Controler    (10/100 Ethernet Controller)                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define EMC_MODULE_TYPE                 Hermon_IP
#define EMC_ACCESS                      MEM
#define EMC1_BASE_ADDR                  0xF0825000      /* EMC1 (10/100 Ethernet Controller)  */
#define EMC2_BASE_ADDR                  0xF0826000      /* EMC2 (10/100 Ethernet Controller)  */
#define EMC_PHYS_BASE_ADDR(module)      ((module == 0) ? EMC1_BASE_ADDR : EMC2_BASE_ADDR)
#define EMC_RX_INTERRUPT(module)        ((module == 0) ? EMC1RX_INTERRUPT  : EMC2RX_INTERRUPT)
#define EMC_TX_INTERRUPT(module)        ((module == 0) ? EMC1TX_INTERRUPT  : EMC2TX_INTERRUPT)
#define EMC_NUM_OF_MODULES              2

#ifndef DYNAMIC_BASE_ADDRESS
#define EMC_BASE_ADDR(module)           EMC_PHYS_BASE_ADDR(module)
#else
#define EMC_BASE_ADDR(module)           EMC_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* GMAC Module                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define GMAC_MODULE_TYPE                Yarkon_IP
#define GMAC_ACCESS                     MEM
#define GMAC1_BASE_ADDR                 0xF0802000
#define GMAC2_BASE_ADDR                 0xF0804000
#define GMAC_PHYS_BASE_ADDR(module)      ((module == 0) ? GMAC1_BASE_ADDR : GMAC2_BASE_ADDR)
#define GMAC_INTERRUPT(module)           ((module == 0) ? GMAC1_INTERRUPT : GMAC2_INTERRUPT)
#define GMAC_NUM_OF_MODULES             2

#ifndef DYNAMIC_BASE_ADDRESS
#define GMAC_BASE_ADDR(module)          GMAC_PHYS_BASE_ADDR(module)
#else
#define GMAC_BASE_ADDR(module)          GMAC_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Memory Control                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define MC_MODULE_TYPE                  Poleg_IP
#define MC_ACCESS                       MEM
#define MC_PHYS_BASE_ADDR               0xF0824000

#ifndef DYNAMIC_BASE_ADDRESS
#define MC_BASE_ADDR                    MC_PHYS_BASE_ADDR
#else
#define MC_BASE_ADDR                    MC_VIRT_BASE_ADDR
#endif

#define MCPHY_ACCESS                    MEM
#define MCPHY_PHYS_BASE_ADDR            0xF05F0000

#ifndef DYNAMIC_BASE_ADDRESS
#define MCPHY_BASE_ADDR                 MCPHY_PHYS_BASE_ADDR
#else
#define MCPHY_BASE_ADDR                 MCPHY_VIRT_BASE_ADDR
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* SD Module                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define SD_MODULE_TYPE                  Hermon_IP
#define SD_ACCESS                       MEM
#define SD_PHYS_BASE_ADDR(module)       (0xF0840000 + (0x2000 * (module)))
#define SD_GROUP_INTERRUPT(module)      (14 + (module))
#define SD_INTERRUPT(module)            (((module) == 0)? SDRDR_INTERRUPT : MMC_INTERRUPT)
#define SD_NUM_OF_MODULES               2
#define SD_CLK_TARGET_FREQ_Z1           (48 * _1MHz_)
#define SD_CLK_TARGET_FREQ_A1           (50 * _1MHz_)


#ifndef DYNAMIC_BASE_ADDRESS
#define SD_BASE_ADDR(module)            SD_PHYS_BASE_ADDR(module)
#else
#define SD_BASE_ADDR(module)            SD_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* AES Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define AES_MODULE_TYPE                 Yarkon_IP
#define AES_ACCESS                      MEM
#define AES_PHYS_BASE_ADDR              0xF0858000

#ifndef DYNAMIC_BASE_ADDRESS
#define AES_BASE_ADDR                   AES_PHYS_BASE_ADDR
#else
#define AES_BASE_ADDR                   AES_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* DES Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define DES_MODULE_TYPE                 Yarkon_IP
#define DES_ACCESS                      MEM
#define DES_PHYS_BASE_ADDR              0xF0859000

#ifndef DYNAMIC_BASE_ADDRESS
#define DES_BASE_ADDR                   DES_PHYS_BASE_ADDR
#else
#define DES_BASE_ADDR                   DES_VIRT_BASE_ADDR
#endif
#define DES_INTERRUPT_POLARITY           INTERRUPT_POLARITY_LEVEL_HIGH
#define DES_INTERRUPT_PRIORITY           0
#define DES_INTERRUPT_PROVIDER           CHIP_INTERRUPT_PROVIDER
#define DES_NUM_OF_INPUTS                3
#define DES_CLK                          2000000
#define DES_SOURCE_CLOCK                 PLL2
#define DES_MUX(input)                   CHIP_MuxDES(input)


/*---------------------------------------------------------------------------------------------------------*/
/* STRAP Module                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define STRP_MODULE_TYPE                Poleg_IP

#define STRP_INTERRUPT_POLARITY          INTERRUPT_POLARITY_LEVEL_HIGH
#define STRP_INTERRUPT_PRIORITY          0
#define STRP_INTERRUPT_PROVIDER          CHIP_INTERRUPT_PROVIDER
#define STRP_NUM_OF_INPUTS               3
#define STRP_CLK                         2000000
#define STRP_SOURCE_CLOCK                PLL2
#define STRP_MUX(input)                  CHIP_MuxSTRP(input)


/*---------------------------------------------------------------------------------------------------------*/
/* FUSE Module                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define FUSE_MODULE_TYPE                Poleg_IP
#define FUSE_OTP_ACCESS                 MEM
#define FUSE_PHYS_BASE_ADDR(module)     (0xF0189000 + ((module) * 0x1000))

#ifndef DYNAMIC_BASE_ADDRESS
#define FUSE_BASE_ADDR(module)          FUSE_PHYS_BASE_ADDR(module)
#else
#define FUSE_BASE_ADDR(module)          FUSE_VIRT_BASE_ADDR(module)
#endif
#define FUSE_INTERRUPT_POLARITY          INTERRUPT_POLARITY_LEVEL_HIGH
#define FUSE_INTERRUPT_PRIORITY          0
#define FUSE_INTERRUPT_PROVIDER          CHIP_INTERRUPT_PROVIDER
#define FUSE_NUM_OF_INPUTS               3
#define FUSE_CLK                         2000000
#define FUSE_SOURCE_CLOCK                PLL2
#define FUSE_MUX(input)                  CHIP_MuxFUSE(input)


/*---------------------------------------------------------------------------------------------------------*/
/* VCD Module  (VIDEO CAPTURE AND DIFFERENTIATION)                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define VCD_MODULE_TYPE                 Hermon_IP
#define VCD_ACCESS                      MEM
#define VCD_PHYS_BASE_ADDR              0xF0810000

#define VCD_MEM_PORT1                   3
#define VCD_MEM_PORT2                   4

#define VCD_MAX_WIDTH                   2032
#define VCD_MAX_HIGHT                   1536
#define USE_INTERNAL_GFX

#define VCD_FRAME_A_PHYS_BASE_ADDRESS   0x3E200000

#ifndef DYNAMIC_BASE_ADDRESS
#define VCD_BASE_ADDR                   VCD_PHYS_BASE_ADDR
#else
#define VCD_BASE_ADDR                   VCD_VIRT_BASE_ADDR
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* GFX Module                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define GFX_MODULE_TYPE                 Hermon_IP
#define GFX_ACCESS                      MEM

#define GFXI_BASE_ADDR                  0xF000E000      /* Graphics Core information registers  */
#define GFXOL_BASE_ADDR                 0xF0828000      /* GFX Overlay FIFO and registers  */

#define GFX_PHYS_BASE_ADDR              GFXI_BASE_ADDR

#ifndef DYNAMIC_BASE_ADDRESS
#define GFX_BASE_ADDR                   GFX_PHYS_BASE_ADDR
#else
#define GFX_BASE_ADDR                   GFX_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* ECE Module:  Video Compression map ECE                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
#define ECE_MODULE_TYPE                 Poleg_IP
#define ECE_ACCESS                      MEM
#define ECE_PHYS_BASE_ADDR              0xF0820000
#define ECE_ED_PHYS_BASE_ADDRESS        0x3E800000
#ifndef DYNAMIC_BASE_ADDRESS
#define ECE_BASE_ADDR                   ECE_PHYS_BASE_ADDR
#else
#define ECE_BASE_ADDR                   ECE_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* VDM Module                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define VDM_MODULE_TYPE                 Yarkon_IP
#define VDM_ACCESS                      MEM
#define VDM_PHYS_BASE_ADDR     	        (0xE0800000)
#define VDMA_PHYS_BASE_ADDR             (0xF0822000)

#ifndef DYNAMIC_BASE_ADDRESS
#define VDM_BASE_ADDR         	        VDM_PHYS_BASE_ADDR
#define VDMA_BASE_ADDR         	        VDMA_PHYS_BASE_ADDR
#else
#define VDM_BASE_ADDR 	                VDM_VIRT_BASE_ADDR
#define VDMA_BASE_ADDR 	                VDMA_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PCIE RC Module                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define PCIERC_MODULE_TYPE                 Poleg_IP
#define PCIERC_ACCESS                      MEM
#define PCIERC_PHYS_BASE_ADDR     	        (0xE1000000)

#ifndef DYNAMIC_BASE_ADDRESS
#define PCIERC_BASE_ADDR         	        PCIERC_PHYS_BASE_ADDR
#else
#define PCIERC_BASE_ADDR 	                PCIERC_VIRT_BASE_ADDR
#endif

#define PCIERC_NUM_OF_MSI					32

/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Module                                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_MODULE_TYPE                Hermon_IP
#define GPIO_ACCESS                     MEM
#define GPIO0_BASE_ADDR                 0xF0010000      /* GPIO0 module registers  */
#define GPIO1_BASE_ADDR                 0xF0011000      /* GPIO1 module registers  */
#define GPIO2_BASE_ADDR                 0xF0012000      /* GPIO2 module registers  */
#define GPIO3_BASE_ADDR                 0xF0013000      /* GPIO3 module registers  */
#define GPIO4_BASE_ADDR                 0xF0014000      /* GPIO4 module registers  */
#define GPIO5_BASE_ADDR                 0xF0015000      /* GPIO5 module registers  */
#define GPIO6_BASE_ADDR                 0xF0016000      /* GPIO6 module registers  */
#define GPIO7_BASE_ADDR                 0xF0017000      /* GPIO7 module registers  */
#define GPIO_PHYS_BASE_ADDR(port)       (GPIO0_BASE_ADDR + ((port)*0x1000))
#define GPIO_INTERRUPT(gpio)            ((gpio) < 32  ? GPIO_INTERRUPT0 : (gpio) < 64  ? GPIO_INTERRUPT1 : (gpio) < 96  ? GPIO_INTERRUPT2 :   \
                                         (gpio) < 128 ? GPIO_INTERRUPT3 : (gpio) < 160 ? GPIO_INTERRUPT4 : (gpio) < 192 ? GPIO_INTERRUPT5 :  \
                                         (gpio) < 224 ? GPIO_INTERRUPT6 :  GPIO_INTERRUPT7)

#define GPIO_NUM_OF_MODULES              1
#define GPIO_NUM_OF_PORTS                8
#define GPIO_NUM_OF_GPIOS                256

#ifndef DYNAMIC_BASE_ADDRESS
#define GPIO_BASE_ADDR(port)           GPIO_PHYS_BASE_ADDR(port)
#else
#define GPIO_BASE_ADDR(port)           GPIO_VIRT_BASE_ADDR(port)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* PSPI Module (aka SSPI)                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define PSPI_MODULE_TYPE                Hermon_IP
#define PSPI_ACCESS                     MEM
#define PSPI1_BASE_ADDR                 0xF0200000      /* Peripheral SPI 1 registers  */
#define PSPI2_BASE_ADDR                 0xF0201000      /* Peripheral SPI 2 registers  */

#define ESPI_BASE_ADDR                  0xF008F000      /* eSPI registers  */


#define PSPI_PHYS_BASE_ADDR(module)     (PSPI1_BASE_ADDR + ((module) * 0x1000))
#define PSPI_NUM_OF_MODULES             2

#ifndef DYNAMIC_BASE_ADDRESS
#define PSPI_BASE_ADDR(module)          PSPI_PHYS_BASE_ADDR(module)
#else
#define PSPI_BASE_ADDR(module)          PSPI_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* AHB2 SRAM Module (SRAM2)                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define SRAM2_ACCESS                    MEM
#define SRAM2_PHYS_BASE_ADDR             RAM2_BASE_ADDR
#define SRAM2_MEMORY_SIZE               RAM2_MEMORY_SIZE

#ifndef DYNAMIC_BASE_ADDRESS
#define SRAM2_BASE_ADDR                 SRAM2_PHYS_BASE_ADDR
#else
#define SRAM2_BASE_ADDR                 SRAM2_VIRT_BASE_ADDR
#endif

#define SRAM_BASE_ADDR                  SRAM2_BASE_ADDR
#define SRAM_MEMORY_SIZE                SRAM2_MEMORY_SIZE

/*---------------------------------------------------------------------------------------------------------*/
/* USB Module                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define USB_MODULE_TYPE                 Hermon_IP
#define USB_ACCESS                      MEM

#define USB0_BASE_ADDR                  0xF0830000      /* USB (2.0) Device 0 registers  */
#define USB1_BASE_ADDR                  0xF0831000      /* USB (2.0) Device 1 registers  */
#define USB2_BASE_ADDR                  0xF0832000      /* USB (2.0) Device 2 registers  */
#define USB3_BASE_ADDR                  0xF0833000      /* USB (2.0) Device 3 registers  */
#define USB4_BASE_ADDR                  0xF0834000      /* USB (2.0) Device 4 registers  */
#define USB5_BASE_ADDR                  0xF0835000      /* USB (2.0) Device 5 registers  */
#define USB6_BASE_ADDR                  0xF0836000      /* USB (2.0) Device 6 registers  */
#define USB7_BASE_ADDR                  0xF0837000      /* USB (2.0) Device 7 registers  */
#define USB8_BASE_ADDR                  0xF0838000      /* USB (2.0) Device 8 registers  */
#define USB9_BASE_ADDR                  0xF0839000      /* USB (2.0) Device 9 registers  */

#define USB_PHYS_BASE_ADDR(module)      (USB0_BASE_ADDR + (0x1000 * module))

#define USB_INTERRUPT(module)           (USB_DEV_INTERRUPT_0 + module)
#define USB_IS_FULL_SPEED(module)       0
#define USB_DESC_PHYS_BASE_ADDR(module) (SRAM2_PHYS_BASE_ADDR + 0x800 * (module))
#define USB_DESC_VIRT_BASE_ADDR(module) (SRAM2_BASE_ADDR + 0x800 * (module))
#define USB_NUM_OF_MODULES              10

#ifndef DYNAMIC_BASE_ADDRESS
#define USB_BASE_ADDR(module)           USB_PHYS_BASE_ADDR(module)
#else
#define USB_BASE_ADDR(module)           USB_VIRT_BASE_ADDR(module)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* L2 cache Module                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define L2_CACHE_PHYS_BASE_ADDR         A9_BASE_ADDR

#ifndef DYNAMIC_BASE_ADDRESS
#define L2_CACHE_BASE_ADDR              L2_CACHE_PHYS_BASE_ADDR
#else
#define L2_CACHE_BASE_ADDR              L2_CACHE_VIRT_BASE_ADDR
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Public Key Accelerator Module                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define PKA_MODULE_TYPE                  4
#define PKA_ACCESS                       MEM
#define PKA_PHYS_BASE_ADDR               0xF085B000

#ifndef DYNAMIC_BASE_ADDRESS
#define PKA_BASE_ADDR                   PKA_PHYS_BASE_ADDR
#else
#define PKA_BASE_ADDR                   PKA_VIRT_BASE_ADDR
#endif

#define PKA_INT_NUM                      PKA_INTERRUPT
#define PKA_INT_POLARITY                 INTERRUPT_POLARITY_LEVEL_HIGH
#define PKA_INTERRUPT_PRIORITY           0
#define PKA_INT_PROVIDER                 CHIP_INTERRUPT_PROVIDER
#define PKA_NUM_OF_MODULES               1
#define PKA_SOURCE_CLOCK(module)         PLL2
#define PKA_MUX(module)                  CHIP_MuxPKA(module)


/*---------------------------------------------------------------------------------------------------------*/
/* SHA-1 and SHA-256 Module                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define SHA_MODULE_TYPE                  1
#define SHA_ACCESS                       MEM
#define SHA_PHYS_BASE_ADDR               0xF085A000

#ifndef DYNAMIC_BASE_ADDRESS
#define SHA_BASE_ADDR                   SHA_PHYS_BASE_ADDR
#else
#define SHA_BASE_ADDR                   SHA_VIRT_BASE_ADDR
#endif

/*#define SHA_INTERRUPT(module)            (SHA_INTERRUPT) */
#define SHA_INTERRUPT_POLARITY            INTERRUPT_POLARITY_LEVEL_HIGH
#define SHA_INTERRUPT_PRIORITY           0
#define SHA_INTERRUPT_PROVIDER           CHIP_INTERRUPT_PROVIDER
#define SHA_NUM_OF_MODULES               1
#define SHA_SOURCE_CLOCK(module)         PLL2
#define SHA_MUX(module)                  CHIP_MuxSHA(module)


/*---------------------------------------------------------------------------------------------------------*/
/* RNG                                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
#define RNG_MODULE_TYPE                 2
#define RNG_BASE_ADDR                   0xF000B000      /* RNG  */
#define RNG_ACCESS                      MEM

/*#define RNG_INT_NUM                     RNG_INTERRUPT */
/*#define RNG_INT_POLARITY                INTERRUPT_POLARITY_RISING_EDGE */
/*#define RNG_INT_PROVIDER                CHIP_INTERRUPT_PROVIDER */
/*#define RNG_INTERRUPT_PRIORITY          0 */


/* Notice: RNG does not have an intterupt. current version of RNG driver works only with interrupts.
	Before using the driver need to change it to do polling too */

#define RNG_PRESCALER_CLOCK             CLK_GetAPBFreq(APB1)


/*---------------------------------------------------------------------------------------------------------*/
/* Interrupts                                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define NPCM750_IRQ_GIC_START 32

typedef enum
{
	TWD_LOCALTIMER_INTERRUPT 	 = 29,
	ADC_INTERRUPT                = 0 + NPCM750_IRQ_GIC_START,     /* ADC Module                                         */
	COPR_INTERRUPT               = 1 + NPCM750_IRQ_GIC_START,     /* Coprocessor subsystem                              */
	UART_INTERRUPT_0             = 2 + NPCM750_IRQ_GIC_START,     /* UART 0 Module                                      */
	UART_INTERRUPT_1             = 3 + NPCM750_IRQ_GIC_START,     /* UART 1 Module                                      */
	UART_INTERRUPT_2             = 4 + NPCM750_IRQ_GIC_START,     /* UART 2 Module                                      */
	UART_INTERRUPT_3             = 5 + NPCM750_IRQ_GIC_START,     /* UART 3 Module                                      */
	PECI_INTERRUPT               = 6 + NPCM750_IRQ_GIC_START,     /* PECI module                                        */
	VSYNC_INTERRUPT              = 7 + NPCM750_IRQ_GIC_START,     /* Graphics module via System Manager module          */
	PCIMBX_INTERRUPT             = 8 + NPCM750_IRQ_GIC_START,     /* PCI mailbox module                                 */
	KCS_HIB_INTERRUPT            = 9 + NPCM750_IRQ_GIC_START,     /* KCS/HIB  (from host interface) modules             */
	LPC_MBX_INTERRUPT            = 10 + NPCM750_IRQ_GIC_START,    /* LPC or eSPI Mailbox (new, if we do)                */
	SHM_INTERRUPT                = 11 + NPCM750_IRQ_GIC_START,    /* SHM module                                         */
	PS2_INTERRUPT                = 12 + NPCM750_IRQ_GIC_START,    /* PS/2 Module ???                                    */
	BT_INTERRUPT                 = 13 + NPCM750_IRQ_GIC_START,    /* Block transfer, If we do                           */
	GMAC1_INTERRUPT              = 14 + NPCM750_IRQ_GIC_START,    /* GMAC1 module                                       */
	EMC1RX_INTERRUPT             = 15 + NPCM750_IRQ_GIC_START,    /* EMC1 Rx module                                     */
	EMC1TX_INTERRUPT             = 16 + NPCM750_IRQ_GIC_START,    /* EMC1 Tx module                                     */
	GMAC2_INTERRUPT              = 17 + NPCM750_IRQ_GIC_START,    /* GMAC2 module                                       */
	ESPI_INTERRUPT               = 18 + NPCM750_IRQ_GIC_START,    /* eSPI Module                                        */
	SIOX_INTERRUPT_1             = 19 + NPCM750_IRQ_GIC_START,    /* SIOX Serial GPIO Expander module 1                 */
	SIOX_INTERRUPT_2             = 20 + NPCM750_IRQ_GIC_START,    /* SIOX Serial GPIO Expander module 2                 */
	L2_CACHE_ERR                 = 21 + NPCM750_IRQ_GIC_START,    /* A combined interrupt on L2 Cahche errors           */
	VCD_INTERRUPT                = 22 + NPCM750_IRQ_GIC_START,    /* VCD module                                         */
	DVC_INTERRUPT                = 23 + NPCM750_IRQ_GIC_START,    /* DVC module                                         */
	ECE_INTERRUPT                = 24 + NPCM750_IRQ_GIC_START,    /* ECE module                                         */
	MC_INTERRUPT                 = 25 + NPCM750_IRQ_GIC_START,    /* Memory Controller Interrupt                        */
	MMC_INTERRUPT                = 26 + NPCM750_IRQ_GIC_START,    /* MMC Module (SDHC2)                                 */
	SDRDR_INTERRUPT              = 27 + NPCM750_IRQ_GIC_START,    /* SDHC1 - SD Card Reader side interface (if required)*/
	PSPI2_INTERRUPT              = 28 + NPCM750_IRQ_GIC_START,    /* Slow Peripheral SPI2                               */
	VDMA_INTERRUPT               = 29 + NPCM750_IRQ_GIC_START,    /* VDMA module                                        */
	MCTP_INTERRUPT               = 30 + NPCM750_IRQ_GIC_START,    /* VDMX module. Not used if VDMA used                 */
	PSPI1_INTERRUPT              = 31 + NPCM750_IRQ_GIC_START,    /* Slow Peripheral SPI1                               */
	TIMER_INTERRUPT_0            = 32 + NPCM750_IRQ_GIC_START,    /* Timer Module 0 Timer 0                             */
	TIMER_INTERRUPT_1            = 33 + NPCM750_IRQ_GIC_START,    /* Timer Module 0 Timer 1                             */
	TIMER_INTERRUPT_2            = 34 + NPCM750_IRQ_GIC_START,    /* Timer Module 0 Timer 2                             */
	TIMER_INTERRUPT_3            = 35 + NPCM750_IRQ_GIC_START,    /* Timer Module 0 Timer 3                             */
	TIMER_INTERRUPT_4            = 36 + NPCM750_IRQ_GIC_START,    /* Timer Module 0 Timer 4                             */
	TIMER_INTERRUPT_5            = 37 + NPCM750_IRQ_GIC_START,    /* Timer Module 1 Timer 0                             */
	TIMER_INTERRUPT_6            = 38 + NPCM750_IRQ_GIC_START,    /* Timer Module 1 Timer 1                             */
	TIMER_INTERRUPT_7            = 39 + NPCM750_IRQ_GIC_START,    /* Timer Module 1 Timer 2                             */
	TIMER_INTERRUPT_8            = 40 + NPCM750_IRQ_GIC_START,    /* Timer Module 1 Timer 3                             */
	TIMER_INTERRUPT_9            = 41 + NPCM750_IRQ_GIC_START,    /* Timer Module 1 Timer 4                             */
	TIMER_INTERRUPT_10           = 42 + NPCM750_IRQ_GIC_START,    /* Timer Module 2 Timer 0                             */
	TIMER_INTERRUPT_11           = 43 + NPCM750_IRQ_GIC_START,    /* Timer Module 2 Timer 1                             */
	TIMER_INTERRUPT_12           = 44 + NPCM750_IRQ_GIC_START,    /* Timer Module 2 Timer 2                             */
	TIMER_INTERRUPT_13           = 45 + NPCM750_IRQ_GIC_START,    /* Timer Module 2 Timer 3                             */
	TIMER_INTERRUPT_14           = 46 + NPCM750_IRQ_GIC_START,    /* Timer Module 2 Timer 4                             */
	WDG_INTERRUPT_0              = 47 + NPCM750_IRQ_GIC_START,    /* Timer Module 0 watchdog (also on NMI)              */
	WDG_INTERRUPT_1              = 48 + NPCM750_IRQ_GIC_START,    /* Timer Module 1 watchdog (also on NMI)              */
	WDG_INTERRUPT_2              = 49 + NPCM750_IRQ_GIC_START,    /* Timer Module 2 watchdog (also on NMI)              */
	USB_DEV_INTERRUPT_0          = 51 + NPCM750_IRQ_GIC_START,    /* USB Device 0                                       */
	USB_DEV_INTERRUPT_1          = 52 + NPCM750_IRQ_GIC_START,    /* USB Device 1                                       */
	USB_DEV_INTERRUPT_2          = 53 + NPCM750_IRQ_GIC_START,    /* USB Device 2                                       */
	USB_DEV_INTERRUPT_3          = 54 + NPCM750_IRQ_GIC_START,    /* USB Device 3                                       */
	USB_DEV_INTERRUPT_4          = 55 + NPCM750_IRQ_GIC_START,    /* USB Device 4                                       */
	USB_DEV_INTERRUPT_5          = 56 + NPCM750_IRQ_GIC_START,    /* USB Device 5                                       */
	USB_DEV_INTERRUPT_6          = 57 + NPCM750_IRQ_GIC_START,    /* USB Device 6                                       */
	USB_DEV_INTERRUPT_7          = 58 + NPCM750_IRQ_GIC_START,    /* USB Device 7                                       */
	USB_DEV_INTERRUPT_8          = 59 + NPCM750_IRQ_GIC_START,    /* USB Device 8                                       */
	USB_DEV_INTERRUPT_9          = 60 + NPCM750_IRQ_GIC_START,    /* USB Device 9                                       */
	USB_HST_INTERRUPT_0          = 61 + NPCM750_IRQ_GIC_START,    /* USB Host 0                                         */
	USB_HST_INTERRUPT_1          = 62 + NPCM750_IRQ_GIC_START,    /* USB Host 1                                         */

	SMB_INTERRUPT_0              = 64 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 0                             */
	SMB_INTERRUPT_1              = 65 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 1                             */
	SMB_INTERRUPT_2              = 66 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 2                             */
	SMB_INTERRUPT_3              = 67 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 3                             */
	SMB_INTERRUPT_4              = 68 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 4                             */
	SMB_INTERRUPT_5              = 69 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 5                             */
	SMB_INTERRUPT_6              = 70 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 6                             */
	SMB_INTERRUPT_7              = 71 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 7                             */
	SMB_INTERRUPT_8              = 72 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 8                             */
	SMB_INTERRUPT_9              = 73 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 9                             */
	SMB_INTERRUPT_10             = 74 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 10                            */
	SMB_INTERRUPT_11             = 75 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 11                            */
	SMB_INTERRUPT_12             = 76 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 12                            */
	SMB_INTERRUPT_13             = 77 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 13                            */
	SMB_INTERRUPT_14             = 78 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 14                            */
	SMB_INTERRUPT_15             = 79 + NPCM750_IRQ_GIC_START,    /* SMBus and I2C Module 15                            */

	AES_INTERRUPT                = 80 + NPCM750_IRQ_GIC_START,    /* AES Module                                         */
	DES_INTERRUPT                = 81 + NPCM750_IRQ_GIC_START,    /* 3DES Module                                        */
	SHA_INTERRUPT                = 82 + NPCM750_IRQ_GIC_START,    /* SHA module                                         */
	PKA_INTERRUPT                = 83 + NPCM750_IRQ_GIC_START ,   /* (SECACC) ECC and RSA accelerator module            */

	SPI_INTERRUPT_0              = 85 + NPCM750_IRQ_GIC_START,    /* FIU module 0 if required                           */
	SPI_INTERRUPT_X              = 86 + NPCM750_IRQ_GIC_START,    /* FIU module X if required                           */
	SPI_INTERRUPT_3              = 87 + NPCM750_IRQ_GIC_START,    /* FIU module 3 if required                           */
	GDMA_INTERRUPT_0             = 88 + NPCM750_IRQ_GIC_START,    /* GDMA Module 0                                      */
	GDMA_INTERRUPT_1             = 89 + NPCM750_IRQ_GIC_START,    /* GDMA Module 1                                      */
	GDMA_INTERRUPT_2             = 90 + NPCM750_IRQ_GIC_START,    /* GDMA Module 2                                      */
	GDMA_INTERRUPT_3             = 91 + NPCM750_IRQ_GIC_START,    /* GDMA Module 3 If required                          */
	OTP_INTERRUPT                = 92 + NPCM750_IRQ_GIC_START,    /* Fustraps and Key arrays                            */
	PWM_INTERRUPT_0_PWM0_3       = 93 + NPCM750_IRQ_GIC_START,    /* PWM Module 0 outputing PWM0-3                      */
	PWM_INTERRUPT_1_PWM4_7       = 94 + NPCM750_IRQ_GIC_START,    /* PWM Module 1 outputing PWM4-7                      */

	MFT_INTERRUPT_0              = 96 + NPCM750_IRQ_GIC_START,    /* MFT Module 0                                       */
	MFT_INTERRUPT_1              = 97 + NPCM750_IRQ_GIC_START,    /* MFT Module 1                                       */
	MFT_INTERRUPT_2              = 98 + NPCM750_IRQ_GIC_START,    /* MFT Module 2                                       */
	MFT_INTERRUPT_3              = 99 + NPCM750_IRQ_GIC_START,    /* MFT Module 3                                       */
	MFT_INTERRUPT_4              = 100 + NPCM750_IRQ_GIC_START,   /* MFT Module 4                                       */
	MFT_INTERRUPT_5              = 101 + NPCM750_IRQ_GIC_START,   /* MFT Module 5                                       */
	MFT_INTERRUPT_6              = 102 + NPCM750_IRQ_GIC_START,   /* MFT Module 6                                       */
	MFT_INTERRUPT_7              = 103 + NPCM750_IRQ_GIC_START,   /* MFT Module 7                                       */
	PWM_INTERRUPT_0              = 104 + NPCM750_IRQ_GIC_START,   /* PWM module 0                                       */
	PWM_INTERRUPT_1              = 105 + NPCM750_IRQ_GIC_START,   /* PWM module 1                                       */
	PWM_INTERRUPT_2              = 106 + NPCM750_IRQ_GIC_START,   /* PWM module 2                                       */
	PWM_INTERRUPT_3              = 107 + NPCM750_IRQ_GIC_START,   /* PWM module 3                                       */
	PWM_INTERRUPT_4              = 108 + NPCM750_IRQ_GIC_START,   /* PWM module 4                                       */
	PWM_INTERRUPT_5              = 109 + NPCM750_IRQ_GIC_START,   /* PWM module 5                                       */
	PWM_INTERRUPT_6              = 110 + NPCM750_IRQ_GIC_START,   /* PWM module 6                                       */
	PWM_INTERRUPT_7              = 111 + NPCM750_IRQ_GIC_START,   /* PWM module 7                                       */

	EMC2RX_INTERRUPT             = 114 + NPCM750_IRQ_GIC_START,   /* EMC2 Rx module                                     */
	EMC2TX_INTERRUPT             = 115 + NPCM750_IRQ_GIC_START,   /* EMC2 Tx module                                     */
	GPIO_INTERRUPT0              = 116 + NPCM750_IRQ_GIC_START,   /* GPIO module outputing GPIO0-31                     */
	GPIO_INTERRUPT1              = 117 + NPCM750_IRQ_GIC_START,   /* GPIO module outputing GPIONPCM750_IRQ_GIC_START-63                    */
	GPIO_INTERRUPT2              = 118 + NPCM750_IRQ_GIC_START,   /* GPIO module outputing GPIO64-95                    */
	GPIO_INTERRUPT3              = 119 + NPCM750_IRQ_GIC_START,   /* GPIO module outputing GPIO96-127                   */
	GPIO_INTERRUPT4              = 120 + NPCM750_IRQ_GIC_START,   /* GPIO module outputing GPIO128-159                  */
	GPIO_INTERRUPT5              = 121 + NPCM750_IRQ_GIC_START,   /* GPIO module outputing GPIO160-191                  */
	GPIO_INTERRUPT6              = 122 + NPCM750_IRQ_GIC_START,   /* GPIO module outputing GPIO192-223                  */
	GPIO_INTERRUPT7              = 123 + NPCM750_IRQ_GIC_START,   /* GPIO module outputing GPIO224-255                  */

	PCIE_RC_INTERRUPT            = 127 + NPCM750_IRQ_GIC_START,   /* PCIe Root Complex  (combined A-D and MSI)          */

	/* GIC_INTERRUPT_NUM */
} irq_list_t;

#define KCS_HIB_INT KCS_HIB_INTERRUPT
/*---------------------------------------------------------------------------------------------------------*/
/* GPIOs definition table                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_DEFINITION_TABLE                                                                                                                                                                       \
{                                                                                                                                                                                                   \
	/* Port 0: GPIO 0-15 */                                                                                                                                                                         \
	GPIO_TABLE_ENTRY( 0, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(0),  GPIO_CAP_DEBOUNCE(0),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 30, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(1),  GPIO_CAP_DEBOUNCE(1),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 30, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(2),  GPIO_CAP_DEBOUNCE(2),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 30, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(3),  GPIO_CAP_DEBOUNCE(3),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 30, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(4),  GPIO_CAP_DEBOUNCE(4),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 14, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(5),  GPIO_CAP_DEBOUNCE(5),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 14, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(6),  GPIO_CAP_DEBOUNCE(6),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 14, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(7),  GPIO_CAP_DEBOUNCE(7),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 14, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(8),  GPIO_CAP_DEBOUNCE(8),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(9),  GPIO_CAP_DEBOUNCE(9),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(10), GPIO_CAP_DEBOUNCE(10), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 18, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(11), GPIO_CAP_DEBOUNCE(11), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 18, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(12), GPIO_CAP_DEBOUNCE(12), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 24, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(13), GPIO_CAP_DEBOUNCE(13), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 24, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(14), GPIO_CAP_DEBOUNCE(14), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 24, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 0,15, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(15), GPIO_CAP_DEBOUNCE(15), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 24, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 1: GPIO 16-31 */                                                                                                                                                                        \
	GPIO_TABLE_ENTRY( 1, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(0),  GPIO_MUX_REGID_MFSL3, 15, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 1, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(4),  GPIO_MUX_REGID_MFSL3, 13, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 1, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(8),  GPIO_MUX_REGID_MFSL3, 13, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 1, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(12), GPIO_MUX_REGID_MFSL3, 13, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 1, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(16), GPIO_MUX_REGID_MFSL2, 24, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 1, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(20), GPIO_MUX_REGID_MFSL2, 25, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 1, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(24), GPIO_MUX_REGID_MFSL2, 26, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 1, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(28), GPIO_MUX_REGID_MFSL2, 27, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 1, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(16), GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 28, 1, 0,  GPIO_MUX_REGID_MFSL3, 18, 1, 0), \
	GPIO_TABLE_ENTRY( 1, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(17), GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 29, 1, 0,  GPIO_MUX_REGID_MFSL3, 18, 1, 0), \
	GPIO_TABLE_ENTRY( 1,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  2, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 1,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  2, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 1,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  1, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 1,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  1, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 1,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  0, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 1,15, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  0, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 2: GPIO 32-47 */                                                                                                                                                                        \
	GPIO_TABLE_ENTRY( 2, 0, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  3, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 1, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  4, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  5, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 29, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 28, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 2,15, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 3: GPIO 48-63 */                                                                                                                                                                        \
	GPIO_TABLE_ENTRY( 3, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 11, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 3, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 11, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 11, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 11, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 11, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 11, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 11, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 11, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 12, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 13, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 13, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 30, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 31, 1, 0,  GPIO_MUX_REGID_MFSL3, 19, 1, 0), \
	GPIO_TABLE_ENTRY( 3,13, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 3,14, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 3,15, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 4: GPIO 64-79 */                                                                                                                                                                        \
	GPIO_TABLE_ENTRY( 4, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  0, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  1, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  2, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  3, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  4, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  6, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  7, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  8, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 11, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 13, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 14, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 4,15, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 15, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 5: GPIO 80-94 */                                                                                                                                                                        \
	GPIO_TABLE_ENTRY( 5, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 16, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 5, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 17, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 5, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 18, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 5, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL2, 19, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 5, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 14, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 5, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 14, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 5, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 14, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 5, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 14, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 5, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 14, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 5, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 14, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 5,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 15, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY( 5,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 16, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 5,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 16, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 5,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 17, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 5,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 17, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* GPIO 95 Undefined */                                                                                                                                                                         \
	GPIO_TABLE_ENTRY( 5,15, GPIO_CAP_NO_INPUT, GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 6: GPIO 96-113 */                                                                                                                                                                       \
	GPIO_TABLE_ENTRY( 6, 0, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 1, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 2, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 3, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 4, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 5, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 6, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 7, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 8, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6, 9, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6,10, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6,11, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6,12, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6,13, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6,14, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6,15, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6,16, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 6,17, GPIO_CAP_INPUT,    GPIO_CAP_NO_OUTPUT, GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 7: GPIO 114-127 */                                                                                                                                                                      \
	GPIO_TABLE_ENTRY( 7, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  6, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  6, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  7, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  7, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  8, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1,  8, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 7,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 8: GPIO 128-143 */                                                                                                                                                                      \
	GPIO_TABLE_ENTRY( 8, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(0),  GPIO_CAP_DEBOUNCE(0),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  6, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(1),  GPIO_CAP_DEBOUNCE(1),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  7, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(2),  GPIO_CAP_DEBOUNCE(2),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  7, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(3),  GPIO_CAP_DEBOUNCE(3),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  3, 2, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(4),  GPIO_CAP_DEBOUNCE(4),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 15, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(5),  GPIO_CAP_DEBOUNCE(5),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 15, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(6),  GPIO_CAP_DEBOUNCE(6),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 15, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(7),  GPIO_CAP_DEBOUNCE(7),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 15, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(8),  GPIO_CAP_DEBOUNCE(8),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(9),  GPIO_CAP_DEBOUNCE(9),  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(10), GPIO_CAP_DEBOUNCE(10), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(11), GPIO_CAP_DEBOUNCE(11), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(12), GPIO_CAP_DEBOUNCE(12), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(13), GPIO_CAP_DEBOUNCE(13), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(14), GPIO_CAP_DEBOUNCE(14), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 8,15, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(15), GPIO_CAP_DEBOUNCE(15), GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 12, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 9: GPIO 144-159 */                                                                                                                                                                      \
	GPIO_TABLE_ENTRY( 9, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(0),  GPIO_MUX_REGID_MFSL2, 20, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(4),  GPIO_MUX_REGID_MFSL2, 21, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(8),  GPIO_MUX_REGID_MFSL2, 22, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(12), GPIO_MUX_REGID_MFSL2, 23, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(16), GPIO_MUX_REGID_MFSL3, 11, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(20), GPIO_MUX_REGID_MFSL3, 11, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(24), GPIO_MUX_REGID_MFSL3, 11, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_BLINK(28), GPIO_MUX_REGID_MFSL3, 11, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(16), GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_EVENT(17), GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY( 9,15, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 10, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 10: GPIO 160-175 */                                                                                                                                                                     \
	GPIO_TABLE_ENTRY(10, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 21, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 26, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 31, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 26, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 26, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 26, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 26, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 26, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 16, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  0, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 22, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  1, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  1, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  2, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  2, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(10,15, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  3, 2, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 11: GPIO 176-191 */                                                                                                                                                                     \
	GPIO_TABLE_ENTRY(11, 0, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  3, 2, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  3, 2, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  8, 1, 0,  GPIO_MUX_REGID_MFSL3, 31, 1, 0), \
	GPIO_TABLE_ENTRY(11, 8, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11, 9, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11,10, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11,11, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11,12, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11,13, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11,14, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(11,15, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  5, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
                                                                                                                                                                                                    \
	/* Port 12: GPIO 192-207 */                                                                                                                                                                     \
	GPIO_TABLE_ENTRY(12, 0, GPIO_CAP_NO_INPUT, GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 17, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12, 1, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12, 2, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12, 3, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12, 4, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12, 5, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12, 6, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12, 7, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12, 8, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL1, 14, 1, 0,  GPIO_MUX_REGID_MFSL3, 15, 1, 0), \
	GPIO_TABLE_ENTRY(12, 9, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3,  9, 1, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12,10, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_NONE,   0, 0, 0,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12,11, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 15, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12,12, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 22, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12,13, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 22, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12,14, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 22, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
	GPIO_TABLE_ENTRY(12,15, GPIO_CAP_INPUT,    GPIO_CAP_OUTPUT,    GPIO_CAP_NO_EVENT,  GPIO_CAP_NO_DEBOUNCE,  GPIO_CAP_NO_BLINK,  GPIO_MUX_REGID_MFSL3, 22, 1, 1,  GPIO_MUX_REGID_NONE,   0, 0, 0), \
}


/*---------------------------------------------------------------------------------------------------------*/
/* The non-cached area is not HW, we spare 32MB for linux to use it                                        */
/* Map 32MB at the last 48MB of 2GB (since GFX takes 16MB at the end) for non-cached area,                 */
/* since DDR is wrapping around this should work even if actual DDR is smaller                             */
/*---------------------------------------------------------------------------------------------------------*/
#define NON_CACHED_PHYS_BASE_ADDR       0x7D000000 /* _2GB_ - _16MB_ - _32MB_ = 0x7D000000 */


#ifndef DYNAMIC_BASE_ADDRESS
#define NON_CACHED_BASE_ADDR(module)    NON_CACHED_PHYS_BASE_ADDR(module)
#else
#define NON_CACHED_BASE_ADDR(module)    NON_CACHED_VIRT_BASE_ADDR(module)
#endif

/* Defining IO Memory continuesly mapped blocks                                                            */
/*---------------------------------------------------------------------------------------------------------*/
                                    /* Map:Phisical Addr | Virtual Addr | Block Size     */
                                    /* -----------------------------------------------    */
#define IOMEMORY_BLOCKS             {                                                                \
                                        {   VDM_PHYS_BASE_ADDR, IOMEMORY(VDM_PHYS_BASE_ADDR), 0x00001000   },        \
                                        {   NON_CACHED_PHYS_BASE_ADDR, IOMEMORY(NON_CACHED_PHYS_BASE_ADDR), 0x02000000   },        \
                                        {   AHB1_BASE_ADDR, IOMEMORY(AHB1_BASE_ADDR), 0x00020000   },        \
                                        {   APB2_BASE_ADDR, IOMEMORY(APB2_BASE_ADDR), 0x00020000   },        \
                                        {   APB3_BASE_ADDR, IOMEMORY(APB3_BASE_ADDR), 0x00010000   },        \
                                        {   APB4_BASE_ADDR, IOMEMORY(APB4_BASE_ADDR), 0x00010000   },        \
                                        {   APB5_BASE_ADDR, IOMEMORY(APB5_BASE_ADDR), 0x00005000   },        \
                                        {   A9_BASE_ADDR,   IOMEMORY(A9_BASE_ADDR),   0x00004000   },        \
                                        {   DAP_BASE_ADDR,  IOMEMORY(DAP_BASE_ADDR),  0x00100000   },        \
                                        {   AHB8_BASE_ADDR, IOMEMORY(AHB8_BASE_ADDR), 0x00060000   },        \
                                        {   FIU0_BASE_ADDR, IOMEMORY(FIU0_BASE_ADDR), 0x00002000   },        \
                                        {   FIU3_BASE_ADDR, (FIU0_BASE_ADDR + 0x3000 - 0x01000000),  0x00001000   },        \
                                        {   RAM2_BASE_ADDR, IOMEMORY(RAM2_BASE_ADDR), 0x00030000   },        \
                                    }


#endif /*__NPCM750_CHIP_H */

