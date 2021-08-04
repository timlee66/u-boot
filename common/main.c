// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 */

/* #define	DEBUG	*/

#include <common.h>
#include <autoboot.h>
#include <cli.h>
#include <console.h>
#include <version.h>
#include <asm/io.h>
#ifdef CONFIG_NPCM850_DBG_INTERRUPTS
extern void timer_test(void);
extern void init_gicd(void);
extern void init_gicc(void);
extern void mc_intr(void);
#endif


#ifdef CONFIG_DISABLE_CONSOLE
DECLARE_GLOBAL_DATA_PTR;
#endif

/*
 * Board-specific Platform code can reimplement show_boot_progress () if needed
 */
__weak void show_boot_progress(int val) {}

static void run_preboot_environment_command(void)
{
#ifdef CONFIG_PREBOOT
	char *p;

	p = env_get("preboot");
	if (p != NULL) {
		int prev = 0;

		if (IS_ENABLED(CONFIG_AUTOBOOT_KEYED))
			prev = disable_ctrlc(1); /* disable Ctrl-C checking */

		run_command_list(p, -1, 0);

		if (IS_ENABLED(CONFIG_AUTOBOOT_KEYED))
			disable_ctrlc(prev);	/* restore Ctrl-C checking */
	}
#endif /* CONFIG_PREBOOT */
}

/* We come here after U-Boot is initialised and ready to process commands */
void main_loop(void)
{
	const char *s;

#if 0
	while(1)
	{
	     printf("U");   /* This is for finding uart freq. */
	}
#endif

#ifdef CONFIG_NPCM850_DBG_INTERRUPTS
	init_gicd();
	init_gicc();

	mc_intr();      /* Memory Controller interrupt */

#ifdef CONFIG_NPCM850_DBG_TIMER
	timer_test();
#endif
#endif

#if defined (CONFIG_NPCMX50_CORE1) || defined (CONFIG_NPCMX50_CORE2) || defined (CONFIG_NPCMX50_CORE3)
#ifdef CONFIG_DISABLE_CONSOLE
	gd->flags |= GD_FLG_DISABLE_CONSOLE;
#endif

#if defined (CONFIG_TARGET_ARBEL)
	writel(readl((volatile uint32_t *)(0xC0008000C)) + 0x1, (volatile uint32_t *)(0xC0008000C));            /* Flag for mainloop (shell) per secondary cores */
#elif defined (CONFIG_TARGET_POLEG)
	writel(readl((volatile uint32_t *)(0xfffd000C)) + 0x1, (volatile uint32_t *)(0xfffd000C));            /* Flag for mainloop (shell) per secondary cores */
#endif
#endif
	bootstage_mark_name(BOOTSTAGE_ID_MAIN_LOOP, "main_loop");

	if (IS_ENABLED(CONFIG_VERSION_VARIABLE))
		env_set("ver", version_string);  /* set version variable */

	cli_init();

	run_preboot_environment_command();

	if (IS_ENABLED(CONFIG_UPDATE_TFTP))
		update_tftp(0UL, NULL, NULL);

	s = bootdelay_process();
	if (cli_process_fdt(&s))
		cli_secure_boot_cmd(s);

	autoboot_command(s);

	cli_loop();
	panic("No CLI available");
}
