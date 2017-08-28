/*
	* (C) Copyright 2009-2013 ADVANSEE
	* Benoît Thébaudeau <benoit.thebaudeau@advansee.com>
	*
	* Based on the mpc512x iim code:
	* Copyright 2008 Silicon Turnkey Express, Inc.
	* Martha Marx <mmarx@silicontkx.com>
	*
	* SPDX-License-Identifier:	GPL-2.0+
	*/

#include <common.h>
#include <fuse.h>
#include <asm/errno.h>
#include <asm/io.h>



int fuse_read(u32 bank, u32 word, u32 *val)
{
	FUSE_Read ((FUSE_STORAGE_ARRAY_T)bank, (u16)word, (u8*)val);
	return 0;
}

int fuse_sense(u32 bank, u32 word, u32 *val)
{
	/* We do not support overriding */
	return -EINVAL;
}



int fuse_prog(u32 bank, u32 word, u32 val)
{
	return FUSE_ProgramByte((FUSE_STORAGE_ARRAY_T)bank, (u16)word, (u8)val);
}

int fuse_override(u32 bank, u32 word, u32 val)
{
	/* We do not support overriding */
	return -EINVAL;
}
