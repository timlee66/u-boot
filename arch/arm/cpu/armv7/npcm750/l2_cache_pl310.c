#include <common.h>

extern  void l2_pl310_init(void);
void set_pl310_ctrl(u32 enable);


#ifndef CONFIG_SYS_L2CACHE_OFF
void v7_outer_cache_enable(void)
{
	printf("l2_pl310_init\n");
	l2_pl310_init();
	set_pl310_ctrl(1);
}

void v7_outer_cache_disable(void)
{
	set_pl310_ctrl(0);
}
#endif /* !CONFIG_SYS_L2CACHE_OFF */
