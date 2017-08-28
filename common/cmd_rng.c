/*
	* (C) Copyright 2016
	*  Nuvoton Technology Corporation
	*
	* SPDX-License-Identifier:	GPL-2.0+
	*/

/*
	* RNG Functions
	*
	*/

#include <stdlib.h>


#include <common.h>
#include <command.h>





int do_rng(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int ac, count, rand_num;

	if (argc > 2)
		return CMD_RET_USAGE;

	ac = 1;
	if (argc == 2)
	{
	    ac = (unsigned int)simple_strtoul(argv[1], NULL, 10);
	}

	for (count = 0; count < ac; count++)
	{
		rand_num = rand();
		printf("0x%x\n", rand_num);
	}

	return 0;

}


U_BOOT_CMD(
	rng,	2,	1,	do_rng,
	"generate random number",
	"[count]\n"
		"    - generate [count] random numbers. count default is 1."
);


