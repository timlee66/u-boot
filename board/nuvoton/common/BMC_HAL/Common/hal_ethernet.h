/*---------------------------------------------------------------------------------------------------------*/
/*  Nuvoton Technology Corporation confidential                                                            */
/*                                                                                                         */
/*  Copyright (c) 2009 by Nuvoton Technology Corporation                                                   */
/*  All rights reserved                                                                                    */
/*                                                                                                         */
/*<<<------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   hal_ethernet.h                                                                                        */
/*            This file contains Ethernet common definitions for HAL modules                               */
/* Project:                                                                                                */
/*            BMC HAL                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef _HAL_ETHERNET_H
#define _HAL_ETHERNET_H

/*---------------------------------------------------------------------------------------------------------*/
/* ETH types                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	EMC_TYPE    = 0,
	GMAC_TYPE   = 1,
} ETH_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Receive callback                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
	void (*Rx)(ETH_T type, UINT devNum, UINT8** packet, UINT32 size, HAL_STATUS status);
	void (*Tx)(ETH_T type, UINT devNum, UINT8*  packet, UINT32 size, HAL_STATUS status);
	void (*Error)(ETH_T type, UINT devNum, HAL_STATUS status);
} ETH_CALLBACK_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Ethernet speed type                                                                                     */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	ETH_SPEED_10    = 10,
	ETH_SPEED_100   = 100,
	ETH_SPEED_1000  = 1000,
} ETH_SPEED_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Ethernet duplex type                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
typedef enum
{
	ETH_DUPLEX_FULL,
	ETH_DUPLEX_HALF,
} ETH_DUPLEX_T;

/*---------------------------------------------------------------------------------------------------------*/
/* PHY Register read/write function types                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
typedef UINT32  (*ETH_PHY_REG_READ_T)   (UINT devNum, UINT phyNum, UINT32 regAddr);
typedef void    (*ETH_PHY_REG_WRITE_T)  (UINT devNum, UINT phyNum, UINT32 regAddr, UINT32 data);


/*---------------------------------------------------------------------------------------------------------*/
/* Phy operations structure                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
	const UINT32 Ident;

	HAL_STATUS  (*PHY_Init)                 (ETH_PHY_REG_READ_T phy_reg_read, ETH_PHY_REG_WRITE_T phy_reg_write);
	UINT32      (*PHY_GetID)                (UINT devNum, UINT phyNum);
	HAL_STATUS  (*PHY_Reset)                (UINT devNum, UINT phyNum);
	HAL_STATUS  (*PHY_AutoNegotiate)        (UINT devNum, UINT phyNum, ETH_SPEED_T  speed, ETH_DUPLEX_T  duplex);
	HAL_STATUS  (*PHY_GetLinkCapabilities)  (UINT devNum, UINT phyNum, ETH_SPEED_T* speed, ETH_DUPLEX_T* duplex);
	HAL_STATUS  (*PHY_SetLinkCapabilities)  (UINT devNum, UINT phyNum, ETH_SPEED_T  speed, ETH_DUPLEX_T  duplex);
} ETH_PHY_OPS_T;


/*---------------------------------------------------------------------------------------------------------*/
/* MAC address size                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define MAC_ADDR_SIZE           6


/* EMC RMII PHY  */
/*---------------------------------------------------------------------------------------------------------*/
/* Basic Mode control register fields                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define HAL_BMCR_COLLISION_TEST         7, 1
#define HAL_BMCR_DUPLEX_MODE            8, 1
#define HAL_BMCR_RESTART_AUTONEG        9, 1
#define HAL_BMCR_ISOLATE               10, 1
#define HAL_BMCR_POWER_DOWN            11, 1
#define HAL_BMCR_AUTONEG               12, 1
#define HAL_BMCR_SPEED_SELECT          13, 1
#define HAL_BMCR_LOOPBACK              14, 1
#define HAL_BMCR_RESET                 15, 1

/*---------------------------------------------------------------------------------------------------------*/
/* Mii Status Control Register fields                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define HAL_BMSR_LINK                   2, 1
#define HAL_BMSR_AUTONEG_COMPLETE       5, 1
#define HAL_BMSR_10_HD                 11, 1
#define HAL_BMSR_10_FD                 12, 1
#define HAL_BMSR_100_HD                13, 1
#define HAL_BMSR_100_FD                14, 1


/*---------------------------------------------------------------------------------------------------------*/
/* PHY status register fields                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define PHYSTS_LINK                 0, 1
#define PHYSTS_SPEED                1, 1
#define PHYSTS_DUPLEX               2, 1
#define PHYSTS_LOOPBACK_STAT        3, 1
#define PHYSTS_AUTONEG_COMPLETE     4, 1
#define PHYSTS_JABBER               5, 1
#define PHYSTS_PHY_RMT_FAULT        6, 1
#define PHYSTS_PAGE_RCV             8, 1
#define PHYSTS_DESCRAM_LOCK         9, 1
#define PHYSTS_SIG_DETECT          10, 1
#define PHYSTS_FALSE_CAR_SENSE     11, 1
#define PHYSTS_POLARITY_STAT       12, 1
#define PHYSTS_RX_ERR_LATCH        13, 1

/*---------------------------------------------------------------------------------------------------------*/
/* Auto-Negotiation Advertisement Register fields                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define ANAR_10_HALF                5, 1
#define ANAR_10_FULL                6, 1
#define ANAR_100_HALF               7, 1
#define ANAR_100_FULL               8, 1


/* GMAC RGMII PHY  */

/*---------------------------------------------------------------------------------------------------------*/
/* MII CON Fields                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
#define MII_CON_SPEED1              6, 1
#define MII_CON_COLLISION           7, 1
#define MII_CON_DUPLEX              8, 1
#define MII_CON_RESTART_AN          9, 1
#define MII_CON_ISOLATE            10, 1
#define MII_CON_POWER_DOWN         11, 1
#define MII_CON_AN_ENABLE          12, 1
#define MII_CON_SPEED2             13, 1
#define MII_CON_INT_LB             14, 1
#define MII_CON_RST                15, 1

/*---------------------------------------------------------------------------------------------------------*/
/* MII STAT Fields                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define MII_STAT_EXTENDED           0, 1
#define MII_STAT_JABBER             1, 1
#define MII_STAT_LINK               2, 1
#define MII_STAT_AN_ABILITY         3, 1
#define MII_STAT_REMOTE_FAULT       4, 1
#define MII_STAT_AN_COMPLETED       5, 1
#define MII_STAT_PREAMBLE_SUPPRES   6, 1
#define MII_STAT_EXTENDED_STATUS    7, 1


/*---------------------------------------------------------------------------------------------------------*/
/* AUX Status summary register                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
#define AUX_STAT_PAUSE_TX           0, 1
#define AUX_STAT_PAUSE_RX           1, 1
#define AUX_STAT_LINK               2, 1
#define AUX_STAT_NEXT_PAGE          3, 1
#define AUX_STAT_AN_CAP             4, 1
#define AUX_STAT_AN_PAGE            5, 1
#define AUX_STAT_REMOTE_FLT         6, 1
#define AUX_STAT_PARALEL_FLT        7, 1
#define AUX_STAT_HCD                8, 3
#define AUX_STAT_AN_NEXT_PAGE      11, 1
#define AUX_STAT_AN_ABILITY        12, 1
#define AUX_STAT_AN_ACK            13, 1
#define AUX_STAT_AN_COMPLETE_ACK   14, 1
#define AUX_STAT_AN_COMPLETE       15, 1


typedef enum
{
	AUX_STAT_HCD_10HALF     = 1,
	AUX_STAT_HCD_10FULL     = 2,
	AUX_STAT_HCD_100HALF    = 3,
	AUX_STAT_HCD_100FULL    = 5,
	AUX_STAT_HCD_1000HALF   = 6,
	AUX_STAT_HCD_1000FULL   = 7,
} AUX_STAT_HCD_T;


/*---------------------------------------------------------------------------------------------------------*/
/* BASET_CON register                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
#define BASET_CON_1000HALF          8, 1
#define BASET_CON_1000FULL          9, 1


/*---------------------------------------------------------------------------------------------------------*/
/* AN_ADV register                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#define AN_ADV_10HALF               5, 1
#define AN_ADV_10FULL               6, 1
#define AN_ADV_100HALF              7, 1
#define AN_ADV_100FULL              8, 1

#endif /*_HAL_ETHERNET_H */

