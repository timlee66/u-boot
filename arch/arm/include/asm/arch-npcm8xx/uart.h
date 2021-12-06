/* SPDX-License-Identifier: GPL-2.0+ */

#ifndef _NPCM_UART_H_
#define _NPCM_UART_H_

struct npcm_uart {
	union {
		unsigned int	rbr;
		unsigned int	thr;
		unsigned int	dll;
	};
	union {
		unsigned int	ier;
		unsigned int	dlm;
	};
	union {
		unsigned int	iir;
		unsigned int	fcr;
	};
	unsigned int	lcr;
	unsigned int	mcr;
	unsigned int	lsr;
	unsigned int	msr;
	unsigned int	tor;
};

#define	IER_DBGACK	BIT(4)
#define	IER_MSIE	BIT(3)
#define	IER_RLSE	BIT(2)
#define	IER_THREIE	BIT(1)
#define	IER_RDAIE	BIT(0)

#define	IIR_FMES	BIT(7)
#define	IIR_RFTLS	BIT(5)
#define	IIR_DMS		BIT(4)
#define	IIR_IID		BIT(1)
#define	IIR_NIP		BIT(0)

#define	FCR_RFITL_1B	(0 << 4)
#define	FCR_RFITL_4B	(4 << 4)
#define	FCR_RFITL_8B	(8 << 4)
#define	FCR_RFITL_14B	(12 << 4)
#define	FCR_DMS		BIT(3)
#define	FCR_TFR		BIT(2)
#define	FCR_RFR		BIT(1)
#define	FCR_FME		BIT(0)

#define	LCR_DLAB	BIT(7)
#define	LCR_BCB		BIT(6)
#define	LCR_SPE		BIT(5)
#define	LCR_EPS		BIT(4)
#define	LCR_PBE		BIT(3)
#define	LCR_NSB		BIT(2)
#define	LCR_WLS_8b	3
#define	LCR_WLS_7b	2
#define	LCR_WLS_6b	1
#define	LCR_WLS_5b	0

#define	MCR_LBME	BIT(4)
#define	MCR_OUT2	BIT(3)
#define	MCR_RTS		BIT(1)
#define	MCR_DTR		BIT(0)

#define	LSR_ERR_RX	BIT(7)
#define	LSR_TE		BIT(6)
#define	LSR_THRE	BIT(5)
#define	LSR_BII		BIT(4)
#define	LSR_FEI		BIT(3)
#define	LSR_PEI		BIT(2)
#define	LSR_OEI		BIT(1)
#define	LSR_RFDR	BIT(0)

#define	MSR_DCD		BIT(7)
#define	MSR_RI		BIT(6)
#define	MSR_DSR		BIT(5)
#define	MSR_CTS		BIT(4)
#define	MSR_DDCD	BIT(3)
#define	MSR_DRI		BIT(2)
#define	MSR_DDSR	BIT(1)
#define	MSR_DCTS	BIT(0)

#endif
