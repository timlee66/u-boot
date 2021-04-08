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

#ifndef __POLEG_GCR_H_
#define __POLEG_GCR_H_

/* On-Chip POLEG NPCM750 VERSIONS */
#define POLEG_Z1                    0x00A92750
#define POLEG_A1                    0x04A92750
#define POLEG_NPCM750				0x00000000
#define POLEG_NPCM730				0x00300395
#define POLEG_NPCM710				0x00200380

/* Multiple Function Pin Select Register 1 (MFSEL1) */
#define MFSEL1_SIRQSE               31
#define MFSEL1_IOX1SEL              30
#define MFSEL1_DVH1SEL              27
#define MFSEL1_LPCSEL               26
#define MFSEL1_PECIB                25
#define MFSEL1_GSPISEL              24
#define MFSEL1_SMISEL               22
#define MFSEL1_CLKOSEL              21
#define MFSEL1_DVOSEL               18
#define MFSEL1_KBCICSEL             17
#define MFSEL1_R2MDSEL              16
#define MFSEL1_R2ERRSEL             15
#define MFSEL1_RMII2SEL             14
#define MFSEL1_R1MDSEL              13
#define MFSEL1_R1ERRSEL             12
#define MFSEL1_HSI2SEL              11
#define MFSEL1_HSI1SEL              10
#define MFSEL1_BSPSEL               9
#define MFSEL1_SMB2SEL              8
#define MFSEL1_SMB1SEL              7
#define MFSEL1_SMB0SEL              6
#define MFSEL1_S0CS3SEL             5
#define MFSEL1_S0CS2SEL             4
#define MFSEL1_S0CS1SEL             3
#define MFSEL1_SMB5SEL              2
#define MFSEL1_SMB4SEL              1
#define MFSEL1_SMB3SEL              0

/* Multiple Function Pin Select Register 3 (MFSEL3) */
#define MFSEL3_HSDVOSEL             26
#define MFSEL3_MMCCDSEL             25
#define MFSEL3_GPOCSEL              22
#define MFSEL3_WDO2SEL              20
#define MFSEL3_WDO1SEL              19
#define MFSEL3_IOXHSEL              18
#define MFSEL3_PCIEPUSE             17
#define MFSEL3_CLKRUNSEL            16
#define MFSEL3_IOX2SEL              14
#define MFSEL3_PSPI2SEL             13
#define MFSEL3_SD1SEL               12
#define MFSEL3_MMC8SEL              11
#define MFSEL3_MMCSEL               10
#define MFSEL3_RMII1SEL             9
#define MFSEL3_SMB15SEL             8
#define MFSEL3_SMB14SEL             7
#define MFSEL3_SMB13SEL             6
#define MFSEL3_SMB12SEL             5
#define MFSEL3_PSPI1SEL             3
#define MFSEL3_SMB7SEL              2
#define MFSEL3_SMB6SEL              1
#define MFSEL3_SCISEL               0

/* Multiple Function Pin Select Register 4 (MFSEL4) */
#define MFSEL4_SMB11DDC             29
#define MFSEL4_SXCS1SEL             28
#define MFSEL4_SPXSEL               27
#define MFSEL4_RG2SEL               24
#define MFSEL4_RG2MSEL              23
#define MFSEL4_RG1SEL               22
#define MFSEL4_RG1MSEL              21
#define MFSEL4_SP3QSEL              20
#define MFSEL4_S3CS3SEL             19
#define MFSEL4_S3CS2SEL             18
#define MFSEL4_S3CS1SEL             17
#define MFSEL4_SP3SEL               16
#define MFSEL4_SP0QSEL              15
#define MFSEL4_SMB11SEL             14
#define MFSEL4_SMB10SEL             13
#define MFSEL4_SMB9SEL              12
#define MFSEL4_SMB8SEL              11
#define MFSEL4_DBGTRSEL             10
#define MFSEL4_CKRQSEL              9
#define MFSEL4_ESPISEL              8
#define MFSEL4_MMCRSEL              6
#define MFSEL4_SD1PSEL              5
#define MFSEL4_ROSEL                4
#define MFSEL4_ESPIPMESEL           2
#define MFSEL4_BSPASEL              1
#define MFSEL4_JTAG2SEL             0

/* USB PHY1 Control Register (USB1PHYCTL) */
#define USB1PHYCTL_RS				28
/* USB PHY2 Control Register (USB2PHYCTL) */
#define USB2PHYCTL_RS				28

/* Integration Control Register (INTCR) */
#define  INTCR_DUDKSMOD             30
#define  INTCR_DDC3I                29
#define  INTCR_KVMSI                28
#define  INTCR_DEHS                 27
#define  INTCR_GGPCT2_0             24
#define  INTCR_SGC2                 23
#define  INTCR_DSNS_TRIG            21
#define  INTCR_DAC_SNS              20
#define  INTCR_SGC1                 19
#define  INTCR_LDDRB                18
#define  INTCR_GIRST                17
#define  INTCR_DUDKSEN              16
#define  INTCR_DACOFF               15
#define  INTCR_DACSEL               14
#define  INTCR_GFXINT               12
#define  INTCR_DACOSOVR             10
#define  INTCR_GFXIFDIS             8
#define  INTCR_GFXINT2              7
#define  INTCR_VGAIOEN              6
#define  INTCR_R1EN                 5
#define  INTCR_PSPIFEN              4
#define  INTCR_HIFEN                3
#define  INTCR_SMBFEN               2
#define  INTCR_MFTFEN               1
#define  INTCR_KCSRST_MODE          0

/* Integration Control Register (INTCR2) */
#define  INTCR2_WDC                   21


/* Integration Control Register 3 (INTCR3) */
#define  INTCR3_PCIRREL             30
#define  INTCR3_GFXRSTDLY           26
#define  INTCR3_GFXRDEN             25
#define  INTCR3_DACSBYOFF           23
#define  INTCR3_RCCORER             22
#define  INTCR3_GFX_ORL0            20
#define  INTCR3_PECIVSEL            19
#define  INTCR3_PCIEDEM             17
#define  INTCR3_USBPHYSW            12
#define  INTCR3_GMMAP               8
#define  INTCR3_FIU_FIX             6
#define  INTCR3_GFXACCDIS           4
#define  INTCR3_HSRDIS              2
#define  INTCR3_BIOSEN              1
#define  INTCR3_HHRDIS              0

/* Serial Ports (SPSWC) */
#define SPSWC_RTSS  7
#define SPSWC_DTRS  6
#define SPSWC_DCDI  5
#define SPSWC_SPMOD 0
#define SPMOD_MASK 7
#define SPMOD_MODE1 0
#define SPMOD_MODE2 1
#define SPMOD_MODE3 2
#define SPMOD_MODE4 3
#define SPMOD_MODE5 4
#define SPMOD_MODE6 5
#define SPMOD_MODE7 6

/* I2C Segment Pin Select Register (I2CSEGSEL) */
#define I2CSEGSEL_S0DECFG			3
#define I2CSEGSEL_S4DECFG			17

/* I2C Segment Control Register (I2CSEGCTL) */
#define I2CSEGCTL_S0DEN				20
#define I2CSEGCTL_S0DWE				21
#define I2CSEGCTL_S4DEN				24
#define I2CSEGCTL_S4DWE				25


/**************************************************************************************************************************/
/*   SDHCn Interface Reset Value Register 1 (SDnIRV1) (reg40_dt) Updated for                                              */
/**************************************************************************************************************************/
#define  SDnIRV1_corecfg_8bitsupport     27              /* 27 (8-Bit Support). Eight-bit support for embedded devices. The default is 1 (Core supports 8bit MMC interface)       */
#define  SDnIRV1_corecfg_maxblklength    25             /* 26-25 corecfg_maxblklength (Maximum Block Length). Maximum block length is supported by the Core.                     */
#define  SDnIRV1_corecfg_timeoutclkunit  24              /* 24 corecfg_timeoutclkunit (Timeout Clock Unit). The timeout clock unit can be either MHz or KHz. The default          */
#define  SDnIRV1_corecfg_timeoutclkfreq  19             /* 23-19 corecfg_timeoutclkfreq (Timeout Clock Frequency). The default is 25 MHz (see corecfg_timeoutclkunit             */
#define  SDnIRV1_corecfg_tuningcount     13             /* 18-13 corecfg_tuningcount (Tuning Count). Selects the looped clock phase (tap). Assumes tuning is used (instead       */
#define  SDnIRV1_test_mode               12              /* 12 test_mode (Test Mode). Enable Test mode. The Test Mode signal is used for DFT purposes. It muxes in the            */
#define  SDnIRV1_corectrl_otapdlysel     8              /* 11-8 corectrl_otapdlysel (Output Tap Delay). The delay of the output clock to the SD (rxclk_out), for SD sampling.    */
#define  SDnIRV1_corectrl_otapdlyena     7               /* 7 corectrl_otapdlyena (Output Tap Delay Enable). Not Supported.                                                       */
#define  SDnIRV1_corectrl_itapchgwin     6               /* 6 corectrl_itapchgwin. Not Supported.                                                                                 */
#define  SDnIRV1_corectrl_itapdlysel     1              /* 5-1 corectrl_itapdlysel (Input Tap Delay). The delay of the looped clock from SD (rxclk_in). It must be configured    */
#define  SDnIRV1_corectrl_itapdlyena     0               /* 0 corectrl_itapdlyena (Input Tap Delay Enable). Used to enable selective Tap delay line on the loop-backed SD         */

/**************************************************************************************************************************/
/*   SDHCn Interface Reset Value Register 2 (SDnIRV2) (reg44_dt) Updated for                                              */
/**************************************************************************************************************************/
#define  SDnIRV2_corecfg_asyncwkupena      30              /* 30 corecfg_asyncwkupena (Asynchronous Wake-Up). Determines the wake-up signal generation mode.                        */
#define  SDnIRV2_corecfg_retuningmodes     26              /* 27-26 corecfg_retuningmodes (Retuning Modes). Must be set to ‘00’ (Mode0 retuning).                                   */
#define  SDnIRV2_corecfg_tuningforsdr50    25              /* 25 corecfg_tuningforsdr50 (Tuning for SDR50). Set to 1 if the Application wants Tuning be used for SDR50              */
#define  SDnIRV2_corecfg_retuningtimercnt  21              /* 24-21 corecfg_retuningtimercnt (Timer Count for Retuning). This is the Timer Count for the Retuning timer for         */
#define  SDnIRV2_corecfg_sdr50support      15              /* 15 corecfg_sdr50support (SDR50 Support). Simple Data Rate: 50 MHz, 50 Mbps.                                           */
#define  SDnIRV2_corecfg_slottype          13              /* 14-13 corecfg_slottype (Slot Type). Used by the card detection. 1 for eMMC, 0 for SD.                                 */
#define  SDnIRV2_corecfg_baseclkfreq       0  

#endif
