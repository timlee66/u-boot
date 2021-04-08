/*--------------------------------------------------------------------------
 * Copyright (c) 2009-2010 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------*/
#ifndef _COMMON_H_
#define _COMMON_H_
 
/*------------------------------------------------------------------------*/
/*----------------   Constants and macros definitions  -------------------*/
/*------------------------------------------------------------------------*/

/*----------------------------------------------------------------*/
/* Memory Map - Make sure it is compatible with linkObj.def file! */
/*----------------------------------------------------------------*/
extern	const char _MSG_START, _USER_START, _HEAP_START;// Taken from 'linkObj.def' file
#define CORE_HEAP_BA					(&_HEAP_START)	// Heap base address - Do not change this address and beyond, unless memory has been allocated using malloc()
#define CORE_MSG_BA						0xFFFEF000			// Host-Core message system registers and buffer
#define	CORE_MSG_SIZE					0x400			// Allocated RAM for the Host-Core message system
#define CORE_USER_BA					(&_USER_START)	// Use this address and beyond at any way you wish
#define CORE_SPI_FLASH_BA				0x20000			// SPI Flash address
#define DATA_BUFFERS_BA					0x3000000;
//#define DATA_BUFFERS_BA					0xfffe9000;


#define SCRPAD41  0xf0800e7c
#define SCRPAD71  0xf0800ef4
#define SCRPAD72  0xf0800ef8
#define SCRPAD73  0xf0800efc


// Calculated allocated RAM for Messages
//extern	const char _MSG_END;							// Taken from 'linkObj.def' file
//#define	CORE_MSG_SIZE					(_MSG_END - _MSG_START)
//#if (CORE_MSG_SIZE < 0x280)
//#error Not enough RAM is allocated for Messaging System. Change 'linkObj.def' file
//#endif

// Verify that the address of CORE_MSG_BA is valid
//#ifdef __NSC_CRCC__
//	#if (CORE_MSG_BA != _MSG_START)
//	#error "Error! 'CORE_MSG_BA' and _MSG_START (in 'linkObj.def' file) mismatch"
//	#endif
//#endif

#define TEB2_PATH				"V:\\TEB\\TEB2_DB\\Release\\Unified_FPGA\\TEB2_FPGA_Latest.fpg"
#define CE_PATH					"V:\\uRider12A\\FPGA\\uRider12A_Latest.fpg"
#define VAN3_PATH				"NONE"

/*-------------------*/
/* Commands Messages */
/*-------------------*/

// Test list for executing tests using MSG_RunTest() function by the Host
typedef enum
{
	TEST_NULL = 0,										// Zero must not be used for any test
	RND_GET_SEED,
	TEST_DEMO,
	TEST_ALL,
	TEST_LOG,		// Example only
	TEST_TEST1,		// Example only
	TEST_TEST2,		// Example only
	TEST_TEST3		// Example only
} TEST_ENUM;

typedef enum
{
	DATA_NULL=0,
	DATA_INT_NUM_START
} DATA_ENUM;
// Commands list used by MSG_Command() function by the Core or Host
typedef enum
{
	CMD_NULL = 0,										// Zero must not be used for any command
	CMD_RESET_VBKUP,
	CMD_RESET_VSBY,
	CMD_RESET_VCC,
	CMD_RESET_VDD,
	CMD_RESET_VTT,
	CMD_RESET_EXT_PURST,
	CMD_RESET_EXT_RST,
	CMD_RESET_LPC,
	CMD_RESET_WD_COLD,
	CMD_RESET_WD_WARM,
	CMD_RESET_DBG_COLD,
	CMD_RESET_DBG_WARM,
	CMD_RESET_SW,
	CMD_RESET_PU_FULL,
	CMD_CHANGE_FMCLK
} CMD_ENUM;

// Test-Commands list used by MSG_TestCommand() function by the Core or Host
// (typically used for asking the core to perform a certain operation during test)
typedef enum
{
	TCMD_NULL = 0,										// Zero must not be used for any Test Command

	// Examples (delete them):
	TCMD_KEYPRESS = 1,
	TCMD_INTERRUPT,						// Ask core to generate interrupt
	TCMD_CLEAR_STS						// Ask core to clear status registers
} TCMD_ENUM;


/*-----------------*/
/* Template macros */
/*-----------------*/
typedef enum
{
	PRE_SILICON,
	PRE_SILICON_TEB,
	POST_SILICON,
	POST_SILICON_TEB,
} TMPL_TEST_MODE;

typedef enum
{
	EXT_FLASH,
	INT_FLASH,
	MAIN_RAM,
	CORE_RAM
} TMPL_FW_LOC;

typedef enum
{
	BOOTER,
	JTAG,
	FAST_LOAD_TO_PALLADUM,
} TMPL_DWLOAD_TOOL;

typedef enum
{
	HOST_DEBUG,
	DEBUGGER,
	DEBUG,
	RELESE
} TMPL_DEBUG_ENV;

#pragma pack(1)				// Compiler define each BYTE as 8 bit
typedef struct
{
	/*-----------------------------------------------------------------------------
	* Flash Signature
	*----------------------------------------------------------------------------*/
	/* 00 */ WORD	Signature;			// Flash Signature
	/* 02 */ BYTE	H_SEC_DES;			// Secure Firmware	
	/* 03 */ BYTE	JTAG_DIS_HD;		// Header JTAG Disable Code	

	/*-----------------------------------------------------------------------------
	* MSWC Core Registers Configuration
	*----------------------------------------------------------------------------*/
	/* 04 */ BYTE   HCBAL_DAT;			// Host Configuration Base Address Low
	/* 05 */ BYTE   HCBAH_DAT;			// Host Configuration Base Address High
	/* 06 */ WORD	reserved1;

	/*-----------------------------------------------------------------------------
	* High Frequency Clock Generator (HFCG) Module Configuration
	*----------------------------------------------------------------------------*/
	/* 08 */ BYTE   HFCGML_DAT;			// HFCG, M value, Low
	/* 09 */ BYTE   HFCGMH_DAT;			// HFCG, M value, High
	/* 0A */ BYTE   HFCGN_DAT;			// HFCG, N value
	/* 0B */ BYTE   HFCGP_DAT;			// bits 7-4 - Flash Clock Divider
										// bits 3-0 - Core Clock Divider
	/* 0C */ WORD	reserved2;

	/*-----------------------------------------------------------------------------
	* Flash Interface Unit (FIU) Module Configuration
	*----------------------------------------------------------------------------*/
	/* 0E */ BYTE   FL_SIZE_DAT;		// bits 6-0 - Flash size
	/* 0F */ BYTE   BURST_DAT;			// bits 1-0 - Flash Read Burst
	/* 10 */ BYTE   SPI_FL_DAT;			// bits 7-6 - Read Mode Select
	/* 11 */ BYTE   MISC_CTL;			// bit 7    - Bypass on bad Firmware Checksum
										// bit 6    - Disabled Shared BIOS
	/* 12 */ BYTE   CRCCON_DAT;			// bit 1    - Checksum or CRC (inverted)
	/* 13 */ BYTE	reserved3;

	/*-----------------------------------------------------------------------------
	* General Settings
	*----------------------------------------------------------------------------*/
	/* 14 */ DWORD  FIRMW_START;		// Firmware Start Address
	/* 18 */ DWORD  FIRMW_SIZE;			// Firmware Size (in words)
	/* 1C */ WORD	Reserved4;
	/* 1E */ WORD	Reserved5;
	/* 20 */ DWORD  FCKSM_START;		// Firmware CRC or Checksum start Address
	/* 24 */ DWORD  FCKSM_SIZE;			// Firmware CRC or Checksum size (in words)
	/* 28 */ WORD	Reserved6;
	/* 2A */ WORD	Reserved7;
	/* 2C */ DWORD	ALT_HEAD_OFS;		// Alternate Header Base Address Offset
	/* 30 */ WORD	Reserved8[8];

	/* 40 */ WORD	HEADER_CKSM;		// Checksum of Header (bytes 0h to 3Eh modulo 16)
	/* 42 */ WORD	Reserved9;
	/* 44 */ DWORD	FIRMW_CKSM;			// CRC or Checksum of Firmware
	/* 48 */ DWORD	ENCR_FIRMW_CKSM;	// CRC or Checksum of Encrypted Firmware
	/* 4C */ WORD	Reserved10[72];
	/* E0 */ BYTE	ECK[16];			// Encrypted Firmware Key
	/* F0 */ WORD	ECK_CKSM;			// Checksum of Encrypted Key
	/* F2 */ WORD	Reserved11[7];
} TMPL_FLASH_HEADER;

#pragma pack()

#pragma pack(1)
typedef struct
{
	/* 6C */ BYTE	OVLP_MISC4_ECP;		// bit 7	- Valid OVLP_MISC4 value (1 - valid)
										// bits 3-0 - Overlap ON Delay
	/* 6D */ BYTE	FRCDIV2_LOW_ECP;	// bits 7-0	- FRCDIV2 Bits 7-0
	/* 6E */ BYTE	FRCDIV2_MISC3_ECP;	// bit 7	- Valid FRCDIV2 value (1 - valid)
										// bit 5	- VCORE Exist Enable in Idle
										// bit 4	- Low-Current VCORF Regulator Disable in Idle
										// bit 0	- FRCDIV2 Bit 8
	/* 6F */ BYTE	RESERVED_ECP;
	/* 70 */ BYTE	RAM_1T_TCTL_ECP;	// bit 7	- Valid RAM_1T_TCTL_OTP (1 - valid)
										// bit 6	- Temp. Sensing Disable
										// bit 5	- Low Sample Rate Select
										// bit 4	- Temp. Measurement Window Select
										// bit 3	- Temp. Measurement Delay Select
										// bits 2-0	- Temperature Free-Running Trimming
	/* 71 */ BYTE	W_H_THR_ECP;		// bits 7-0	- Warm-Hot Temperature Threshold
	/* 72 */ BYTE	C_W_THR_ECP;		// bits 7-0	- Cold-Warm Temperature Threshold
	/* 73 */ BYTE	RAM_1T_TMP2_ECP;	// bit 7	- Valid RAM_1T_TMP2_OTP item (1 - valid)
										// bits 5-3	- Hot Region Divider
										// bits 2-0	- Warm Region Divider
	/* 74 */ BYTE	RAM_1T_TMP1_ECP;	// bit 7	- Valid RAM_1T_TMP1_OTP	item (1 - valid)
										// bit 6	- Valid C_W_THR_OTP item (1 - valid)
										// bit 5	- Valid W_H_THR_OTP item (1 - valid)
										// bits 2-0	- Cold Region Divider
	/* 75 */ BYTE	SRID_CTL_ECP;		// bits 7-0	- Chip ID Control
	/* 76 */ BYTE	DEVICE_ID_ECP;		// bits 4-0	- Device ID Control
	/* 77 */ BYTE	MISC2_ECP;			// bit 7	- Valid SRID_CTL_OTP item (1 - valid)
										// bit 6	- Valid DEVICE_ID_OTP item (1 - valid)
										// bits 2-1	- Memory Mapping Control
	/* 78 */ BYTE	JT_CTL_ECP;			// bits 7-0	- JTAG Disable Code
	/* 79 */ BYTE	PROT_ECP;			// bits 7-6	- Eng. OTP Read Protect
										// bits 5-4	- Eng. OTP Write Protect
										// bit 3	- Secure Mode Select
										// bit 0	- Gang Programmer Read/Write Protect
	/* 7A */ BYTE	ARST_DLY_ECP;		// bits 5-0	- Additional Reset Delay
	/* 7B */ BYTE	SFT_STRP_ECP;		// bit 4	- BBRM Supported
										// bit 0	- PECI 3.0 Supported
	/* 7C */ BYTE	RAM_1T_MODE_ECP;	// bit 7	- Valid RAM_1T_MODE_OTP item (1 - valid)
										// bits 6-0	- RAM Mode
	/* 7D */ BYTE	RAM_1T_CTL_ECP;		// bits 7-6	- Bad Block
										// bits 5-1	- Free-Running Clock Divider
										// bit 0	- Staggered Refresh
	/* 7E */ BYTE	FRCDIV_LOW_ECP;		// bits 7-0	- FRCDIV Bits 7-0
	/* 7F */ BYTE	FRCDIV_MISC_ECP;	// bit 7	- Old Refresh Mode Select
										// bit 6	- Valid PROT_OTP item (1 - valid)
										// bit 5	- Valid SFT_STRP_OTP item (1 - valid)
										// bit 4	- Valid RAM_1T_CTL_OTP item (1 - valid)
										// bit 3	- Copy Firmware from External Flash
										// bits 2-1	- Booter Flow Modifier
										// bit 0	- FRCDIV Bit 8
} TMPL_OTP_ECP;

#pragma pack()

/*-------------*/
/* Test macros */
/*-------------*/


/*------------------------------------------------------------------------*/
/*---------------------   Data Types definitions   -----------------------*/
/*------------------------------------------------------------------------*/
#define MHz_50			0x32000000
#define MHz_40			0x28000000
#define MHz_20			0x14000000
#define MHz_25			0x19000000
#define KHz_32			0x00008300
#define KHz_930			0x0003A800





//run from DRAM
// #ifdef _DEBUG
// #define POLEG_MEM_START	0x8000
// #define FW_CODE_START	0x9500
// #endif

//run from DRAM
#ifdef _DEBUG_DDR
#define POLEG_MEM_START	0x8000
#define FW_CODE_START	0x9500
#endif

//run from SRAM
#ifdef _DEBUG_SRAM
#define POLEG_MEM_START	0xFFFB0000
#define FW_CODE_START	0xFFFB1500
#endif

#define HEADER_SIZE		0x100
/*------------------------------------------------------------------------*/
/*---------------------------   Externals   ------------------------------*/
/*------------------------------------------------------------------------*/

typedef struct
{
	TMPL_TEST_MODE		Test_Mode;
	TMPL_DEBUG_ENV		Debug_Env;
	BYTE				Package_Type;
	TMPL_FW_LOC			Fw_location;
	TMPL_DWLOAD_TOOL	Fw_Download_Tool;
	TMPL_FLASH_HEADER	Flash_Header;
	const char*			Fw_File_Path;
	TMPL_OTP_ECP		Ecp_Configuration;


} Template_Parameters;

#if 0
#ifdef _MAIN
Template_Parameters Template_Params = {PRE_SILICON,DEBUG, 0x80, MAIN_RAM, JTAG, 0, 0,0};
#else
extern Template_Parameters Template_Params;
#endif
#endif
/*------------------------------------------------------------------------*/
/*-----------------------   Function headers   ---------------------------*/
/*------------------------------------------------------------------------*/

#endif // _COMMON_H_
