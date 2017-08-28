#if 0
/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2008 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   cmd_gmac.c                                                                                            */
/*            This file contains GMAC test commands                                                        */
/*  Project:                                                                                               */
/*            U-Boot                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#include <common.h>
#include <command.h>
#include <net.h>

#undef MAX
#undef MIN

#include "BMC_HAL/Chips/chip_if.h"
#include "BMC_HAL/Modules/gmac/Yarkon_IP/gmac_drv.h"

#ifdef CONFIG_CMD_GMAC

/*---------------------------------------------------------------------------------------------------------*/
/* Sellecting GMAC port                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define EMC_TYPE                0
#define GMAC_TYPE               1

#if     CONCAT2(ETH0_TYPE, _TYPE) == GMAC_TYPE
	#define GMAC_TEST_PORT   0
#elif   CONCAT2(ETH1_TYPE, _TYPE) == GMAC_TYPE
	#define GMAC_TEST_PORT   1
#else
	#error GMAC Port was not defined!
#endif


/*---------------------------------------------------------------------------------------------------------*/
/* Local defines                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
#define GMAC_TEST_BUFFER_SIZE   128
#define GMAC_TEST_BUFFER_NUM    1
#define GMAC_TEST_SEED          0xFA462131

#define GMAC_TEST_BUFF_WAITING  0
#define GMAC_TEST_BUFF_SUCC     1
#define GMAC_TEST_BUFF_FAIL     2

/*---------------------------------------------------------------------------------------------------------*/
/* Local Variables                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* For loopback test */
static UINT8 gmac_test_mac[] = {0x00, 0x16, 0x46 ,0x9d, 0xcc, 0xc3 };
static UINT8 gmac_test_buffer[GMAC_TEST_BUFFER_NUM][GMAC_TEST_BUFFER_SIZE] = {{0}};
static UINT8 gmac_test_sync  [GMAC_TEST_BUFFER_NUM] = {0};
static UINT8 gmac_test_curbuf = 0;

/* for buffer test */
static UINT32 gmac_test_buff_ptr    = 0;
static UINT32 gmac_test_buff_size   = 0;


/*---------------------------------------------------------------------------------------------------------*/
/* ICMP stuff for PING                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
	/* Ethernet */
	UINT16  padding;

	UINT8   mac_dest[6];
	UINT8   mac_src[6];
	UINT8   eth_type[2];

	/* IP */
	UINT8   ip_ver;
	UINT8   ip_type;
	UINT16  length;
	UINT16  ip_id;
	UINT16  ip_flags;
	UINT8   ip_ttl;
	UINT8   ip_protocol;
	UINT16  ip_checksum;
	UINT32  ip_addr_src;
	UINT32  ip_addr_dest;

	/* ICMP */
	UINT8   icmp_type;
	UINT8   icmp_code;
	UINT16  icmp_checksum;
	UINT16  icmp_ident;
	UINT16  icmp_seq;
}  ICMP_packet_t;

/*---------------------------------------------------------------------------------------------------------*/
/* Default ICMP ping request packet                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
ICMP_packet_t ICMP_packet =

{
	0x0000,

	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},     /* Dest MAC */
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},     /* Src MAC */
	{0x8,  0x0},                              /* ETH type */

	0x45,    0x0,     0x1C00,
	0x0100,          0x0040,
	0xFF,    0x1,     0x0000,
	0x00000000,
	0x00000000,

	0x8,     0x0,     0xFEF7,
	0x0000,     0x0100,
};




/*---------------------------------------------------------------------------------------------------------*/
/* Simple non-cryptographic random function implementation                                                 */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct rand_strct {UINT32 a; UINT32 b; UINT32 c; UINT32 d; } rand_device_t;
#define rand_rot(x,k) (((x)<<(k))|((x)>>(32-(k))))

UINT32 RandGet( rand_device_t *x )
{
	UINT32 e = x->a - rand_rot(x->b, 27);
	x->a = x->b ^ rand_rot(x->c, 17);
	x->b = x->c + x->d;
	x->c = x->d + e;
	x->d = e + x->a;
	return x->d;
}

void RandInit( rand_device_t *x, UINT32 seed )
{
	UINT32 i;
	x->a = 0xf1ea5eed, x->b = x->c = x->d = seed;
	for (i=0; i<20; ++i)
	{
        RandGet(x);
	}
}
/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/





/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GMACtest_LoopbackCallback                                                              */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  buffer -                                                                               */
/*                  size -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a callback for loopback test                                           */
/*---------------------------------------------------------------------------------------------------------*/
void GMACtest_LoopbackCallback(UINT8* buffer, UINT32 size)
{
	UINT8 i = 0;

	printf("Executing loopback test callback\n");

	/*-----------------------------------------------------------------------------------------------------*/
	/* If size is different definetly something wrong                                                      */
	/*-----------------------------------------------------------------------------------------------------*/
	if (size != GMAC_TEST_BUFFER_SIZE)
	{
        gmac_test_sync[gmac_test_curbuf] = GMAC_TEST_BUFF_FAIL;

        printf("GMAC buffer test: received buffer size (0x%lX) not much the expected (0x%X)", \
                size, GMAC_TEST_BUFFER_SIZE);
        return;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking the buffer content                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	for (i=0; i<size; ++i)
	{
        if (gmac_test_buffer[gmac_test_curbuf][i] != buffer[i])
        {
            /*---------------------------------------------------------------------------------------------*/
            /* If buffer is different, we mark it as failure                                               */
            /*---------------------------------------------------------------------------------------------*/
            gmac_test_sync[gmac_test_curbuf] = GMAC_TEST_BUFF_FAIL;
            return;
        }
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* If everything is OK, we marked the buffer as succefull                                              */
	/*-----------------------------------------------------------------------------------------------------*/
	gmac_test_sync[gmac_test_curbuf] = GMAC_TEST_BUFF_SUCC;

}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GMACtest_bufferCallback                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  buffer -                                                                               */
/*                  size -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a callback for buffer send test                                        */
/*---------------------------------------------------------------------------------------------------------*/
void GMACtest_bufferCallback(UINT32* buffer, UINT32 size)
{
	UINT8 i = 0;

	printf("Executing buffer test callback\n");

	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking buffer size                                                                                */
	/*-----------------------------------------------------------------------------------------------------*/
	if (size != gmac_test_buff_size)
	{
        printf("GMAC buffer test: received buffer size (0x%lX) not much the expected (0x%lX)", \
                size, gmac_test_buff_size);
        return;
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking the buffer content                                                                         */
	/*-----------------------------------------------------------------------------------------------------*/
	for (i=0; i<size; ++i)
	{
        if (((UINT8*)gmac_test_buff_ptr)[i] != buffer[i])
        {
            printf("GMAC buffer test: received buffer element (0x%lX) not much the expected (0x%X) in offset %d", \
                    buffer[i], ((UINT8*)gmac_test_buff_ptr)[i], i);
            return;
        }
	}
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GMACtest_loopback                                                                      */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  internal -                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs GMAC loopback test                                               */
/*---------------------------------------------------------------------------------------------------------*/
UINT32 GMACtest_loopback(BOOLEAN internal)
{
	UINT32         i = 0;
	UINT32         j = 0;
	rand_device_t  RandDevice;


	printf("-----==== GMAC Loopback test ====-----\n");

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initializing random device                                                                          */
	/*-----------------------------------------------------------------------------------------------------*/
	RandInit(&RandDevice, GMAC_TEST_SEED);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initializing test buffers                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	for (i=0; i<GMAC_TEST_BUFFER_NUM; ++i)
	{
        gmac_test_sync[i] = GMAC_TEST_BUFF_WAITING;

        for (j=0; j<GMAC_TEST_BUFFER_SIZE; ++j)
        {
           gmac_test_buffer[i][j] = RandGet(&RandDevice) & 0xFF;
        }
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initializing GMAC Driver                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	GMAC_Initialize(GMAC_TEST_PORT, TRUE, (ETH_Rx_Callback_t)GMACtest_LoopbackCallback, gmac_test_mac);

	if (internal)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Setting internal loopback if needed                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        GMAC_LoopbackInternalMode(GMAC_TEST_PORT, TRUE);
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Checking loop                                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	for (i=0; i<GMAC_TEST_BUFFER_NUM; ++i)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Setting current buffer                                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        gmac_test_curbuf = i;

        /*-------------------------------------------------------------------------------------------------*/
        /* Sending the buffer to GMAC driver                                                               */
        /*-------------------------------------------------------------------------------------------------*/
        GMAC_Send(GMAC_TEST_PORT, &gmac_test_buffer[i], GMAC_TEST_BUFFER_SIZE);

        /*-------------------------------------------------------------------------------------------------*/
        /* Waiting for callback execution                                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        /*while(gmac_test_sync[i] == GMAC_TEST_BUFF_WAITING); */

        /*-------------------------------------------------------------------------------------------------*/
        /* Callback executed, checking the result                                                          */
        /*-------------------------------------------------------------------------------------------------*/
        if (gmac_test_sync[i] == GMAC_TEST_BUFF_FAIL)
        {
            printf("Buffer %ld failed in verification\n", i);
            return 1;
        }
	}


	return 0;
}


UINT32 GMACtest_sendbuffer(void* buffer, UINT32 size, BOOLEAN internal)
{
	printf("-----==== GMAC Buffer test ====-----\n");

	/*-----------------------------------------------------------------------------------------------------*/
	/* Initializing GMAC Driver                                                                            */
	/*-----------------------------------------------------------------------------------------------------*/
	GMAC_Initialize(GMAC_TEST_PORT, TRUE, (ETH_Rx_Callback_t)GMACtest_bufferCallback, gmac_test_mac);

	if (internal)
	{
        /*-------------------------------------------------------------------------------------------------*/
        /* Setting internal loopback if needed                                                             */
        /*-------------------------------------------------------------------------------------------------*/
        GMAC_LoopbackInternalMode(GMAC_TEST_PORT, TRUE);
	}


	/*-----------------------------------------------------------------------------------------------------*/
	/* Setting parapeters for the callback                                                                 */
	/*-----------------------------------------------------------------------------------------------------*/
	gmac_test_buff_ptr  = (UINT32)buffer;
	gmac_test_buff_size = size;

	/*-----------------------------------------------------------------------------------------------------*/
	/* Sending the buffer to GMAC driver                                                                   */
	/*-----------------------------------------------------------------------------------------------------*/
	GMAC_Send(GMAC_TEST_PORT, buffer, size);

	return 0;
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GMACtest_getMAC                                                                        */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  mac -                                                                                  */
/*                  str -                                                                                  */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine parse MAC string into MAC array                                           */
/*---------------------------------------------------------------------------------------------------------*/
static void GMACtest_getMAC(char* str, UINT8* mac)
{
	char *s;
	char *e;
	INT32 x;

	s = str;

	for (x = 0; x < 6; ++x)
	{
        mac[x] = (UINT32)simple_strtoul(s, &e, 16);

        if (s)
        {
            s = (*e) ? e + 1 : e;
        }
	}
}




/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GMACtes_ping_callback                                                                  */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  data -                                                                                 */
/*                  size -                                                                                 */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is a callback for GMAC ping test                                          */
/*---------------------------------------------------------------------------------------------------------*/
void GMACtes_ping_callback(UINT32* data, UINT32 size)
{
	UINT32 i=0;
	UINT8* d = (UINT8*)data;

	printf("Returned Size = %ld Data=", size);
	for (i=0; i<size; i++)
	{
        printf("0x%X ", d[i]);
	}
	printf("\n");
}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:        GMACtes_ping                                                                           */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  dest_ip -                                                                              */
/*                  dest_mac -                                                                             */
/*                                                                                                         */
/* Returns:         none                                                                                   */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine performs GMAC ping test                                                   */
/*---------------------------------------------------------------------------------------------------------*/
void GMACtes_ping(UINT32 dest_ip, UINT8* dest_mac)
{
	UINT32 i=0;
	UINT8*  ptr;

	GMAC_Initialize(0, TRUE, (ETH_Rx_Callback_t)GMACtes_ping_callback, gmac_test_mac);

	/* Setting MAC addresses */
	for (i=0; i<6; i++)
	{
        ICMP_packet.mac_dest[i] = dest_mac[i];
        ICMP_packet.mac_src[i] = gmac_test_mac[i];
	}

	/* Setting IP addresses */
	ICMP_packet.ip_addr_src     = 0xa00a8c0;
	ICMP_packet.ip_addr_dest    = dest_ip;

	/* Setting Checksum */
	ICMP_packet.ip_checksum     = 0;
	ICMP_packet.icmp_checksum   = 0;
	ICMP_packet.ip_checksum     = ~NetCksum(((UINT8*)&ICMP_packet) + 14, 20 / 2);
	ICMP_packet.icmp_checksum   = ~NetCksum(((UINT8*)&ICMP_packet) + 34, 8 / 2);


	printf("Source IP=%lx, Dest IP= %lx\n",ICMP_packet.ip_addr_src, ICMP_packet.ip_addr_dest);
	printf("Sending Data=");
	ptr=(UINT8*)&ICMP_packet + 2;
	for(i=0; i<sizeof(ICMP_packet)-2; ++i)
	{
        printf("0x%X ", ptr[i]);
	}
	printf("\n");

	GMAC_Send(0, ((void*)&ICMP_packet) + 2, sizeof(ICMP_packet) - 2);



}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:        do_gmac                                                                                */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*                  argc -                                                                                 */
/*                  argv -                                                                                 */
/*                  cmdtp -                                                                                */
/*                  flag -                                                                                 */
/*                                                                                                         */
/* Returns:                                                                                                */
/* Side effects:                                                                                           */
/* Description:                                                                                            */
/*                  This routine is the main hadle for all GMAC commands                                   */
/*---------------------------------------------------------------------------------------------------------*/
int do_gmac (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	UINT32 i=0;

	for (i=0; i<argc; ++i)
	{
        printf("%ld: %s\n",i, argv[i]);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Loopback test                                                                                       */
	/*-----------------------------------------------------------------------------------------------------*/
	if (!strcmp(argv[1], "lp"))
	{
        if (argc > 2)
            if (!strcmp(argv[2], "int"))
                return GMACtest_loopback(TRUE);

        return GMACtest_loopback(FALSE);
	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Send buffer test                                                                                    */
	/*-----------------------------------------------------------------------------------------------------*/
	else if (!strcmp(argv[1], "send_buffer"))
	{
        if (argc < 4)
            return 1;
        else if (argc < 5)
                return GMACtest_sendbuffer((void*)simple_strtoul(argv[2], NULL, 16), \
                                            simple_strtoul(argv[3], NULL, 16), FALSE);
        else
            return GMACtest_sendbuffer((void*)simple_strtoul(argv[2], NULL, 16), \
                                        simple_strtoul(argv[3], NULL, 16), TRUE);

	}

	/*-----------------------------------------------------------------------------------------------------*/
	/* Ping test                                                                                           */
	/*-----------------------------------------------------------------------------------------------------*/
	else if (!strcmp(argv[1], "ping"))
	{
        if (argc < 5)
        {
            printf("Specify IP and MAC addresses\n");
        }

        UINT8   dest_mac[8];
        UINT32  dest_ip;



        dest_ip = string_to_ip(argv[2]);
        GMACtest_getMAC(argv[3], dest_mac);

        printf("IP=0x%lX MAC=", dest_ip);
        for (i=0; i<6; i++)
            printf("%X",dest_mac[i]);
        printf("\n");

        GMACtes_ping(dest_ip, dest_mac);


	}

	return 0;
}





/*---------------------------------------------------------------------------------------------------------*/
/* Registering the command                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
U_BOOT_CMD(
	gmac,  5,  1,  do_gmac,
	"GMAC test command",
	"<test_type> [test_param]\n\n"
	"---------------------------------------------------------------------------|\n"
	"| test_type  |    test_param(s)     |           Description                |\n"
	"-------------+----------------------+--------------------------------------|\n"
	"| lp         |     [ int ]          |                                      |\n"
	"|            |                      | int       - using internal loopback  |\n"
	"|            |                      | if empty  - using external loopback  |\n"
	"| send_buffer| <ptr> <size> [ int ] |                                      |\n"
	"|            |                      | ptr       - memory address of the    |\n"
	"|            |                      |             buffer                   |\n"
	"|            |                      | size      - buffer size              |\n"
	"|            |                      | int       - using internal loopback  |\n"
	"|            |                      | if empty  - using external loopback  |\n"
	"| ping       | <ip>  <mac>          |                                      |\n"
	"|            |                      | ip        - IP adress of destination |\n"
	"|            |                      | mac       - MAC adress of destination|\n"
	"---------------------------------------------------------------------------|\n"
);


#endif
#endif   /* IF 0 */
