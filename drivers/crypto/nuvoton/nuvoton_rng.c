/*---------------------------------------------------------------------------------------------------------*/
/*	Nuvoton Technology Corporation Confidential 														   */
/*																										   */
/*	Copyright (c)      2016 by Nuvoton Technology Corporation											   */
/*	All rights reserved 																				   */
/*																										   */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:																						   */
/*	 nuvoton_rng.c																						   */
/*			  This file contains HW IF to Nuvoton BMC RNG engine                				    	   */
/*---------------------------------------------------------------------------------------------------------*/

#ifdef CONFIG_NUVOTON_POLEG_RNG

#include <common.h>
#include <asm/io.h>
#include "nuvoton_rng.h"

/*#define DEBUG_CMD     // Comment when not needed */
#ifdef DEBUG_CMD
	#define DBG_MSG(f, x...)     printf(f, ## x)
#else
	#define DBG_MSG(f, x...)
#endif


static volatile nuvoton_bmc_rng *rng_dev = (nuvoton_bmc_rng *)RNG_BASE_ADDR;


void poleg_rng_init(void)
{
	int init;

	/*-----------------------------------------------------------------------------------------------------*/
	/* check if rng  enabled                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	init = readb(&(rng_dev->RNGCS));
	if ((init & RNG_ENABLE) == 0)
	{
        DBG_MSG("Nuvuton BMC init RNG mod\n");
		/* init rng */
		writeb(RNG_CLK_SET(RNG_CLKP_20_25_MHz) | RNG_ENABLE, &(rng_dev->RNGCS));
		writeb(RNGMODE_M1ROSEL,          &(rng_dev->RNGMODE));
	}

}


void poleg_rng_disable(void)
{
	/* disable rng */
	writeb(0, &(rng_dev->RNGCS));
	writeb(0, &(rng_dev->RNGMODE));
}


void srand(unsigned int seed)
{
	/* no need to seed for now, TODO: revisit this*/
	return;
}


unsigned int rand_r(unsigned int *seedp)
{
	int  i;
	unsigned int ret_val = 0;


	poleg_rng_init();

	/* Wait for RNG done ( 4 bytes) */

	for(i=0; i<4 ; i++){
          while ((readb(&rng_dev->RNGCS ) & RNG_DATA_VALID) == 0); /*wait until DVALID is set */
          ret_val |= (((unsigned int)readb(&rng_dev->RNGD) & 0x000000FF) << (i*8));
	}

	DBG_MSG("Nuvuton BMC RNG= 0x%lx\t\t", ret_val);

	return ret_val;
}


unsigned int rand(void)
{
	return rand_r(0);
}


#endif /* CONFIG_NUVOTON_POLEG_RNG */
