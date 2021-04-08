/******************************************************************************
    Copyright:  © 2008 Nuvoton (TA)
    
    created:    11 7, 2005   15:57
    filename:   CoreRegisters.h
    author:     Avi Fishman
    
    purpose:    Macros for accessing Core Registers
******************************************************************************/

#ifndef __CoreRegisters_h__
#define __CoreRegisters_h__

#       define _BIT(reg,bit)        (1 << bit)
#       define HW_BYTE(add) (*(( volatile unsigned char *)(uintptr_t)(add)))
#       define HW_WORD(add) (*(( volatile unsigned short *)(uintptr_t)(add)))
#       define HW_DWORD(add) (*(( volatile unsigned int *)(uintptr_t)(add)))
#       define _READ_REG(RegName) (RegName)
#       define _WRITE_REG(RegName,data) (RegName=data)
#       define _SET_BIT_REG(RegName, Bit) (RegName|=(1<<Bit))
#       define _CLEAR_BIT_REG(RegName, Bit) (RegName&=(~(1<<Bit)))
#       define _READ_BIT_REG(RegName, Bit) ((RegName>>Bit)&1)

/******************************************************************************
* Macro: READ_REG()
*           
* Purpose:  Read core register regardless of it size (BYTE, WORD or DWORD)
*           
* Params:   RegisterName - Core register name as defined in the chip header file
*           
* Returns:  The value of the register according to the size of register
*           
* Comments: RegisterName must be HW_BYTE(x), HW_WORD(x) or HW_DWORD(x)
*           
******************************************************************************/
#define READ_REG(RegisterName) _READ_REG(RegisterName)


/******************************************************************************
* Macro: WRITE_REG()
*           
* Purpose:  Write to core register regardless of it size (BYTE, WORD or DWORD)
*           
* Params:   RegisterName - Core register name as defined in the chip header file
*           Value - The value to write into the register according its size
*           
* Returns:  none
*           
* Comments: RegisterName must be HW_BYTE(x), HW_WORD(x) or HW_DWORD(x)
*           
******************************************************************************/
#define WRITE_REG(RegisterName, Value) _WRITE_REG(RegisterName, Value)


/******************************************************************************
* Macro: SET_BIT_REG()
*           
* Purpose:  Set a bit in core register
*           
* Params:   RegisterName - Core register name as defined in the chip header file
*           BitNum - The number of bit to set
*           
* Returns:  none
*           
* Comments: RegisterName must be HW_BYTE(x), HW_WORD(x) or HW_DWORD(x)
*           This macro is doing WRITE_REG(RegName, READ_REG(RegName)|(1<<Bit)) 
*           
******************************************************************************/
#define SET_BIT_REG(RegisterName, BitNum) _SET_BIT_REG(RegisterName, BitNum)


/******************************************************************************
* Macro: CLEAR_BIT_REG()
*           
* Purpose:  Clear a bit in core register
*           
* Params:   RegisterName - Core register name as defined in the chip header file
*           BitNum - The number of bit to clear
*           
* Returns:  none
*           
* Comments: RegisterName must be HW_BYTE(x), HW_WORD(x) or HW_DWORD(x)
*           This macro is doing WRITE_REG(RegName, READ_REG(RegName)&(~(1<<Bit)))
*           
******************************************************************************/
#define CLEAR_BIT_REG(RegisterName, BitNum) _CLEAR_BIT_REG(RegisterName, BitNum)


/******************************************************************************
* Macro: READ_BIT_REG()
*           
* Purpose:  Read a bit from core register
*           
* Params:   RegisterName - Core register name as defined in the chip header file
*           BitNum - The number of bit to read
*           
* Returns:  The value of the bit.
*           
* Comments: RegisterName must be HW_BYTE(x), HW_WORD(x) or HW_DWORD(x)
*           This macro is doing ((READ_REG(RegName)>>Bit)&1)
*           
******************************************************************************/
#define READ_BIT_REG(RegisterName, BitNum) _READ_BIT_REG(RegisterName, BitNum)

#endif //__CoreRegisters_h__
