#include <stdio.h>
#include <stdlib.h>
#include <asm/io.h>
#include <asm/system.h>
#include "aarch64.h"
#include "gic_v2.h"
#include "mc_intr.h"


void mc_intr(void)
{
	// GIC Init with MC IRQ on CPU-0
	gic_v2_initialize(MC_IRQ , 0x1);

	// Enable IRQ 
	enable_irq();
}
