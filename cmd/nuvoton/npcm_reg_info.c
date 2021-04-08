// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2001
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/*
 * Misc functions
 */
#include <common.h>
#include <asm/io.h>


#define GCR_BA  0xF0800000
/* GCR REG's */
#define PDID	    0x000
#define PWRON	    0x004
#define SWSTRPS	    0x008
#define MISCPE	    0x014
#define SPLDCNT	    0x018
#define FLOCKR2	    0x020
#define FLOCKR3	    0x024
#define A35_MODE	0x034
#define SPSWC	    0x038
#define INTCR	    0x03C
#define INTSR	    0x040
#define OBSCR1	    0x044
#define OBSDR1	    0x048
#define IFCR	    0x050
#define INTCR2	    0x060
#define SRCNT	    0x068
#define RESSR	    0x06C
#define RLOCKR1	    0x070
#define FLOCKR1	    0x074
#define DSCNT	    0x078
#define MDLR	    0x07C
#define SCRPAD_C	0x080
#define SCRPAD_B	0x084
#define DACLVLR	    0x098
#define INTCR3	    0x09C
#define PCIRCTL	    0x0A0
#define VSINTR	    0x0AC
#define SD2SUR1	    0x0B4
#define SD2SUR2	    0x0B8
#define SD2IRV3	    0x0BC
#define INTCR4	    0x0C0
#define OBSCR2	    0x0C4
#define OBSDR2	    0x0C8
#define I2CSEGSEL	0x0E0
#define I2CSEGCTL	0x0E4
#define VSRCR	    0x0E8
#define MLOCKR	    0x0EC
#define ETSR	    0x110
#define DFT1R	    0x114
#define DFT2R	    0x118
#define DFT3R	    0x11C
#define EDFFSR	    0x120
#define INTCRPCE3	0x128
#define INTCRPCE2	0x12C
#define INTCRPCE0	0x130
#define INTCRPCE1	0x134
#define DACTEST	    0x138
#define SCRPAD	    0x13C
#define USB1PYCTL	0x140
#define USB2PYCTL	0x144
#define USB3PYCTL	0x148
#define INTSR2	    0x14C
#define INTCRPCE2B	0x150
#define INTCRPCE0B	0x154
#define INTCRPCE1B	0x158
#define INTCRPCE3B	0x15C
#define INTCRPCE2C	0x170
#define INTCRPCE0C	0x174
#define INTCRPCE1C	0x178
#define INTCRPCE3C	0x17C
#define SD2IRV4	    0x220
#define SD2IRV5	    0x224
#define SD2IRV6	    0x228
#define SD2IRV7	    0x22C
#define SD2IRV8	    0x230
#define SD2IRV9     0x234
#define SD2IRV10	0x238
#define SD2IRV11	0x23C
#define MFSEL1	    0x260
#define MFSEL2	    0x264
#define MFSEL3	    0x268
#define MFSEL4	    0x26C
#define MFSEL5	    0x270
#define MFSEL6	    0x274
#define MFSEL7	    0x278
#define MFSEL_LK1	0x280
#define MFSEL_LK2	0x284
#define MFSEL_LK3	0x288
#define MFSEL_LK4	0x28C
#define MFSEL_LK5	0x290
#define MFSEL_LK6	0x294
#define MFSEL_LK7	0x298
#define MFSEL_SET1	0x2A0
#define MFSEL_SET2	0x2A4
#define MFSEL_SET3	0x2A8
#define MFSEL_SET4	0x2AC
#define MFSEL_SET5	0x2B0
#define MFSEL_SET6	0x2B4
#define MFSEL_SET7	0x2B8
#define MFSEL_CLR1	0x2C0
#define MFSEL_CLR2	0x2C4
#define MFSEL_CLR3	0x2C8
#define MFSEL_CLR4	0x2CC
#define MFSEL_CLR5	0x2D0
#define MFSEL_CLR6	0x2D4
#define MFSEL_CLR7	0x2D8

#define CLK_BA  0xF0801000
/* CLK REG's */
#define CLKEN1     0x00
#define CLKEN2     0x28
#define CLKEN3     0x30
#define CLKEN4     0x70
#define CLKSEL     0x04
#define CLKDIV1    0x08
#define CLKDIV2    0x2C
#define CLKDIV3    0x58
#define CLKDIV4    0x7C
#define PLLCON0    0x0C
#define PLLCON1    0x10
#define PLLCON2    0x54
#define PLLCONG    0x60
#define SWRSTR     0x14
#define IPSRST1    0x20
#define IPSRST2    0x24
#define IPSRST3    0x34
#define IPSRST4    0x74
#define WD0RCR     0x38
#define WD1RCR     0x3C
#define WD2RCR     0x40
#define SWRSTC1    0x44
#define SWRSTC2    0x48
#define SWRSTC3    0x4C
#define TIPRSTC    0x50
#define CORSTC     0x5C
#define WD0RCRB    0x80
#define WD1RCRB    0x84
#define WD2RCRB    0x88
#define SWRSTC1B   0x8C
#define SWRSTC2B   0x90
#define SWRSTC3B   0x94
#define TIPRSTCB   0x98
#define CORSTCB    0x9C
#define AHBCKFI    0x64
#define SECCNT     0x68
#define CNTR25M    0x6C
#define BUSTO      0x78
#define IPSRSTDIS1 0xA0
#define IPSRSTDIS2 0xA4
#define IPSRSTDIS3 0xA8
#define IPSRSTDIS4 0xAC
#define CLKENDIS1  0xB0
#define CLKENDIS2  0xB4
#define CLKENDIS3  0xB8
#define CLKENDIS4  0xBC
#define THRTL_CNT  0xC0



struct reg_info{
    char reg_name[20];
    u32 reg_offset;
};


struct reg_info gcr_arry[]={
{"PDID", PDID },	    
{"PWRON", PWRON },		    
{"SWSTRPS", SWSTRPS },
{"MISCPE", MISCPE },
{"SPLDCNT", SPLDCNT },
{"FLOCKR2", FLOCKR2 },
{"FLOCKR3", FLOCKR3 },
{"A35_MODE", A35_MODE },
{"SPSWC", SPSWC },
{"INTCR", INTCR },
{"INTSR", INTSR },
{"OBSCR1", OBSCR1 },
{"OBSDR1", OBSDR1 },
{"IFCR", IFCR },
{"INTCR2", INTCR2 },
{"SRCNT", SRCNT },
{"RESSR", RESSR },
{"RLOCKR1", RLOCKR1 },
{"FLOCKR1", FLOCKR1 },
{"DSCNT", DSCNT },
{"MDLR", MDLR },
{"SCRPAD_C", SCRPAD_C },
{"SCRPAD_B", SCRPAD_B },
{"DACLVLR", DACLVLR },
{"INTCR3", INTCR3 },
{"PCIRCTL", PCIRCTL },
{"VSINTR", VSINTR },
{"SD2SUR1", SD2SUR1 },
{"SD2SUR2", SD2SUR2 },
{"SD2IRV3", SD2IRV3 },
{"INTCR4", INTCR4 },
{"OBSCR2", OBSCR2 },
{"OBSDR2", OBSDR2 },
{"I2CSEGSEL", I2CSEGSEL },
{"I2CSEGCTL", I2CSEGCTL },
{"VSRCR", VSRCR },
{"MLOCKR", MLOCKR },
{"ETSR", ETSR },
{"DFT1R", DFT1R },
{"DFT2R", DFT2R },
{"DFT3R", DFT3R },
{"EDFFSR", EDFFSR },
{"INTCRPCE3", INTCRPCE3 },
{"INTCRPCE2", INTCRPCE2 },
{"INTCRPCE0", INTCRPCE0 },
{"INTCRPCE1", INTCRPCE1 },
{"DACTEST", DACTEST },
{"SCRPAD", SCRPAD },
{"USB1PYCTL", USB1PYCTL },
{"USB2PYCTL", USB2PYCTL },
{"USB3PYCTL", USB3PYCTL },
{"INTSR2", INTSR2 },
{"INTCRPCE2B", INTCRPCE2B },
{"INTCRPCE0B", INTCRPCE0B },
{"INTCRPCE1B", INTCRPCE1B },
{"INTCRPCE3B", INTCRPCE3B },
{"INTCRPCE2C", INTCRPCE2C },
{"INTCRPCE0C", INTCRPCE0C },
{"INTCRPCE1C", INTCRPCE1C },
{"INTCRPCE3C", INTCRPCE3C },
{"SD2IRV4", SD2IRV4 },
{"SD2IRV5", SD2IRV5 },
{"SD2IRV6", SD2IRV6 },
{"SD2IRV7", SD2IRV7 },
{"SD2IRV8", SD2IRV8 },
{"SD2IRV9", SD2IRV9 },
{"SD2IRV10", SD2IRV10 },
{"SD2IRV11", SD2IRV11 },
{"MFSEL1", MFSEL1 },
{"MFSEL2", MFSEL2 },
{"MFSEL3", MFSEL3 },
{"MFSEL4", MFSEL4 },
{"MFSEL5", MFSEL5 },
{"MFSEL6", MFSEL6 },
{"MFSEL7", MFSEL7 },
{"MFSEL_LK1", MFSEL_LK1 },
{"MFSEL_LK2", MFSEL_LK2 },
{"MFSEL_LK3", MFSEL_LK3 },
{"MFSEL_LK4", MFSEL_LK4 },
{"MFSEL_LK5", MFSEL_LK5 },
{"MFSEL_LK6", MFSEL_LK6 },
{"MFSEL_LK7", MFSEL_LK7 },
{"MFSEL_SET1", MFSEL_SET1 },
{"MFSEL_SET2", MFSEL_SET2 },
{"MFSEL_SET3", MFSEL_SET3 },
{"MFSEL_SET4", MFSEL_SET4 },
{"MFSEL_SET5", MFSEL_SET5 },
{"MFSEL_SET6", MFSEL_SET6 },
{"MFSEL_SET7", MFSEL_SET7 },
{"MFSEL_CLR1", MFSEL_CLR1 },
{"MFSEL_CLR2", MFSEL_CLR2 },
{"MFSEL_CLR3", MFSEL_CLR3 },
{"MFSEL_CLR4", MFSEL_CLR4 },
{"MFSEL_CLR5", MFSEL_CLR5 },
{"MFSEL_CLR6", MFSEL_CLR6 },
{"MFSEL_CLR7", MFSEL_CLR7 }
};

struct reg_info clk_arry[]={
{"CLKEN1", CLKEN1 },
{"CLKEN2", CLKEN2 },
{"CLKEN3", CLKEN3 },
{"CLKEN4", CLKEN4 },
{"CLKSEL", CLKSEL },
{"CLKDIV1", CLKDIV1 },
{"CLKDIV2", CLKDIV2 },
{"CLKDIV3", CLKDIV3 },
{"CLKDIV4", CLKDIV4 },
{"PLLCON0", PLLCON0 },
{"PLLCON1", PLLCON1 },
{"PLLCON2", PLLCON2 },
{"PLLCONG", PLLCONG },
{"SWRSTR", SWRSTR },
{"IPSRST1", IPSRST1 },
{"IPSRST2", IPSRST2 },
{"IPSRST3", IPSRST3 },
{"IPSRST4", IPSRST4 },
{"WD0RCR", WD0RCR },
{"WD1RCR", WD1RCR },
{"WD2RCR", WD2RCR },
{"SWRSTC1", SWRSTC1 },
{"SWRSTC2", SWRSTC2 },
{"SWRSTC3", SWRSTC3 },
{"TIPRSTC", TIPRSTC },
{"CORSTC", CORSTC },
{"WD0RCRB", WD0RCRB },
{"WD1RCRB", WD1RCRB },
{"WD2RCRB", WD2RCRB },
{"SWRSTC1B", SWRSTC1B },
{"SWRSTC2B", SWRSTC2B },
{"SWRSTC3B", SWRSTC3B },
{"TIPRSTCB", TIPRSTCB },
{"CORSTCB", CORSTCB },
{"AHBCKFI", AHBCKFI },
{"SECCNT", SECCNT },
{"CNTR25M", CNTR25M },
{"BUSTO", BUSTO },
{"IPSRSTDIS1", IPSRSTDIS1 },
{"IPSRSTDIS2", IPSRSTDIS2 },
{"IPSRSTDIS3", IPSRSTDIS3 },
{"IPSRSTDIS4", IPSRSTDIS4 },
{"CLKENDIS1", CLKENDIS1 },
{"CLKENDIS2", CLKENDIS2 },
{"CLKENDIS3", CLKENDIS3 },
{"CLKENDIS4", CLKENDIS4 },
{"THRTL_CNT", THRTL_CNT } 
};


void npcm_reginfo(void)
{
    printf("\n GCR 0x%x: OFFSET   VALUE     REG-NAME\n", GCR_BA);
    for(int i=0; i < sizeof(gcr_arry)/sizeof(gcr_arry[0]); i++)
        printf("\t\t 0x%03x  0x%08x  %s\n", gcr_arry[i].reg_offset, readl((uintptr_t)(GCR_BA + gcr_arry[i].reg_offset)), gcr_arry[i].reg_name);

    printf("\n CLK 0x%x: OFFSET   VALUE     REG-NAME\n", CLK_BA);
    for(int i=0; i < sizeof(clk_arry)/sizeof(clk_arry[0]); i++)
        printf("\t\t 0x%03x  0x%08x  %s\n", clk_arry[i].reg_offset, readl((uintptr_t)(CLK_BA + clk_arry[i].reg_offset)), clk_arry[i].reg_name);
}

static int do_reginfo(cmd_tbl_t *cmdtp, int flag, int argc,
		       char * const argv[])
{
	npcm_reginfo();

	return 0;
}

 /**************************************************/

U_BOOT_CMD(
	reginfo,	2,	1,	do_reginfo,
	"print NPCM register information",
	""
);
