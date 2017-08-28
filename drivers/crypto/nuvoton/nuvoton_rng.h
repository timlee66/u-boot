/*---------------------------------------------------------------------------------------------------------*/
/*	Nuvoton Technology Corporation Confidential 														   */
/*																										   */
/*	Copyright (c)      2016 by Nuvoton Technology Corporation											   */
/*	All rights reserved 																				   */
/*																										   */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:																						   */
/*	 nuvoton_rng.h																						   */
/*			  This file contains HW IF to nuvoton BMC RNG engine            							   */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef __NUVOTON_NPCM750_RNG_H
#define __NUVOTON_NPCM750_RNG_H


#ifdef CONFIG_NUVOTON_POLEG_RNG

/*---------------------------------------------------------------------------------------------------------*/
/* RNG additional IF (rand, srand IF declared on common.h)                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void poleg_rng_init(void);
void poleg_rng_disable(void);


/*---------------------------------------------------------------------------------------------------------*/
/* RNG registers                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define RNG_BASE_ADDR            0xF000B000

#pragma pack(1)
typedef struct nuvoton_bmc_rng {
	u32 RNGCS;
	u32 RNGD;
	u32 RNGMODE;
} nuvoton_bmc_rng;
#pragma pack()



/*---------------------------------------------------------------------------------------------------------*/
/* Core Domain Clock Frequency Range                                                                       */
/* Core domain clock frequency range for the selected value is higher than or equal to the                 */
/* actual Core domain clock frequency                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	RNG_CLKP_80_100_MHz = 0x00, /*default */
	RNG_CLKP_60_80_MHz  = 0x01,
	RNG_CLKP_50_60_MHz  = 0x02,
	RNG_CLKP_40_50_MHz  = 0x03,
	RNG_CLKP_30_40_MHz  = 0x04,
	RNG_CLKP_25_30_MHz  = 0x05,
	RNG_CLKP_20_25_MHz  = 0x06, /* recommended by poleg spec, match APB1. */
	RNG_CLKP_5_20_MHz   = 0x07,
	RNG_CLKP_2_15_MHz   = 0x08,
	RNG_CLKP_9_12_MHz   = 0x09,
	RNG_CLKP_7_9_MHz    = 0x0A,
	RNG_CLKP_6_7_MHz    = 0x0B,
	RNG_CLKP_5_6_MHz    = 0x0C,
	RNG_CLKP_4_5_MHz    = 0x0D,
	RNG_CLKP_3_4_MHz    = 0x0E,
	RNG_NUM_OF_CLKP
}RNG_CLKP_T;





/*---------------------------------------------------------------------------------------------------------*/
/* RNGCS register fields                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define RNG_CLK_SET(clkp)       (clkp << 2)    /* should be 20-25 MHz */
#define RNG_DATA_VALID          (0x01 << 1)
#define RNG_ENABLE              (0x01)


/*---------------------------------------------------------------------------------------------------------*/
/* RNGMODE register fields                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define  RNGMODE_M1ROSEL    (0x02 << 0)        /* 4-0 M1ROSEL (Ring Oscillator Select for Method I).                                                                    */


#endif /*  CONFIG_NUVOTON_POLEG_RNG */

#endif /* __NUVOTON_NPCM750_RNG_H */
