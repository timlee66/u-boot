// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2021 Nuvoton Technology Corp.
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/cpu.h>
#include <asm/arch/gcr.h>
#include <asm/armv8/mmu.h>
#include <asm/system.h>
#include <asm/global_data.h>
#include <cpu_func.h>

/* System Counter */
struct sctr_regs {
	u32 cntcr;
	u32 cntsr;
	u32 cntcv1;
	u32 cntcv2;
	u32 resv1[4];
	u32 cntfid0;
	u32 cntfid1;
	u32 cntfid2;
	u32 resv2[1001];
	u32 counterid[1];
};

#define SC_CNTCR_ENABLE		BIT(0)
#define SC_CNTCR_HDBG		BIT(1)
#define SC_CNTCR_FREQ0		BIT(8)
#define SC_CNTCR_FREQ1		BIT(9)

#define SYSCNT_CTRL_BASE_ADDR   0xF07FC000

DECLARE_GLOBAL_DATA_PTR;

int print_cpuinfo(void)
{
	struct npcm_gcr *gcr = (struct npcm_gcr *)npcm_get_base_gcr();
	unsigned int id = 0;
	unsigned long mpidr_val = 0;
	unsigned int mdlr = 0;

	asm volatile("mrs %0, mpidr_el1" : "=r" (mpidr_val));

	mdlr = readl(&gcr->mdlr);

	printf("CPU-%d: ", (unsigned int)(mpidr_val & 0x3));

	switch (mdlr) {
	case ARBEL_NPCM845:
		printf("NPCM845 ");
		break;
	case ARBEL_NPCM830:
		printf("NPCM830 ");
		break;
	case ARBEL_NPCM810:
		printf("NPCM810 ");
		break;
	default:
		printf("NPCM8XX ");
		break;
	}

	id = readl(&gcr->pdid);
	switch (id) {
	case ARBEL_Z1:
		printf("Z1 @ ");
		break;
	case ARBEL_A1:
		printf("A1 @ ");
		break;
	default:
		printf("Unknown\n");
		break;
	}

	return 0;
}

int arch_cpu_init(void)
{
	if (!IS_ENABLED(CONFIG_SYS_DCACHE_OFF)) {
		/* enable cache to speed up system running */
		if (get_sctlr() & CR_M)
			return 0;

		icache_enable();
		__asm_invalidate_dcache_all();
		__asm_invalidate_tlb_all();
		set_sctlr(get_sctlr() | CR_C);
	}
	return 0;
}

static struct mm_region npcm_mem_map[1 + CONFIG_NR_DRAM_BANKS + 1] = {
	{
		/* DRAM */
		.phys = 0x0UL,
		.virt = 0x0UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	},
	{
		.phys = 0x80000000UL,
		.virt = 0x80000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	},
	{
		/* List terminator */
		0,
	}
};

struct mm_region *mem_map = npcm_mem_map;

int timer_init(void)
{
	struct sctr_regs *sctr = (struct sctr_regs *)SYSCNT_CTRL_BASE_ADDR;
	unsigned int cntfrq_el0;

	__asm__ __volatile__("mrs %0, CNTFRQ_EL0\n\t" : "=r" (cntfrq_el0) : : "memory");
	writel(cntfrq_el0, &sctr->cntfid0);

	clrsetbits_le32(&sctr->cntcr, SC_CNTCR_FREQ0 | SC_CNTCR_FREQ1,
			SC_CNTCR_ENABLE | SC_CNTCR_HDBG);

	gd->arch.tbl = 0;
	gd->arch.tbu = 0;

	return 0;
}
