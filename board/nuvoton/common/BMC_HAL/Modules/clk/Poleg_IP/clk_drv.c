/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   clk_drv.c                                                                                             */
/*            This file contains CLK module implementation                                                 */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#include "../../../Chips/chip.h"
#include "../../../Common/hal_common.h"


#include "clk_drv.h"
#include "clk_regs.h"


/*#define CLK_DEBUG */

#ifdef CLK_DEBUG
#ifdef __LINUX_KERNEL_ONLY__
	#define CLOCK_DEBUG(fmt,args...)   printk(fmt ,##args)
	#else
	#define CLOCK_DEBUG(fmt,args...)   printf(fmt ,##args)
	#endif
#else
	#define CLOCK_DEBUG(fmt,args...)
#endif
/*---------------------------------------------------------------------------------------------------------*/
/* Spec predefined values                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* PLLCON 0 possible values:                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_333MHZ_PLLCON0_REG_CFG    0x00A02403
#define CLK_500MHZ_PLLCON0_REG_CFG    0x00282201
#define CLK_600MHZ_PLLCON0_REG_CFG    0x00302201
#define CLK_666MHZ_PLLCON0_REG_CFG    0x00A02203
#define CLK_700MHZ_PLLCON0_REG_CFG    0x001C2101
#define CLK_720MHZ_PLLCON0_REG_CFG    0x00902105
#define CLK_750MHZ_PLLCON0_REG_CFG    0x001E2101
#define CLK_800MHZ_PLLCON0_REG_CFG    0x00202101  /* PLL1 setting for 800 MHz in Z2 and later will have to be 0040_2101h (instead of 0040_2201h for Z1). */
#define CLK_825MHZ_PLLCON0_REG_CFG    0x00212101
#define CLK_850MHZ_PLLCON0_REG_CFG    0x00222101
#define CLK_888MHZ_PLLCON0_REG_CFG    0x03782119
#define CLK_900MHZ_PLLCON0_REG_CFG    0x00242101
#define CLK_950MHZ_PLLCON0_REG_CFG    0x00262101
#define CLK_1000MHZ_PLLCON0_REG_CFG   0x00282101
#define CLK_1066MHZ_PLLCON0_REG_CFG   0x00802103

/*---------------------------------------------------------------------------------------------------------*/
/* PLLCON 1 possible values (notice that PLL1 in Z2 has a divider /2, so OTDV1 is smaller in half          */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_333MHZ_PLLCON1_REG_CFG    0x00A02203
#define CLK_444MHZ_PLLCON1_REG_CFG       0x00A02303
#define CLK_500MHZ_PLLCON1_REG_CFG       0x00282101
#define CLK_600MHZ_PLLCON1_REG_CFG       0x00302101
#define CLK_666MHZ_PLLCON1_REG_CFG_Z1    0x00A02203 /* for Z1 */
#define CLK_666MHZ_PLLCON1_REG_CFG    0x00A02103 /* change OTDV1 */
#define CLK_700MHZ_PLLCON1_REG_CFG    0x00382101
#define CLK_720MHZ_PLLCON1_REG_CFG    0x01202105
#define CLK_750MHZ_PLLCON1_REG_CFG    0x003C2101 /* change FBDV */
#define CLK_800MHZ_PLLCON1_REG_CFG    0x00402101 /* change OTDV1 : PLL1 setting for 800 MHz in Z2 and later will have to be 0040_2101h (instead of 0040_2201h for Z1). */
#define CLK_825MHZ_PLLCON1_REG_CFG    0x00422101 /* change FBDV */
#define CLK_850MHZ_PLLCON1_REG_CFG    0x00442101 /* change FBDV */
#define CLK_900MHZ_PLLCON1_REG_CFG    0x00482101 /* change FBDV */
#define CLK_950MHZ_PLLCON1_REG_CFG    0x004C2101 /* change FBDV */
#define CLK_1000MHZ_PLLCON1_REG_CFG   0x00502101 /* change FBDV */
#define CLK_1066MHZ_PLLCON1_REG_CFG   0x01002103

#define CLK_800MHZ_PLLCON0_REG_CFG_BB  0x00402201
/*---------------------------------------------------------------------------------------------------------*/
/* PLLCON 2 possible values                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
#define CLK_960MHZ_PLLCON2_REG_CFG    0x00C02105


#define LOK_TIMEOUT  100000  /* 4ms if 25 MHz */


/*---------------------------------------------------------------------------------------------------------*/
/* Local definitions                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
/* SD Clock Target frequency */
#define SU60_DESIRED_FREQUENCY      60  /* MHz (dont use _1MHz_) */
#define SU_DESIRED_FREQUENCY        30  /* MHz (dont use _1MHz_) */



/*---------------------------------------------------------------------------------------------------------*/
/* Local Functions                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32  CLK_CalculatePLLFrequency (UINT32 pllVal);
static UINT32  CLK_GetPll0Freq (void);
static UINT32  CLK_GetPll1Freq (void);
static UINT32  CLK_GetPll2Freq (void);
/*static UINT32  CLK_GetPll2Freq (void); */
/*static UINT32  CLK_GetGfxPllFreq (void); */


/*---------------------------------------------------------------------------------------------------------*/
/* Functions Implementation                                                                                */
/*---------------------------------------------------------------------------------------------------------*/

HAL_STATUS WaitForPllLock (UINT32 pll)
{
	volatile UINT32 loki_bit = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read LOKI till PLL is locked or timeout expires                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	while (1)
	{

        switch (pll)
        {
            case PLL0:
                loki_bit = READ_REG_FIELD(PLLCON0, PLLCONn_LOKI);
                break;
            case PLL1:
                loki_bit = READ_REG_FIELD(PLLCON1, PLLCONn_LOKI);
                break;
            case PLL2:
                loki_bit = READ_REG_FIELD(PLLCON2, PLLCONn_LOKI);
                break;
            case PLL_GFX:
                loki_bit = READ_REG_FIELD(PLLCONG, PLLCONG_LOKI);
                break;
            default:
                return HAL_ERROR_BAD_PARAM;
        }

        /*-------------------------------------------------------------------------------------------------*/
        /* PLL is locked, clear and break                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        if (loki_bit == 1)
        {

            switch (pll)
            {
                case PLL0:
                    SET_REG_FIELD(PLLCON0, PLLCONn_LOKS, 1);
                    break;
                case PLL1:
                    SET_REG_FIELD(PLLCON1, PLLCONn_LOKS, 1);
                    break;
                case PLL2:
                    SET_REG_FIELD(PLLCON2, PLLCONn_LOKS, 1);
                    break;
                case PLL_GFX:
                    SET_REG_FIELD(PLLCONG, PLLCONG_LOKS, 1);
                    break;
                default:
                    return HAL_ERROR_BAD_PARAM;
            }

            break;
        }


	}

	return HAL_OK;

}

#if defined (CODE_OF_BB_ONLY) /* not updated */
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureClocks                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  straps -  strap values from the PWRON register (flipped according to FUSTRAP register) */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the chip PLLs and selects clock sources for modules            */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ConfigureClocks (void)
{

	/*-----------------------------------------------------------------------------------------------------*/
	/* pll configuration local values                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	UINT32  PLLCON0_l  = 0;
	UINT32  PLLCON1_l  = 0;
	UINT32  PLLCON2_l  = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* clk diveiders local values                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	UINT32  CLKDIV1_l  = 0;
	UINT32  CLKDIV2_l  = 0;
	UINT32  CLKDIV3_l  = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* clk mux local values                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	UINT32  CLKSEL_l   = 0;

	UINT32 CLKSEL_CLKREF_l = 0;

	UINT32 polegVersion = GCR_Get_Chip_Version();

/*---------------------------------------------------------------------------------------------------------*/
/* Z2:                                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	/* 2   1   0   DDR     CORE    AXI     AHB     AHB3    SPI0    APB5     APB2,3    APB1,4     PCI     GFX  */
	/* Straps                                                                                                 */
	/* 3   2   1                                                                                              */
	/* 0   0   0   MCBPCK  FT      FT/2    FT/4    FT/4    FT/4    FT/4     FT/4      FT/4       PLL0/5  PLL0/4 */
	/* 0   0   1   25      25      12.5    6.25    3.1     6.25    3.125    1.5       1.5        4.17    5    */
	/* 0   1   0   1066    1000    500     250     19.23   19.23   125      62.5      31.25      142.8   167  */
	/* 0   1   1   667     500     250     125     17.85   17.85   62.5     62.5      31.25      166.7   167  */
	/* 1   0   0   850     800     400     200     20      20      100      50        25         133     160  */
	/* 1   0   1   900     888     444     222     18.5    18.5    111      55.5      27.75      148     177.6 */
	/* 1   1   0   800     850     425     212.5   19.3    19.3    106.25   53.125    26.6       170     170  */
	/* 1   1   1   800     800     400     200     20      20      100      50        25         133     160  */
	/*-----------------------------------------------------------------------------------------------------*/



	/*-----------------------------------------------------------------------------------------------------*/
	/* Common configurations for the clk mux. will be changes later according to straps 1-3                */
	/*-----------------------------------------------------------------------------------------------------*/
	CLKSEL_l           = REG_READ(CLKSEL);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read CLKDIV default configuration                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	CLKDIV1_l          = REG_READ(CLKDIV1);
	CLKDIV2_l          = REG_READ(CLKDIV2);
	CLKDIV3_l          = REG_READ(CLKDIV3);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Spec: PLL0 is the system clock PLL. Output frequency range is 800 to 1000 (800) MHz.               */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_PLL0);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Spec: PLL1 is the Memory Controller clock PLL. Output frequency range is 667 to 1066 (800) MHz.    */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_MCCKSEL,   CLKSEL_MCCKSEL_PLL1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* PLL2 is for UARTs, timers, USB. Output frequency is 960 MHz.                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_DVCSSEL,   CLKSEL_DVCSSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_CLKOUTSEL, CLKSEL_CLKOUTSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_SUCKSEL,   CLKSEL_SUCKSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_SDCKSEL,   CLKSEL_SDCKSEL_PLL2);
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL0); /*TODO: check */
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXMSEL,   CLKSEL_GFXMSEL_PLL2);

	/*-----------------------------------------------------------------------------------------------------*/
	/* GFX PLL used for graphics                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_GPRFSEL,   CLKSEL_GPRFSEL_CLKREF);  /* PLLG source clock can be clkref (like other plls)\ GFXBPCK \ USB OHCI */
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_PIXCKSEL,  CLKSEL_GPRFSEL_GFXBYPCK);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Clock configuration - TIMER connection should change in spec to allow timer to be connected to the  */
	/* external reference clock by defaultm                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKSEL_l, CLKSEL_TIMCKSEL,  CLKSEL_TIMCKSEL_CLKREF);


	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting the Timer divisor to 2                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_TIMCKDIV, CLKDIV1_TIMCK_DIV(1));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Read clock speed from straps                                                                        */
	/* Values interpretation is according to FUSTRAP spec and not PWRON spec                               */
	/* STRP_Init() must be run prior to calling this function                                              */
	/*-----------------------------------------------------------------------------------------------------*/
		if ( polegVersion == POLEG_VERSION_Z1)
	{
	    switch (STRP_Ckfrq_Get())
		{
			case STRP_CLKFRQ_OPTION0:   /* DDR=MCBPCK  CORE=FT       AXI=FT/2  AHB=FT/4      AHB3=FT/2   SPI0=FT/2     APB5=FT/4     APB2,3=FT/4       APB1,4=FT/4     PCI=FT/5    GFX=FT/4 */

				/* PLLs on difault values: */
				PLLCON0_l  = CLK_600MHZ_PLLCON0_REG_CFG;     /* Configure PLL0 with value divided by 24 for UART */
				PLLCON1_l  = CLK_800MHZ_PLLCON0_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				/* clk selection is for this bypass: */
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_CPUCKSEL, CLKSEL_CPUCKSEL_SYSBPCK);
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_MCCKSEL,  CLKSEL_MCCKSEL_MCBPCK);

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(1));  /* AHB3 clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(5));  /* PCI clock div = 5  */

				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(4));  /* GFX to PLL0/4 */


				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(1));  /* SPI clock div = 4, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPI0CKDV_DIV(1));  /* SPI clock div = 4. take it slow.  */

				break;

			case STRP_CLKFRQ_OPTION2:   /* DDR=1066    CORE=1000     AXI=500   AHB=250       AHB3=19.23  SPI0=19.23    APB5=125      APB2,3=62.5       APB1,4=31.25    PCI=142.8   GFX=167 */
				/* PLLs on difault values: */
				PLLCON0_l  = CLK_600MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_500MHZ_PLLCON0_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(13));  /* AHB3 clock div = 13*2  */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(7));  /* PCI clock div = 5  */

				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(6));  /* GFX to PLL0/6 */


				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(13));  /* SPI clock div = 13, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				break;

			case STRP_CLKFRQ_OPTION3:   /* DDR=667     CORE=500      AXI=250   AHB=125       AHB3=17.85  SPI0=17.85    APB5=62.5     APB2,3=62.5       APB1,4=31.25    PCI=166.7   GFX=167 */
				/* PLLs on difault values: */
				PLLCON0_l  = CLK_600MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_600MHZ_PLLCON0_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;


				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(7));  /* AHB3 clock div = 7*2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(3));  /* PCI clock div = 5  */

				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 2, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(3));  /* GFX to PLL0/3 */

				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(7));  /* SPI clock div = 7, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				break;

			case STRP_CLKFRQ_OPTION4:   /* DDR=850     CORE=800      AXI=400   AHB=200       AHB3=20     SPI0=20       APB5=100      APB2,3=50         APB1,4=25       PCI=133     GFX=160 */
				 /* PLLs on difault values: */
				PLLCON0_l  = CLK_600MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_666MHZ_PLLCON0_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(10));  /* AHB3 clock div = 10*2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(6));  /* PCI clock div = 5  */

				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(5));  /* GFX to PLL0/5 */

				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(10));  /* SPI clock div = 10, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				break;

			case STRP_CLKFRQ_OPTION5:   /* DDR=900     CORE=888      AXI=444   AHB=222       AHB3=18.5   SPI0=18.5     APB5=111      APB2,3=55.5       APB1,4=27.75    PCI=148     GFX=177.6 */
				/* PLLs on difault values: */
				PLLCON0_l  = CLK_600MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_720MHZ_PLLCON0_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(12));  /* AHB3 clock div = 12*2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(6));  /* PCI clock div = 5  */

				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(5));  /* GFX to PLL0/5 */

				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(12));  /* SPI clock div = 12, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				break;

			case STRP_CLKFRQ_OPTION6:   /* DDR=800     CORE=850      AXI=425   AHB=212.5     AHB3=19.3   SPI0=19.3     APB5=106.25   APB2,3=53.125     APB1,4=26.6     PCI=170     GFX=170 */
				/* PLLs on difault values: */
				PLLCON0_l  = CLK_600MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_800MHZ_PLLCON0_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(11)); /* AHB3 clock div = 11*2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(5));  /* PCI clock div = 5  */

				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(5));  /* GFX to PLL0/5 */

				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(11));  /* SPI clock div = 11, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				break;

			case STRP_CLKFRQ_OPTION1:   /* DDR=25      CORE=25       AXI=12.5  AHB=6.25      AHB3=3.1    SPI0=6.25     APB5=3.125    APB2,3=3.125      APB1,4=1.5      PCI=4.17    GFX=5 */
			case STRP_CLKFRQ_OPTION7:   /* DDR=800     CORE=800      AXI=400   AHB=200       AHB3=20     SPI0=20       APB5=100      APB2,3=50         APB1,4=25       PCI=133     GFX=160 */
				/* PLLs on difault values: */
				PLLCON0_l  = CLK_600MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_800MHZ_PLLCON0_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				/* since CORE == DDR clock. connect them both to same PLL */
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_CPUCKSEL, CLKSEL_CPUCKSEL_PLL1);
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL1); /* move to PLL1 too. */

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(10));  /* AHB3 clock div = 10*2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(6));  /* PCI clock div = 5  */

				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(5));  /* GFX to PLL0/5 */

				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(10));  /* SPI clock div = 10, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				break;

			default:
				ASSERT(0);
		}

	}


	/*------------------------------------------------------- */
	/*          Z2 and above: */
	else
	{
		switch (STRP_Ckfrq_Get())
		{
			case STRP_CLKFRQ_OPTION0:   /** Z1: DDR=MCBPCK  CORE=FT       AXI=FT/2  AHB=FT/4     AHB3=FT/2   SPI0=FT/2   APB5=FT/4   APB2,3=FT/4     APB1,4=FT/4     PCI=FT/5    GFX=FT/4 */
										/** Z2: DDR=MCBPCK  CORE=FT       AXI=FT/2  AHB=FT/4	 AHB3=FT/4	 SPI0=FT/4	 APB5=FT/4	 APB2,3=FT/4	 APB1,4=FT/4	 PCI=PLL0/5	 GFX=PLL0/4	*/
			    /* PLLs on default values: */

				PLLCON0_l  = CLK_800MHZ_PLLCON0_REG_CFG;     /* Configure PLL0 with value divided by 24 for UART */
				PLLCON1_l  = CLK_800MHZ_PLLCON1_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(5));
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(4));

				/* clk selection is for this bypass: */
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_CPUCKSEL, CLKSEL_CPUCKSEL_SYSBPCK);
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_MCCKSEL,  CLKSEL_MCCKSEL_MCBPCK);

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(1));  /* AHB3 clock div = 4 */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV1);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(1));  /* SPI clock div = 4, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(1));  /* SPI clock div = 4. take it slow.  */

				break;

			case STRP_CLKFRQ_OPTION2:   /** Z1: DDR=1066   CORE=1000   AXI=500   AHB=250   AHB3=19.23  SPI0=19.23    APB5=125      APB2,3=62.5      APB1,4=31.25    PCI=142.8   GFX=167 */
										/** Z2: DDR=667	   CORE=500	   AXI=250	 AHB=125   AHB3=17.85  SPI0=17.85    APB5=62.5	   APB2,3=62.5	    APB1,4=31.25	PCI=120	    GFX=160	*/

			case STRP_CLKFRQ_OPTION3:   /** Z1: DDR=667    CORE=500    AXI=250   AHB=125   AHB3=17.85  SPI0=17.85    APB5=62.5     APB2,3=62.5      APB1,4=31.25    PCI=166.7   GFX=167 */
										/** Z2: DDR=667	   CORE=500	   AXI=250	 AHB=125   AHB3=17.85  SPI0=17.85	 APB5=62.5	   APB2,3=62.5	    APB1,4=31.25	PCI=120	    GFX=160	*/

			case STRP_CLKFRQ_OPTION4:   /** Z1: DDR=850    CORE=800    AXI=400   AHB=200   AHB3=20     SPI0=20       APB5=100      APB2,3=50        APB1,4=25       PCI=133     GFX=160 */
										/** Z2: DDR=667	   CORE=500	   AXI=250	 AHB=125   AHB3=17.85  SPI0=17.85	 APB5=62.5	   APB2,3=62.5	    APB1,4=31.25	PCI=120	    GFX=160	 */

			case STRP_CLKFRQ_OPTION5:   /** Z1: DDR=900    CORE=888    AXI=444   AHB=222   AHB3=18.5   SPI0=18.5     APB5=111      APB2,3=55.5      APB1,4=27.75    PCI=148     GFX=177.6 */
										/** Z2: DDR=667	   CORE=500	   AXI=250	 AHB=125   AHB3=17.85  SPI0=17.85	 APB5=62.5	   APB2,3=62.5	    APB1,4=31.25	PCI=120	    GFX=160	 */

			case STRP_CLKFRQ_OPTION6:   /** Z1: DDR=800    CORE=850    AXI=425   AHB=212.5 AHB3=19.3   SPI0=19.3     APB5=106.25   APB2,3=53.125    APB1,4=26.6     PCI=170     GFX=170 */
										/** Z2: DDR=667	   CORE=500	   AXI=250	 AHB=125   AHB3=17.85  SPI0=17.85	 APB5=62.5	   APB2,3=62.5	    APB1,4=31.25	PCI=120	    GFX=160	*/

				PLLCON0_l  = CLK_500MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_666MHZ_PLLCON1_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL2);
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(4));
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(3));

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(7));  /* AHB3 clock div = 7*2 */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 4, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 2, it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(7));  /* SPI clock div = 7, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				break;


			case STRP_CLKFRQ_OPTION1:   /** Z1: DDR=25      CORE=25     AXI=12.5    AHB=6.25    AHB3=3.1    SPI0=6.25   APB5=3.125  APB2,3=3.125    APB1,4=1.5      PCI=4.17    GFX=5 */
										/** Z2: DDR=25	    CORE=25	    AXI=12.5	AHB=6.25	AHB3=3.1	SPI0=6.25	APB5=3.125	APB2,3=1.5	    APB1,4=1.5	    PCI=4.167	GFX=5			*/

			case STRP_CLKFRQ_OPTION7:   /** Z1:  DDR=800    CORE=800    AXI=400     AHB=200     AHB3=20     SPI0=20     APB5=100    APB2,3=50       APB1,4=25       PCI=133     GFX=160 */
										/** Z2:  DDR=800	CORE=800	AXI=400 	AHB=200     AHB3=20	    SPI0=20	    APB5=100	APB2,3=50	    APB1,4=25	    PCI=133	    GFX=160							*/

				/* PLLs on difault values: */
				PLLCON0_l  = CLK_800MHZ_PLLCON0_REG_CFG;
				PLLCON1_l  = CLK_800MHZ_PLLCON1_REG_CFG;
				PLLCON2_l  = CLK_960MHZ_PLLCON2_REG_CFG;

				SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL0);
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(6));
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_GFXCKDIV,  CLKDIV2_GFXCKDIV_DIV(5));

				/* since CORE == DDR clock. connect them both to same PLL */
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_CPUCKSEL, CLKSEL_CPUCKSEL_PLL1);
				SET_VAR_FIELD(CLKSEL_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL1); /* move to PLL1 too. */

				/* clk dividers */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK2DIV,   CLKDIV1_CLK2DIV2);  /* AXI  clock div = 2 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_CLK4DIV,   CLKDIV1_CLK4DIV2);  /* AHBn clock div = 4 */
				SET_VAR_FIELD(CLKDIV1_l, CLKDIV1_AHB3CKDIV, CLKDIV1_AHB3CK_DIV(10));  /* AHB3 clock div = 10*2 */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB3CKDIV, CLKDIV2_APBxCKDIV4);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB4CKDIV, CLKDIV2_APBxCKDIV8);  /* APB clock div = 8,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB5CKDIV, CLKDIV2_APBxCKDIV2);  /* APB clock div = 4,  it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPI0CKDV,  CLKDIV3_SPI0CKDV_DIV(10));  /* SPI clock div = 10, but it's after AXI div(2) and APBn div(2) */
				SET_VAR_FIELD(CLKDIV3_l, CLKDIV3_SPIXCKDV,  CLKDIV3_SPIXCKDV_DIV(32));  /* SPI clock div = 32. take it slow.  */

				break;

			default:
				ASSERT(0);
		}
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* in Z1 APB1 must be identical to APB2                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (polegVersion == POLEG_VERSION_Z1)
	{
        SET_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB1CKDIV, READ_VAR_FIELD(CLKDIV2_l, CLKDIV2_APB2CKDIV));
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Switch clock sources to external clock for all muxes (while initing the PLLs)                       */
	/*-----------------------------------------------------------------------------------------------------*/
	/* Switch the clock source to another PLL or external clock (25 MHz) */
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_DVCSSEL,   CLKSEL_DVCSSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_CPUCKSEL,  CLKSEL_CPUCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_MCCKSEL,   CLKSEL_MCCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_CLKOUTSEL, CLKSEL_CLKOUTSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_TIMCKSEL,  CLKSEL_TIMCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_SUCKSEL,   CLKSEL_SUCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_SDCKSEL,   CLKSEL_SDCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_GFXMSEL,   CLKSEL_GFXMSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_GPRFSEL,   CLKSEL_GPRFSEL_CLKREF);
	SET_VAR_FIELD(CLKSEL_CLKREF_l, CLKSEL_PIXCKSEL,  CLKSEL_PIXCKSEL_CLKREF);

	REG_WRITE(CLKSEL, CLKSEL_CLKREF_l);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Change PLL configuration (leave PLL in reset mode):                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	/* Set PWDEN bit */
	SET_VAR_FIELD(PLLCON0_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);
	SET_VAR_FIELD(PLLCON1_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);
	SET_VAR_FIELD(PLLCON2_l, PLLCONn_PWDEN, PLLCONn_PWDEN_POWER_DOWN);

	/* set to power down == 1 */
	REG_WRITE(PLLCON0, PLLCON0_l);
	REG_WRITE(PLLCON1, PLLCON1_l);
	REG_WRITE(PLLCON2, PLLCON2_l);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait 10usec for PLL:                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	/*3. Wait 2  us or more */
	CLK_Delay_MicroSec(10);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable PLLs after configuring. Delays to prevent power issues.                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(PLLCON0, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);
	CLK_Delay_MicroSec(10);
	SET_REG_FIELD(PLLCON1, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);
	CLK_Delay_MicroSec(10);
	SET_REG_FIELD(PLLCON2, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);
	CLK_Delay_MicroSec(10);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait 750usec for PLL to stabilize:                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	/* TODO : check return value */
	WaitForPllLock(PLL0);
	WaitForPllLock(PLL1);
	WaitForPllLock(PLL2);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Change CLKDIV configuration:                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(CLKDIV1, CLKDIV1_l);
	REG_WRITE(CLKDIV2, CLKDIV2_l);
	REG_WRITE(CLKDIV3, CLKDIV3_l);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for 200 clock cycles between clkDiv change and clkSel change:                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(400);

	/*----------------------------------------------------------------------------------------------------*/
	/* Change clock source according to configuration:                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	/*-----------------------------------------------------------------------------------------------------*/
	/* in Z1 clksel all at once: REG_WRITE(CLKSEL, CLKSEL_l);                                              */
	/*                                                                                                     */
	/* in z2 in order to avoid power surge clksel is st gradualy, according to this scema:                 */
	/* [ brackets] mean do it in the same step                                                             */
	/*                                                                                                     */
	/* MC, [DVC,   CPU], GFXMSEL, GPRFSEL,  GFXCKSEL, [PIXCKSEL, CLKOUTSEL, SUCKSEL, TIMCKSEL, UARTCKSEL,  */
	/* SDCKSEL]                                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/

	SET_REG_FIELD(CLKSEL, CLKSEL_MCCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_MCCKSEL));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_DVCSSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_DVCSSEL));
	SET_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_CPUCKSEL));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_GFXMSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_GFXMSEL));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_GPRFSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_GPRFSEL ));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_GFXCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_GFXCKSEL));
	CLK_Delay_MicroSec(20);

	SET_REG_FIELD(CLKSEL, CLKSEL_PIXCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_PIXCKSEL));
	SET_REG_FIELD(CLKSEL, CLKSEL_CLKOUTSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_CLKOUTSEL ));
	SET_REG_FIELD(CLKSEL, CLKSEL_SUCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_SUCKSEL ));
	SET_REG_FIELD(CLKSEL, CLKSEL_TIMCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_TIMCKSEL ));
	SET_REG_FIELD(CLKSEL, CLKSEL_UARTCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_UARTCKSEL ));
	SET_REG_FIELD(CLKSEL, CLKSEL_SDCKSEL, READ_VAR_FIELD(CLKSEL_l,  CLKSEL_SDCKSEL ));
	CLK_Delay_MicroSec(20);


	/* Wait 10usec for PLL:   */
	CLK_Delay_MicroSec(10);

	/* Now we can init the fuse clock */
	FUSE_Init();
}
#endif /* defined (CODE_OF_BB_ONLY) */


#if defined (USB_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureUSBClock                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the USB clock to 60MHz by checking which PLL is                             */
/*                  dividable by 60 and setting correct SEL and DIV values.                                */
/*                  Assumes that one of the PLLs is dividable by 60                                        */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS  CLK_ConfigureUSBClock(void)
{
	UINT32  pll0Freq;   /* In Hz */
	UINT32  pll1Freq;   /* In Hz */
	UINT32  choosenPllFreq;
	UINT32  su60Divider = 0;
	UINT32  suDivider = 0;

	pll0Freq = CLK_GetPll0Freq();
	pll1Freq = CLK_GetPll1Freq();

	if (pll0Freq % SU60_DESIRED_FREQUENCY == 0)
	{
        SET_REG_FIELD(CLKSEL, CLKSEL_SUCKSEL,   CLKSEL_SUCKSEL_PLL0);
        choosenPllFreq = pll0Freq;
	}
	else if (pll1Freq % SU60_DESIRED_FREQUENCY == 0)
	{
        SET_REG_FIELD(CLKSEL, CLKSEL_SUCKSEL,   CLKSEL_SUCKSEL_PLL1);
        choosenPllFreq = pll1Freq;
	}
	else
	{
        return HAL_ERROR_BAD_FREQ;
	}

	su60Divider = choosenPllFreq / (SU60_DESIRED_FREQUENCY*_1MHz_);
	suDivider   = choosenPllFreq / (SU_DESIRED_FREQUENCY*_1MHz_);
	SET_REG_FIELD(CLKDIV2, CLKDIV2_SU48CKDIV, su60Divider - 1);
	CLK_Delay_Cycles(200);
	SET_REG_FIELD(CLKDIV2, CLKDIV2_SUCKDIV, suDivider - 1);
	CLK_Delay_Cycles(200);

	return HAL_OK;

}
#endif /* defined (USB_MODULE_TYPE) */


#if defined (UART_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureUartClock                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures the Uart clock source to be closest to 24MHz by                */
/*                  modifying the UART divider.                                                            */
/*                  In _PALLADIUM_ bypass mode the UART input frequency is set to be highest possible -    */
/*                  same as APB frequency                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_ConfigureUartClock(void)
{
	UINT32 uart_clk; /*Hz */

	/*-------------------------------------------------------------------------------------------------*/
	/* Set UART to 24MHz. Source is PLL2 (960Mhz), which is divided by 2, so we get 24Mhz = 960/2/20   */
	/*-------------------------------------------------------------------------------------------------*/
	UINT32 uartDesiredFreq  = 24*_1MHz_; /*Hz */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Normal configuration - UART from PLL0 with divider calculated from PLL0 configuration to get 24MHz  */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-------------------------------------------------------------------------------------------------*/
	/* Calculate the divider given PLL2 output and desired frequency:                                  */
	/*-------------------------------------------------------------------------------------------------*/
	UINT32 pllFreq = CLK_GetPll2Freq();
	UINT32 uartDiv = pllFreq/uartDesiredFreq;
	uart_clk = pllFreq / uartDiv;

	/*----------------------------------------------------[]---------------------------------------------*/
	/* Set divider:                                                                                    */
	/*-------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(CLKDIV1, CLKDIV1_UARTDIV, CLKDIV1_UART_DIV(uartDiv));

	/*-------------------------------------------------------------------------------------------------*/
	/* Choose PLL0 as a source:                                                                        */
	/*-------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(CLKSEL, CLKSEL_UARTCKSEL, CLKSEL_UARTCKSEL_PLL2);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for 200 clock cycles between clkDiv change and clkSel change:                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(200);

	return uart_clk;

}
#endif /* defined (UART_MODULE_TYPE) */


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPLL0toAPBdivisor                                                                */
/*                                                                                                         */
/* Parameters:      apb: number of APB                                                                     */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the value achieved by dividing PLL0 frequency to APB frequency    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetPLL0toAPBdivisor(UINT32 apb)
{
	volatile UINT32 apb_divisor = 1;

	apb_divisor = apb_divisor * (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV) + 1);       /* AXI divider ( div by 1\2) */
	apb_divisor = apb_divisor * (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV) + 1);       /* AHBn divider (div by 1\2\3\4) */

	switch (apb)
	{
        case APB1:
            apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB1CKDIV));     /* APB divider */
            break;
        case APB2:
            apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB2CKDIV));     /* APB divider */
            break;
        case APB3:
            apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB3CKDIV));     /* APB divider */
            break;
        case APB4:
            apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB4CKDIV));     /* APB divider */
            break;
        case APB5:
            apb_divisor = apb_divisor * (1 << READ_REG_FIELD(CLKDIV2, CLKDIV2_APB5CKDIV));     /* APB divider */
            break;
        case SPI0:
            apb_divisor = apb_divisor * (READ_REG_FIELD(CLKDIV3, CLKDIV3_SPI0CKDV) + 1);       /* SPI0 divider */
            break;
        case SPI3:
            apb_divisor = apb_divisor * (READ_REG_FIELD(CLKDIV1, CLKDIV1_AHB3CKDIV) + 1);       /* SPI0 divider */
            break;
        default:
            apb_divisor = 0xFFFFFFFF;
            break;
	}

	return apb_divisor;
}






/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_Cycles                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  cycles -  num of cycles to delay                                                       */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs delay in number of cycles (delay in C code).                     */
/*                  For a more accurate delay, use : CLK_Delay_MicroSec                                    */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_Cycles (UINT32 cycles)
{
	UINT            cacheState = 0;
	volatile UINT   i          = 0;
	volatile UINT32 iterations = 0;


	/*-----------------------------------------------------------------------------------------------------*/
	/* The measurements were done on PD over 50 cycles, fetches from ROM:                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	const UINT CYCLES_IN_ONE_ITERATION_CACHE_DISABLED  =   145;
	/* const UINT CYCLES_IN_ONE_ITERATION_CACHE_ENABLED      6 */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate number of iterations                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	iterations = cycles/CYCLES_IN_ONE_ITERATION_CACHE_DISABLED + 1;

	/*-----------------------------------------------------------------------------------------------------*/
	/* The actual wait loop:                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	ICACHE_SAVE_DISABLE(cacheState);
	for (i = 0; i < iterations; i++);
	ICACHE_RESTORE(cacheState);

	return iterations;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetTimeStamp                                                                       */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         Current time stamp                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void  CLK_GetTimeStamp (UINT32 time_quad[2])
{
	UINT32 Seconds;
	UINT32 RefClocks;

	do
	{
        Seconds = REG_READ(SECCNT);
        RefClocks = REG_READ(CNTR25M);
	} while (REG_READ(SECCNT) != Seconds);

	time_quad[0] = RefClocks;
	time_quad[1] = Seconds;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_MicroSec                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  microSec -  number of microseconds to delay                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs a busy delay (empty loop)                                        */
/*                  the number of iterations is based on current CPU clock calculation and cache           */
/*                  enabled/disabled                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_MicroSec(UINT32 microSec)
{
#if 0 /* _USE_CLK_ITERATION_ */
	UINT            cacheState  = 0;
	UINT32          FOUT        = 0;
	UINT32          part1       = 0;
	UINT32          part2       = 0;
	volatile UINT   i           = 0;
	volatile UINT32 iterations  = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate CPU frequency (in KHz)                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	FOUT = CLK_GetCPUFreq() / 1000;

	/*-----------------------------------------------------------------------------------------------------*/
	/* FOUT/cyclesPerIteration - iterations in each millisecond (FOUT is in KHz)                           */
	/* FOUT/(cyclesPerIteration*1000) - iterations in each uSec                                            */
	/*-----------------------------------------------------------------------------------------------------*/
/*    iterations = (microSec*FOUT)/(CYCLES_IN_ONE_ITERATION_CACHE_DISABLED*1000); */
	part1 = microSec/CYCLES_IN_ONE_ITERATION_CACHE_DISABLED;
	part1 = MAX(1, part1);
	part2 = FOUT/1000;
	part2 = MAX(1, part2);
	iterations = part1*part2;

	/*-----------------------------------------------------------------------------------------------------*/
	/* The actual wait loop:                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	ICACHE_SAVE_DISABLE(cacheState);
	for (i = 0; i < iterations; i++);
	ICACHE_RESTORE(cacheState);

	return iterations;

#else  /* use the new clk counter */

/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* This register is reset by only VSB power-up reset. The value of this register                           */
/* represents a counter with a 25 MHz clock, used to update the SECCNT register. This field is updated every*/
/* 640ns. The 4 LSB of this field are always 0. When this field reaches a value of 25,000,000 it goes to 0 */
/* and SEC_CNT field is updated.                                                                           */
/*---------------------------------------------------------------------------------------------------------*/


	/* not testing that microSec < 33 sec (2^25bit) us */

	UINT32 iUsCnt1[2], iUsCnt2[2];
	UINT32 delay;  /* Acctual delay generated by FW */
	UINT32 minimum_delay = (microSec * EXT_CLOCK_FREQUENCY_MHZ) + CNTR25M_ACCURECY; /* this is equivalent to microSec/0.64 + minimal tic length.*/

	CLK_GetTimeStamp(iUsCnt1);

	do
	{
        CLK_GetTimeStamp(iUsCnt2);
        delay =  ((EXT_CLOCK_FREQUENCY_MHZ * _1MHz_) * (iUsCnt2[1] - iUsCnt1[1])) + (iUsCnt2[0] - iUsCnt1[0]);
	}
	while(delay < minimum_delay);


	return (UINT32)(delay / EXT_CLOCK_FREQUENCY_MHZ);


#endif



}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_Delay_Since                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  microSecDelay -  number of microseconds to delay since t0_time. if zero: no delay.     */
/*                  t0_time       - start time , to measure time from.                                     */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  get a time stamp, delay microSecDelay from it. If microSecDelay has already passed     */
/*                  since the time stamp , then no delay is executed. returns the time that elapsed since  */
/*                  t0_time .                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_Delay_Since (UINT32 microSecDelay, UINT32 t0_time[2])
{

	UINT32 iUsCnt2[2];
	UINT32 timeElapsedSince;  /* Acctual delay generated by FW */
	UINT32 minimum_delay = (microSecDelay * EXT_CLOCK_FREQUENCY_MHZ) + CNTR25M_ACCURECY; /* this is equivalent to microSec/0.64 + minimal tic length.*/


	do
	{
        CLK_GetTimeStamp(iUsCnt2);
        timeElapsedSince =  ((EXT_CLOCK_FREQUENCY_MHZ * _1MHz_) * (iUsCnt2[1] - t0_time[1])) + (iUsCnt2[0] - t0_time[0]);
	}
	while(timeElapsedSince < minimum_delay);

	/*-----------------------------------------------------------------------------------------------------*/
	/* return elapsed time                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	return (UINT32)(timeElapsedSince / EXT_CLOCK_FREQUENCY_MHZ);
}



#if defined (TIMER_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureTimerClock                                                                */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs Timer clock configuration                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_ConfigureTimerClock (void)
{

	SET_REG_FIELD(CLKEN1, CLKEN1_TIMER0_4, 1);
	SET_REG_FIELD(CLKEN1, CLKEN1_TIMER5_9, 1);
	SET_REG_FIELD(CLKEN3, CLKEN3_TIMER10_14, 1);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for 200 clock cycles between clkDiv or clkSel change:                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(200);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting the Timer divisor to 1                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(CLKDIV1, CLKDIV1_TIMCKDIV, CLKDIV1_TIMCK_DIV(1));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait for 200 clock cycles between clkDiv or clkSel change:                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(200);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Sellecting 25MHz clock source                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(CLKSEL, CLKSEL_TIMCKSEL, CLKSEL_TIMCKSEL_CLKREF);

	/* Wait 10usec for PLL:   */
	CLK_Delay_MicroSec(10);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Returning the clock frequency                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	return EXT_CLOCK_FREQUENCY_MHZ*_1MHz_;
}
#endif /* defined (TIMER_MODULE_TYPE) */


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureEMCClock                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  ethNum -  ethernet module number                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures EMC clocks                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void    CLK_ConfigureEMCClock(UINT32 ethNum)
{
	if (ethNum == 0)                          /* ETH0 - EMC1 */
	{
       SET_REG_FIELD(CLKEN1, CLKEN1_EMC1, 1);
	}
	else if (ethNum == 1)                     /* ETH1 - EMC2 */
	{
       SET_REG_FIELD(CLKEN1, CLKEN1_EMC2, 1);
	}
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureGMACClock                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  ethNum -  ethernet module number                                                       */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures GMAC clocks                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void    CLK_ConfigureGMACClock(UINT32 ethNum)
{
	if (ethNum == 2)                          /* ETH2 - GMAC1 */
	{
        SET_REG_FIELD(CLKEN2, CLKEN2_GMAC1, 1);
	}
	else if (ethNum == 3)                     /* ETH3 - GMAC2 */
	{
        SET_REG_FIELD(CLKEN2, CLKEN2_GMAC2, 1);
	}
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureSDClock                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sdNum -  SD module number                                                              */
/*                                                                                                         */
/* Returns:         SD clock frequency                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the SD clock to frequency closest to 48MHz from beneath                     */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_ConfigureSDClock(UINT32 sdNum)
{
	UINT32  divider;
	UINT32  pll0Freq;   /*In Hz */
	UINT32  sdhci_clock;   /*In Hz */
	UINT32  target_freq;   /*In Hz */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (sdNum >= SD_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure Clock Enable Register                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	if (sdNum == SD1_DEV)
	{
        SET_REG_FIELD(CLKEN2, CLKEN2_SDHC, 1);
	}
	else if (sdNum == SD2_DEV)
	{
        SET_REG_FIELD(CLKEN2, CLKEN2_MMC, 1);
	}

	pll0Freq = CLK_GetPll0Freq();
	CLOCK_DEBUG("clk_drv: %s pll0Freq =%d \n",__FUNCTION__, pll0Freq);

#ifndef _PALLADIUM_
	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate rounded up divider to produce closest to Target output clock                              */
	/*-----------------------------------------------------------------------------------------------------*/
	if (GCR_Get_Chip_Version() == POLEG_VERSION_Z1)
	{
        target_freq = SD_CLK_TARGET_FREQ_Z1;
	}
	else
	{
        target_freq = SD_CLK_TARGET_FREQ_A1;
	}
	divider = (pll0Freq % target_freq == 0) ? (pll0Freq / target_freq) :          \
                                              (pll0Freq / target_freq) + 1;
#endif
	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting clock divider                                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	if (sdNum == SD1_DEV)  /* SD Card */
	{
#ifdef _PALLADIUM_
        divider = 2;     /* Set maximum freq for Palladium (in reality around 600KHz) */
#endif
        SET_REG_FIELD(CLKDIV2, CLKDIV2_SD1CKDIV, (divider/2)-1);
        CLOCK_DEBUG("clk_drv: %s CLKDIV2_SD1CKDIV =%d sdhci =%d\n",__FUNCTION__, READ_REG_FIELD(CLKDIV2, CLKDIV2_SD1CKDIV), sdNum);
	}
	else if (sdNum == SD2_DEV)  /* eMMC */
	{
#ifdef _PALLADIUM_
        divider = 1;     /* Set maximum freq for Palladium (in reality around 600KHz) */
#endif
        SET_REG_FIELD(CLKDIV1, CLKDIV1_MMCCKDIV, divider-1);
        CLOCK_DEBUG("clk_drv: %s CLKDIV1_MMCCKDIV =%d sdhci =%d\n",__FUNCTION__, READ_REG_FIELD(CLKDIV1, CLKDIV1_MMCCKDIV),sdNum);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Wait to the divider to stabilize (according to spec)                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	CLK_Delay_Cycles(200);

	/*-----------------------------------------------------------------------------------------------------*/
	/* SD clock uses always PLL0                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(CLKSEL, CLKSEL_SDCKSEL, CLKSEL_SDCKSEL_PLL0);

	sdhci_clock = pll0Freq/divider;
	CLOCK_DEBUG("clk_drv: %s sdhci_clock =%d sdhci =%d\n",__FUNCTION__, sdhci_clock, sdNum);

	return sdhci_clock;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetEth                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of EMC module                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetEMC(UINT32 deviceNum)
{
	if (deviceNum == 0)
	{
        SET_REG_FIELD(IPSRST1, IPSRST1_EMC1, 1);
        SET_REG_FIELD(IPSRST1, IPSRST1_EMC1, 0);
	}
	else if (deviceNum == 1)
	{
        SET_REG_FIELD(IPSRST1, IPSRST1_EMC2, 1);
        SET_REG_FIELD(IPSRST1, IPSRST1_EMC2, 0);
	}
}


#if defined (FIU_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetFIU                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of FIU                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetFIU (void)
{
	SET_REG_FIELD(IPSRST1, IPSRST1_SPI0, 1);
	SET_REG_FIELD(IPSRST1, IPSRST1_SPI0, 0);

	SET_REG_FIELD(IPSRST1, IPSRST1_SPI3, 1);
	SET_REG_FIELD(IPSRST1, IPSRST1_SPI3, 0);

	SET_REG_FIELD(IPSRST3, IPSRST3_SPIX, 1);
	SET_REG_FIELD(IPSRST3, IPSRST3_SPIX, 0);
}
#endif /*#if defined (FIU_MODULE_TYPE) */

#if defined (UART_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetUART                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of UART                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetUART(UINT32 deviceNum)
{
	SET_REG_FIELD(IPSRST1, IPSRST1_UART01, 1);
	SET_REG_FIELD(IPSRST1, IPSRST1_UART01, 0);

	SET_REG_FIELD(IPSRST1, IPSRST1_UART23, 1);
	SET_REG_FIELD(IPSRST1, IPSRST1_UART23, 0);
}
#endif /* #if defined (UART_MODULE_TYPE) */


#if defined (AES_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetAES                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of AES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetAES (void)
{
	SET_REG_FIELD(IPSRST1, IPSRST1_AES, 1);
	SET_REG_FIELD(IPSRST1, IPSRST1_AES, 0);
}
#endif /*if defined (AES_MODULE_TYPE) */


#if defined (MC_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetMC                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of MC                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetMC (void)
{
	SET_REG_FIELD(IPSRST1, IPSRST1_MC, 1);
	CLK_Delay_MicroSec(100);
	SET_REG_FIELD(IPSRST1, IPSRST1_MC, 0);
	CLK_Delay_MicroSec(100);


	/* Force re-training of DDR (because DDR module is reinitialized*/
	SET_REG_FIELD(INTCR2, INTCR2_MC_INIT, 0);
}
#endif /* #if defined (MC_MODULE_TYPE) */


#if defined (TIMER_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetTIMER                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of Timer                                                */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetTIMER (UINT32 deviceNum)
{
	if (deviceNum <= 4)
	{
        SET_REG_FIELD(IPSRST1, IPSRST1_TIM0_4, 1);
        SET_REG_FIELD(IPSRST1, IPSRST1_TIM0_4, 0);
	}
	else if (deviceNum <= 9)
	{
        SET_REG_FIELD(IPSRST1, IPSRST1_TIM5_9, 1);
        SET_REG_FIELD(IPSRST1, IPSRST1_TIM5_9, 0);
	}
	else
	{
        SET_REG_FIELD(IPSRST3, IPSRST3_TIMER10_14, 1);
        SET_REG_FIELD(IPSRST3, IPSRST3_TIMER10_14, 0);
	}

}
#endif /* #if defined (TIMER_MODULE_TYPE) */


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetSD                                                                            */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sdNum -  SD module number                                                              */
/*                                                                                                         */
/* Returns:         BMC HAL Error code                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of SD                                                   */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS CLK_ResetSD(UINT32 sdNum)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (sdNum >= SD_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	if (sdNum == SD1_DEV)
	{
        SET_REG_FIELD(IPSRST2, IPSRST2_SDHC, 1);
        SET_REG_FIELD(IPSRST2, IPSRST2_SDHC, 0);
	}
	else if (sdNum == SD2_DEV)
	{
        SET_REG_FIELD(IPSRST2, IPSRST2_MMC, 1);
        SET_REG_FIELD(IPSRST2, IPSRST2_MMC, 0);
	}

	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetGMAC                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of GMAC                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ResetGMAC(UINT32 deviceNum)
{
	if (deviceNum == 2)
	{
        SET_REG_FIELD(IPSRST2, IPSRST2_GMAC1, 1);
        SET_REG_FIELD(IPSRST2, IPSRST2_GMAC1, 0);
	}
	else if (deviceNum == 3)
	{
        SET_REG_FIELD(IPSRST2, IPSRST2_GMAC2, 1);
        SET_REG_FIELD(IPSRST2, IPSRST2_GMAC2, 0);
	}

}

#if defined (PSPI_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ResetPSPI                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  deviceNum -                                                                            */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs SW reset of PSPI                                                 */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS CLK_ResetPSPI(UINT32 deviceNum)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (deviceNum >= PSPI_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	if (deviceNum == PSPI1_DEV)
	{
        SET_REG_FIELD(IPSRST2, IPSRST2_PSPI1, 1);
        SET_REG_FIELD(IPSRST2, IPSRST2_PSPI1, 0);
	}
	else if (deviceNum == PSPI2_DEV)
	{
        SET_REG_FIELD(IPSRST2, IPSRST2_PSPI2, 1);
        SET_REG_FIELD(IPSRST2, IPSRST2_PSPI2, 0);
	}

	return HAL_OK;
}
#endif /* #if defined (PSPI_MODULE_TYPE) */

/*Calculates the PLL frequency in Hz given PLL register value */
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_CalculatePLLFrequency                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  pllVal    -                                                                            */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns the PLL frequency in Hz                                           */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32  CLK_CalculatePLLFrequency (UINT32 pllVal)
{
	UINT32  FIN         = EXT_CLOCK_FREQUENCY_KHZ; /* 25MHz in KHz units */
	UINT32  FOUT        = 0;
	UINT32  NR          = 0;
	UINT32  NF          = 0;
	UINT32  NO          = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Extract PLL fields:                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	NR = READ_VAR_FIELD(pllVal, PLLCONn_INDV);   /* PLL Input Clock Divider */
	NF = READ_VAR_FIELD(pllVal, PLLCONn_FBDV);   /* PLL VCO Output Clock Feedback Divider). */
	NO = (READ_VAR_FIELD(pllVal, PLLCONn_OTDV1)) * (READ_VAR_FIELD(pllVal, PLLCONn_OTDV2));   /* PLL Output Clock Divider 1 */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate PLL frequency in Hz:                                                                     */
	/*-----------------------------------------------------------------------------------------------------*/
	FOUT = ((10*FIN*NF)/(NO*NR));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Notice: for better accurecy we multiply the "MONE" in 10, and later in 100 to get to Hz units.      */
	/*-----------------------------------------------------------------------------------------------------*/

	/*-----------------------------------------------------------------------------------------------------*/
	/* Returning value in Hertz:                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	return  FOUT*100 ;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll0Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL0 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32  CLK_GetPll0Freq (void)
{
	UINT32  pllVal      = 0;

	pllVal = REG_READ(PLLCON0);
	return CLK_CalculatePLLFrequency(pllVal);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll1Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL1 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32  CLK_GetPll1Freq(void)
{
	UINT32  pllVal      = 0;

	pllVal = REG_READ(PLLCON1);
	if (GCR_Get_Chip_Version() == POLEG_VERSION_Z1)
	{
        return CLK_CalculatePLLFrequency(pllVal);
	}
	else
	{
        return (CLK_CalculatePLLFrequency(pllVal)/2);
	}
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetPll2Freq                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the frequency of PLL2 in Hz                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static UINT32  CLK_GetPll2Freq(void)
{
	UINT32  pllVal      = 0;

	pllVal = REG_READ(PLLCON2);
	return (CLK_CalculatePLLFrequency(pllVal)/2);
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetMemoryFreq                                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates Memory frequency in Hz                                         */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetMemoryFreq (void)
{
	UINT32  FOUT        = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate CPU clock:                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (READ_REG_FIELD(CLKSEL, CLKSEL_MCCKSEL) == CLKSEL_MCCKSEL_PLL1)
	{
        FOUT = CLK_GetPll1Freq();
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_MCCKSEL) == CLKSEL_MCCKSEL_CLKREF)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Reference clock 25MHz:                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        FOUT = EXT_CLOCK_FREQUENCY_MHZ; /*FOUT is specified in MHz */
	}
	else
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* External clock, assume low freq ref clock (25MHz):                                              */
        /*-------------------------------------------------------------------------------------------------*/
        FOUT = EXT_CLOCK_FREQUENCY_MHZ; /*FOUT is specified in MHz */
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Returing value in Hertz                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	return FOUT;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetCPUFreq                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine calculates CPU frequency in Hz                                            */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  CLK_GetCPUFreq (void)
{
	UINT32  FOUT        = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Calculate CPU clock:                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (READ_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL) == CLKSEL_CPUCKSEL_PLL0)
	{
        FOUT = CLK_GetPll0Freq();
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL) == CLKSEL_CPUCKSEL_PLL1)
	{
        FOUT = CLK_GetPll1Freq();
	}
	else if (READ_REG_FIELD(CLKSEL, CLKSEL_CPUCKSEL) == CLKSEL_CPUCKSEL_CLKREF)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Reference clock 25MHz:                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        FOUT = EXT_CLOCK_FREQUENCY_MHZ; /*FOUT is specified in MHz */
	}
	else
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* External clock, assume low freq ref clock (25MHz):                                              */
        /*-------------------------------------------------------------------------------------------------*/
        FOUT = EXT_CLOCK_FREQUENCY_MHZ; /*FOUT is specified in MHz */
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Returing value in Hertz                                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	return FOUT;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetSPIFreq                                                                         */
/*                                                                                                         */
/* Parameters:      apb number,1 to 5                                                                      */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns SPI frequency  in Hz                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetSPIFreq (UINT32 spi)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Avalilable APBs between 1 to 5                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
     if (spi == 0)
     {
         return  CLK_GetCPUFreq()  / CLK_GetPLL0toAPBdivisor(SPI0);  // UINT32  CLK_GetPLL0toAPBdivisor (APB_CLK apb)
     }
     if (spi == 3)
     {
         return  CLK_GetCPUFreq()  / CLK_GetPLL0toAPBdivisor(SPI3);  // UINT32  CLK_GetPLL0toAPBdivisor (APB_CLK apb)
     }
     else
        return HAL_ERROR_BAD_PARAM;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetAPBFreq                                                                         */
/*                                                                                                         */
/* Parameters:      apb number,1 to 5                                                                      */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns APB frequency  in Hz                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetAPBFreq (UINT32 apb)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Avalilable APBs between 1 to 5                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
     if ((apb > APB5) && (apb < APB1))
        return HAL_ERROR_BAD_PARAM;

	/*-----------------------------------------------------------------------------------------------------*/
	/* In Yarkon APB frequency is CPU frequency divided by AHB0 Clock dividor, AHB1 Clock dividor and APB  */
	/* Clock divider                                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/

	return  CLK_GetCPUFreq()  / CLK_GetPLL0toAPBdivisor(apb);  // UINT32  CLK_GetPLL0toAPBdivisor (APB_CLK apb)
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetCPFreq                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns CP frequency in Hz                                                */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetCPFreq (void)
{
	UINT32  clk2Div = 0;

	UINT32  clk4Div = 0;

	clk2Div = (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV) + 1);

	clk4Div = (READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV) + 1);


	/*-----------------------------------------------------------------------------------------------------*/
	/* In Poleg APB frequency is CPU frequency divided by AHB0 Clock dividor, AHB1 Clock dividor and APB   */
	/* Clock divider                                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	return  (CLK_GetCPUFreq()  / (clk2Div * clk4Div)) ;

}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_SetCPFreq                                                                          */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets CP frequency in Hz                                                   */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS CLK_SetCPFreq (UINT32  cpFreq)
{
	UINT32  clkDiv = CLK_GetCPUFreq() / cpFreq;

	switch (clkDiv)
	{
        case 1:
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV1);
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV1);
            break;
        case 2:
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV2);
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV1);
            break;
        case 3:
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV3);
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV1);
            break;
        case 4:
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV2);
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV2);
            break;
        case 6:
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV3);
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV2);
            break;
        case 8:
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV, CLKDIV1_CLK4DIV4);
            SET_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV, CLKDIV1_CLK2DIV2);
            break;
        default:
		{
            return HAL_ERROR;
		}
	}

	CLK_Delay_MicroSec(20);

	return  HAL_OK;


}




#if defined (SD_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetSDClock                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  sdNum -  SD module number                                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns the SD base clock frequency in Hz                                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 CLK_GetSDClock (UINT32 sdNum)
{
	UINT32  divider;
	UINT32  pll0Freq;   /*In Hz */

	pll0Freq = CLK_GetPll0Freq();
	if ((SD_DEV_NUM_T)sdNum == SD1_DEV)
	{
        divider = 1 + READ_REG_FIELD(CLKDIV2, CLKDIV2_SD1CKDIV);
	}
	else if (sdNum == SD2_DEV)
	{
        divider = 1 + READ_REG_FIELD(CLKDIV1, CLKDIV1_MMCCKDIV);
	}
	else
	{
	    return HAL_ERROR_BAD_DEVNUM;
	}

	return (pll0Freq/divider);
}
#endif  /*#if defined (SD_MODULE_TYPE) */


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigurePCIClock                                                                  */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs configuration of PCI clock depending on                          */
/*                  presence of VGA BIOS as specified by STRAP13                                           */
/*---------------------------------------------------------------------------------------------------------*/
void CLK_ConfigurePCIClock(void)
{
	UINT32 PLLCON1_l = 0;

	/* Need to pgm the PCI clock to 96 MHz. */

	/* Done in following steps: */
	/* 1.  Delay 2us */
	/* 2.  Change CLKSEL1.GFXCKSEL ( bits 17-16)  from 1h to 3h (select PLL2) */
	/* 3.  Delay 2 us */
	/* 4.  Change CLKDIV1.PCICKDIV (bits 5-2) from 02h to 04h (divide by 5) */
	/* 5.  Delay 2us */

	PLLCON1_l  = CLK_333MHZ_PLLCON1_REG_CFG;

	SET_VAR_FIELD(PLLCON1_l, PLLCONn_PWDEN, PLLCONn_PWDEN_NORMAL);

	REG_WRITE(PLLCON1, PLLCON1_l);


	CLK_Delay_MicroSec(2);

	SET_REG_FIELD(CLKSEL, CLKSEL_GFXCKSEL,  CLKSEL_GFXCKSEL_PLL2); /* changed to workaroung PCI issue */

	CLK_Delay_MicroSec(2);

	SET_REG_FIELD(CLKDIV1, CLKDIV1_PCICKDIV,  CLKDIV1_PCICK_DIV(5));  /* PCI clock div = 5  */

	CLK_Delay_MicroSec(2);


/*    1.  PLL1 change to 33MHz -> PLLCON1 = A02403 */
/*    2.  PCI Clock = 96 MHz ->  CLKSEL.GFXCKSEL selects PLL2/2 */
/*                                              CLKDIV1.PCICKDIV = 4   (480/5 = 96) */
/* */
/* */
/*    3.  DRAM reference (in MR6: bits 5-0 = 0Ah) */
/*    4.  Poleg VREF = 0Ah   F05F01c8h bits 9-4 */
/*    5.  Reftersh period. MC CTL_51 value should be divide by 2 */
}

#if defined (FIU_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_ConfigureFIUClock                                                                  */
/*                                                                                                         */
/* Parameters:      UINT8  fiu, UINT8 clkDiv                                                                                */
/* Returns:         HAL_STATUS                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine config the FIU clock (according to the header )                           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS CLK_ConfigureFIUClock(UINT8  fiu, UINT8 clkDiv)
{

     /*----------------------------------------------------------------------------------------------------*/
     /* Defines the clock divide ratio from AHB to FIU0 clock.                                             */
     /*----------------------------------------------------------------------------------------------------*/
     UINT32  ratio = 0;

     /*----------------------------------------------------------------------------------------------------*/
     /* Ignored if FIU_Clk_Divider is either 0 or 0FFh.                                                    */
     /*----------------------------------------------------------------------------------------------------*/
     if ((clkDiv == 0) || (clkDiv == 0xFF))
        return HAL_ERROR_BAD_PARAM;

     /* set SPIn clk div */
     switch (fiu)
     {
        case FIU_MODULE_0:
            SET_REG_FIELD(CLKDIV3, CLKDIV3_SPI0CKDV,  (CLKDIV3_SPI0CKDV_DIV(clkDiv) & 0x1F));
            break;
        case  FIU_MODULE_3:
            SET_REG_FIELD(CLKDIV1, CLKDIV1_AHB3CKDIV, (CLKDIV1_AHB3CK_DIV(clkDiv)   & 0x1F));
            break;
        case  FIU_MODULE_X:
            SET_REG_FIELD(CLKDIV3, CLKDIV3_SPIXCKDV,  (CLKDIV3_SPIXCKDV_DIV(clkDiv) & 0x1F));
            break;
        default:
            return HAL_ERROR_BAD_DEVNUM;


     }

     /*----------------------------------------------------------------------------------------------------*/
     /* After changing this field, ensure a delay of 25 SPI0 clock cycles before changing CPUCKSEL field in*/
     /* CLKSEL register or accessing the AHB18 bus.                                                        */
     /*----------------------------------------------------------------------------------------------------*/
     ratio = READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK2DIV) * READ_REG_FIELD(CLKDIV1, CLKDIV1_CLK4DIV) * clkDiv;

     /* delay is according to ratio. Take some buffer too */
     CLK_Delay_Cycles(50 * ratio);

     return HAL_OK;
}
#endif /*  defined (FIU_MODULE_TYPE) */


#if defined (FIU_MODULE_TYPE)
/*---------------------------------------------------------------------------------------------------------*/
/* Function:        CLK_GetFIUClockDiv                                                                     */
/*                                                                                                         */
/* Parameters:      UINT8  fiu                                                                                   */
/* Returns:         CLKDIV                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine config the FIU clock (according to the header )                           */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 CLK_GetFIUClockDiv(UINT8  fiu)
{

     /*----------------------------------------------------------------------------------------------------*/
     /* Defines the clock divide ratio from AHB to FIU0 clock.1                                            */
     /*----------------------------------------------------------------------------------------------------*/
     switch (fiu)
     {
        case FIU_MODULE_0:
            return READ_REG_FIELD(CLKDIV3, CLKDIV3_SPI0CKDV) + 1;
        case  FIU_MODULE_3:
            return READ_REG_FIELD(CLKDIV1, CLKDIV1_AHB3CKDIV) + 1;
        case  FIU_MODULE_X:
            return READ_REG_FIELD(CLKDIV3, CLKDIV3_SPIXCKDV) + 1;
        default:
            return 0xFF;
     }


}
#endif/* defined (FIU_MODULE_TYPE) */

