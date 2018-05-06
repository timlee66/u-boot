/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation Confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2014-2015 by Nuvoton Technology Corporation                                              */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   uart_drv.c                                                                                            */
/*            This file contains implementation of UART driver                                             */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/


#include "../../../Chips/chip_if.h"

#include "uart_drv.h"
#include "uart_regs.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_Init                                                                              */
/*                                                                                                         */
/* Parameters:      devNum - uart module number                                                            */
/*                  muxMode - configuration mode (last setting is the one that is active)                  */
/*                  baudRate - BAUD for the UART module                                                    */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs UART initialization                                              */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_Init(UART_DEV_T devNum, UART_MUX_T muxMode, UART_BAUDRATE_T baudRate)
{
	UINT32 FCR_Val      = 0;

	BOOLEAN CoreSP  = FALSE;
	BOOLEAN sp1     = FALSE;
	BOOLEAN sp2     = FALSE;
	UINT32  ret     = 0;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Reseting the module                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	/* removed since resets all UARTS */
	/*CLK_ResetUART(devNum); */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Muxing for UART0                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum == UART0_DEV)
	{
        CoreSP = TRUE;
	}

#if defined NPCM650
	/*-----------------------------------------------------------------------------------------------------*/
	/* Muxing for UART1                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	else if (devNum == UART1_DEV)
	{
        CoreSP = FALSE;

        switch (muxMode)
        {
            case UART_MUX_CORE_SNOOP:
            case UART_MUX_CORE_TAKEOVER:
                {
                    sp1 = TRUE;
                    sp2 = TRUE;
                    break;
                }
            case UART_MUX_CORE_SP2__SP1_SI1:
                {
                    sp1 = TRUE;
                    break;
                }
            case UART_MUX_CORE_SP2__SP1_SI2:
                {
                    sp2= TRUE;
                    break;
                }

	        case UART_MUX_SKIP_CONFIG:
	            {
	                /* Do nothing. Don't call CHIP_Mux_UART. Assuming it was called before */
	                break;
	            }

            /*---------------------------------------------------------------------------------------------*/
            /* Illegal mux mode                                                                            */
            /*---------------------------------------------------------------------------------------------*/
            default: return HAL_ERROR_BAD_PARAM;
        }
	}
#elif (defined NPCM750 || defined NPCM750_CP)
	/*-----------------------------------------------------------------------------------------------------*/
	/* Enable serial interfaces according to mux mode                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	switch (muxMode)
	{
        case UART_MUX_MODE4_HSP1_SI1____HSP2_SI2____UART1_s_SI1___UART3_s_SI2__UART2_s_HSP1:
        case UART_MUX_MODE6_HSP1_SI1____HSP2_SI2____UART1_s_SI1___UART3_s_SI2__UART2_s_HSP2:
        case UART_MUX_MODE7_HSP1_SI1____HSP2_UART2__UART1_s_HSP1__UART3_SI2:
            {
                sp1 = TRUE;
                sp2 = TRUE;
                break;
            }
        case UART_MUX_MODE5_HSP1_SI1____HSP2_UART2__UART1_s_HSP1__UART3_s_SI1:
            {
                sp1 = TRUE;
                break;
            }
        case UART_MUX_MODE1_HSP1_SI2____HSP2_UART2__UART1_s_HSP1__UART3_s_SI2:
        case UART_MUX_MODE2_HSP1_UART1__HSP2_SI2____UART2_s_HSP2__UART3_s_SI2:
        case UART_MUX_MODE3_HSP1_UART1__HSP2_UART2__UART3_SI2:
            {
                sp2= TRUE;
                break;
            }

        case UART_MUX_SKIP_CONFIG:
            {
                /* Do nothing. Don't call CHIP_Mux_UART. Assuming it was called before */
                break;
            }

        /*---------------------------------------------------------------------------------------------*/
        /* Illegal mux mode                                                                            */
        /*---------------------------------------------------------------------------------------------*/
        default: return DEFS_STATUS_INVALID_PARAMETER;
	}
#endif
	if (muxMode != UART_MUX_SKIP_CONFIG)
	{
        GCR_Mux_Uart(muxMode, CoreSP, sp1, sp2);
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Disable interrupts                                                                                  */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(UART_LCR(devNum), 0);            /* prepare to Init UART */
	REG_WRITE(UART_IER(devNum), 0x0);          /* Disable all UART interrupt */

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set baudrate                                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	ret += UART_SetBaudrate(devNum, baudRate);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set port for 8 bit, 1 stop, no parity                                                               */
	/*-----------------------------------------------------------------------------------------------------*/
	ret += UART_SetBitsPerChar(devNum, 8);
	ret += UART_SetStopBit(devNum, UART_STOPBIT_1);
	ret += UART_SetParity(devNum, UART_PARITY_NONE);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set the RX FIFO trigger level, reset RX, TX FIFO                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	FCR_Val = 0;
	SET_VAR_FIELD(FCR_Val, FCR_RFITL, FCR_RFITL_4B);
	SET_VAR_FIELD(FCR_Val, FCR_TFR, 1);
	SET_VAR_FIELD(FCR_Val, FCR_RFR, 1);
	SET_VAR_FIELD(FCR_Val, FCR_FME, 1);

	REG_WRITE(UART_FCR(devNum), FCR_Val);
	REG_WRITE(UART_TOR(devNum), 0x0);

	if (ret > 0)
        return HAL_ERROR;
	else
        return HAL_OK;

}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PutC                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  c - char to write to UART                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write single char to UART                                                 */
/*                  Note that the function is blocking till char can be send                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_PutC(UART_DEV_T devNum, const UINT8 c )
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait until Tx ready                                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	while (!READ_REG_FIELD(UART_LSR(devNum), LSR_THRE));


	/*-----------------------------------------------------------------------------------------------------*/
	/* Put the char                                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(UART_THR(devNum), (c & 0xFF));

	return HAL_OK;
}





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_GetC                                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads char from UART                                                      */
/*                  Note that the function is blocking till char is available                              */
/*---------------------------------------------------------------------------------------------------------*/
UINT8 UART_GetC( UART_DEV_T devNum )
{
	UINT8 Ch;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return 0;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait until char is available                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	while (!UART_TestRX(devNum));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Reading the char                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	Ch = REG_READ(UART_RBR(devNum)) & 0xFF;

	return Ch;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_PutC_NB                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  c - char to write to UART                                                              */
/*                                                                                                         */
/* Returns:         HAL_OK if the char was written or error if it couldn't be written                      */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine write single char to UART in NON-Blocking manner                          */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_PutC_NB(UART_DEV_T devNum, const UINT8 c )
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Put the char                                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	REG_WRITE(UART_THR(devNum), (c & 0xFF));

	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_GetC_NB                                                                           */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         HAL_OK if char was read or error if it no char was available                           */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads char from UART in NON-Blocking manner                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_GetC_NB( UART_DEV_T devNum, UINT8* c )
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* wait until char is available                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	if (!UART_TestRX(devNum))
	{
        return HAL_ERROR_QUEUE_EMPTY;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Reading the char                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	*c = (REG_READ(UART_RBR(devNum)) & 0xFF);

	return HAL_OK;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_TestRX                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine test if there is a char in RX fifo                                        */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN UART_TestRX( UART_DEV_T devNum )
{

	if (READ_REG_FIELD(UART_LSR(devNum), LSR_RFDR))
        return TRUE;
	else
        return FALSE;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_TestTX                                                                            */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine test if there is a char in TX fifo                                        */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN UART_TestTX( UART_DEV_T devNum )
{
	if (!READ_REG_FIELD(UART_LSR(devNum), LSR_THRE))
        return TRUE;
	else
        return FALSE;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_ResetFIFOs                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  rxFifo - if TRUE RX fifo is reseted                                                    */
/*                  txFifo - if TRUE TX fifo is reseted                                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs FIFO reset                                                       */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_ResetFIFOs(UART_DEV_T devNum, BOOLEAN txFifo, BOOLEAN rxFifo)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Reseting fifos                                                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	if (txFifo)
	{
        SET_REG_FIELD(UART_FCR(devNum), FCR_TFR, 1);
	}

	if (rxFifo)
	{
        SET_REG_FIELD(UART_FCR(devNum), FCR_RFR, 1);
	}

	return HAL_OK;
}






/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetTxIrqState                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  On -                                                                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine enables/disables Tx Interrupt                                             */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_SetTxIrqState(UART_DEV_T devNum, BOOLEAN On)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting Tx State                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (On)
	{
        SET_REG_FIELD(UART_IER(devNum), IER_THREIE, 1);
	}
	else
	{
        SET_REG_FIELD(UART_IER(devNum), IER_THREIE, 0);
	}

	return HAL_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetRxIrqState                                                                     */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  On -                                                                                   */
/*                  timeout -                                                                              */
/*                  triggerLevel -                                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs Rx interrupt enable/disable and configuration                    */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_SetRxIrqState(UART_DEV_T devNum, BOOLEAN On)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting Rx state                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (On)
	{
        SET_REG_FIELD(UART_IER(devNum), IER_RDAIE, 1);
        SET_REG_FIELD(UART_TOR(devNum), TOR_TOIE, 1);
	}
	else
	{
        SET_REG_FIELD(UART_IER(devNum), IER_RDAIE, 0);
        SET_REG_FIELD(UART_TOR(devNum), TOR_TOIE, 0);
	}

	return HAL_OK;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetRxConfig                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  timeout -                                                                              */
/*                  triggerLevel -                                                                         */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs Rx irq configurations                                            */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_SetRxConfig(UART_DEV_T devNum, UINT8 timeout, UART_RXFIFO_TRIGGER_T triggerLevel)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting Rx interrupt timeout                                                                        */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(UART_TOR(devNum), TOR_TOIC, (timeout & 0x7F));

	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting Rx interrupt FIFO trigger level                                                             */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(UART_FCR(devNum), FCR_RFITL, (triggerLevel<<2));

	return HAL_OK;
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetParity                                                                         */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  parity -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets parity configuration                                                 */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_SetParity(UART_DEV_T devNum, UART_PARITY_T parity)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	if (parity != UART_PARITY_NONE)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Parity enable, choosing type                                                                    */
        /*-------------------------------------------------------------------------------------------------*/
        SET_REG_FIELD(UART_LCR(devNum), LCR_PBE, 1);

        if (parity == UART_PARITY_EVEN)
        {
            SET_REG_FIELD(UART_LCR(devNum), LCR_EPE, 1);

        }
        else if (parity == UART_PARITY_ODD)
        {
            SET_REG_FIELD(UART_LCR(devNum), LCR_EPE, 0);
        }
        else
        {
            /*---------------------------------------------------------------------------------------------*/
            /* Unknown parity type                                                                         */
            /*---------------------------------------------------------------------------------------------*/
            return HAL_ERROR_BAD_PARAM;
        }

	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* No parity                                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	else
	{
        SET_REG_FIELD(UART_LCR(devNum), LCR_PBE, 0);
	}

	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBitsPerChar                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  bits -                                                                                 */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set bits per char                                                         */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_SetBitsPerChar(UART_DEV_T devNum, UINT32 bits)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	switch (bits)
	{
        case 5:   SET_REG_FIELD(UART_LCR(devNum), LCR_WLS, LCR_WLS_5bit);   break;
        case 6:   SET_REG_FIELD(UART_LCR(devNum), LCR_WLS, LCR_WLS_6bit);   break;
        case 7:   SET_REG_FIELD(UART_LCR(devNum), LCR_WLS, LCR_WLS_7bit);   break;
        default:
        case 8:   SET_REG_FIELD(UART_LCR(devNum), LCR_WLS, LCR_WLS_8bit);   break;
	}

	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBaudrate                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  baudrate -                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets new baudrate                                                         */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_SetBaudrate(UART_DEV_T devNum, UART_BAUDRATE_T baudrate)
{
	INT32       divisor     = 0;
	UINT32      uart_clock  = 0;
	HAL_STATUS  ret         = HAL_OK;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configuring UART clock                                                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	uart_clock = CLK_ConfigureUartClock();

	/*-----------------------------------------------------------------------------------------------------*/
	/* Computing the divisor for the given baudrate.                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	divisor = ((INT32)uart_clock / ((INT32)baudrate * 16)) - 2;

	/* since divisor is rounded down check if it is better when rounded up */
	if ( ((INT32)uart_clock / (16 * (divisor + 2)) - (INT32)baudrate) >
         ((INT32)baudrate - (INT32)uart_clock / (16 * ((divisor+1) + 2))) )
	{
        divisor++;
	}

	if (divisor < 0 )
	{
        divisor = 0;
        ret = HAL_ERROR_BAD_FREQ;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Set baud rate to baudRate bps                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	SET_REG_FIELD(UART_LCR(devNum), LCR_DLAB, 1);    /* prepare to access Divisor */
	REG_WRITE(UART_DLL(devNum), LSB(divisor));
	REG_WRITE(UART_DLM(devNum), MSB(divisor));
	SET_REG_FIELD(UART_LCR(devNum), LCR_DLAB, 0);   /* prepare to access RBR, THR, IER */

	return ret;


}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetStopBit                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  stopbit -                                                                              */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets number of stopbits                                                   */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_SetStopBit(UART_DEV_T devNum, UART_STOPBIT_T stopbit)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	if (stopbit == UART_STOPBIT_1)
	{
        SET_REG_FIELD(UART_LCR(devNum), LCR_NSB, 0);
	}
	else if (stopbit == UART_STOPBIT_DYNAMIC)
	{
        SET_REG_FIELD(UART_LCR(devNum), LCR_NSB, 1);
	}
	else
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Unknown stopbits configuration                                                                  */
        /*------------------------------------------------------------------------------------------------*/
        return HAL_ERROR_BAD_PARAM;
	}

	return HAL_OK;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_SetBreak                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets break on the given UART                                              */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_SetBreak(UART_DEV_T devNum, BOOLEAN state)
{
	/*-----------------------------------------------------------------------------------------------------*/
	/* Parameters check                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	if (devNum >= UART_NUM_OF_MODULES)
	{
        return HAL_ERROR_BAD_DEVNUM;
	}

	if (state)
	{
        SET_REG_FIELD(UART_LCR(devNum), LCR_BCB, 1);
	}
	else
	{
        SET_REG_FIELD(UART_LCR(devNum), LCR_BCB, 0);
	}

	return HAL_OK;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_UartIsInit                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns TRUE is UART module was init, FALSE otherwise                     */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN UART_UartIsInit(UART_DEV_T devNum)
{

	// AviF:
	//The easiest way to check is LCR register - offset C: after reset it is 0x00 after configuration it is 0x03 (WLS: Word Length Select is 8 bits).
	//To check baud rate you need also to change DLAB bit before so more complicated.
	//
	//So now it is simple for u-boot:
	//If BB set UART0.LCR == 0x03 use UART0
	//If BB set UART3.LCR == 0x03 use UART3
	//They can't be both set.

	if (REG_READ(UART_LCR(devNum)) != 0)
		return TRUE;
	else
		return FALSE;

}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        UART_Irq                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum -                                                                               */
/*                  rxCallback -                                                                           */
/*                  rxParam -                                                                              */
/*                  txCallback -                                                                           */
/*                  txParam -                                                                              */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs IRQ handling                                                     */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS UART_Isr(UART_DEV_T devNum,  UART_irq_callback_t rxCallback, void* rxParam,
                                        UART_irq_callback_t txCallback, void* txParam)
{
	HAL_STATUS  ret = HAL_OK;
	UINT32      iir = REG_READ(UART_IIR(devNum)) & 0xF;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking if we got any interrupts at all                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	if (READ_VAR_FIELD(iir, IIR_NIP))
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* if no interrupts actually occurred, we return "not handled"                                     */
        /*-------------------------------------------------------------------------------------------------*/
        ret = HAL_ERROR_NOT_HANDLED;
	}
	else
	{
        switch (READ_VAR_FIELD(iir, IIR_IID))
        {
            /*---------------------------------------------------------------------------------------------*/
            /* We don't support modem interrups yet                                                        */
            /*---------------------------------------------------------------------------------------------*/
            case IIR_IID_MODEM:                                     break;

            /*---------------------------------------------------------------------------------------------*/
            /* Tx Interrupt                                                                                */
            /*---------------------------------------------------------------------------------------------*/
            case IIR_IID_THRE:      txCallback(devNum, txParam);    break;


            /*---------------------------------------------------------------------------------------------*/
            /* Rx Interrupts                                                                               */
            /*---------------------------------------------------------------------------------------------*/
            case IIR_IID_TOUT:
            case IIR_IID_RDA:       rxCallback(devNum, rxParam);    break;


            /*---------------------------------------------------------------------------------------------*/
            /* WE should never get here                                                                    */
            /*---------------------------------------------------------------------------------------------*/
            default:                                                break;

            /*---------------------------------------------------------------------------------------------*/
            /* Error interrupts                                                                            */
            /*---------------------------------------------------------------------------------------------*/
            case IIR_IID_RLS:
            {
                UINT32 lsr = REG_READ(UART_LSR(devNum));
                if      (READ_VAR_FIELD(lsr, LSR_OEI))
                    ret = HAL_ERROR_OVERRUN_OCCURRED;
                else if (READ_VAR_FIELD(lsr, LSR_PEI))
                    ret = HAL_ERROR_BAD_PARITY;
                else if (READ_VAR_FIELD(lsr, LSR_FEI))
                    ret = HAL_ERROR_BAD_FRAME;
                else if (READ_VAR_FIELD(lsr, LSR_BII))
                    ret = HAL_ERROR_BREAK_OCCURRED;
                else
                    ret = HAL_ERROR;

                break;
            }
        }
	}

	return ret;
}

