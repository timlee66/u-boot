#include <common.h>
#include <asm/io.h>
#include <asm/pl310.h>

extern  void l2_pl310_init(void);

static const struct pl310_regs *const pl310 =
	(struct pl310_regs *)CONFIG_SYS_PL310_BASE;

#ifndef CONFIG_SYS_L2CACHE_OFF
void v7_outer_cache_enable(void)
{
	printf("l2_pl310_init\n");
	l2_pl310_init();
	/* Enable the L2 cache */
	setbits_le32(&pl310->pl310_ctrl, L2X0_CTRL_EN);
}

void v7_outer_cache_disable(void)
{
	/* Disable the L2 cache */
	clrbits_le32(&pl310->pl310_ctrl, L2X0_CTRL_EN);
}
#endif /* !CONFIG_SYS_L2CACHE_OFF */
