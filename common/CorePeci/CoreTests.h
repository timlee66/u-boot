/*--------------------------------------------------------------------------
 * Copyright (c) 2009-2010 by Nuvoton Technology Israel
 * All rights reserved.
 *--------------------------------------------------------------------------
 * File Contents:
 *           Holds the tests' headers
 *--------------------------------------------------------------------------*/
#ifndef _CORE_TESTS_H_
#define _CORE_TESTS_H_



// Register PECI_CTL_STS
#define DONE_EN			0x40
#define ABRT_ERR		0x10
#define CRC_ERR			0x08
#define DONE			0x02
#define START_BUSY		0x01

#define DEVICE_ADDR		0x30

#define GET_TEMP_0		0x01


// msgCmdParam1
#define START_TEST		0x00
#define DONE_TEST		0x01


void check_int(void);



#endif // _CORE_TESTS_H_
