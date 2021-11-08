/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#ifndef _NPCM_PSPI_H_
#define _NPCM_PSPI_H_


/* PSPI_CTL1 fields */
#define PSPI_CTL1_SCDV6_0	9
#define PSPI_CTL1_SCIDL		8
#define PSPI_CTL1_SCM		7
#define PSPI_CTL1_EIW		6
#define PSPI_CTL1_EIR		5
#define PSPI_CTL1_SPIEN		0

/* PSPI_STAT fields */
#define PSPI_STAT_RBF		1
#define PSPI_STAT_BSY		0

enum pspi_dev {
	PSPI1_DEV = 0,
	PSPI2_DEV = 1
};

struct npcm_pspi_regs {
	unsigned short pspi_data;
	unsigned short pspi_ctl1;
	unsigned char pspi_stat;
};

#endif
