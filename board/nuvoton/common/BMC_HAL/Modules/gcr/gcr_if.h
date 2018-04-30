/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   gcr_if.h                                                                                              */
/*            This file contains GCR interface                                                             */
/*  Project:                                                                                               */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _GCR_IF_H
#define _GCR_IF_H

#include "defs.h"
#include "../../Common/hal_error.h"
#include "../sd/sd_if.h"

/*---------------------------------------------------------------------------------------------------------*/
/* VCD Head Configuration                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	GCR_VCD_HEAD1,
	GCR_VCD_HEAD2,
} GCR_VCD_HEAD_T;

/*---------------------------------------------------------------------------------------------------------*/
/* GFX Configuration                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	GCR_GFX_CONFIG_OUTPUT_FROM_HEAD1,
	GCR_GFX_CONFIG_OUTPUT_FROM_HEAD2,
	GCR_GFX_CONFIG_INPUT_TO_KVM,
}GCR_GFX_CONFIG_T;



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_Uart                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  redirection_mode - Mode of UART redirection                                            */
/*                  CoreSP           - Mux CoreSP                                                          */
/*                  sp1              - Mux SP1                                                             */
/*                  sp2              - Mux SP2                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects UART muxing                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_Uart(UINT redirection_mode, BOOLEAN CoreSP, BOOLEAN sp1, BOOLEAN sp2);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Core_Uart_Get                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns current UART number to be used by UBOOT\LINUX                     */
/*---------------------------------------------------------------------------------------------------------*/
UINT GCR_Core_Uart_Get (void);


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
void GCR_Mux_SPIFash(UINT devNum);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_MuxFIU                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum  - SPI Flash device number                                                      */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SPI Flash muxing                                                  */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GCR_MuxFIU (   FIU_MODULE_T      devNum,
                          BOOLEAN           cs0Enable,
                          BOOLEAN           cs1Enable,
                          BOOLEAN           cs2Enable,
                          BOOLEAN           cs3Enable,
                          BOOLEAN           quadMode);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_RMII                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - Ethernet device number                                                        */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects RMII mux                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_RMII(UINT devNum);


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
void GCR_SetDacSense(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_RMII                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  devNum - Gigabit Ethernet device number                                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects GMII mux                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_GMII(UINT devNum);


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
void GCR_Mux_GPIO(UINT gpio_num);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_SIOXH                                                                          */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  enable - if TRUE enables the mux, otherwise disable                                    */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SIOXH mux                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_Mux_SIOXH(BOOLEAN enable);


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
void GCR_Mux_PSPI(UINT devNum);


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
void GCR_Mux_GSPI(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_PowerOn_GetMemorySize_limited                                                      */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns configured DDR memory size                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 GCR_PowerOn_GetMemorySize_limited(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_PowerOn_GetMemorySize                                                              */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns configured DDR memory size                                        */
/*---------------------------------------------------------------------------------------------------------*/
UINT32  GCR_PowerOn_GetMemorySize(void);


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
void GCR_PinPullUpDown(UINT32 gpio_num, BOOLEAN enable);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_Mux_SD                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                 sdNum    - [in], SD device number                                                       */
/*                 sdType   - [in], SD1 device type (SD or MMC)                                            */
/*                                                                                                         */
/* Returns:         BMC HAL Error code                                                                     */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine selects SD mux                                                            */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GCR_Mux_SD(SD_DEV_NUM_T sdNum, SD_DEV_TYPE_T sdType);


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
void GCR_EmmcBootTimeConfig (UINT32 clk);


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
void GCR_EmmcBootStart (BOOLEAN ack);


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
BOOLEAN GCR_EmmcBootInvalid (void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_EmmcBootEnd                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine ends a EMMC boot sequence, either when the boot sequence is done,         */
/*or an error is discovered.                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_EmmcBootEnd (void);


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
void GCR_SetVCDInput(GCR_VCD_HEAD_T headNum);


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
void GCR_SetGFXConfig(GCR_GFX_CONFIG_T config);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_ConfigUSBBurstSize                                                                 */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Configures the USB burst size in USB test register                                     */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_ConfigUSBBurstSize(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_ResetTypeIsPor                                                                     */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Returns true if reset source was POR (powerOn) reset                                   */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GCR_ResetTypeIsPor(void);

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
BOOLEAN GCR_ResetTypeIsCor(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_MarkCfgDone                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Sets a flag in scratch register specifying that CLK/DDR configuration were performed   */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_MarkCfgDone(void);


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
BOOLEAN GCR_CfgWasDone(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_EnableJtag                                                                         */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Enable Jtag access                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_EnableJtag(void);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GCR_DisableJtag                                                                        */
/*                                                                                                         */
/* Parameters:      none                                                                                   */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  Disable Jtag access                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void GCR_DisableJtag(void);


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
void GCR_DisableModules(UINT32 mdlr);


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
UINT32 GCR_Get_Chip_Version(void);


#endif

