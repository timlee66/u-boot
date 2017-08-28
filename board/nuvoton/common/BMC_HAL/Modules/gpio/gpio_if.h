/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   gpio_if.h                                                                                             */
/*            This file contains GPIO driver interface                                                     */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _GPIO_IF_H
#define _GPIO_IF_H

#include "defs.h"
#include "../../Common/hal_error.h"

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                                  TYPES                                                  */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Input related types                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	GPIO_EVENT_RISING_EDGE,
	GPIO_EVENT_FALLING_EDGE,
	GPIO_EVENT_HIGH_LEVEL,
	GPIO_EVENT_LOW_LEVEL,
} GPIO_EVENT_T;

typedef void (*GPIO_EVEN_HANDLE_T)(void* args);


/*---------------------------------------------------------------------------------------------------------*/
/* Output related types                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	GPIO_OUTPUT_PUSH_PULL,
	GPIO_OUTPUT_OPEN_DRAIN,
} GPIO_OUTPUT_T;


typedef enum
{
	GPIO_BLINK_OFF          = 0x0,
	GPIO_BLINK_0_5_to_1     = 0x1,
	GPIO_BLINK_1_to_2       = 0x2,
	GPIO_BLINK_1_to_4       = 0x3,
	GPIO_BLINK_0_25_to_0_5  = 0x4,
	GPIO_BLINK_0_25_to_1    = 0x5,
	GPIO_BLINK_3_to_4       = 0x6,
	GPIO_BLINK_0_75_to_1    = 0x7,
} GPIO_BLINK_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Muxing related types                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
	UINT position;
	UINT size;
	UINT value;
} GPIO_MUX_FIELD_T;


/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                               GPIO TABLES                                               */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* GPIO Table entry description:                                                                           */
/* -----------------------------                                                                           */
/* GPIO Table contains entries, 64 bit each, that contain information about GPIO port and bit ID,          */
/* GPIO capabilities (INPUT/OUTPUT/EVENT SUPPORT/HW DEBOUNCE/BLINKING) and muxing                          */
/*                                                                                                         */
/* Lower 32bit word decription:                                                                            */
/*                                                                                                         */
/* +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-+-+-+-+-+-+-+-+-+-+                 */
/* |              |            GPIO CAPABILITIES                     |      GPIO ID      |                 */
/* +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-+-+-+-+-+-+-+-+-+-+                 */
/* |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|                 */
/* +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-+-+-+-+-+-+-+-+-+-+                 */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | | | | | | | | | | |                  */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | | | | | | +-+-+-+-+---- Port Number  */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | | | | | |                            */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | +-+-+-+-+-------------- Bit Number   */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |                                      */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +-------------- Input Capability       */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |                                         */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +----------------- Output Capability      */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |                                            */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  +--+--+--+--+--+-------------------- Event Cap. Field       */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |                                                              */
/*   |  |  |  |  |  |  |  |  +--+--+--+--+--+-------------------------------------- HW Debounce Cap. Field */
/*   |  |  |  |  |  |  |  |                                                                                */
/*   |  |  +--+--+--+--+--+-------------------------------------------------------- Blinking Cap. Field    */
/*   |  |                                                                                                  */
/*   +--+-------------------------------------------------------------------------- RESERVERD              */
/*                                                                                                         */
/* Upper 32bit word description:                                                                           */
/*                                                                                                         */
/* +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-+-+-+-+-+-+-+-+-+-+                 */
/* |RSRVD|                  MUX 2                     |              MUX 1               |                 */
/* +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-+-+-+-+-+-+-+-+-+-+                 */
/* |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0|                 */
/* +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-+-+-+-+-+-+-+-+-+-+                 */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | | | | | | | | | | |                  */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | | | | | | | +-+-+-+-- Mux Reg1 ID    */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | | | | | | |                          */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | | | | | | |                          */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | | +-+-+-+-+---------- Field 1 Pos    */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | |                                    */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +--+-+-------------------- Field 1 Size   */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |                                            */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  +--+--+---------------------------- Field 1 Val    */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |                                                     */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |                                                     */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |  +--+--+--+------------------------------------- Mux Reg2 ID    */
/*   |  |  |  |  |  |  |  |  |  |  |  |  |                                                                 */
/*   |  |  |  |  |  |  |  |  +--+--+--+--+------------------------------------------------- Field 2 Pos    */
/*   |  |  |  |  |  |  |  |                                                                                */
/*   |  |  |  |  |  +--+--+---------------------------------------------------------------- Field 2 Size   */
/*   |  |  |  |  |                                                                                         */
/*   |  |  +--+--+------------------------------------------------------------------------- Field 2 Val    */
/*   |  |                                                                                                  */
/*   |  |                                                                                                  */
/*   +--+---------------------------------------------------------------------------------- RESERVED       */
/*                                                                                                         */
/*                                                                                                         */
/* Fields Description:                                                                                     */
/* -------------------                                                                                     */
/* Port number              - Number of the port of the specified GPIO                                     */
/*                                                                                                         */
/* Bit number               - Number of the bit in the port of the specified GPIO                          */
/*                                                                                                         */
/* Input Capability Bit     - Mark if the GPIO is capable to receive input                                 */
/*                                                                                                         */
/* Output Capability Bit    - Mark if the GPIO capable of transmit output                                  */
/*                                                                                                         */
/* Event Cap. Field         - Position in EVENT register if GPIO is event generation capable               */
/*                            (if not capable the field is filled with 1)                                  */
/*                                                                                                         */
/* HW Debounce Cap. Field   - Position in Debounce register if GPIO is debounce capable                    */
/*                            (if not capable the field is filled with 1)                                  */
/*                                                                                                         */
/* Blinking Cap. Field      - Position in BLINK register if GPIO is blinking capable                       */
/*                            (if not capable the field is filled with 1)                                  */
/*                                                                                                         */
/*                                                                                                         */
/* Mux Reg ID               - Number of MFSEL register that mux the current GPIO                           */
/*                            (if no muxing needed the field is filled with 0)                             */
/*                                                                                                         */
/* Mux Field 1 Pos          - Position of the first field in the MFSEL register that sets the mux          */
/*                                                                                                         */
/* Mux Field 1 Size         - Size of the first field                                                      */
/*                            (if no muxing set the size should be 0)                                      */
/*                                                                                                         */
/* Mux Field 1 Value1       - Value of the first field that muxes the GPIO                                 */
/*                                                                                                         */
/* Mux Field 2 Pos          - Position of the second field in the MFSEL register that sets the mux         */
/*                                                                                                         */
/* Mux Field 2 Size         - Size of the second field                                                     */
/*                            (if no additional muxing set the size should be 0)                           */
/*                                                                                                         */
/* Mux Field 2 Value1       - Value of the second field that muxes the GPIO                                */
/*                                                                                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/*                                           GPIO TABLE RELATED                                            */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Entry type                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
	UINT32  id;        /* Lower word contain GPIO ID and Capabilities */
	UINT32  mux;       /* Upper word contain muxing information */
} GPIO_TABLE_ENTRY_T;


/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Table Fields description                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_TABLE_ID_PORT_NUM          0, 5
#define GPIO_TABLE_ID_PORT_BIT          5, 5
#define GPIO_TABLE_CAP_INPUT_BIT       10, 1
#define GPIO_TABLE_CAP_OUTPUT_BIT      11, 1
#define GPIO_TABLE_CAP_EVENT_FIELD     12, 5
#define GPIO_TABLE_CAP_DEB_FIELD       17, 5
#define GPIO_TABLE_CAP_BLINK_FIELD     22, 5


#define GPIO_TABLE_MUX_REGID1           0, 4
#define GPIO_TABLE_MUX_FIELD1_POS       4, 5
#define GPIO_TABLE_MUX_FIELD1_SIZE      9, 3
#define GPIO_TABLE_MUX_FIELD1_VAL      12, 3
#define GPIO_TABLE_MUX_REGID2          15, 4
#define GPIO_TABLE_MUX_FIELD2_POS      19, 5
#define GPIO_TABLE_MUX_FIELD2_SIZE     24, 3
#define GPIO_TABLE_MUX_FIELD2_VAL      27, 3


/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Table entry creation macro                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_TABLE_ENTRY(port, port_bit, cap_in, cap_out, cap_event, cap_db, cap_blink, mux_reg1, mux_field1_p, mux_field1_s, mux_field1_val, mux_reg2, mux_field2_p, mux_field2_s, mux_field2_val)    \
        {                                                                                                   \
             (  BUILD_FIELD_VAL(GPIO_TABLE_ID_PORT_NUM,         port                )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_ID_PORT_BIT,         port_bit            )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_CAP_INPUT_BIT,       cap_in              )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_CAP_OUTPUT_BIT,      cap_out             )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_CAP_EVENT_FIELD,     cap_event           )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_CAP_DEB_FIELD,       cap_db              )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_CAP_BLINK_FIELD,     cap_blink           ) ),                    \
                                                                                                            \
             (  BUILD_FIELD_VAL(GPIO_TABLE_MUX_REGID1,          mux_reg1            )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_MUX_FIELD1_POS,      mux_field1_p        )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_MUX_FIELD1_SIZE,     mux_field1_s        )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_MUX_FIELD1_VAL,      mux_field1_val      )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_MUX_REGID2,          mux_reg2            )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_MUX_FIELD2_POS,      mux_field2_p        )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_MUX_FIELD2_SIZE,     mux_field2_s        )   |                   \
                BUILD_FIELD_VAL(GPIO_TABLE_MUX_FIELD2_VAL,      mux_field2_val      ) )                     \
        }


/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Capabilities                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define     GPIO_CAP_INPUT          1
#define     GPIO_CAP_NO_INPUT       0

#define     GPIO_CAP_OUTPUT         1
#define     GPIO_CAP_NO_OUTPUT      0

#define     GPIO_CAP_EVENT(x)       (x)
#define     GPIO_CAP_NO_EVENT       0x3F

#define     GPIO_CAP_BLINK(x)       (x)
#define     GPIO_CAP_NO_BLINK       0x3F

#define     GPIO_CAP_DEBOUNCE(x)    (x)
#define     GPIO_CAP_NO_DEBOUNCE    0x3F



/*---------------------------------------------------------------------------------------------------------*/
/* GPIO Mux Register ID                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	GPIO_MUX_REGID_NONE     = 0,
	GPIO_MUX_REGID_MFSL1    = 1,
	GPIO_MUX_REGID_MFSL2    = 2,
	GPIO_MUX_REGID_MFSL3    = 3,
	GPIO_MUX_REGID_MFSL4    = 4,
	GPIO_MUX_REGID_MFSL5    = 5,
	GPIO_MUX_REGID_MFSL6    = 6,
	GPIO_MUX_REGID_MFSL7    = 7,
} GPIO_MUX_REGID_T;






/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
/*                                           FUNCTIONS INTERFACE                                           */
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* End of GPIO argument list marker                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIO_END_OF_ARG_LIST        -1


/*---------------------------------------------------------------------------------------------------------*/
/* Defines group of GPIOs (1 or more) that should be passed to variadic interface functions                */
/* that can handle multiple GPIO numbers                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define GPIOS(gpios...)         gpios, GPIO_END_OF_ARG_LIST



HAL_STATUS GPIO_Init(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigureInput                                                                    */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  handler             - Handler function for GPIO event                                  */
/*                  handler_args        - Handler function argument                                        */
/*                  event_type          - Event type (Raising/Falling edge, High/Low level)                */
/*                  debounce            - Enable HW debounce                                               */
/*                  internal_resistor   - Enable internal Pull-Up/Down                                     */
/*                  gpio_num            - One or more GPIO number(s) to configure                          */
/*                                        SHOULD BE SET USING 'GPIOS' macro                                */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures one or more GPIOs to input                                     */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GPIO_ConfigureInput( GPIO_EVEN_HANDLE_T  handler,
                                void*               handler_args,
                                GPIO_EVENT_T        event_type,
                                BOOLEAN             debounce,
                                BOOLEAN             internal_resistor,
                                UINT                gpio_num, ... );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_ConfigureOutput                                                                   */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  type     -  Output type (Push Pull/ Open Drain)                                        */
/*                  gpio_num -  One or more GPIO number(s) to configure                                    */
/*                              SHOULD BE SET USING 'GPIOS' macro                                          */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine configures one or more GPIOs to output                                    */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GPIO_ConfigureOutput(GPIO_OUTPUT_T type, BOOLEAN init_val, UINT gpio_num, ... );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Blink                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  type     - Blinking timing configuration                                               */
/*                  gpio_num - One or more GPIO number(s) to configure                                     */
/*                             SHOULD BE SET USING 'GPIOS' macro                                           */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine sets one or more GPIOs to start blinking.                                 */
/*                  The given GPIOs must be configured to Output                                           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GPIO_Blink(GPIO_BLINK_T type, UINT gpio_num, ... );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Write                                                                             */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  value    - Value of the GPIO(s) to set                                                 */
/*                  gpio_num - One or more GPIO number(s) to configure                                     */
/*                             SHOULD BE SET USING 'GPIOS' macro                                           */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine set value of one or more GPIOs.                                           */
/*                  The given GPIOs must be configured to Output                                           */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GPIO_Write(BOOLEAN value, UINT gpio_num, ... );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Read                                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num - GPIO number to read the value from                                          */
/*                                                                                                         */
/* Returns:         Value of the given GPIO number                                                         */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine reads value of given GPIO                                                 */
/*                  The given GPIOs must be configured to Input                                            */
/*---------------------------------------------------------------------------------------------------------*/
BOOLEAN GPIO_Read(UINT gpio_num);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_GetMuxInfo                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num        - Number of given GPIO                                                 */
/*                  mux_regid1      - ID of the first muxing register                                      */
/*                  mux_field1      - Muxing info of the first register                                    */
/*                  mux_regid2      - ID of the second muxing register                                     */
/*                  mux_field2      - Muxing info of the second register                                   */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:    none                                                                                   */
/* Description:                                                                                            */
/*                  This routine returns muxing information for given GPIO                                 */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GPIO_GetMuxInfo( UINT                gpio_num,
                            GPIO_MUX_REGID_T*   mux_regid1,
                            GPIO_MUX_FIELD_T*   mux_field1,
                            GPIO_MUX_REGID_T*   mux_regid2,
                            GPIO_MUX_FIELD_T*   mux_field2  );


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_GetGPIOInfo                                                                       */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  gpio_num -                                                                             */
/*                  port_bit -                                                                             */
/*                  port_num -                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine returns GPIO information                                                  */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GPIO_GetGPIOInfo(UINT gpio_num, UINT* port_num, UINT* port_bit);


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GPIO_Isr                                                                               */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  arg -                                                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs GPIO IRQ handling                                                */
/*                  It should be called as a handler for all GPIO interrupts                               */
/*---------------------------------------------------------------------------------------------------------*/
HAL_STATUS GPIO_Isr(UINT32 arg);

#endif /*_GPIO_IF_H */

