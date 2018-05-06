/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   gcr_drv.c                                                                                             */
/*            This file contains GCR (Global Conrol Registers) module access functions                     */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define GCR_C

#include "../../../Common/hal_common.h"
#include "../../../Chips/chip.h"

#include "gcr_drv.h"
#include "gcr_regs.h"

#include "../../uart/uart_if.h"
#include "../../gpio/gpio_if.h"
#include "../../strp/strp_if.h"
#include "../../pspi/pspi_if.h"
#include "../../sd/sd_if.h"
#include "../../fiu/fiu_if.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Register access macroes                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
#define POR_RESET()         (READ_REG_FIELD(INTCR2, INTCR2_PORST))          /*Reset source was POR (powerOn) reset */
#define COR_RESET()         (READ_REG_FIELD(INTCR2, INTCR2_CORST))          /*Reset source was COR reset */
#define MARK_CFG_DONE()      SET_REG_FIELD(INTCR2, INTCR2_CFG_DONE, 1)         /*Chip configuration (MC, CLK, Flash, etc') is done */
#define CFG_IS_DONE()       (READ_REG_FIELD(INTCR2, INTCR2_CFG_DONE))            /*Chip configuration (MC, CLK, Flash, etc') is done */
#define ENABLE_JTAG()        SET_REG_FIELD(FLOCKR1, FLOCKR1_JTAGDIS, 0)     /*Rnables J-Tag */
#define DISABLE_JTAG()       SET_REG_FIELD(FLOCKR1, FLOCKR1_JTAGDIS, 1)       /*Disables J-Tag */


#if defined (UART_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_Uart                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  CoreSP -                                                                               */
/*                  redirection_mode -                                                                     */
/*                  sp1 -                                                                                  */
/*                  sp2 -                                                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects UART muxing                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_Uart(UINT redirection_mode, BOOLEAN CoreSP, BOOLEAN sp1, BOOLEAN sp2)
{
	/* 111 combination is reserved: */
	if (redirection_mode >= 7)
        return;

	SET_REG_FIELD(SPSWC, SPSWC_SPMOD, redirection_mode & 0x7); /* redirection mode number in enum == value at register */

	if (CoreSP)
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_BSPSEL,  1);
        SET_REG_FIELD(MFSEL4, MFSEL4_BSPASEL, 1); /* use BSPRXD + BSPTXD */
	}
	if (sp1)
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_HSI1SEL, 1);
        SET_REG_FIELD(MFSEL4, MFSEL4_BSPASEL, 0); /* Select TXD2+RXD2 */
	}
	if (sp2)
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_HSI2SEL, 1);
        SET_REG_FIELD(MFSEL4, MFSEL4_BSPASEL, 0); /* Select TXD2+RXD2 */
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Core_Uart_Get                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns current UART number to be used by UBOOT\LINUX                     */
/*---------------------------------------------------------------------------------------------------------*/
UINT GCR_Core_Uart_Get (void)
{
	UART_DEV_T core_uart = UART0_DEV;

	if (UART_UartIsInit(UART0_DEV) == TRUE)
		core_uart = UART0_DEV;
	else
		core_uart = UART3_DEV;

	return core_uart;

}
#endif /* UART_MODULE_TYPE */



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_SPIFash                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum  - SPI Flash device number                                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SPI Flash muxing                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_SPIFash(UINT devNum)
{
	switch (devNum)
	{
        case 1:     SET_REG_FIELD(MFSEL1, MFSEL1_S0CS1SEL, 1);    break;
        case 2:     SET_REG_FIELD(MFSEL1, MFSEL1_S0CS2SEL, 1);    break;
        case 3:     SET_REG_FIELD(MFSEL1, MFSEL1_S0CS3SEL, 1);    break;
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_MuxFIU                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum  - SPI Flash device number                                                      */
/*                  csXEnable - enable CS.                                                                 */
/*                  quadMode -  is Quad mode                                                               */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects FIU muxing . Notice it does not disable a CS!                     */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GCR_MuxFIU (   FIU_MODULE_T      devNum,
                          BOOLEAN           cs0Enable,
                          BOOLEAN           cs1Enable,
                          BOOLEAN           cs2Enable,
                          BOOLEAN           cs3Enable,
                          BOOLEAN           quadMode)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* CS2 and CS3 are multiplexed with spid2 and spid3. So when in quad mode, can't use CS2 and CS3       */
	/*-----------------------------------------------------------------------------------------------------*/

	switch (devNum)
	{
        case FIU_MODULE_0:
        {
            /* config CS */
            if ( cs0Enable == TRUE)
            {
                /* nothing to do. */
            }

            /* config CS1 */
            if ( cs1Enable == TRUE)
            {
                SET_REG_FIELD(MFSEL1, MFSEL1_S0CS1SEL, 1);
            }

            /* Config CS2 */
            if ( cs2Enable == TRUE)
            {
                SET_REG_FIELD(MFSEL1, MFSEL1_S0CS2SEL, 1);
            }

            /* Config CS3 */
            if ( cs3Enable == TRUE)
            {
                SET_REG_FIELD(MFSEL1, MFSEL1_S0CS3SEL, 1);
            }


            /* select io bus width (1/2/4  <=> single/dual/quad ) */
            if (quadMode == TRUE)
            {
                SET_REG_FIELD (MFSEL1, MFSEL1_S0CS2SEL, 0);  /*  0: GPIO33/SPI0D2 ,1: nSPI0CS2     */
                SET_REG_FIELD (MFSEL1, MFSEL1_S0CS3SEL, 0);  /*  0: GPIO34/SPI0D3 ,1: nSPI0CS3     */
                SET_REG_FIELD (MFSEL4, MFSEL4_SP0QSEL , 1);  /* enable quad */
            }
            break;
        }
        case FIU_MODULE_3:
        {
            /* Select SPI3 */
            SET_REG_FIELD(MFSEL4, MFSEL4_SP3SEL, 1);

            /* config CS */
            if ( cs0Enable == TRUE)
            {
                /* nothing to do. */
            }

            /* config CS1 */
            if ( cs1Enable == TRUE)
            {
                SET_REG_FIELD(MFSEL4, MFSEL4_S3CS1SEL, 1);
            }

            /* Config CS2 */
            if ( cs2Enable == TRUE)
            {
                SET_REG_FIELD(MFSEL4, MFSEL4_S3CS2SEL, 1);
            }

            /* Config CS3 */
            if ( cs3Enable == TRUE)
            {
                SET_REG_FIELD(MFSEL4, MFSEL4_S3CS3SEL, 1);
            }


            /* select io bus width (1/2/4  <=> single/dual/quad ) */
            if (quadMode == TRUE)
            {
                SET_REG_FIELD (MFSEL4, MFSEL4_S3CS2SEL, 0);  /*  0: GPIO33/SPI0D2 ,1: nSPI0CS2     */
                SET_REG_FIELD (MFSEL4, MFSEL4_S3CS3SEL, 0);  /*  0: GPIO34/SPI0D3 ,1: nSPI0CS3     */
                SET_REG_FIELD (MFSEL4, MFSEL4_SP3QSEL , 1);  /* enable quad */
            }
            break;
        }
        case FIU_MODULE_X:
        {
            /* config CS */
            if ( cs0Enable == TRUE)
            {
                /* nothing to do. */
            }

            /* config CS1 */
            if ( cs1Enable == TRUE)
            {
                SET_REG_FIELD(MFSEL4, MFSEL4_SXCS1SEL, 1);
            }

            SET_REG_FIELD(MFSEL4, MFSEL4_SPXSEL, 1);
            break;
        }

        default:
           break;
	}
	return HAL_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_RMII                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects RMII mux                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_RMII(UINT devNum)
{
	if(devNum == 0)            /* ETH0 - EMC1 */
	{
        SET_REG_FIELD(MFSEL3, MFSEL3_RMII1SEL, 1);
        SET_REG_FIELD(MFSEL1, MFSEL1_R1MDSEL, 1);
        SET_REG_FIELD(MFSEL1, MFSEL1_R1ERRSEL, 1);
        SET_REG_FIELD(INTCR,  INTCR_R1EN, 1);
	}
	else if(devNum == 1)       /* ETH1 - EMC2 */
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_RMII2SEL, 1);
        SET_REG_FIELD(MFSEL1, MFSEL1_R2MDSEL, 1);
        SET_REG_FIELD(MFSEL1, MFSEL1_R2ERRSEL, 1);
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_SetDacSense                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Makes the trigger for sense circuits to be GFX core output signal (miscOUT2)           */
/*                  instead of VSYNC falling edge                                                          */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_SetDacSense(void)
{
	SET_REG_FIELD(INTCR, INTCR_DAC_SNS, 1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_GMII                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects GMII mux                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_GMII(UINT devNum)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting the RGMII Muxing                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum == 2)            /* ETH2 - GMAC1 */
	{
        SET_REG_FIELD(MFSEL4, MFSEL4_RG1SEL, 1);
        SET_REG_FIELD(MFSEL4, MFSEL4_RG1MSEL, 1);
	}
	else if (devNum == 3)       /* ETH3 - GMAC2 */
	{
        SET_REG_FIELD(MFSEL4, MFSEL4_RG2SEL, 1);
        SET_REG_FIELD(MFSEL4, MFSEL4_RG2MSEL, 1);
	}

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_SIOXH                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  enable -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SIOXH mux                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_SIOXH(BOOLEAN enable)
{
	if (enable)
	{
        SET_REG_FIELD(MFSEL3, MFSEL3_IOXHSEL, 1);
	}
	else
	{
        SET_REG_FIELD(MFSEL3, MFSEL3_IOXHSEL, 0);
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_PSPI                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - Peripheral SPI device number                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs muxing for Peripheral SPI device                                 */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_PSPI(UINT devNum)
{
	switch (devNum)
	{
        case PSPI1_DEV:     SET_REG_FIELD(MFSEL3, MFSEL3_PSPI1SEL, 0x2);          break;
        case PSPI2_DEV:     SET_REG_FIELD(MFSEL3, MFSEL3_PSPI2SEL, 0x1);          break;
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_GSPI                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs muxing for Graphics Core SPI Signals                             */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_GSPI(void)
{
	SET_REG_FIELD(MFSEL1, MFSEL1_GSPISEL, 1);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_PowerOn_GetMemorySize_limited                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns configured DDR memory size                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 GCR_PowerOn_GetMemorySize_limited(void)
{


	UINT32 pwronRAMsize =  READ_REG_FIELD(INTCR3, INTCR3_GMMAP);


	switch(pwronRAMsize)
	{
        case 0:
            return 0x08000000;        /* 128 MB. */
        case 1:
            return 0x10000000;        /* 256 MB. */
        case 2:
        /* 3 and 4 should be 1 GB and 2 GB but as a workaround
           to correctly load linux we set it as 512 MB         */
        case 3:
        case 4:
            return 0x20000000;        /* 512 MB. */

        default:
           break;
	}

	return 0;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_PowerOn_GetMemorySize                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns configured DDR memory size                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 GCR_PowerOn_GetMemorySize(void)
{
	UINT32 pwronRAMsize =  READ_REG_FIELD(INTCR3, INTCR3_GMMAP);

	switch(pwronRAMsize)
	{
        case 0:
            return 0x08000000;        /* 128 MB. */
        case 1:
            return 0x10000000;        /* 256 MB. */
        case 2:
            return 0x20000000;        /* 512 MB. */
        case 3:
            return 0x40000000;        /* 1GB. */
        case 4:
            return 0x80000000;        /* 2GB. */

        default:
           break;
	}

	return 0;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_GPIO                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num -                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs mux for given GPIO                                               */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_GPIO(UINT gpio_num)
{
	GPIO_MUX_REGID_T    mux_reg1, mux_reg2;
	GPIO_MUX_FIELD_T    mux1, mux2;


	/*-----------------------------------------------------------------------------------------------------*/
	/* Getting Mux information from GPIO module                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	GPIO_GetMuxInfo(gpio_num, &mux_reg1, &mux1, &mux_reg2, &mux2);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Bypass for using DEFS.H macros                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	#define GCR_GPIO_MUX_FIELD1  mux1.position, mux1.size
	#define GCR_GPIO_MUX_FIELD2  mux2.position, mux2.size


	/*-----------------------------------------------------------------------------------------------------*/
	/* Muxing                                                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (mux1.size != 0)
	{
        SET_REG_FIELD(MFSEL(mux_reg1), GCR_GPIO_MUX_FIELD1, mux1.value);
	}

	if (mux2.size != 0)
	{
        SET_REG_FIELD(MFSEL(mux_reg2), GCR_GPIO_MUX_FIELD2, mux2.value);
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_PinPullUpDown                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num    -                                                                          */
/*                  enable      -                                                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets internal Pull Up/Down resistor for the given pin                     */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_PinPullUpDown(UINT32 gpio_num, BOOLEAN enable)
{
	UINT    port_num = 0;
	UINT    port_bit = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Retriving GPIO info                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	GPIO_GetGPIOInfo(gpio_num, &port_num, &port_bit);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting Pull Up/Down configuration                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	if (enable)
	{
        SET_REG_BIT(GPIOPxPE(port_num), port_bit);
	}
	else
	{
        CLEAR_REG_BIT(GPIOPxPE(port_num), port_bit);
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_SD                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                 sdNum    - [in], SD device number                                                       */
/*                 devType  - [in], SD1 device type (SD or MMC or MMC8)                                    */
/*                                                                                                         */
/* Returns:         BMC HAL Error code                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SD mux                                                            */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GCR_Mux_SD(SD_DEV_NUM_T sdNum, SD_DEV_TYPE_T sdType)
{
	UINT32          SDnIRVn;
	if (sdNum >= SD_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	if (sdNum == SD1_DEV)
	{
		/*-----------------------------------------------------------------------------------------------------*/
        /* SD Fix internal straps reset value                                                                  */
        /*-----------------------------------------------------------------------------------------------------*/
        SDnIRVn = REG_READ(SD1IRV2);
        SET_VAR_FIELD(SDnIRVn, SDnIRV2_corecfg_sdr50support, 0);   /* disable sdr50 support */
        SET_VAR_FIELD(SDnIRVn, SDnIRV2_corecfg_baseclkfreq, 0x32); /* Base Clock Frequency is 50MHz (0x32) */
        REG_WRITE(SD1IRV2, SDnIRVn);

        SDnIRVn = REG_READ(SD1IRV1);
        SET_VAR_FIELD(SDnIRVn, SDnIRV1_corectrl_otapdlyena,   1);    /* Output Tap Delay Enable */
        SET_VAR_FIELD(SDnIRVn, SDnIRV1_corectrl_otapdlysel, 0xF);    /* Output Tap Delay: 15 = ~3ns */
        REG_WRITE(SD1IRV1, SDnIRVn);

		/*-----------------------------------------------------------------------------------------------------*/
        /* SD Mux select                                                                                       */
        /*-----------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(MFSEL3, MFSEL3_SD1SEL, 1);
	}
	else if (sdNum == SD2_DEV)
	{
		/*-----------------------------------------------------------------------------------------------------*/
        /* eMMC Fix internal straps reset value                                                                */
        /*-----------------------------------------------------------------------------------------------------*/
        SDnIRVn = REG_READ(SD2IRV2);
        SET_VAR_FIELD(SDnIRVn, SDnIRV2_corecfg_sdr50support, 0);   /* disable sdr50 support */
        SET_VAR_FIELD(SDnIRVn, SDnIRV2_corecfg_baseclkfreq, 0x32); /* Base Clock Frequency is 50MHz (0x32) */
        REG_WRITE(SD2IRV2, SDnIRVn);

        SDnIRVn = REG_READ(SD2IRV1);
        SET_VAR_FIELD(SDnIRVn, SDnIRV1_corectrl_otapdlyena,   1);    /* Output Tap Delay Enable */
        SET_VAR_FIELD(SDnIRVn, SDnIRV1_corectrl_otapdlysel, 0xF);    /* Output Tap Delay: 15 = ~3ns */
        REG_WRITE(SD2IRV1, SDnIRVn);

		/*-----------------------------------------------------------------------------------------------------*/
        /* eMMC Mux select                                                                                     */
        /*-----------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(MFSEL3, MFSEL3_MMCSEL, 1);

        if(GCR_Get_Chip_Version() == POLEG_Z1)
        {
            /* Z1 only */
            SET_REG_FIELD(MFSEL3, MFSEL3_SMB13SEL, 1);
        }
        else
        {
            /* Z2 and up */
            SET_REG_FIELD(MFSEL3, MFSEL3_MMCCDSEL, 0);
            SET_REG_FIELD(MFSEL4, MFSEL4_MMCRSEL, 1);
        }

        SET_REG_FIELD(MFSEL3, MFSEL3_MMC8SEL, sdType == SD_DEV_MMC8); /* 8 bit mux according to sdType */
        SET_REG_FIELD(SD2IRV1, SDnIRV1_corecfg_8bitsupport, sdType == SD_DEV_MMC8); /* 8-Bit Support according to sdType */
	}

	return HAL_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_EmmcBootTimeConfig                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  clk - Card clock frequency (in Hz units)                                               */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates and configures the number of SDCLK clock cycles:               */
/*                   1) In a 50 millisecond time.                                                          */
/*                   1) In a 1 second time.                                                                */
/*                  These values are being used to determine whether Boot Time-Out has occured.            */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_EmmcBootTimeConfig (UINT32 clk)
{
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_EmmcBootStart                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  ack - TRUE means expect the boot acknowledge data, FALSE otherwise.                    */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine initiates a EMMC boot sequence.                                           */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_EmmcBootStart (BOOLEAN ack)
{
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_EmmcBootInvalid                                                                    */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         TRUE if boot is invalid, FALSE otherwise                                               */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine checks whether the current boot process is invalid,                       */
/*                  i.e., one of the following has occured:                                                */
/*                   1) The device did not respond in time with the boot acknowledge sequence or data.     */
/*                   2) An error in data structure occurred.                                               */
/*                   3) A CRC error occurred when reading from the eMMC device.                            */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GCR_EmmcBootInvalid (void)
{
	return 0;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_EmmcBootEnd                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ends a EMMC boot sequence, either when the boot sequence is done,         */
/*                  or an error is discovered.                                                             */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_EmmcBootEnd (void)
{
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_SetVCDInput                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  headNum - Number of the head (1 or 2)                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the intput for the VCD                                            */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_SetVCDInput(GCR_VCD_HEAD_T headNum)
{
	if (headNum == GCR_VCD_HEAD1)
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_DVH1SEL, 1);
	}
	else if (headNum == GCR_VCD_HEAD2)
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_DVH1SEL, 0);
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_SetGFXInput                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  config - GFX configuration                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects the intput for the VCD                                            */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_SetGFXConfig(GCR_GFX_CONFIG_T config)
{
	if (config == GCR_GFX_CONFIG_OUTPUT_FROM_HEAD1)
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, MFSEL1_DVOSEL_OUTPUT_HEAD1);
	}
	else if (config == GCR_GFX_CONFIG_OUTPUT_FROM_HEAD2)
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, MFSEL1_DVOSEL_OUTPUT_HEAD2);
	}
	else if (config == GCR_GFX_CONFIG_INPUT_TO_KVM)
	{
        SET_REG_FIELD(MFSEL1, MFSEL1_DVOSEL, MFSEL1_DVOSEL_INPUT_KVM);
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_ConfigUSBBurstSize                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the USB burst size in USB test register                                     */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_ConfigUSBBurstSize(void)
{

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_ResetTypeIsPor                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns true if reset source was POR (powerOn) reset                                   */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GCR_ResetTypeIsPor()
{
	return POR_RESET();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_ResetTypeIsCor                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns TRUE if and only if a Core reset is performed                                  */
/*                  and FALSE on any other reset type.                                                     */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GCR_ResetTypeIsCor()
{
	return (COR_RESET() & !POR_RESET());
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_MarkCfgDone                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Sets a flag in scratch register specifying that CLK/DDR configuration were performed   */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_MarkCfgDone()
{
	MARK_CFG_DONE();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_CfgWasDone                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns true if a flag in scratch register indicates                                   */
/*                  that CLK/DDR configuration was already done                                            */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GCR_CfgWasDone()
{
	return CFG_IS_DONE();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_EnableJtag                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Enable Jtag access                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_EnableJtag()
{
	ENABLE_JTAG() ;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_DisableJtag                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Disable Jtag access                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_DisableJtag()
{
	DISABLE_JTAG();
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_DisableModules                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  mdlr -  Value to write to MDLR register                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Writes to the MDLR register which determines which modules to disable                  */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_DisableModules(UINT32 mdlr)
{
	REG_WRITE(MDLR, mdlr);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Get_Chip_Version                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                                                                                                         */
/* Returns:         PDID value                                                                             */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the chip version (POLEG_VERSION_Z1/ POLEG_VERSION_Z2              */
/*                                                         POLEG_VERSION_A1)                               */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 GCR_Get_Chip_Version(void)
{
	return REG_READ(PDID);
}



#undef GCR_C

