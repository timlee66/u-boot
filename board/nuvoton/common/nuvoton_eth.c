/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   nuvoton_eth.c                                                                                         */
/*            This file contains Ehternet module implementation                                            */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

#include <config.h>
#include <common.h>
#include <malloc.h>
#include <net.h>
#include <netdev.h>
#include <phy.h>
#include "miiphy.h"

#include "BMC_HAL/Boards/board.h"

void CLK_ConfigureGMACClock (UINT32 ethNum);
void CLK_ResetGMAC (UINT32 deviceNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Function forward declarations                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int   nuvoton_eth_recv    (struct eth_device *dev);

extern int nuc970_eth_register(ulong base_addr, unsigned int dev_num);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        board_eth_init                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  bis -                                                                                  */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs ethernet module initialization                                   */
/*---------------------------------------------------------------------------------------------------------*/
int board_eth_init(bd_t *bis)
{
	UINT32              devNum;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configuring every ETH device                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
#if defined(CONFIG_NUC970_ETH)
	for(devNum=0; devNum < CHIP_NUM_OF_EMC_ETH; devNum++)
	{
        /*-----------------------------------------------------------------------------------------------------*/
        /* Enable Clock                                                                                   */
        /*-----------------------------------------------------------------------------------------------------*/
        CLK_ConfigureEMCClock(devNum);

        /*-----------------------------------------------------------------------------------------------------*/
        /* Muxing RMII MDIO                                                                                    */
        /*-----------------------------------------------------------------------------------------------------*/
        GCR_Mux_RMII(devNum);

        /*-----------------------------------------------------------------------------------------------------*/
        /* Reset EMC module                                                                                     */
        /*-----------------------------------------------------------------------------------------------------*/
        CLK_ResetEMC(devNum);
        nuc970_eth_register((UINT32)EMC_BASE_ADDR(devNum), devNum);
	}
#endif  /* CONFIG_NUC970_ETH */

#if defined(CONFIG_ETH_DESIGNWARE)
	for(devNum=0; devNum < CHIP_NUM_OF_GMAC_ETH; devNum++)
	{
        /*-----------------------------------------------------------------------------------------------------*/
        /* enabling the GMAC clocks                                                                            */
        /*-----------------------------------------------------------------------------------------------------*/
        CLK_ConfigureGMACClock(devNum+2);

        /*-----------------------------------------------------------------------------------------------------*/
        /* Setting ETH muxing                                                                                  */
        /*-----------------------------------------------------------------------------------------------------*/

        GCR_Mux_GMII(devNum+2);

        /*-----------------------------------------------------------------------------------------------------*/
        /* Reseting the device                                                                                 */
        /*-----------------------------------------------------------------------------------------------------*/
        /*        GMAC_Reset_L(devNum); */
        CLK_ResetGMAC(devNum+2);

        if (designware_initialize( (UINT32)GMAC_BASE_ADDR(devNum), PHY_INTERFACE_MODE_RGMII) < 0)
            return -1;
	}
#endif  /* CONFIG_DESIGNWARE_ETH */

	return 0;
}


