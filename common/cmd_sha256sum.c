/*
	* (C) Copyright 2011
	* Joe Hershberger, National Instruments, joe.hershberger@ni.com
	*
	* (C) Copyright 2000
	* Wolfgang Denk, DENX Software Engineering, wd@denx.de.
	*
	* SPDX-License-Identifier:	GPL-2.0+
	*/

#include <common.h>
#include <command.h>
#include <hash.h>
#include <u-boot/sha256.h>

int do_sha256sum(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int flags = HASH_FLAG_ENV;
	int ac;
	char * const *av;

	if (argc < 3)
		return CMD_RET_USAGE;

	av = argv + 1;
	ac = argc - 1;
#ifdef CONFIG_SHA256SUM_VERIFY
	if (strcmp(*av, "-v") == 0) {
		flags |= HASH_FLAG_VERIFY;
		av++;
		ac--;
	}
#endif

	return hash_command("sha256", flags, cmdtp, flag, ac, av);
}

#ifdef CONFIG_SHA256SUM_VERIFY
U_BOOT_CMD(
	sha256sum,	5,	1,	do_sha256sum,
	"compute sha256 message digest",
	"address count [[*]sum]\n"
		"    - compute sha256 message digest [save to sum]\n"
	"sha256sum -v address count [*]sum\n"
		"    - verify sha256sum of memory area"
);
#else
U_BOOT_CMD(
	sha256sum,	4,	1,	do_sha256sum,
	"compute sha256 message digest",
	"address count [[*]sum]\n"
		"    - compute sha256 message digest [save to sum]"
);
#endif
