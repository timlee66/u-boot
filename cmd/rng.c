/*
 * Command for Random Number Generator
 *
 * Copyright (C) 2019, NUVOTON, Incorporated
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <linux/errno.h>

static int do_rng(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int ac, count, rand_num;

	if (argc > 2)
		return CMD_RET_USAGE;

	ac = 1;
	if (argc == 2) {
		ac = (unsigned int)simple_strtoul(argv[1], NULL, 10);
	}

	for (count = 0; count < ac; count++) {
		rand_num = rand();
		printf("0x%x\n", rand_num);
	}

	return 0;
}

U_BOOT_CMD(
	rng,	2,	1,	do_rng,
	"Generate andom number",
	     "rng [count] - generate [count] random numbers. count default is 1\n"
);
