/*--------------------------------------------------------------------------
 * Copyright (c) 2009-2010 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------
 * File Contents:
 *           Holds the utility headers
 *--------------------------------------------------------------------------*/
#ifndef _CORE_UTILITY_H_
#define _CORE_UTILITY_H_


/*------------------------------------------------------------------------*/
/*----------------   Constants and macros definitions  -------------------*/
/*------------------------------------------------------------------------*/

typedef enum
{
	FMCLK_50_MHz,
	FMCLK_40_MHz,
	FMCLK_33_MHz,
	FMCLK_10_MHz
} FMCLK_FREQ;

typedef enum
{
	FMCLK_50_FCLK_50_CLK_50_MHz,
	FMCLK_50_FCLK_50_CLK_25_MHz,
	FMCLK_50_FCLK_50_CLK_16_67_MHz,
	FMCLK_50_FCLK_50_CLK_12_5_MHz,
	FMCLK_50_FCLK_50_CLK_6_25_MHz,
	FMCLK_50_FCLK_50_CLK_4_17_MHz,
	FMCLK_40_FCLK_40_CLK_20_MHz,
	FMCLK_40_FCLK_40_CLK_13_33_MHz,
	FMCLK_40_FCLK_40_CLK_6_67_MHz,
	FMCLK_40_FCLK_40_CLK_5_MHz,
	FMCLK_40_FCLK_40_CLK_4_MHz,
	FMCLK_33_FCLK_33_CLK_16_5_MHz,
	FMCLK_33_FCLK_33_CLK_8_25_MHz,
	FMCLK_33_FCLK_33_CLK_5_5_MHz,
	FMCLK_33_FCLK_33_CLK_4_125_MHz,
	FMCLK_10_FCLK_10_CLK_10_MHz

} EC_CLOCKS_FREQ;

typedef enum
{
	WAIT,
	IDLE,
	DEEP_IDLE,
	DEEP_IDLE_INSTANT_WU
} EC_PWR_STATE;


#define WITH_WAIT		1
#define WITHOUT_WAIT	0
/*------------------------------------------------------------------------*/
/*---------------------   Data Types definitions   -----------------------*/
/*------------------------------------------------------------------------*/
// 12b		10b		10b -> DWORD
//  MHz		 KHz	 Hz
struct CLK_Core_EC_Clocks
{
	DWORD FMCLK; 	  
	DWORD FCLK;
	DWORD CLK;			// CLK=4.125 MHz	-> CLK[31:20]=4; CLK[19:10]=125; CLK[9:0]=0
	DWORD LFCLK;			// LFCLK=32.768 KHz -> LFCLK[31:20]=0; LFCLK[19:10]=32; LFCLK[9:0]=768
	DWORD FRCLK;			// FRCLK=930 KHz	-> FRCLK[31:20]=0; FRCLK[19:10]=930; FRCLK[9:0]=0
};

//extern CLK_Core_EC_Clocks CLK_C_EC_Clocks;

typedef enum
{
	TWD_MODE_ONE_SHOT,
	TWD_MODE_PERIODIC,
	TWD_MODE_LAST,

}ENUM_TWD_MODE;

#define CLKSEL_TIMCKSEL (0x3 << 14)
typedef enum{
	TWD_SOURCE_PLL0 = 0,
	TWD_SOURCE_PLL1,
	TWD_SOURCE_CLKREF,
	TWD_SOURCE_PLL2_DIV2,
	TWD_LAST
}ENUM_TWD_SOURCE;
// TCR

#define TCSR_FREEZE_EN		(1<<31)
#define TCSR_CEN				(1<<30)
#define TCSR_IEN				(1<<29)
#define TCSR_MODE			(3<<27)
#define TCSR_MODE_ONE_SHOT	(0<<27)
#define TCSR_MODE_PERIODIC	(1<<27)
#define TCSR_CRST			(1<<26)


/*------------------------------------------------------------------------*/
/*---------------------------   Externals   ------------------------------*/
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/*-----------------------   Function headers   ---------------------------*/
/*------------------------------------------------------------------------*/

/*-------------------------  Clocks Functions   -------------------------*/
void	CLK_Set_FMCLK						(FMCLK_FREQ FMCLK_Freq);
void	CLK_Set_FCLK						(BYTE FCLK_Divisor);
void	CLK_Set_CLK							(BYTE CLK_Divisor);
void	CLK_Set_EC_Clocks					(EC_CLOCKS_FREQ EC_Clocks_Freq);
void	CLK_Calibrate_LFCG					(void);
/*-------------------------  Chip ID Functions   -------------------------*/
void	ID_PrintChipID						(void);
/*-------------------------  ICU Functions   -----------------------------*/
void	ICU_Enable_Interrupt				(BYTE Interrupt_number);
void	ICU_Disable_Interrupt				(BYTE Interrupt_number);
void	ICU_Clear_Interrupt					(BYTE Interrupt_number);
/*-------------------------  PMC Functions   -----------------------------*/
void	PMC_Enter_Power_State				(EC_PWR_STATE Power_State, BOOL Wait_Enable);
/*-------------------------  DFT Functions   -----------------------------*/
void	WriteSIB							(int LdnMoudule, BYTE Offset, BYTE Data);
BYTE	ReadSIB								(int LdnMoudule, BYTE Offset);
void	ConfigWriteSIB						(BYTE Index, BYTE Data);
BYTE	ConfigReadSIB						(BYTE Index);
/*-------------------------  General Purpose Functions   -------------------------*/

BOOL Timer_Init(int TimerModule, int TimerPort, ENUM_TWD_MODE twd_mode,int TickPerSecond);

BOOL Timer_Start(int TimerModule, int TimerPort);
BOOL Timer_Init_No_Start(int TimerModule, int TimerPort, ENUM_TWD_MODE twd_mode,int TickPerSecond,ENUM_TWD_SOURCE clk_source);


BOOL	TWD_Timers_Cfg(void);
void configure_timer(DWORD save_twd_index, DWORD save_twd_module);

void  CLK_GetTimeStamp(DWORD time_quad[2]);
DWORD CLK_Delay_Since(DWORD microSecDelay, DWORD t0_time[2]);

#endif // _CORE_UTILITY_H_
