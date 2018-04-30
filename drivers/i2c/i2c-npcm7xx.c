/*
 * Copyright (c) 2014-2017 Nuvoton Technology corporation.
 *
 * Released under the GPLv2 only.
 * SPDX-License-Identifier: GPL-2.0
 */



#include <common.h>
#include <fuse.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <config.h>
#include <command.h>
#include <malloc.h>
#include <mapmem.h>

#include <common.h>
//#include <asm/arch/clock.h>
#include <asm/io.h>
#include <i2c.h>
#include <watchdog.h>
#include <dm.h>
#include <fdtdec.h>
#include <dm/device.h>
#include <asm/bitops.h>
#include <asm/global_data.h>
#include <bitfield.h>
#include <common.h>
#include <fdtdec.h>
#include <malloc.h>
#include <dm/device.h>
#include <dm/device-internal.h>
#include <dm/lists.h>
#include <dm/pinctrl.h>
#include <dm/platdata.h>
#include <dm/uclass.h>
#include <dm/uclass-internal.h>
#include <dm/util.h>
#include <linux/err.h>
#include <linux/list.h>


DECLARE_GLOBAL_DATA_PTR;




/*
 * Bitfield access macros
 *
 * FIELD_{GET,PREP} macros take as first parameter shifted mask
 * from which they extract the base mask and shift amount.
 * Mask must be a compilation time constant.
 *
 * Example:
 *
 *  #define REG_FIELD_A  GENMASK(6, 0)
 *  #define REG_FIELD_B  BIT(7)
 *  #define REG_FIELD_C  GENMASK(15, 8)
 *  #define REG_FIELD_D  GENMASK(31, 16)
 *
 * Get:
 *  a = FIELD_GET(REG_FIELD_A, reg);
 *  b = FIELD_GET(REG_FIELD_B, reg);
 *
 * Set:
 *  reg = FIELD_PREP(REG_FIELD_A, 1) |
 *	  FIELD_PREP(REG_FIELD_B, 0) |
 *	  FIELD_PREP(REG_FIELD_C, c) |
 *	  FIELD_PREP(REG_FIELD_D, 0x40);
 *
 * Modify:
 *  reg &= ~REG_FIELD_C;
 *  reg |= FIELD_PREP(REG_FIELD_C, c);
 */

#define __bf_shf(x) (__builtin_ffsll(x) - 1)

/**
 * FIELD_FIT() - check if value fits in the field
 * @_mask: shifted mask defining the field's length and position
 * @_val:  value to test against the field
 *
 * Return: true if @_val can fit inside @_mask, false if @_val is too big.
 */
#define FIELD_FIT(_mask, _val)						\
	({								\
		!((((typeof(_mask))_val) << __bf_shf(_mask)) & ~(_mask)); \
	})

/**
 * FIELD_PREP() - prepare a bitfield element
 * @_mask: shifted mask defining the field's length and position
 * @_val:  value to put in the field
 *
 * FIELD_PREP() masks and shifts up the value.  The result should
 * be combined with other fields of the bitfield using logical OR.
 */
#define FIELD_PREP(_mask, _val)						\
	({								\
		((typeof(_mask))(_val) << __bf_shf(_mask)) & (_mask);	\
	})

/**
 * FIELD_GET() - extract a bitfield element
 * @_mask: shifted mask defining the field's length and position
 * @_reg:  32bit value of entire bitfield
 *
 * FIELD_GET() extracts the field specified by @_mask from the
 * bitfield passed in as @_reg by masking and shifting it down.
 */
#define FIELD_GET(_mask, _reg)						\
	({								\
		(typeof(_mask))(((_reg) & (_mask)) >> __bf_shf(_mask));	\
	})

#ifdef	__KERNEL__
#define BIT(nr)			(1UL << (nr))
#define BIT_ULL(nr)		(1ULL << (nr))
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#define BIT_ULL_MASK(nr)	(1ULL << ((nr) % BITS_PER_LONG_LONG))
#define BIT_ULL_WORD(nr)	((nr) / BITS_PER_LONG_LONG)
#define BITS_PER_BYTE		8
#define BITS_TO_LONGS(nr)	DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#endif
/* Create a contiguous bitmask starting at bit position @l
and ending at * position @h.
For example
* GENMASK_ULL(39, 21) gives us the 64bit vector 0x000000ffffe00000. */

#define GENMASK(h, l) \
	(((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))



//static struct regmap *gcr_regmap = NULL;

#define  I2CSEGCTL_OFFSET 0xE4

#define NPCM7XX_SECCNT          (0x68)
#define NPCM7XX_CNTR25M         (0x6C)

#define  I2CSEGCTL_VAL	0x0333F000

#define ENABLE	1
#define DISABLE	0


#ifndef ASSERT
#ifdef DEBUG
#define ASSERT(cond)  {if (!(cond)) for (;;) ; }		 /* infinite loop*/
#else
#define ASSERT(cond)
#endif
#endif


#define I2C_VERSION "0.0.2"

// #define CONFIG_NPCM750_I2C_DEBUG
#ifdef CONFIG_NPCM750_I2C_DEBUG
//#define dev_err(a, f, x...) printf("NPCM750-I2C: %s() dev_err:" f, __func__, ## x)
#define I2C_DEBUG(f, x...)  printf("NPCM750-I2C: %s():%d " f, __func__, \
					__LINE__, ## x)
#else
#define I2C_DEBUG(f, x...)
#endif
#define HAL_PRINT(f, x...)		printf(f, ## x)


#define SMB_CAPABILITY_END_OF_BUSY_SUPPORT
// #define SMB_CAPABILITY_TIMEOUT_SUPPORT

// Using SW PEC instead of HW PEC:
//#define SMB_CAPABILITY_HW_PEC_SUPPORT
//#define SMB_STALL_TIMEOUT_SUPPORT
#define SMB_RECOVERY_SUPPORT

// override issue #614:  TITLE :CP_FW: SMBus may fail to supply stop condition in Master Write operation
#define SMB_SW_BYPASS_HW_ISSUE_SMB_STOP

// if end device reads more data than avalilable, ask issuer or request for more data.
#define SMB_WRAP_AROUND_BUFFER

#define SMB_BYTES_QUICK_PROT						0xFFFF
#define SMB_BYTES_BLOCK_PROT						0xFFFE
#define SMB_BYTES_EXCLUDE_BLOCK_SIZE_FROM_BUFFER	0xFFFD

#define ARP_ADDRESS_VAL			0x61

typedef enum {
	SMB_SLAVE = 1,
	SMB_MASTER
} SMB_MODE_T;

/*
 * External SMB Interface driver states values, which indicate to the
 * upper-level layer the status of the
 * operation it initiated or wake up events from one of the buses
 */
typedef enum {
	SMB_NO_STATUS_IND = 0,
	SMB_SLAVE_RCV_IND = 1,
	SMB_SLAVE_XMIT_IND = 2,
	SMB_SLAVE_XMIT_MISSING_DATA_IND = 3,
	SMB_SLAVE_RESTART_IND = 4,
	SMB_SLAVE_DONE_IND = 5,
	SMB_MASTER_DONE_IND = 6,
	SMB_NO_DATA_IND = 7,
	SMB_NACK_IND = 8,
	SMB_BUS_ERR_IND = 9,
	SMB_WAKE_UP_IND = 10,
	SMB_MASTER_PEC_ERR_IND = 11,
	SMB_MASTER_BLOCK_BYTES_ERR_IND = 12,
	SMB_SLAVE_PEC_ERR_IND = 13,
#ifdef SMB_WRAP_AROUND_BUFFER
	SMB_SLAVE_RCV_MISSING_DATA_IND = 14,
#endif
} SMB_STATE_IND_T;

/* SMBus Operation type values  */
typedef enum {
	SMB_NO_OPER	= 0,
	SMB_WRITE_OPER  = 1,
	SMB_READ_OPER   = 2
} SMB_OPERATION_T;



/* SMBus Bank (FIFO mode) */

typedef enum {
	SMB_BANK_0  = 0,
	SMB_BANK_1  = 1
} SMB_BANK_T;

/* Internal SMBus Interface driver states values, which reflect events which occurred on the bus  */
typedef enum {
	SMB_DISABLE = 0,
	SMB_IDLE,
	SMB_MASTER_START,
	SMB_SLAVE_MATCH,
	SMB_OPER_STARTED,
	SMB_REPEATED_START,
	SMB_STOP_PENDING
} SMB_STATE_T;


typedef enum {
	SMB_SLAVE_ADDR1 = 0,
	SMB_SLAVE_ADDR2,
	SMB_SLAVE_ADDR3,
	SMB_SLAVE_ADDR4,
	SMB_SLAVE_ADDR5,
	SMB_SLAVE_ADDR6,
	SMB_SLAVE_ADDR7,
	SMB_SLAVE_ADDR8,
	SMB_SLAVE_ADDR9,
	SMB_SLAVE_ADDR10,
	SMB_GC_ADDR,
	SMB_ARP_ADDR
} SMB_ADDR_T;


typedef enum {
	SMB_LEVEL_LOW  = 0,
	SMB_LEVEL_HIGH = 1
} SMB_LEVEL_T;



// Common registers
#define NUVOTON_SMBSDA(bus)			 (bus->base + 0x000)
#define NUVOTON_SMBST(bus)				(bus->base + 0x002)
#define NUVOTON_SMBCST(bus)			 (bus->base + 0x004)
#define NUVOTON_SMBCTL1(bus)			(bus->base + 0x006)
#define NUVOTON_SMBADDR1(bus)			(bus->base + 0x008)
#define NUVOTON_SMBCTL2(bus)			(bus->base + 0x00A)
#define NUVOTON_SMBADDR2(bus)			(bus->base + 0x00C)
#define NUVOTON_SMBCTL3(bus)			(bus->base + 0x00E)
#define NUVOTON_SMBCST2(bus)			(bus->base + 0x018)  // Control Status 2
#define NUVOTON_SMBCST3(bus)			(bus->base + 0x019)  // Control Status 3 Register
#define SMB_VER(bus)			(bus->base + 0x01F)  // SMB Version Register

// BANK 0 registers
#define NUVOTON_SMBADDR3(bus)			(bus->base + 0x010)
#define NUVOTON_SMBADDR7(bus)			(bus->base + 0x011)
#define NUVOTON_SMBADDR4(bus)			(bus->base + 0x012)
#define NUVOTON_SMBADDR8(bus)			(bus->base + 0x013)
#define NUVOTON_SMBADDR5(bus)			(bus->base + 0x014)
#define NUVOTON_SMBADDR9(bus)			(bus->base + 0x015)
#define NUVOTON_SMBADDR6(bus)			(bus->base + 0x016)
#define NUVOTON_SMBADDR10(bus)			(bus->base + 0x017)

#define NUVOTON_SMBADDR(bus, i)		 (bus->base + 0x008 + (u32)(((int)i*4) + (((int)i < 2) ? 0 : ((int)i-2)*(-2)) + (((int)i < 6) ? 0 : (-7))))

#define NUVOTON_SMBCTL4(bus)			(bus->base + 0x01A)
#define NUVOTON_SMBCTL5(bus)			(bus->base + 0x01B)
#define NUVOTON_SMBSCLLT(bus)			(bus->base + 0x01C)  // SMB SCL Low Time (Fast-Mode)
#define NUVOTON_SMBFIF_CTL(bus)		 (bus->base + 0x01D)  // FIFO Control
#define NUVOTON_SMBSCLHT(bus)			(bus->base + 0x01E)  // SMB SCL High Time (Fast-Mode)

// BANK 1 registers
#define NUVOTON_SMBFIF_CTS(bus)		 (bus->base + 0x010)  // FIFO Control and Status
#define NUVOTON_SMBTXF_CTL(bus)		 (bus->base + 0x012)  // Tx-FIFO Control
#if defined (SMB_CAPABILITY_TIMEOUT_SUPPORT)
#define NUVOTON_SMBT_OUT(bus)			(bus->base + 0x014)  // Bus Time-Out
#endif
#if defined (SMB_CAPABILITY_HW_PEC_SUPPORT)
#define NUVOTON_SMBPEC(bus)			 (bus->base + 0x016)  // PEC Data
#endif
#define NUVOTON_SMBTXF_STS(bus)		 (bus->base + 0x01A)  // Tx-FIFO Status
#define NUVOTON_SMBRXF_STS(bus)		 (bus->base + 0x01C)  // Rx-FIFO Status
#define NUVOTON_SMBRXF_CTL(bus)		 (bus->base + 0x01E)  // Rx-FIFO Control



/* NUVOTON_SMBST register fields */
#define NUVOTON_SMBST_XMIT		BIT(0)
#define NUVOTON_SMBST_MASTER		BIT(1)
#define NUVOTON_SMBST_NMATCH		BIT(2)
#define NUVOTON_SMBST_STASTR		BIT(3)
#define NUVOTON_SMBST_NEGACK		BIT(4)
#define NUVOTON_SMBST_BER		BIT(5)
#define NUVOTON_SMBST_SDAST		BIT(6)
#define NUVOTON_SMBST_SLVSTP		BIT(7)

/* NUVOTON_SMBCST register fields */
#define NUVOTON_SMBCST_BUSY		BIT(0)
#define NUVOTON_SMBCST_BB		BIT(1)
#define NUVOTON_SMBCST_MATCH		BIT(2)
#define NUVOTON_SMBCST_GCMATCH		BIT(3)
#define NUVOTON_SMBCST_TSDA		BIT(4)
#define NUVOTON_SMBCST_TGSCL		BIT(5)
#define NUVOTON_SMBCST_MATCHAF		BIT(6)
#define NUVOTON_SMBCST_ARPMATCH		BIT(7)

/* NUVOTON_SMBCTL1 register fields */
#define NUVOTON_SMBCTL1_START		BIT(0)
#define NUVOTON_SMBCTL1_STOP		BIT(1)
#define NUVOTON_SMBCTL1_INTEN		BIT(2)
#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
#define NUVOTON_SMBCTL1_EOBINTE		BIT(3)
#endif
#define NUVOTON_SMBCTL1_ACK		BIT(4)
#define NUVOTON_SMBCTL1_GCMEN		BIT(5)
#define NUVOTON_SMBCTL1_NMINTE		BIT(6)
#define NUVOTON_SMBCTL1_STASTRE		BIT(7)

/* NUVOTON_SMBADDRx register fields */
#define NUVOTON_SMBADDRx_ADDR		GENMASK(6, 0)
#define NUVOTON_SMBADDRx_SAEN		BIT(7)

/* NUVOTON_SMBCTL2 register fields*/
#define SMBCTL2_ENABLE		BIT(0)
#define SMBCTL2_SCLFRQ6_0		GENMASK(7, 1)

/* NUVOTON_SMBCTL3 register fields */
#define SMBCTL3_SCLFRQ8_7		GENMASK(1, 0)
#define SMBCTL3_ARPMEN		BIT(2)
#define SMBCTL3_IDL_START		BIT(3)
#define SMBCTL3_400K_MODE		BIT(4)
#define SMBCTL3_BNK_SEL		BIT(5)
#define SMBCTL3_SDA_LVL		BIT(6)
#define SMBCTL3_SCL_LVL		BIT(7)

/* NUVOTON_SMBCST2 register fields */
#define NUVOTON_SMBCST2_MATCHA1F		BIT(0)
#define NUVOTON_SMBCST2_MATCHA2F		BIT(1)
#define NUVOTON_SMBCST2_MATCHA3F		BIT(2)
#define NUVOTON_SMBCST2_MATCHA4F		BIT(3)
#define NUVOTON_SMBCST2_MATCHA5F		BIT(4)
#define NUVOTON_SMBCST2_MATCHA6F		BIT(5)
#define NUVOTON_SMBCST2_MATCHA7F		BIT(5)
#define NUVOTON_SMBCST2_INTSTS		BIT(7)

/* NUVOTON_SMBCST3 register fields */
#define NUVOTON_SMBCST3_MATCHA8F		BIT(0)
#define NUVOTON_SMBCST3_MATCHA9F		BIT(1)
#define NUVOTON_SMBCST3_MATCHA10F		BIT(2)
#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
#define NUVOTON_SMBCST3_EO_BUSY		BIT(7)
#endif

/* NUVOTON_SMBCTL4 register fields */
#define SMBCTL4_HLDT		GENMASK(5, 0)
#ifdef SMB_CAPABILITY_FORCE_SCL_SDA
#define SMBCTL4_LVL_WE		BIT(7)
#endif

/* NUVOTON_SMBCTL5 register fields */
#define SMBCTL5_DBNCT		GENMASK(3, 0)

/* NUVOTON_SMBFIF_CTS register fields */
#define NUVOTON_SMBFIF_CTS_RXF_TXE		BIT(1)
#define NUVOTON_SMBFIF_CTS_RFTE_IE		BIT(3)
#define NUVOTON_SMBFIF_CTS_CLR_FIFO		BIT(6)
#define NUVOTON_SMBFIF_CTS_SLVRSTR		BIT(7)

/* NUVOTON_SMBTXF_CTL register fields */
#ifdef SMB_CAPABILITY_32B_FIFO
#define NUVOTON_SMBTXF_CTL_TX_THR		GENMASK(5, 0)
#else
#define NUVOTON_SMBTXF_CTL_TX_THR		GENMASK(4, 0)
#endif
#define NUVOTON_SMBTXF_CTL_THR_TXIE		BIT(6)

#if defined (SMB_CAPABILITY_TIMEOUT_SUPPORT)

/* NUVOTON_SMBT_OUT register fields */
#define NUVOTON_SMBT_OUT_TO_CKDIV		GENMASK(5, 0)
#define NUVOTON_SMBT_OUT_T_OUTIE		BIT(6)
#define NUVOTON_SMBT_OUT_T_OUTST		BIT(7)
#endif

/* NUVOTON_SMBTXF_STS register fields  */
#ifdef SMB_CAPABILITY_32B_FIFO
#define NUVOTON_SMBTXF_STS_TX_BYTES		GENMASK(5, 0)
#else
#define NUVOTON_SMBTXF_STS_TX_BYTES		GENMASK(4, 0)
#endif
#define NUVOTON_SMBTXF_STS_TX_THST		BIT(6)

/* NUVOTON_SMBRXF_STS register fields */
#ifdef SMB_CAPABILITY_32B_FIFO
#define NUVOTON_SMBRXF_STS_RX_BYTES		GENMASK(5, 0)
#else
#define NUVOTON_SMBRXF_STS_RX_BYTES		GENMASK(4, 0)
#endif
#define NUVOTON_SMBRXF_STS_RX_THST		BIT(6)

/* NUVOTON_SMBFIF_CTL register fields */
#define NUVOTON_SMBFIF_CTL_FIFO_EN		BIT(4)

/* NUVOTON_SMBRXF_CTL register fields */
#ifdef SMB_CAPABILITY_32B_FIFO
#define NUVOTON_SMBRXF_CTL_RX_THR		GENMASK(5, 0)
#define NUVOTON_SMBRXF_CTL_THR_RXIE		BIT(6)
#define NUVOTON_SMBRXF_CTL_LAST_PEC		BIT(7)
#else
#define NUVOTON_SMBRXF_CTL_RX_THR		GENMASK(4, 0)
#define NUVOTON_SMBRXF_CTL_LAST_PEC		BIT(5)
#define NUVOTON_SMBRXF_CTL_THR_RXIE		BIT(6)
#endif

/* SMB_VER register fields */
#define SMB_VER_VERSION		GENMASK(6, 0)
#define SMB_VER_FIFO_EN		BIT(7)



/* stall/stuck timeout */
#define DEFAULT_STALL_COUNT		25


/* Data abort timeout  */
#define ABORT_TIMEOUT	 1000

/* SMBus spec. values in KHz */
#define SMBUS_FREQ_MIN	10

#define SMBUS_FREQ_MAX	1000
#define SMBUS_FREQ_100KHz   100
#define SMBUS_FREQ_400KHz   400
#define SMBUS_FREQ_1MHz	1000



/* SMBus FIFO SIZE (when FIFO hardware exist)*/
#ifdef SMB_CAPABILITY_32B_FIFO
#define SMBUS_FIFO_SIZE	32
#else
#define SMBUS_FIFO_SIZE	16
#endif


/* SCLFRQ min/max field values  */
#define SCLFRQ_MIN		10
#define SCLFRQ_MAX		511

/* SCLFRQ field position  */
#define SCLFRQ_0_TO_6		GENMASK(6, 0)
#define SCLFRQ_7_TO_8		GENMASK(8, 7)

/* SMB Maximum Retry Trials (on Bus Arbitration Loss) */
#define SMB_RETRY_MAX_COUNT	0


#define SMB_NUM_OF_ADDR				10 // TBD move to device tree
#define SMB_FIFO(bus)				true   /* All modules support FIFO */


// for logging:
#define NPCM7XX_I2C_EVENT_START   BIT(0)
#define NPCM7XX_I2C_EVENT_STOP    BIT(1)
#define NPCM7XX_I2C_EVENT_ABORT   BIT(2)
#define NPCM7XX_I2C_EVENT_WRITE   BIT(3)
#define NPCM7XX_I2C_EVENT_READ    BIT(4)
#define NPCM7XX_I2C_EVENT_BER     BIT(5)
#define NPCM7XX_I2C_EVENT_NACK    BIT(6)
#define NPCM7XX_I2C_EVENT_TO      BIT(7)

#define NPCM7XX_I2C_EVENT_LOG(event)   bus->event_log |= event

/* Status of one SMBus module*/
typedef struct nuvoton_i2c_bus {
	struct udevice			*dev;
	//struct i2c_adapter		adap;
	unsigned char __iomem		*base;
	/* Synchronizes I/O mem access to base. */
	spinlock_t			lock;
	volatile int    cmd_complete;
	int				irq;
	int				cmd_err;
	struct i2c_msg			*msgs;
	int				msgs_num;
	int				module__num;
	u32				apb_clk;
	u32             freq;   // 50K, 100K, 400K, 1M

	/* Current state of SMBus */
	volatile SMB_STATE_T  state;

	/* Type of the last SMBus operation */
	SMB_OPERATION_T			operation;

	/* Mode of operation on SMBus */
	SMB_MODE_T			master_or_slave;
//#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
	/* The indication to the hi level after Master Stop */
	SMB_STATE_IND_T                 stop_indication;
//#endif
	/* SMBus slave device's Slave Address in 8-bit format - for master  */
	u8				dest_addr;

	/* Buffer where read data should be placed */
	u8                              *read_data_buf;

	/* Number of bytes to be read */
	u16				read_size;

	/* Number of bytes already read */
	u16				read_index;

	/* Buffer with data to be written */
	u8                              *write_data_buf;

	/* Number of bytes to write */
	u16				write_size;

	/* Number of bytes already written */
	u16				write_index;

	/* use fifo hardware or not */
	bool				fifo_use;

	/* fifo threshold size */
	u8				threshold_fifo;

	/* PEC bit mask per slave address.
	1: use PEC for this address,
	0: do not use PEC for this address */
	u16				PEC_mask;

	/* Use PEC CRC  */
	bool				PEC_use;

	/* PEC CRC data */
	u8				crc_data;

	/* Use read block */
	bool				read_block_use;

	/* Number of retries remaining */
	u8				retry_count;

	/* interrupt counter */
	u8				int_cnt;

	/* log events, fir debugging */
	u8				event_log;

#ifdef SMB_STALL_TIMEOUT_SUPPORT
	u8				stall_counter;
	u8				stall_threshold;
#endif


// override issue #614:  TITLE :CP_FW: SMBus may fail to supply stop condition
// in Master Write operation. If needed : define it at hal_cfg.h
#ifdef SMB_SW_BYPASS_HW_ISSUE_SMB_STOP
	/* The indication to the hi level after Master Stop */
	u32				clk_period_us;
	u32				interrupt_time_stamp[2];
#endif
} nuvoton_i2c_bus_t;



static bool nuvoton_smb_init_module(nuvoton_i2c_bus_t *bus, SMB_MODE_T mode, u16 bus_freq);

static bool nuvoton_smb_master_start_xmit(nuvoton_i2c_bus_t *bus, u8 slave_addr, u16 nwrite, u16 nread,
				u8 *write_data, u8 *read_data, bool use_PEC);
static void nuvoton_smb_master_abort(nuvoton_i2c_bus_t *bus);

#ifdef TBD
static void nuvoton_smb_recovery(nuvoton_i2c_bus_t *bus);
#endif //TBD



#ifdef SMB_STALL_TIMEOUT_SUPPORT
static void nuvoton_smb_set_stall_threshhold(nuvoton_i2c_bus_t *bus, u8 threshold);
static void nuvoton_smb_stall_handler(nuvoton_i2c_bus_t *bus);
#endif

#ifdef TBD
static void nuvoton_smb_init(SMB_CALLBACK_T operation_done);
static bool nuvoton_smb_module_is_busy(nuvoton_i2c_bus_t *bus);
static bool nuvoton_smb_bus_is_busy(nuvoton_i2c_bus_t *bus);
static void nuvoton_smb_re_enable_module(nuvoton_i2c_bus_t *bus);
static bool nuvoton_smb_interrupt_is_pending(void);
#endif

#ifdef SMB_CAPABILITY_FORCE_SCL_SDA
static void nuvoton_smb_set_SCL(nuvoton_i2c_bus_t *bus, SMB_LEVEL_T level);
static void nuvoton_smb_set_SDA(nuvoton_i2c_bus_t *bus, SMB_LEVEL_T level);
#endif // SMB_CAPABILITY_FORCE_SCL_SDA

#ifdef CONFIG_NPCM750_I2C_DEBUG_PRINT
static void nuvoton_smb_print_regs(nuvoton_i2c_bus_t *bus);
static void nuvoton_smb_print_module_regs(nuvoton_i2c_bus_t *bus);
static void nuvoton_smb_print_version(void);
#endif


typedef void (*SMB_CALLBACK_T)(nuvoton_i2c_bus_t *bus, SMB_STATE_IND_T op_status, u16 info);

/*  LOCAL FUNCTIONS FORWARD DECLARATIONS */


static inline void nuvoton_smb_write_byte(nuvoton_i2c_bus_t *bus, u8 data);
static inline bool nuvoton_smb_read_byte(nuvoton_i2c_bus_t *bus, u8 *data);
static inline void nuvoton_smb_select_bank(nuvoton_i2c_bus_t *bus, SMB_BANK_T bank);
static inline u16  nuvoton_smb_get_index(nuvoton_i2c_bus_t *bus);


static inline void nuvoton_smb_master_start(nuvoton_i2c_bus_t *bus);
static inline void nuvoton_smb_master_stop(nuvoton_i2c_bus_t *bus);
static inline void nuvoton_smb_abort_data(nuvoton_i2c_bus_t *bus);
static inline void nuvoton_smb_stall_after_start(nuvoton_i2c_bus_t *bus, bool stall);
static inline void nuvoton_smb_nack(nuvoton_i2c_bus_t *bus);


static		void nuvoton_smb_reset(nuvoton_i2c_bus_t *bus);
static		void nuvoton_smb_int_enable(nuvoton_i2c_bus_t *bus, bool enable);

static		bool nuvoton_smb_init_clk(nuvoton_i2c_bus_t *bus, SMB_MODE_T mode,
			u16 bus_freq);
static		void nuvoton_smb_int_master_handler(nuvoton_i2c_bus_t *bus);

static		void nuvoton_smb_write_to_fifo(nuvoton_i2c_bus_t *bus,
						u16 max_bytes_to_send);

static		void nuvoton_smb_calc_PEC(nuvoton_i2c_bus_t *bus, u8 data);
static inline   void nuvoton_smb_write_PEC(nuvoton_i2c_bus_t *bus);
static inline   u8   nuvoton_smb_get_PEC(nuvoton_i2c_bus_t *bus);

static		void nuvoton_smb_callback(nuvoton_i2c_bus_t *bus,
			SMB_STATE_IND_T op_status, u16 info);


//static void inline _npcm7xx_get_time_stamp(u32 time_quad[2]);
//static u32  inline _npcm7xx_delay_relative(u32 microSecDelay, u32 t0_time[2]);







/**************************************************************************************************************************/
/*   Multiple Function Pin Select Register 1 (MFSEL1) Updated for                                                         */
/**************************************************************************************************************************/
#ifndef MFSEL1
#define  MFSEL1                         (GCR_BASE_ADDR + 0x00C)		/* Offset: GCR_BA + 00Ch */
#define  MFSEL1_SIRQSEL                  BIT(31)              /* 31 SIRQSEL (Serial IRQ Select). Selects GPIO or SERIRQ.                                                               */
#define  MFSEL1_IOX1SEL                  BIT(30)              /* 30 IOX1SEL (Serial GPIO Expander 1 Select). Selects I/O Expander 1 interface option.                                  */
#define  MFSEL1_DVH1SEL                  BIT(27)              /* 27 DVH1SEL (Digital Video Head 1 Select). Selects VCD digital video input source when internal.                       */
#define  MFSEL1_LPCSEL                   BIT(26)              /* 26 LPCSEL (LPC Select). Selects GPIOs or LPC signals.                                                                 */
#define  MFSEL1_PECIB                    BIT(25)              /* 25 PECIB (PECI Bypass). Enables PECI PHY bypass on pins GPIOE11-10. When this bit is 1, MFSEL3.IOXHSEL                */
#define  MFSEL1_GSPISEL                  BIT(24)              /* 24 GSPISEL (Graphics SPI Select). Selects Graphics Core SPI Signals or GPIO option.                                   */
#define  MFSEL1_SMISEL                   BIT(22)              /* 22 SMISEL (SMI Select). Selects nSMI or GPIO170 option.                                                               */
#define  MFSEL1_CLKOSEL                  BIT(21)              /* 21 CLKOSEL (Clockout Select). Selects CLKOUT or GPIIO160 option.                                                      */
#define  MFSEL1_DVOSEL                   GENMASK(20, 18)             /* 20-18 DVOSEL (DVO Select). Selects DVO output/input signals or GPIO option:                                           */
#define  MFSEL1_KBCICSEL                 BIT(17)              /* 17 KBCICSEL (KBC Interface Controller Select). Selects Keyboard Controller Interface Control Signals or GPIO          */
#define  MFSEL1_R2MDSEL                  BIT(16)              /* 16 R2MDSEL (RMII2 MDIO Select). Selects RMII2 MDIO or GPIO option.                                                    */
#define  MFSEL1_R2ERRSEL                 BIT(15)              /* 15 R2ERRSEL (RMII2 R2RXERR Select). Selects RMII2 R2RXERR or GPIO90 option.                                           */
#define  MFSEL1_RMII2SEL                 BIT(14)              /* 14 RMII2SEL (RMII2 Select). Selects RMII2 (GMAC2 module) or GPIO option.                                              */
#define  MFSEL1_R1MDSEL                  BIT(13)              /* 13 R1MDSEL (RMII1 MDIO Select). Selects RMII1 MDIO or GPIO option.                                                    */
#define  MFSEL1_R1ERRSEL                 BIT(12)              /* 12 R1ERRSEL (RMII1 R1RXERR Select). Selects RMII1 R1RXERR or GPIO56 option.                                           */
#define  MFSEL1_HSI2SEL                  BIT(11)              /* 11 HSI2SEL (Host Serial Interface 2 Select). Selects Host Serial Interface 2 or GPIO option.                          */
#define  MFSEL1_HSI1SEL                  BIT(10)              /* 10 HSI1SEL (Host Serial Interface 1 Select). Selects Host Serial Interface 1 or GPIO option.                          */
#define  MFSEL1_BSPSEL                   BIT(9 )              /* 9 BSPSEL (BMC Serial Port Select). Selects Core Serial Port 0 or GPIO option.                                         */
#define  MFSEL1_SMB2SEL                  BIT(8 )              /* 8 SMB2SEL (SMB2 Select). Selects SMB2 or GPIO option.                                                                 */
#define  MFSEL1_SMB1SEL                  BIT(7 )              /* 7 SMB1SEL (SMB1 Select). Selects SMB1 or GPIO option.                                                                 */
#define  MFSEL1_SMB0SEL                  BIT(6 )              /* 6 SMB0SEL (SMB0 Select). Selects SMB0 or GPIO option.                                                                 */
#define  MFSEL1_S0CS3SEL                 BIT(5 )              /* 5 S0CS3SEL (SPI0CS3 Select). Selects nSPI0CS3/SPI0D3 or GPIO34 option. When this bit is set, SP0QSEL bit              */
#define  MFSEL1_S0CS2SEL                 BIT(4 )              /* 4 S0CS2SEL (SPI0CS2 Select). Selects nSPI0CS2/SPI0D2 or GPIO33 option. When this bit is set, SP0QSEL bit              */
#define  MFSEL1_S0CS1SEL                 BIT(3 )              /* 3 S0CS1SEL (SPI0CS1 Select). Selects nSPI0CS1 or GPIO32 option.                                                       */
#define  MFSEL1_SMB5SEL                  BIT(2 )              /* 2 SMB5SEL (SMBus 5 Select). Selects SMB5 or GPIO option.                                                              */
#define  MFSEL1_SMB4SEL                  BIT(1 )              /* 1 SMB4SEL (SMBus 4 Select). Selects SMB4 or GPIO option.                                                              */
#define  MFSEL1_SMB3SEL                  BIT(0 )              /* 0 SMB3SEL (SMBus 3 Select). Selects SMB3 or GPIO option.                                                              */


/**************************************************************************************************************************/
/*   Multiple Function Pin Select Register 3 (MFSEL3) Updated for                                                         */
/**************************************************************************************************************************/
#define  MFSEL3                         (GCR_BASE_ADDR + 0x064) 		/* Offset: GCR_BA + 064h */
#define  MFSEL3_HSDVOSEL                 BIT(26)              /* 26 HSDVOSEL (HSTL DVO Select). Selects DDR DVO on RGMII2 pins (HSTL levels). In Z2 and later. */
#define  MFSEL3_MMCCDSEL                 BIT(25)              /* 25 MMCCDSEL (nMMCCD Select). Selects GPIO155 or nMMCCD. In Z2 and later.                         */
#define  MFSEL3_GPOCSEL                  BIT(22)              /* 22 GPOCSEL (GPOC Select). Selects either GPOI207-206 and GPIO205-204 or CRT2 digital signals.                         */
#define  MFSEL3_WDO2SEL                  BIT(20)              /* 20 WDO2SEL (nWDO2 Select). Selects GPIO219 or nWDO2.                                                                  */
#define  MFSEL3_WDO1SEL                  BIT(19)              /* 19 WDO1SEL (nWDO1 Select). Selects GPIO218 or nWDO1.                                                                  */
#define  MFSEL3_IOXHSEL                  BIT(18)              /* 18 IOXHSEL (Host Serial I/O Expander Select). Selects Host SIOX pins or other options. When this bit is set,          */
#define  MFSEL3_PCIEPUSE                 BIT(17)              /* 17 PCIEPUSE (PCI Express PHY Usage). Selects the PCI Express interface connected to the PHY. PIPE bus                 */
#define  MFSEL3_CLKRUNSEL                BIT(16)              /* 16 CLKRUNSEL (CLKRUN Select). Selects GPIO168 or LPC signal nCLKRUN. When this bit is 1, MFSEL4.8                     */
#define  MFSEL3_IOX2SEL                  BIT(14)              /* 14 IOX2SEL (I/O Expander 2 Select). Selects I/O Expander 2 interface option.                                          */
#define  MFSEL3_PSPI2SEL                 BIT(13)              /* 13 PSPI2SEL (PSPI2 Select). Selects PSPI Signals or GPIO option.                                                      */
#define  MFSEL3_SD1SEL                   BIT(12)              /* 12 SD1SEL (SD1 Select). Selects SD1 or GPIO option.                                                                   */
#define  MFSEL3_MMC8SEL                  BIT(11)              /* 11 MMC8SEL (MMC Select). Selects four additional data lines for MMC or GPIO option.                                   */
#define  MFSEL3_MMCSEL                   BIT(10)              /* 10 MMCSEL (MMC Select). Selects MMC or GPIO option.                                                                   */
#define  MFSEL3_RMII1SEL                 BIT(9 )              /* 9 RMII1SEL (RMII1 Select). Selects RMII1 (EMC1 or GMAC1 module) or GPIO option.                                       */
#define  MFSEL3_SMB15SEL                 BIT(8 )              /* 8 SMB15SEL (SMB15 Select). Selects SMBus15 signals or GPIO21-20. When this bit is set, MFSEL2 bits 25-24              */
#define  MFSEL3_SMB14SEL                 BIT(7 )              /* 7 SMB14SEL (SMB14 Select). Selects SMBus14 signals or GPIO23-22. When this bit is set, MFSEL2 bits 27-26              */
#define  MFSEL3_SMB13SEL                 BIT(6 )              /* 6 SMB13SEL (SMB13 Select). Selects SMBus13 signals or GPIO223-222.                                                    */
#define  MFSEL3_SMB12SEL                 BIT(5 )              /* 5 SMB12SEL (SMB12 Select). Selects SMBus12 signals or GPIO221-220.                                                    */
#define  MFSEL3_PSPI1SEL                 GENMASK(4, 3 )             /* 4-3 PSPI1SEL (PSPI1 Select). Selects PSPI1, FANINs or GPIOs.                                                          */
#define  MFSEL3_SMB7SEL                  BIT(2 )              /* 2 SMB7SEL (SMB7 Select). Selects SMBus7 signals or GPIO174-173.                                                       */
#define  MFSEL3_SMB6SEL                  BIT(1 )              /* 1 SMB6SEL (SMB6 Select). Selects SMBus6 signals or GPIO172-171.                                                       */
#define  MFSEL3_SCISEL                   BIT(0 )              /* 0 SCISEL (SCI Select). Selects nSCIPME or GPIO169.                                                                    */

/**************************************************************************************************************************/
/*   I2C Segment Pin Select Register (I2CSEGSEL) (New in )																*/
/**************************************************************************************************************************/
#define  I2CSEGSEL					  (GCR_BASE_ADDR + 0x0E0) 		/* Offset: GCR_BA + 0E0h */
#define  I2CSEGSEL_S4DESEL			   BIT(23) 		  /* 23 S4DESEL. Selects either GPIO or SMBus 4 Drive Enable signals. If this bit is set, MFSEL3 bit 13 must be 0.		 */
#define  I2CSEGSEL_S0DESEL			   BIT(22) 		  /* 22 S0DESEL. Selects either GPIO or SMBus 0 Drive Enable signals. If this bit is set, MFSEL1 bits 18-20 must be 0.	 */
#define  I2CSEGSEL_S5SDSEL			   BIT(21) 		  /* 21 S5SDSEL. Selects either GPIOs or SMBus 5 segment D signals. If this bit is set, MFSEL1 bit 17 must be 0.		   */
#define  I2CSEGSEL_S5SCSEL			   BIT(20) 		  /* 20 S5CSEL. Selects either GPIOs or SMBus 5 segment C signals. If this bit is set, MFSEL1 bit 24 must be 0.			*/
#define  I2CSEGSEL_S5SBSEL			   BIT(19) 		  /* 19 S5SBSEL. Selects either GPIOs or SMBus 5 segment B signals. If this bit is set, MFSEL1 bit 24 must be 0.		   */
#define  I2CSEGSEL_S4DECFG			   GENMASK(18, 17) 		 /* 18-17 S4DECFG. Selects SMBus 4 Drive Enable controls.																 */
#define  I2CSEGSEL_S4SDSEL			   BIT(16) 		  /* 16 S4SDSEL. Selects either GPIOs or SMBus 4 segment D signals. If this bit is set, MFSEL2 bits 27-26 must be 0.	   */
#define  I2CSEGSEL_S4SCSEL			   BIT(15) 		  /* 15 S4CSEL. Selects either GPIOs or SMBus 4 segment C signals. If this bit is set, MFSEL2 bits 25-24 must be 0.		*/
#define  I2CSEGSEL_S4SBSEL			   BIT(14) 		  /* 14 S4SBSEL. Selects either GPIOs or SMBus 4 segment B signals. If this bit is set, MFSEL3 bit 13 must be 0.		   */
#define  I2CSEGSEL_S3SDSEL			   BIT(13) 		  /* 13 S3SDSEL. Selects either GPIOs or SMBus 3 segment D signals. If this bit is set, MFSEL2.31-30 must be 0.			*/
#define  I2CSEGSEL_S3SCSEL			   BIT(12) 		  /* 12 S3SCSEL. Selects either GPIOs or SMBus 3 segment C signals. If this bit is set, MFSEL1.DVOSEL must be 0.		   */
#define  I2CSEGSEL_S3SBSEL			   BIT(11) 		  /* 11 S3SBSEL. Selects either GPIOs or SMBus 3 segment B signals. If this bit is set, MFSEL1.DVOSEL must be 0.		   */
#define  I2CSEGSEL_S2SDSEL			   BIT(10) 		  /* 10 S2SDSEL. Selects either GPIOs or SMBus 2 segment D signals. If this bit is set, MFSEL3.14 must be 0.			   */
#define  I2CSEGSEL_S2SCSEL			   BIT(9 ) 	   /* 9 S2SCSEL. Selects either GPIOs or SMBus 2 segment C signals. If this bit is set, MFSEL1.DVOSEL must be 0.			*/
#define  I2CSEGSEL_S2SBSEL			   BIT(8 ) 	   /* 8 S2SBSEL. Selects either GPIOs or SMBus 2 segment B signals. If this bit is set, MFSEL1.DVOSEL must be 0.			*/
#define  I2CSEGSEL_S1SDSEL			   BIT(7 ) 	   /* 7 S1SDSEL. Selects either GPIOs or SMBus 1 segment D signals. If this bit is set, MFSEL3.14 must be 0.				*/
#define  I2CSEGSEL_S1SCSEL			   BIT(6 ) 	   /* 6 S1SCSEL. Selects either GPIOs or SMBus 1 segment C signals. If this bit is set, MFSEL1.DVOSEL must be 0.			*/
#define  I2CSEGSEL_S1SBSEL			   BIT(5 ) 	   /* 5 S1SBSEL. Selects either GPIOs or SMBus 1 segment B signals. If this bit is set, MFSEL1.DVOSEL must be 0.			*/
#define  I2CSEGSEL_S0DECFG			   GENMASK(4, 3 ) 	  /* 4-3 S0DECFG. Selects SMBus 0 Drive Enable controls.																   */
#define  I2CSEGSEL_S0SDSEL			   BIT(2 ) 	   /* 2 S0SDSEL. Selects either GPIOs or SMBus 0 segment D signals. If this bit is set, MFSEL1.DVOSEL must be 0.			*/
#define  I2CSEGSEL_S0SCSEL			   BIT(1 ) 	   /* 1 S0SCSEL. Selects either GPIOs or SMBus 0 segment C signals. If this bit is set, MFSEL1.DVOSEL must be 0.			*/
#define  I2CSEGSEL_S0SBSEL			   BIT(0 ) 	   /* 0 S0SBSEL. Selects either GPIOs or SMBus 0 segment B signals. If this bit is set, MFSEL1.DVOSEL must be 0.			*/

/**************************************************************************************************************************/
/*   I2C Segment Control Register (I2CSEGCTL) (New in )																   */
/**************************************************************************************************************************/
#define  I2CSEGCTL					  (GCR_BASE_ADDR + 0x0E4)		/* Offset: GCR_BA + 0E4h */

#define  I2CSEGCTL_S4D_WE_EN             GENMASK(25, 24)              /* SMB4 drive enable ( sum of the two bits below : I2CSEGCTL_S4DWE, I2CSEGCTL_S4DEN)       */
#define  I2CSEGCTL_S4DWE                 BIT(25)              /* 25 S4DWE. This bit must be written as 1 to enable writing to S4DEN bit in the same write transaction.                 */
#define  I2CSEGCTL_S4DEN                 BIT(24)              /* 24 S4DEN. SMBus 4 Drive enable software control status. This field can be written only if S4DWE bit is 1     */

#define  I2CSEGCTL_S0D_WE_EN             GENMASK(21, 20)              /* SMB0 drive enable ( sum of the two bits below : I2CSEGCTL_S0DWE, I2CSEGCTL_S0DEN)       */
#define  I2CSEGCTL_S0DWE                 BIT(21)              /* 21 S0DWE. This bit must be written as 1 to enable writing to S0DEN bit in the same write transaction.                 */
#define  I2CSEGCTL_S0DEN                 BIT(20)              /* 20 S0DEN.  SMBus 0 Drive enable software control status. This field can be written only if S0DWE bit is 1 in the same write trans */

#define  I2CSEGCTL_S4DWE				BIT(25)			  /* 25 S4DWE. This bit must be written as 1 to enable writing to S4DEN bit in the same write transaction.				 */
#define  I2CSEGCTL_S0DWE				BIT(21)			  /* 21 S0DWE. This bit must be written as 1 to enable writing to S0DEN bit in the same write transaction.				 */
#define  I2CSEGCTL_WEN5SS				BIT(17)			  /* 17 WEN5SS. This bit must be written as 1 to enable writing to SMB5SS field in the same write						  */
#define  I2CSEGCTL_WEN4SS				BIT(16)			  /* 16 WEN4SS. This bit must be written as 1 to enable writing to SMB4SS field in the same write						  */
#define  I2CSEGCTL_WEN3SS				BIT(15)			  /* 15 WEN3SS. This bit must be written as 1 to enable writing to SMB3SS field in the same write						  */
#define  I2CSEGCTL_WEN2SS				BIT(14)			  /* 14 WEN2SS. This bit must be written as 1 to enable writing to SMB2SS field in the same write						  */
#define  I2CSEGCTL_WEN1SS				BIT(13)			  /* 13 WEN1SS. This bit must be written as 1 to enable writing to SMB1SS field in the same write						  */
#define  I2CSEGCTL_WEN0SS				BIT(12)			  /* 12 WEN0SS. This bit must be written as 1 to enable writing to SMB0SS field in the same write						  */
#define  I2CSEGCTL_SMB5SS				BIT(11)			 /* SMB5SS. Selects SMB5 sub-segment operation. Signals in segments that are not selected float. If the				   */
#define  I2CSEGCTL_SMB4SS				BIT(8 )		  /* SMB4SS. Selects SMB4 sub-segment operation. Signals in segments that are not selected float. If the				   */
#define  I2CSEGCTL_SMB3SS				BIT(6 )		  /* SMB3SS. Selects SMB0 sub-segment operation. Signals in segments that are not selected float. If the				   */
#define  I2CSEGCTL_SMB2SS				BIT(4 )		  /* SMB2SS. Selects SMB2 sub-segment operation. Signals in segments that are not selected float. If the				   */
#define  I2CSEGCTL_SMB1SS				BIT(2 )		  /* SMB1SS. Selects SMB1 sub-segment operation. Signals in segments that are not selected float. If the				   */
#define  I2CSEGCTL_SMB0SS				BIT(0 )		  /* SMB0SS. Selects SMB0 sub-segment operation. Signals in segments that are not selected float. If the				   */


/**************************************************************************************************************************/
/*   Multiple Function Pin Select Register 4 (MFSEL4) (Updated in )                                                       */
/**************************************************************************************************************************/
#define  MFSEL4                         (GCR_BASE_ADDR + 0x0B0)		/* Offset: GCR_BA + 0B0h */
#define  MFSEL4_SMB11DDC                 GENMASK(30, 29)             /* 30-29 SMB11DDC. Enables SMB11 to control or emulate the DDC signals.                                                  */
#define  MFSEL4_SXCS1SEL                 BIT(28)              /* 28 SXCS1SEL (SPIX Chip Select 1 Select). Selects SPIXCS1 or GPIO signals.                                             */
#define  MFSEL4_SPXSEL                   BIT(27)              /* 27 SPXSEL (SPIX Select). Selects SPIX or GPIO signals.                                                                */
#define  MFSEL4_RG2SEL                   BIT(24)              /* 24 RG2SEL (RGMII 2 Select). Selects RGMII 2 (for GMAC2) instead of GPIO.                                              */
#define  MFSEL4_RG2MSEL                  BIT(23)              /* 23 RG2MSEL (RGMII 2 MDIO Select). Selects RGMII 2 MDIO or GPIO.                                                       */
#define  MFSEL4_RG1SEL                   BIT(22)              /* 22 RG1SEL (RGMII 1 Select). Selects RGMII 1 (for GMAC1) instead of GPIO.                                              */
#define  MFSEL4_RG1MSEL                  BIT(21)              /* 21 RG1MSEL (RGMII 1 MDIO Select). Selects RGMII1 MDIO or GPIO.                                                        */
#define  MFSEL4_SP3QSEL                  BIT(20)              /* 20 SP3QSEL (SPI3 Quad Select). Selects SPI3 quad data option. When this bit is set, MFSEL4 bits S3CS2SEL              */
#define  MFSEL4_S3CS3SEL                 BIT(19)              /* 19 S3CS3SEL (SPI3CS3 Select). Selects SPI3CS3 or GPIO signals.                                                        */
#define  MFSEL4_S3CS2SEL                 BIT(18)              /* 18 S3CS2SEL (SPI3CS2 Select). Selects SPI3CS2 or GPIO signals.                                                        */
#define  MFSEL4_S3CS1SEL                 BIT(17)              /* 17 S3CS1SEL (SPI3CS1 Select). Selects SPI3CS1 or GPIO signals.                                                        */
#define  MFSEL4_SP3SEL                   BIT(16)              /* 16 SP3SEL (SPI3 Select). Selects SPI3 or GPIO signals.                                                                */
#define  MFSEL4_SP0QSEL                  BIT(15)              /* 15 SP0QSEL (SPI0 Quad Select). Selects SPI0 quad data option. When this bit is set, MFSEL1 bits S0CS2SEL              */
#define  MFSEL4_SMB11SEL                 BIT(14)              /* 14 SMB11SEL (SMB11 Select). Selects SMB11 or GPIO option.                                                             */
#define  MFSEL4_SMB10SEL                 BIT(13)              /* 13 SMB10SEL (SMB10 Select). Selects SMB10 or GPIO option.                                                             */
#define  MFSEL4_SMB9SEL                  BIT(12)              /* 12 SMB9SEL (SMB9 Select). Selects SMB9 or GPIO option.                                                                */
#define  MFSEL4_SMB8SEL                  BIT(11)              /* 11 SMB8SEL (SMB8 Select). Selects SMB8 or GPIO option.                                                                */
#define  MFSEL4_DBGTRSEL                 BIT(10)              /* 10 DBGTRSEL (Debug Trace Select). Selects debug trace or GPIO signals.                                                */
#define  MFSEL4_CKRQSEL                  BIT(9 )              /* 9 CKRQSEL (nCKRQ Signal Select). Selects GPIO or PCIe nCLKREQ signal.                                                 */
#define  MFSEL4_ESPISEL                  BIT(8 )              /* 8 ESPISEL (eSPI Signal Select). Selects either LPC or eSPI signals. If this bit is set, MFSEL1 bit LPCSEL must        */
#define  MFSEL4_MMCRSEL                  BIT(6 )              /* 6 MMCRSEL (MMC Reset Control Select). Selects either MMC reset control or card detect signals. If this bit is         */
#define  MFSEL4_SD1PSEL                  BIT(5 )              /* 5 SD1PSEL (SDHC1 Power Control Select). Selects either SDHC1 power control or card detect signals. If this            */
#define  MFSEL4_ROSEL                    BIT(4 )              /* 4 ROSEL (RIng Oscillator Output Select). Selects either RNGOSCOUT (Ring Oscillator output) or                         */
#define  MFSEL4_ESPIPMESEL               GENMASK(3, 2 )             /* 3-2 ESPIPMESEL (ESPI PME Connection Select). Selects nSCIPME connection to PME# and SCI# virtual wires                */
#define  MFSEL4_BSPASEL                  BIT(1 )              /* 1 BSPASEL (BMC Serial Port Alternate Port Select). Selects BSP (BMC UART0) onto Serial Interface 2 pins.              */
#define  MFSEL4_JTAG2SEL                 BIT(0 )              /* 0 JTAG2SEL (Coprocessor Debug Port Select). Selects SI1/GPIOs or JTAG2.                                               */

#endif

/*-----------------------npcm7xx_i2c_mux------------------------------------------------------------------------------*/
/* Function:		CHIP_MuxSMB																			*/
/*																										 */
/* Parameters:																							 */
/*				  smb_module - SMB Module number														 */
/*																										 */
/* Returns:		 none																				   */
/* Side effects:																						   */
/* Description:																							*/
/*				  This routine selects the SMBx function to the corresponding pin.					   */
/*				  NOTE: This function does not set the segment! Use CHIP_Mux_Segment in addition	  */
/*---------------------------------------------------------------------------------------------------------*/

void npcm7xx_i2c_mux (unsigned int smb_module, int bEnable) {

	I2C_DEBUG("\tI2C%d: mux %d\n", smb_module, bEnable);

	switch (smb_module)
	{
	case 0:
		if (FIELD_GET(MFSEL1_SMB0SEL , readl(MFSEL1)) == !bEnable) {
			writel((readl(MFSEL1) & ~MFSEL1_SMB0SEL) | FIELD_PREP(MFSEL1_SMB0SEL, bEnable), MFSEL1);
		}
		writel((readl(I2CSEGSEL) & ~I2CSEGSEL_S0DECFG) | FIELD_PREP(I2CSEGSEL_S0DECFG, 0x0 ), I2CSEGSEL); // Smbus 0 Drive enabled: set it to float.), I2CSEGSEL);
		writel((readl(I2CSEGCTL) & ~I2CSEGCTL_S0D_WE_EN) | FIELD_PREP(I2CSEGCTL_S0D_WE_EN, 0x3) , I2CSEGCTL); // Smbus 0 Drive enabled: set it to float.), );
		break;

	case 1:
		if (FIELD_GET(MFSEL1_SMB0SEL , readl(MFSEL1)) == !bEnable) {
			writel((readl(MFSEL1) & ~MFSEL1_SMB1SEL) | FIELD_PREP(MFSEL1_SMB1SEL, bEnable), MFSEL1);
		}
		break;

	case 2:
		if (FIELD_GET(MFSEL1_SMB0SEL , readl(MFSEL1)) == !bEnable) {
			writel((readl(MFSEL1) & ~MFSEL1_SMB2SEL) | FIELD_PREP(MFSEL1_SMB2SEL, bEnable), MFSEL1);
		}
		break;

	case 3:
		if (FIELD_GET(MFSEL1_SMB0SEL , readl(MFSEL1)) == !bEnable) {
			writel((readl(MFSEL1) & ~MFSEL1_SMB3SEL) | FIELD_PREP(MFSEL1_SMB3SEL, bEnable), MFSEL1);
		}
		break;

	case 4:
		if (FIELD_GET(MFSEL1_SMB0SEL , readl(MFSEL1)) == !bEnable) {
			writel((readl(MFSEL1) & ~MFSEL1_SMB4SEL) | FIELD_PREP(MFSEL1_SMB4SEL, bEnable), MFSEL1);
		}
		writel((readl(I2CSEGSEL) & ~I2CSEGSEL_S4DECFG) | FIELD_PREP(I2CSEGSEL_S4DECFG, 0x0), I2CSEGSEL);  // Smbus 4 Drive enabled: set it to float.)
		writel((readl(I2CSEGCTL) & ~I2CSEGCTL_S4D_WE_EN) | FIELD_PREP(I2CSEGCTL_S4D_WE_EN, 0x3) , I2CSEGCTL); // Smbus 4 Drive enabled: set it to float.)
		break;

	case 5:
		if (FIELD_GET(MFSEL1_SMB5SEL , readl(MFSEL1)) == !bEnable) {
			writel((readl(MFSEL1) & ~MFSEL1_SMB5SEL) | FIELD_PREP(MFSEL1_SMB5SEL, bEnable), MFSEL1);
		}
		break;

	case 6:
		if (FIELD_GET(MFSEL3_SMB6SEL , readl(MFSEL3)) == !bEnable) {
			writel((readl(MFSEL3) & ~MFSEL3_SMB6SEL) | FIELD_PREP(MFSEL3_SMB6SEL, bEnable), MFSEL3);
		}
		break;

	case 7:
		if (FIELD_GET(MFSEL3_SMB7SEL , readl(MFSEL3)) == !bEnable) {
			writel((readl(MFSEL3) & ~MFSEL3_SMB7SEL) | FIELD_PREP(MFSEL3_SMB7SEL, bEnable), MFSEL3);
		}
		break;

	case 8:
		if (FIELD_GET(MFSEL4_SMB8SEL , readl(MFSEL4)) == !bEnable) {
			writel((readl(MFSEL4) & ~MFSEL4_SMB8SEL) | FIELD_PREP(MFSEL4_SMB8SEL, bEnable), MFSEL4);
		}
		break;

	case 9:
		if (FIELD_GET(MFSEL4_SMB9SEL , readl(MFSEL4)) == !bEnable) {
			writel((readl(MFSEL4) & ~MFSEL4_SMB9SEL) | FIELD_PREP(MFSEL4_SMB9SEL, bEnable), MFSEL4);
		}
		break;

	case 10:
		if (FIELD_GET(MFSEL4_SMB10SEL , readl(MFSEL4)) == !bEnable) {
			writel((readl(MFSEL4) & ~MFSEL4_SMB10SEL) | FIELD_PREP(MFSEL4_SMB10SEL, bEnable), MFSEL4);
		}
		break;

	case 11:
		if (FIELD_GET(MFSEL4_SMB11SEL , readl(MFSEL4)) == !bEnable) {
						writel((readl(MFSEL4) & ~MFSEL4_SMB11SEL) | FIELD_PREP(MFSEL4_SMB11SEL, bEnable), MFSEL4);
	   	}

	case 12:
		if (FIELD_GET(MFSEL3_SMB12SEL , readl(MFSEL3)) == !bEnable) {
			writel((readl(MFSEL3) & ~MFSEL3_SMB12SEL) | FIELD_PREP(MFSEL3_SMB12SEL, bEnable), MFSEL3);
		}
		break;

	case 13:
		if (FIELD_GET(MFSEL3_SMB13SEL , readl(MFSEL3)) == !bEnable) {
			writel((readl(MFSEL3) & ~MFSEL3_SMB13SEL) | FIELD_PREP(MFSEL3_SMB13SEL, bEnable), MFSEL3);
		}
		break;

	case 14:
		if (FIELD_GET(MFSEL3_SMB14SEL , readl(MFSEL3)) == !bEnable) {
			writel((readl(MFSEL3) & ~MFSEL3_SMB14SEL) | FIELD_PREP(MFSEL3_SMB14SEL, bEnable), MFSEL3);
		}
		break;

	case 15:
		if (FIELD_GET(MFSEL3_SMB15SEL , readl(MFSEL3)) == !bEnable) {
			writel((readl(MFSEL3) & ~MFSEL3_SMB15SEL) | FIELD_PREP(MFSEL3_SMB15SEL, bEnable), MFSEL3);
		}
		break;

	default:

		break;
	}

	//CHIP_SMBPullUp(smb_module, bEnable);
}





/* INTERFACE FUNCTIONS */

static inline void nuvoton_smb_write_byte(nuvoton_i2c_bus_t *bus, u8 data)
{
	I2C_DEBUG("\t\tSDA master bus%d wr 0x%x\n", bus->module__num, data);

	writeb(data, NUVOTON_SMBSDA(bus));
	nuvoton_smb_calc_PEC(bus, data);
#ifdef SMB_STALL_TIMEOUT_SUPPORT
	bus->stall_counter = 0;
#endif
}

static inline bool nuvoton_smb_read_byte(nuvoton_i2c_bus_t *bus, u8 *data)
{
	*data = readb(NUVOTON_SMBSDA(bus));
	I2C_DEBUG("\t\tSDA master bus%d rd 0x%x\n", bus->module__num, *data);
	nuvoton_smb_calc_PEC(bus, *data);
#ifdef SMB_STALL_TIMEOUT_SUPPORT
	bus->stall_counter = 0;
#endif
	return true;
}

static inline void nuvoton_smb_select_bank(nuvoton_i2c_bus_t *bus, SMB_BANK_T bank)
{
	if (bus->fifo_use == true)
		writeb((readb(NUVOTON_SMBCTL3(bus)) & ~SMBCTL3_BNK_SEL) | FIELD_PREP(SMBCTL3_BNK_SEL, bank), NUVOTON_SMBCTL3(bus));
}

static inline u16 nuvoton_smb_get_index(nuvoton_i2c_bus_t *bus)
{
	u16 index = 0;

	if (bus->operation == SMB_READ_OPER)
		index = bus->read_index;
	else
		if (bus->operation == SMB_WRITE_OPER)
			index = bus->write_index;

	return index;
}



static inline void nuvoton_smb_master_start(nuvoton_i2c_bus_t *bus)
{
	NPCM7XX_I2C_EVENT_LOG(NPCM7XX_I2C_EVENT_START);
	writeb(readb(NUVOTON_SMBCTL1(bus)) | NUVOTON_SMBCTL1_START, NUVOTON_SMBCTL1(bus));
#ifdef SMB_STALL_TIMEOUT_SUPPORT
	bus->stall_counter = 0;
#endif
}

static inline void nuvoton_smb_master_stop(nuvoton_i2c_bus_t *bus)
{
	NPCM7XX_I2C_EVENT_LOG(NPCM7XX_I2C_EVENT_STOP);
#ifdef SMB_SW_BYPASS_HW_ISSUE_SMB_STOP
	// override HW issue: SMBus may fail to supply stop condition in Master Write operation.
	// Need to delay at least 5 us from the last interrupt, before issueing a stop
	CLK_Delay_Since(5, bus->interrupt_time_stamp);

#endif //   SMB_SW_BYPASS_HW_ISSUE_SMB_STOP

	writeb(readb(NUVOTON_SMBCTL1(bus)) | NUVOTON_SMBCTL1_STOP, NUVOTON_SMBCTL1(bus));

	if (bus->fifo_use) {
		u8 smbfif_cts;
		writeb(readb(NUVOTON_SMBRXF_STS(bus)) | NUVOTON_SMBRXF_STS_RX_THST, NUVOTON_SMBRXF_STS(bus));
		smbfif_cts = readb(NUVOTON_SMBFIF_CTS(bus));
		smbfif_cts = smbfif_cts | NUVOTON_SMBFIF_CTS_SLVRSTR;
		smbfif_cts = smbfif_cts | NUVOTON_SMBFIF_CTS_RXF_TXE;
		writeb(smbfif_cts, NUVOTON_SMBFIF_CTS(bus));
		writeb(readb(NUVOTON_SMBFIF_CTS(bus)) | NUVOTON_SMBFIF_CTS_SLVRSTR |NUVOTON_SMBFIF_CTS_RXF_TXE, NUVOTON_SMBFIF_CTS(bus));

		writeb(0, NUVOTON_SMBTXF_CTL(bus));
	}

#ifdef SMB_STALL_TIMEOUT_SUPPORT
	bus->stall_counter = 0;
#endif

}

static inline void nuvoton_smb_abort_data(nuvoton_i2c_bus_t *bus)
{
	volatile unsigned int timeout = ABORT_TIMEOUT;

	NPCM7XX_I2C_EVENT_LOG(NPCM7XX_I2C_EVENT_ABORT);

	/* Generate a STOP condition*/
	nuvoton_smb_master_stop(bus);

	/* Clear NEGACK, STASTR and BER bits  */
	writeb(NUVOTON_SMBST_STASTR|NUVOTON_SMBST_NEGACK|NUVOTON_SMBST_BER, NUVOTON_SMBST(bus));

	/* Wait till STOP condition is generated */
	while (--timeout)
		if (!FIELD_GET(NUVOTON_SMBCTL1_STOP, readb(NUVOTON_SMBCTL1(bus))))
			break;

	if ( timeout <= 1)
		printf("nuvoton_smb_abort_data: abort timeout!\n");

	/* Clear BB (BUS BUSY) bit  */
	//writeb(NUVOTON_SMBCST_BB, NUVOTON_SMBCST(bus));
}

static inline void nuvoton_smb_stall_after_start(nuvoton_i2c_bus_t *bus, bool stall)
{
	I2C_DEBUG("\t\tSDA stall bus%d\n", bus->module__num);
	writeb((readb(NUVOTON_SMBCTL1(bus)) & ~NUVOTON_SMBCTL1_STASTRE) | FIELD_PREP(NUVOTON_SMBCTL1_STASTRE, stall), NUVOTON_SMBCTL1(bus));
}

static inline void nuvoton_smb_nack(nuvoton_i2c_bus_t *bus)
{
	writeb(readb(NUVOTON_SMBCTL1(bus)) | NUVOTON_SMBCTL1_ACK, NUVOTON_SMBCTL1(bus));
}


static void nuvoton_smb_disable(nuvoton_i2c_bus_t *bus)
{
	int i;

	/* Slave Addresses Removal  */
	for (i = SMB_SLAVE_ADDR1; i < SMB_NUM_OF_ADDR; i++)
		writeb(0, NUVOTON_SMBADDR(bus, i));

	/* Disable module. */
	writeb((readb(NUVOTON_SMBCTL2(bus)) & ~SMBCTL2_ENABLE) | FIELD_PREP(SMBCTL2_ENABLE, DISABLE), NUVOTON_SMBCTL2(bus));


	/* Set module disable */
	bus->state = SMB_DISABLE;
}


static bool nuvoton_smb_enable(nuvoton_i2c_bus_t *bus)
{
	writeb((readb(NUVOTON_SMBCTL2(bus)) & ~SMBCTL2_ENABLE) | FIELD_PREP(SMBCTL2_ENABLE, ENABLE), NUVOTON_SMBCTL2(bus));
	return true;
}

static bool nuvoton_smb_init_module(nuvoton_i2c_bus_t *bus, SMB_MODE_T mode,
			u16 bus_freq)
{
	int	i;


	if ((bus_freq < SMBUS_FREQ_MIN) || (bus_freq > SMBUS_FREQ_MAX)){

		I2C_DEBUG("I2C%d, unspuported frequency. Change to 100KHz.\n", bus->module__num);
		bus_freq = 100;
	}
	/* Check whether module already enabled or frequency is out of bounds*/
	if (((bus->state != SMB_DISABLE) &&
	(bus->state != SMB_IDLE)) ||
	(bus_freq < SMBUS_FREQ_MIN) || (bus_freq > SMBUS_FREQ_MAX))
		return false;

	/* Mux SMB module pins*/
	//lint -e{792} suppress PC-Lint warning on 'void cast of void expression'
#if 1
	npcm7xx_i2c_mux((unsigned int)bus->module__num, 1);
#endif

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure FIFO disabled mode so slave will not use fifo (maste will set it on if supported) */
	/*-----------------------------------------------------------------------------------------------------*/
	bus->threshold_fifo = SMBUS_FIFO_SIZE;
	writeb(readb(NUVOTON_SMBFIF_CTL(bus)) & ~NUVOTON_SMBFIF_CTL_FIFO_EN, NUVOTON_SMBFIF_CTL(bus));

	bus->fifo_use = false;

	/* Configure SMB module clock frequency  */
	if (!nuvoton_smb_init_clk(bus, mode, bus_freq)) {
		printf("nuvoton_smb_init_clk failed\n");
		return false;
	}

	nuvoton_smb_select_bank(bus, SMB_BANK_0); // select bank 0 for SMB addresses

	/* Configure slave addresses (by default they are disabled) */
	for (i = 0; i < SMB_NUM_OF_ADDR; i++)
		writeb(0, NUVOTON_SMBADDR(bus, i));

	nuvoton_smb_select_bank(bus, SMB_BANK_1); // by default most access is in bank 1

	/* Enable module - before configuring CTL1 !*/
	if (!nuvoton_smb_enable(bus))
		return false;
	else
		bus->state = SMB_IDLE;

	/* Enable SMB interrupt and New Address Match interrupt source */
	writeb((readb(NUVOTON_SMBCTL1(bus)) & ~NUVOTON_SMBCTL1_NMINTE) | FIELD_PREP(NUVOTON_SMBCTL1_NMINTE, ENABLE), NUVOTON_SMBCTL1(bus));
	nuvoton_smb_int_enable(bus, true);

	return true;
}


static bool nuvoton_smb_master_start_xmit(nuvoton_i2c_bus_t *bus, u8 slave_addr,
				u16 nwrite, u16 nread, u8 *write_data,
				u8 *read_data, bool use_PEC)
{
	unsigned long lock_flags;

#ifdef CONFIG_NPCM750_I2C_DEBUG
	I2C_DEBUG("bus%d slave_addr=%x nwrite=%d nread=%d write_data=%p read_data=%p use_PEC=%d\n",
		bus->module__num, slave_addr, nwrite, nread, write_data, read_data, use_PEC);
	if (nwrite && nwrite != SMB_BYTES_QUICK_PROT) {
		int i;
		char str[32*3+4];
		char *s = str;
		for (i=0; (i<nwrite && i<32); i++)
			s += sprintf(s, "%02x ", write_data[i]);
		I2C_DEBUG("\t\t\twrite_data = %s\n", str);
	}
#endif

	/*-----------------------------------------------------------------------------------------------------*/
	/* Allow only if bus is not busy						 */
	/*-----------------------------------------------------------------------------------------------------*/
	if ((bus->state != SMB_IDLE)
#if defined SMBUS_SIZE_CHECK
		||
		((nwrite >= _32KB_) && (nwrite != SMB_BYTES_QUICK_PROT)) ||
		((nread >= _32KB_) && (nread != SMB_BYTES_BLOCK_PROT) &&
		(nread != SMB_BYTES_QUICK_PROT))
#endif
		)	{
		I2C_DEBUG("\tbus%d->state != SMB_IDLE\n", bus->module__num);
		return false;
	}

	spin_lock_irqsave(&bus->lock, lock_flags);

	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure FIFO mode only for master mode Slave mode for linux will not use fifo */
	/*-----------------------------------------------------------------------------------------------------*/
	//lint -e{774, 506} suppress PC-Lint warning on 'Boolean within 'left side of && within if' always evaluates to True'
	if (SMB_FIFO(bus) && FIELD_GET(SMB_VER_FIFO_EN, readb( SMB_VER(bus)))){
		bus->fifo_use = true;
		writeb(readb(NUVOTON_SMBFIF_CTL(bus)) | NUVOTON_SMBFIF_CTL_FIFO_EN, NUVOTON_SMBFIF_CTL(bus));
	}
	else
	{
		bus->fifo_use = false;
	}

	/* Update driver state*/
	bus->master_or_slave = SMB_MASTER;
	bus->state = SMB_MASTER_START;
	if ((nwrite > 0) || (nwrite != SMB_BYTES_QUICK_PROT))
		bus->operation = SMB_WRITE_OPER;
	else
		bus->operation = SMB_READ_OPER;

	bus->dest_addr	= (u8)(slave_addr << 1);  /* Translate 7-bit to 8-bit format  */
	bus->write_data_buf = write_data;
	bus->write_size	= nwrite;
	bus->write_index	= 0;
	bus->read_data_buf  = read_data;
	bus->read_size	= nread;
	bus->read_index	= 0;
	bus->PEC_use		= use_PEC;
	bus->read_block_use = false;
	bus->retry_count	= SMB_RETRY_MAX_COUNT;

	/* Check if transaction uses Block read protocol  */
	if ((bus->read_size == SMB_BYTES_BLOCK_PROT) ||
		(bus->read_size == SMB_BYTES_EXCLUDE_BLOCK_SIZE_FROM_BUFFER)) {
		bus->read_block_use = true;

		/* Change nread in order to configure recieve threshold to 1  */
		nread = 1;
	}

	/* clear BER just in case it is set due to a previous transaction */
	writeb(NUVOTON_SMBST_BER, NUVOTON_SMBST(bus));

	/* Initiate SMBus master transaction  */
	/* Generate a Start condition on the SMBus  */
	if (bus->fifo_use == true) {
		/* select bank 1 for FIFO registers  */
		nuvoton_smb_select_bank(bus, SMB_BANK_1);

		/* clear FIFO and relevant status bits. */
		writeb(readb(NUVOTON_SMBFIF_CTS(bus)) | NUVOTON_SMBFIF_CTS_SLVRSTR |NUVOTON_SMBFIF_CTS_CLR_FIFO |NUVOTON_SMBFIF_CTS_RXF_TXE, NUVOTON_SMBFIF_CTS(bus));

		if (bus->operation == SMB_READ_OPER) {
			/* This is a read only operation. Configure the FIFO*/
			/* threshold according to the needed number of bytes to read.*/
			if (nread > SMBUS_FIFO_SIZE)
				writeb((readb(NUVOTON_SMBRXF_CTL(bus)) & ~NUVOTON_SMBRXF_CTL_RX_THR) | FIELD_PREP(NUVOTON_SMBRXF_CTL_RX_THR, SMBUS_FIFO_SIZE), NUVOTON_SMBRXF_CTL(bus));
			else {
				writeb((readb(NUVOTON_SMBRXF_CTL(bus)) & ~NUVOTON_SMBRXF_CTL_RX_THR) | FIELD_PREP(NUVOTON_SMBRXF_CTL_RX_THR, (u8)(nread)), NUVOTON_SMBRXF_CTL(bus));

				if ((bus->read_size != SMB_BYTES_BLOCK_PROT) &&
				(bus->read_size != SMB_BYTES_EXCLUDE_BLOCK_SIZE_FROM_BUFFER))
					writeb(readb(NUVOTON_SMBRXF_CTL(bus)) | NUVOTON_SMBRXF_CTL_LAST_PEC, NUVOTON_SMBRXF_CTL(bus));
			}
		}
	}

	bus->int_cnt = 100;
	bus->event_log = 0;

	nuvoton_smb_master_start(bus);

	spin_unlock_irqrestore(&bus->lock, lock_flags);

	return true;
}

#ifdef TBD
static bool nuvoton_smb_module_is_busy(nuvoton_i2c_bus_t *bus)
{
	return (FIELD_GET(NUVOTON_SMBCST_BUSY, readb( NUVOTON_SMBCST(bus) ||
		  FIELD_GET(NUVOTON_SMBST_SLVSTP, readb( NUVOTON_SMBCST(bus));
}

static bool nuvoton_smb_bus_is_busy(nuvoton_i2c_bus_t *bus)
{
	return FIELD_GET(NUVOTON_SMBCST_BB, readb( NUVOTON_SMBCST(bus));
}
#endif //TBD


static void nuvoton_smb_read_from_fifo(nuvoton_i2c_bus_t *bus, u8 bytes_in_fifo)
{
	while (bytes_in_fifo--) {
		/* Keep read data */
		u8 data = readb(NUVOTON_SMBSDA(bus));

		nuvoton_smb_calc_PEC(bus, data);
		if (bus->read_index < bus->read_size) {
			bus->read_data_buf[bus->read_index++] = data;
			if ((bus->read_index == 1) && bus->read_size == SMB_BYTES_BLOCK_PROT)
				/* First byte indicates length in block protocol*/
				bus->read_size = data;
		}
	}
}

static void nuvoton_smb_master_fifo_read(nuvoton_i2c_bus_t *bus)
{
	u16 rcount;
	u8 fifo_bytes;
	SMB_STATE_IND_T ind = SMB_MASTER_DONE_IND;

	rcount = bus->read_size - bus->read_index;


	/* In order not to change the RX_TRH during transaction (we found that this might */
	/* be problematic if it takes too much time to read the FIFO) we read the data in the*/
	/* following way. If the number of bytes to read == FIFO Size + C (where C < FIFO Size) */
	/* then first read C bytes and in the next interrupt we read rest of the data.*/
	if ((rcount < (2 * SMBUS_FIFO_SIZE)) && (rcount > SMBUS_FIFO_SIZE))
		fifo_bytes = (u8)(rcount - SMBUS_FIFO_SIZE);
	else
		fifo_bytes = FIELD_GET(NUVOTON_SMBRXF_STS_RX_BYTES, readb( NUVOTON_SMBRXF_STS(bus)));

	if (rcount - fifo_bytes == 0) {
		/* last byte is about to be read - end of transaction.  */
		/* Stop should be set before reading last byte.  */
#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
		/* Enable "End of Busy" interrupt.*/
		writeb(readb(NUVOTON_SMBCTL1(bus)) | NUVOTON_SMBCTL1_EOBINTE, NUVOTON_SMBCTL1(bus));
#endif
		nuvoton_smb_master_stop(bus);

		nuvoton_smb_read_from_fifo(bus, fifo_bytes);

		if (nuvoton_smb_get_PEC(bus) != 0)
			ind = SMB_MASTER_PEC_ERR_IND;

#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
		bus->state = SMB_STOP_PENDING;
		bus->stop_indication = ind;
#else
		/* Reset state for new transaction*/
		bus->state = SMB_IDLE;

		/* Notify upper layer of transaction completion  */
		nuvoton_smb_callback(bus, ind, bus->read_index);
#endif
	} else {
		nuvoton_smb_read_from_fifo(bus, fifo_bytes);
		rcount = bus->read_size - bus->read_index;

		if (rcount > 0) {
			writeb(readb(NUVOTON_SMBRXF_STS(bus)) |
			NUVOTON_SMBRXF_STS_RX_THST | NUVOTON_SMBFIF_CTS_RXF_TXE,
			NUVOTON_SMBRXF_STS(bus));


			if (rcount > SMBUS_FIFO_SIZE)
				writeb((readb(NUVOTON_SMBRXF_CTL(bus)) & ~NUVOTON_SMBRXF_CTL_RX_THR) | FIELD_PREP(NUVOTON_SMBRXF_CTL_RX_THR, SMBUS_FIFO_SIZE), NUVOTON_SMBRXF_CTL(bus));
			else {
				writeb((readb(NUVOTON_SMBRXF_CTL(bus)) & ~NUVOTON_SMBRXF_CTL_RX_THR) | FIELD_PREP(NUVOTON_SMBRXF_CTL_RX_THR, (u8)(rcount)), NUVOTON_SMBRXF_CTL(bus));
				writeb(readb(NUVOTON_SMBRXF_CTL(bus)) | NUVOTON_SMBRXF_CTL_LAST_PEC, NUVOTON_SMBRXF_CTL(bus));
			}
		}
	}

#ifdef SMB_STALL_TIMEOUT_SUPPORT
	bus->stall_counter = 0;
#endif
}


static void nuvoton_smb_write_to_fifo(nuvoton_i2c_bus_t *bus, u16 max_bytes_to_send)
{
	I2C_DEBUG("\t\t\tSDA master bus%d fifo wr %d bytes\n", bus->module__num, max_bytes_to_send);

	/* Fill the FIFO , while the FIFO is not full and there are more bytes to write*/
	while ((max_bytes_to_send--) && (SMBUS_FIFO_SIZE -
					FIELD_GET(NUVOTON_SMBTXF_STS_TX_BYTES , readb(NUVOTON_SMBTXF_STS(bus))))) {
		/* write the data */
		if (bus->write_index < bus->write_size) {
			if ((bus->PEC_use == true) &&
			((bus->write_index + 1) == bus->write_size) &&
			((bus->read_size == 0) ||
			(bus->master_or_slave == SMB_SLAVE))) {
				/* Master send PEC in write protocol, Slave send PEC in read protocol.*/
				nuvoton_smb_write_PEC(bus);
				bus->write_index++;
			} else
				nuvoton_smb_write_byte(bus, bus->write_data_buf[bus->write_index++]);
		} else {

/* define this at hal_cfg or chip file, if one wishes to use this feature. Otherwise driver will xmit 0xFF */
#ifdef SMB_WRAP_AROUND_BUFFER
			/* We're out of bytes. Ask the higher level for more bytes. Let it know that driver used all its' bytes */

			/* clear the status bits*/
			writeb(readb(NUVOTON_SMBTXF_STS(bus)) | NUVOTON_SMBTXF_STS_TX_THST, NUVOTON_SMBTXF_STS(bus));

			/* Reset state for the remaining bytes transaction  */
			bus->state = SMB_SLAVE_MATCH;

			/* Notify upper layer of transaction completion  */
			nuvoton_smb_callback(bus, SMB_SLAVE_XMIT_MISSING_DATA_IND,
				bus->write_index);

			writeb(NUVOTON_SMBST_SDAST, NUVOTON_SMBST(bus));
#else
			nuvoton_smb_write_byte(bus, 0xFF);
#endif
		}
	}
}


static bool nuvoton_smb_init_clk(nuvoton_i2c_bus_t *bus, SMB_MODE_T mode, u16 bus_freq)
{
	u16  k1		= 0;
	u16  k2		= 0;
	u8   dbnct	 = 0;
	u16  sclfrq	= 0;
	u8   hldt		= 7;
	bool fastMode	= false;
	u32  source_clock_freq;

	source_clock_freq = bus->apb_clk;


	/* Frequency is less or equal to 100 KHz */
	if (bus_freq <= SMBUS_FREQ_100KHz) {
		/* Set frequency: */
		/* SCLFRQ = T(SCL)/4/T(CLK) = FREQ(CLK)/4/FREQ(SCL) = FREQ(CLK) / ( FREQ(SCL)*4 )*/
		sclfrq = (u16)((source_clock_freq / ((u32)bus_freq * _1KHz_ * 4)));   // bus_freq is KHz

		/* Check whether requested frequency can be achieved in current CLK */
		if ((sclfrq < SCLFRQ_MIN) || (sclfrq > SCLFRQ_MAX))
			return false;

		if (source_clock_freq >= 40000000)
			hldt = 17;
		else if (source_clock_freq >= 12500000)
			hldt = 15;
		else
			hldt = 7;
	}

	/* Frequency equal to 400 KHz*/
	else if (bus_freq == SMBUS_FREQ_400KHz) {
		sclfrq	 = 0;
		fastMode	= true;

		if ((mode == SMB_MASTER && source_clock_freq < 7500000) ||
		(mode == SMB_SLAVE  && source_clock_freq < 10000000))
			/* 400KHz cannot be supported for master core clock < 7.5 MHz or slave core clock < 10 MHz  */
			return false;

		/* Master or Slave with frequency > 25 MHz */
		if (mode == SMB_MASTER || source_clock_freq > 25000000) {
			/* Set HLDT:  */
			/* SDA hold time:  (HLDT-7) * T(CLK) >= 300  */
			/* HLDT = 300/T(CLK) + 7 = 300 * FREQ(CLK) + 7*/
			hldt = (u8)DIV_CEILING((300 * (source_clock_freq / _1KHz_)), ((u32)_1MHz_)) + 7;

			if (mode == SMB_MASTER) {
				/* Set k1:*/
				/* Clock low time: k1 * T(CLK) - T(SMBFO) >= 1300  */
				/* T(SMBRO) = T(SMBFO) = 300 */
				/* k1 = (1300 + T(SMBFO)) / T(CLK) = 1600 * FREQ(CLK) */
				k1 = ROUND_UP(((u16)DIV_CEILING((1600 * (source_clock_freq / _1KHz_)), ((u32)_1MHz_))), 2);

				/* Set k2:*/
				/* START setup: (k2 - 1) * T(CLK) - T(SMBFO) >= 600*/
				/* T(SMBRO) = T(SMBFO) = 300 */
				/* k2 = (600 + T(SMBFO)) / T(CLK) + 1 = 900 * FREQ(CLK) + 1 */
				k2 = ROUND_UP(((u16)DIV_CEILING((900 * (source_clock_freq / _1KHz_)), ((u32)_1MHz_)) + 1), 2);

				/* Check whether requested frequency can be achieved in current CLK*/
				if ((k1 < SCLFRQ_MIN) || (k1 > SCLFRQ_MAX) || (k2 < SCLFRQ_MIN) || (k2 > SCLFRQ_MAX))
					return false;
			}
		}

		/* Slave with frequency 10-25 MHz */
		else {
			hldt  = 7;
			dbnct = 2;
		}
	}

	/* Frequency equal to 1 MHz */
	else if (bus_freq == SMBUS_FREQ_1MHz) {
		sclfrq	 = 0;
		fastMode	= true;

		if ((mode == SMB_MASTER && source_clock_freq < 15000000) ||
		(mode == SMB_SLAVE  && source_clock_freq < 24000000))

			/* 1MHz cannot be supported for master core clock < 15 MHz or slave core clock < 24 MHz  */
			return false;

		/* Master or Slave with frequency > 40 MHz */
		if (mode == SMB_MASTER || source_clock_freq > 40000000) {

			/* Set HLDT:  */
			/* SDA hold time:  (HLDT-7) * T(CLK) >= 120  */
			/* HLDT = 120/T(CLK) + 7 = 120 * FREQ(CLK) + 7*/
			hldt = (u8)DIV_CEILING((120 * (source_clock_freq / _1KHz_)), ((u32)_1MHz_)) + 7;

			if (mode == SMB_MASTER) {

				/* Set k1:*/
				/* Clock low time: k1 * T(CLK) - T(SMBFO) >= 500*/
				/* T(SMBRO) = T(SMBFO) = 120 */
				/* k1 = (500 + T(SMBFO)) / T(CLK) = 620 * FREQ(CLK)*/
				k1 = ROUND_UP(((u16)DIV_CEILING((620 * (source_clock_freq / _1KHz_)), ((u32)_1MHz_))), 2);


				/* Set k2:*/
				/* START setup: (k2 - 1) * T(CLK) - T(SMBFO) >= 260*/
				/* T(SMBRO) = T(SMBFO) = 120 */
				/* k2 = (260 + T(SMBFO)) / T(CLK) + 1 = 380 * FREQ(CLK) + 1 */
				k2 = ROUND_UP(((u16)DIV_CEILING((380 * (source_clock_freq / _1KHz_)), ((u32)_1MHz_)) + 1), 2);


				/* Check whether requested frequency can be achieved in current CLK*/
				if ((k1 < SCLFRQ_MIN) || (k1 > SCLFRQ_MAX) || (k2 < SCLFRQ_MIN) || (k2 > SCLFRQ_MAX)) {
					return false;
				}
			}
		}

		/* Slave with frequency 24-40 MHz */
		else {
			hldt  = 7;
			dbnct = 2;
		}
	}

	/* Frequency larger than 1 MHz  */
	else
		return false;



	/* After clock parameters calculation update the register*/
	writeb((readb(NUVOTON_SMBCTL2(bus)) & ~SMBCTL2_SCLFRQ6_0) | FIELD_PREP(SMBCTL2_SCLFRQ6_0, sclfrq & 0x7F ), NUVOTON_SMBCTL2(bus));

	writeb((readb(NUVOTON_SMBCTL3(bus)) & ~SMBCTL3_SCLFRQ8_7) | FIELD_PREP(SMBCTL3_SCLFRQ8_7, (sclfrq >> 7) & 0x3), NUVOTON_SMBCTL3(bus));


	writeb((readb(NUVOTON_SMBCTL3(bus)) & ~SMBCTL3_400K_MODE) | FIELD_PREP(SMBCTL3_400K_MODE, fastMode), NUVOTON_SMBCTL3(bus));


	/* Select Bank 0 to access NUVOTON_SMBCTL4/NUVOTON_SMBCTL5  */
	nuvoton_smb_select_bank(bus, SMB_BANK_0);

	if (bus_freq >= SMBUS_FREQ_400KHz) {

		/* k1 and k2 are relevant for master mode only*/
		if (mode == SMB_MASTER) {

			/* Set SCL Low/High Time:  */
			/* k1 = 2 * SCLLT7-0 -> Low Time  = k1 / 2*/
			/* k2 = 2 * SCLLT7-0 -> High Time = k2 / 2*/
			writeb((u8)k1 / 2, NUVOTON_SMBSCLLT(bus));
			writeb((u8)k2 / 2, NUVOTON_SMBSCLHT(bus));
		}

		/* DBNCT is relevant for slave mode only*/
		else
			writeb((readb(NUVOTON_SMBCTL5(bus)) & ~SMBCTL5_DBNCT) | FIELD_PREP(SMBCTL5_DBNCT, dbnct), NUVOTON_SMBCTL5(bus));
	}

	writeb((readb(NUVOTON_SMBCTL4(bus)) & ~SMBCTL4_HLDT) | FIELD_PREP(SMBCTL4_HLDT, hldt), NUVOTON_SMBCTL4(bus));

	I2C_DEBUG("I2C%d sclfrq = %d, hldt = %d, dbnct = %d, bus_freq = %d, SMBCTL2= 0x%x, SMBCTL3= 0x%x, SMBCTL4= 0x%x, SMBCTL5= 0x%x\n",
				bus->module__num, sclfrq, hldt, dbnct, bus_freq, readb(NUVOTON_SMBCTL2(bus)),
				readb(NUVOTON_SMBCTL3(bus)),  readb(NUVOTON_SMBCTL4(bus)),
				readb(NUVOTON_SMBCTL5(bus)));


	/* Return to Bank 1*/
	nuvoton_smb_select_bank(bus, SMB_BANK_1);

	return true;
}


#if defined (SMB_CAPABILITY_TIMEOUT_SUPPORT)
static void nuvoton_smb_enable_timeout(nuvoton_i2c_bus_t *bus, bool enable)
{
	u8 toCkDiv;
	u8 smbEnabled;
	u8 smbctl1 = 0;


	I2C_DEBUG("\t\t\tSDA master bus%d enable TO %d\n", bus->module__num, enable);

	if (enable) {

		/* TO_CKDIV may be changed only when the SMB is disabled*/
		smbEnabled = FIELD_GET(SMBCTL2_ENABLE, readb( NUVOTON_SMBCTL2(bus)));

		/* If SMB is enabled - disable the SMB module */
		if (smbEnabled) {

			/* Save NUVOTON_SMBCTL1 relevant bits. It is being cleared when the module is disabled  */
			smbctl1 = readb(NUVOTON_SMBCTL1(bus)) & (NUVOTON_SMBCTL1_GCMEN | NUVOTON_SMBCTL1_INTEN | NUVOTON_SMBCTL1_NMINTE);

			/* Disable the SMB module  */
			writeb((readb(NUVOTON_SMBCTL2(bus)) & ~SMBCTL2_ENABLE) | FIELD_PREP(SMBCTL2_ENABLE, DISABLE), NUVOTON_SMBCTL2(bus));
		}

		/* Clear EO_BUSY pending bit*/
		writeb(readb(NUVOTON_SMBT_OUT(bus)) | NUVOTON_SMBT_OUT_T_OUTST, NUVOTON_SMBT_OUT(bus));

		/* Configure the division of the SMB Module Basic clock (BCLK) to generate the 1 KHz clock of the  */
		/* timeout detector. */
		/* The timeout detector has an “n+1” divider, controlled by TO_CKDIV and a fixed divider by 1000.  */
		/* Together they generate the 1 ms clock cycle*/
		toCkDiv = (u8)(((bus->apb_clk / _1KHz_) / 1000) - 1);

		/* Set the bus timeout clock divisor */
		writeb((readb(NUVOTON_SMBT_OUT(bus)) & ~NUVOTON_SMBT_OUT_TO_CKDIV) | FIELD_PREP(NUVOTON_SMBT_OUT_TO_CKDIV, toCkDiv), NUVOTON_SMBT_OUT(bus));

		/* If SMB was enabled - re-enable the SMB module */
		if (smbEnabled) {

			/* Enable the SMB module*/
			(void)nuvoton_smb_enable(bus);

			/* Restore NUVOTON_SMBCTL1 status  */
			writeb(smbctl1, NUVOTON_SMBCTL1(bus));
		}
	}


	/* Enable/Disable the bus timeout interrupt */
	writeb((readb(NUVOTON_SMBT_OUT(bus)) & ~NUVOTON_SMBT_OUT_T_OUTIE) | FIELD_PREP(NUVOTON_SMBT_OUT_T_OUTIE, enable), NUVOTON_SMBT_OUT(bus));
}
#endif

static void nuvoton_smb_int_master_handler(nuvoton_i2c_bus_t *bus)
{

	/* A negative acknowledge has occurred*/
	if (FIELD_GET(NUVOTON_SMBST_NEGACK , readb(NUVOTON_SMBST(bus)))) {
		I2C_DEBUG("\tNACK bus = %d\n", bus->module__num);
		NPCM7XX_I2C_EVENT_LOG(NPCM7XX_I2C_EVENT_NACK);
		if (bus->fifo_use) {
			/* if there are still untransmitted bytes in TX FIFO reduce them from write_index  */
			bus->write_index -= FIELD_GET(NUVOTON_SMBTXF_STS_TX_BYTES, readb( NUVOTON_SMBTXF_STS(bus)));

			I2C_DEBUG("\tNACK bus%d fifo, write_index = %d\n", bus->module__num, bus->write_index);

			/* clear the FIFO*/
			writeb(NUVOTON_SMBFIF_CTS_CLR_FIFO, NUVOTON_SMBFIF_CTS(bus));
		}

		/* In master write operation, NACK is a problem  */

		/* number of bytes sent to master less than required */
		/* notify upper layer. */
		nuvoton_smb_master_abort(bus);

		// writeb(NUVOTON_SMBST_NEGACK, NUVOTON_SMBST(bus));
		bus->state = SMB_IDLE;
		nuvoton_smb_callback(bus, SMB_NACK_IND, bus->write_index);

		/* In Master mode, NEGACK should be cleared only after generating STOP.*/
		/* In such case, the bus is released from stall only after the software clears NEGACK  */
		/* bit. Then a Stop condition is sent.  */
		writeb(NUVOTON_SMBST_NEGACK, NUVOTON_SMBST(bus));

		return;
	}


	/* Master mode: a Bus Error has been identified */
	if (FIELD_GET(NUVOTON_SMBST_BER , readb(NUVOTON_SMBST(bus)))) {
		/* Check whether bus arbitration or Start or Stop during data transfer */

		printf("I2C%d BER! SA=0x%x nwrite=%d, nread=%d, state %d, op=%d, ind=%d, int_cnt=%d, log=0x%x\n",
			bus->module__num, bus->dest_addr,
			bus->write_size, bus->read_size, bus->state, bus->operation, bus->stop_indication,
			bus->int_cnt, bus->event_log);
		NPCM7XX_I2C_EVENT_LOG(NPCM7XX_I2C_EVENT_BER);

		/* Bus arbitration problem should not result in recovery*/
		if (FIELD_GET(NUVOTON_SMBST_MASTER , readb(NUVOTON_SMBST(bus))))
			/* Only current master is allowed to issue Stop Condition*/
			nuvoton_smb_master_abort(bus);
		else {

			/* Bus arbitration loss*/
			if (bus->retry_count-- > 0) {
				printf("\tretry bus%d\n", bus->module__num);
				/* Perform a retry (generate a Start condition as soon as the SMBus is free)  */
				writeb(NUVOTON_SMBST_BER, NUVOTON_SMBST(bus));
				nuvoton_smb_master_start(bus);
				return;
			}
		}
		writeb(NUVOTON_SMBST_BER, NUVOTON_SMBST(bus));
		bus->state = SMB_IDLE;
		nuvoton_smb_callback(bus, SMB_BUS_ERR_IND, nuvoton_smb_get_index(bus));
		return;
	}

#if defined (SMB_CAPABILITY_TIMEOUT_SUPPORT)
	/* A Bus Timeout has been identified  */
	if ((FIELD_GET(NUVOTON_SMBT_OUT_T_OUTIE, readb(NUVOTON_SMBT_OUT(bus)) ) == 1) &&  /* bus timeout interrupt is on*/
	  (FIELD_GET(NUVOTON_SMBT_OUT_T_OUTST, readb(NUVOTON_SMBT_OUT(bus)) ))) {         /* and bus timeout status is set */
		nuvoton_smb_master_abort(bus);
		NPCM7XX_I2C_EVENT_LOG(NPCM7XX_I2C_EVENT_TO);
		writeb(readb(NUVOTON_SMBT_OUT(bus)) | NUVOTON_SMBT_OUT_T_OUTST, NUVOTON_SMBT_OUT(bus));/* Clear EO_BUSY pending bit */
		bus->state = SMB_IDLE;
		nuvoton_smb_callback(bus, SMB_BUS_ERR_IND, nuvoton_smb_get_index(bus));
		return;
	}
#endif

#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT

	/* A Master End of Busy (meaning Stop Condition happened)*/
	if ((FIELD_GET(NUVOTON_SMBCTL1_EOBINTE, readb(NUVOTON_SMBCTL1(bus)) ) == 1) &&  /* End of Busy interrupt is on  */
		(FIELD_GET(NUVOTON_SMBCST3_EO_BUSY, readb(NUVOTON_SMBCST3(bus)) ))) {        /* and End of Busy is set */
		I2C_DEBUG("\tEnd of busy bus = %d\n", bus->module__num);
		writeb(readb(NUVOTON_SMBCTL1(bus)) & ~NUVOTON_SMBCTL1_EOBINTE, NUVOTON_SMBCTL1(bus));/* Disable "End of Busy" interrupt*/
		writeb(readb(NUVOTON_SMBCST3(bus)) | NUVOTON_SMBCST3_EO_BUSY, NUVOTON_SMBCST3(bus));/* Clear EO_BUSY pending bit*/

		bus->state = SMB_IDLE;

		if ((bus->write_size == SMB_BYTES_QUICK_PROT) ||
			(bus->read_size == SMB_BYTES_QUICK_PROT) ||
			(bus->read_size == 0)) {
			nuvoton_smb_callback(bus, bus->stop_indication, 0);
		} else {
			nuvoton_smb_callback(bus, bus->stop_indication,
						bus->read_index);
		}
		return;
	}
#endif


	/* Address sent and requested stall occurred (Master mode)  */
	if (FIELD_GET(NUVOTON_SMBST_STASTR , readb(NUVOTON_SMBST(bus)))){
		I2C_DEBUG("\tmaster stall bus = %d\n", bus->module__num);

		ASSERT(FIELD_GET(NUVOTON_SMBST_MASTER, readb(NUVOTON_SMBST(bus))));

		/* Check for Quick Command SMBus protocol (block protocol)*/
		if ((bus->write_size == SMB_BYTES_QUICK_PROT) ||
			(bus->read_size  == SMB_BYTES_QUICK_PROT)) {

			/* No need to write any data bytes - reached here only in Quick Command*/
#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT

			/* Enable "End of Busy" interrupt before issuing a STOP condition. */
			writeb(readb(NUVOTON_SMBCTL1(bus)) | NUVOTON_SMBCTL1_EOBINTE, NUVOTON_SMBCTL1(bus));
#endif
			nuvoton_smb_master_stop(bus);


			/* Update status */
#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
			bus->state = SMB_STOP_PENDING;
			bus->stop_indication = SMB_MASTER_DONE_IND;
#else
			bus->state = SMB_IDLE;

			/* Notify upper layer*/
			nuvoton_smb_callback(bus, SMB_MASTER_DONE_IND, 0);
#endif
		} else if (bus->read_size == 1)

		/* Receiving one byte only - set NACK after ensuring slave ACKed the address byte  */
		nuvoton_smb_nack(bus);

		/* Reset stall-after-address-byte */
		nuvoton_smb_stall_after_start(bus, false);

		/* Clear stall only after setting STOP  */
		writeb(NUVOTON_SMBST_STASTR, NUVOTON_SMBST(bus));
		return;
	}


	/* SDA status is set - transmit or receive, master */
	if (FIELD_GET(NUVOTON_SMBST_SDAST, readb(NUVOTON_SMBST(bus))) ||
		(bus->fifo_use &&
		(FIELD_GET(NUVOTON_SMBRXF_STS_RX_THST, readb(NUVOTON_SMBRXF_STS(bus))) ||
		FIELD_GET(NUVOTON_SMBTXF_STS_TX_THST, readb(NUVOTON_SMBTXF_STS(bus)))))) {

		/* Status Bit is cleared by writing to or reading from SDA (depending on current direction)  */
		I2C_DEBUG("\tSDA master set bus%d state=%d\n", bus->module__num, bus->state);

		switch(bus->state){
			/* Handle unsuccessful bus mastership  */
			case SMB_IDLE:
				/* Perform SMB recovery in Master mode, where state is IDLE, which is an illegal state  */
				nuvoton_smb_master_abort(bus);
				nuvoton_smb_callback(bus, SMB_BUS_ERR_IND, 0);

				printf("\tSDA master bus%d bus is idle\n", bus->module__num);
				return;


				break;

			case SMB_MASTER_START:
				if (FIELD_GET(NUVOTON_SMBST_MASTER , readb(NUVOTON_SMBST(bus)))) {
					u8 addr_byte = bus->dest_addr;

					I2C_DEBUG("\tSDA master bus%d master start\n", bus->module__num);


					bus->crc_data = 0;
					/* Check for Quick Command SMBus protocol */
					if ((bus->write_size == SMB_BYTES_QUICK_PROT) ||
						(bus->read_size  == SMB_BYTES_QUICK_PROT))
						/* Need to stall after successful completion of sending address byte */
						nuvoton_smb_stall_after_start(bus, true);
						/* Prepare address byte */
						if (bus->write_size == 0) {
							if (bus->read_size == 1)
								/* Receiving one byte only - stall after successful completion of sending*/
								/* address byte. If we NACK here, and slave doesn't ACK the address, we might  */
								/* unintentionally NACK the next multi-byte read*/
								nuvoton_smb_stall_after_start(bus, true);

							/* Set direction to Read */
							addr_byte |= (u8)0x1;
							bus->operation = SMB_READ_OPER;  // TaliP: no need for this !!!
						} else
							bus->operation = SMB_WRITE_OPER;  // TaliP: no need for this !!!
					/* Write the address to the bus */
					bus->state = SMB_OPER_STARTED;
					nuvoton_smb_write_byte(bus, addr_byte);
				}
				else
					printf("\tSDA set , bus%d is not a master, write %d 0x%x...\n", bus->module__num, bus->write_size, bus->write_data_buf[0]);
				break;

			/* SDA status is set - transmit or receive: Handle master mode  */
			case SMB_OPER_STARTED:
				if (bus->operation == SMB_WRITE_OPER) {
					u16 wcount;

					I2C_DEBUG("\tSDA master bus%d oper wr\n", bus->module__num);
					NPCM7XX_I2C_EVENT_LOG(NPCM7XX_I2C_EVENT_WRITE);

					if ((bus->fifo_use == true))
						writeb(readb(NUVOTON_SMBTXF_STS(bus)) | NUVOTON_SMBTXF_STS_TX_THST, NUVOTON_SMBTXF_STS(bus));

					/* Master write operation - last byte handling */
					if (bus->write_index == bus->write_size) {
						I2C_DEBUG("\tSDA master bus%d last byte\n", bus->module__num);
						if ((bus->fifo_use == true) && (FIELD_GET(NUVOTON_SMBTXF_STS_TX_BYTES, readb(NUVOTON_SMBTXF_STS(bus)))> 0))
							/* No more bytes to send (to add to the FIFO), however the FIFO is not empty*/
							/* yet. It is still in the middle of transmitting. Currency there is nothing*/
							/* to do except for waiting to the end of the transmission.  */
							/* We will get an interrupt when the FIFO will get empty. */
							return;

						if (bus->read_size == 0) {
							/* all bytes have been written, in a pure write operation */
#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
							/* Enable "End of Busy" interrupt. */
							writeb(readb(NUVOTON_SMBCTL1(bus)) | NUVOTON_SMBCTL1_EOBINTE, NUVOTON_SMBCTL1(bus));
#endif
							// Issue a STOP condition on the bus
							nuvoton_smb_master_stop(bus);
							// Clear SDA Status bit (by writing dummy byte)
							nuvoton_smb_write_byte(bus, 0xFF);

#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
							bus->state = SMB_STOP_PENDING;
							bus->stop_indication = SMB_MASTER_DONE_IND;
#else
							// Reset state for new transaction
							bus->state = SMB_IDLE;
							// Notify upper layer of transaction completion
							nuvoton_smb_callback(bus, SMB_MASTER_DONE_IND, 0);
#endif
						} else {
							/* last write-byte written on previous interrupt - need to restart & send slave address */
							if ((bus->PEC_use == true) &&
								(bus->read_size < SMB_BYTES_EXCLUDE_BLOCK_SIZE_FROM_BUFFER))   // PEC is used but the protocol is not block read protocol
															// then we add extra bytes for PEC support
								bus->read_size += 1; // move this to xmit !!!

							if (bus->fifo_use == true) {
								if (((bus->read_size == 1) ||
								bus->read_size == SMB_BYTES_EXCLUDE_BLOCK_SIZE_FROM_BUFFER ||
								bus->read_size == SMB_BYTES_BLOCK_PROT)) {   // SMBus Block read transaction.

									writeb(0, NUVOTON_SMBTXF_CTL(bus));
									writeb(1, NUVOTON_SMBRXF_CTL(bus));
								} else {

									if (bus->read_size > SMBUS_FIFO_SIZE)
										writeb((readb(NUVOTON_SMBRXF_CTL(bus)) & ~NUVOTON_SMBRXF_CTL_RX_THR) | FIELD_PREP(NUVOTON_SMBRXF_CTL_RX_THR, SMBUS_FIFO_SIZE), NUVOTON_SMBRXF_CTL(bus));
									else {
										// clear the status bits
										writeb((readb(NUVOTON_SMBRXF_CTL(bus)) & ~NUVOTON_SMBRXF_CTL_RX_THR) | FIELD_PREP(NUVOTON_SMBRXF_CTL_RX_THR, (u8)bus->read_size), NUVOTON_SMBRXF_CTL(bus));
										writeb(readb(NUVOTON_SMBRXF_CTL(bus)) | NUVOTON_SMBRXF_CTL_LAST_PEC, NUVOTON_SMBRXF_CTL(bus));
									}
								}
							}

							/* Generate (Repeated) Start upon next write to SDA */
							nuvoton_smb_master_start(bus);

							if (bus->read_size == 1)

								/* Receiving one byte only - stall after successful completion of sending  */
								/* address byte. If we NACK here, and slave doesn't ACK the address, we */
								/* might unintentionally NACK the next multi-byte read*/

								nuvoton_smb_stall_after_start(bus, true);

							/* send the slave address in read direction  */
							nuvoton_smb_write_byte(bus, bus->dest_addr | 0x1);

							/* Next interrupt will occur on read*/
							bus->operation = SMB_READ_OPER;
						}
					} else {
						if ((bus->PEC_use == true) && (bus->write_index == 0)
						&& (bus->read_size == 0))// extra bytes for PEC support
							bus->write_size += 1;

						/* write next byte not last byte and not slave address */
						if ((bus->fifo_use == false) || (bus->write_size == 1)) {
							if ((bus->PEC_use == true) && (bus->read_size == 0) &&
							(bus->write_index + 1 == bus->write_size)) { // Master write protocol to send PEC byte.
								nuvoton_smb_write_PEC(bus);
								bus->write_index++;
							} else
								nuvoton_smb_write_byte(bus, bus->write_data_buf[bus->write_index++]);
						}
						// FIFO is used
						else {
							wcount = bus->write_size - bus->write_index;
							if (wcount > SMBUS_FIFO_SIZE)
								/* data to send is more then FIFO size.  */
								/* Configure the FIFO interrupt to be mid of FIFO.*/
								writeb(NUVOTON_SMBTXF_CTL_THR_TXIE | (SMBUS_FIFO_SIZE / 2), NUVOTON_SMBTXF_CTL(bus));
							else if ((wcount > SMBUS_FIFO_SIZE / 2) && (bus->write_index != 0))
								/* write_index != 0 means that this is not the first write. */
								/* since interrupt is in the mid of FIFO, only half of the fifo is empty.  */
								/* Continue to configure the FIFO interrupt to be mid of FIFO. */
								writeb(NUVOTON_SMBTXF_CTL_THR_TXIE | (SMBUS_FIFO_SIZE / 2), NUVOTON_SMBTXF_CTL(bus));
							else {
#if defined (SMB_CAPABILITY_HW_PEC_SUPPORT)
								if ((bus->PEC_use) && (wcount > 1))
									wcount--; //put the PEC byte last after the FIFO becomes empty.
#endif
								/* This is the first write (write_index = 0) and data to send is less or*/
								/* equal to FIFO size.*/
								/* Or this is the last write and data to send is less or equal half FIFO*/
								/* size.  */
								/* In both cases disable the FIFO threshold interrupt.*/
								/* The next interrupt will happen after the FIFO will get empty.  */
								writeb((u8)0, NUVOTON_SMBTXF_CTL(bus));
							}

							nuvoton_smb_write_to_fifo(bus, wcount);
							writeb((readb(NUVOTON_SMBTXF_STS(bus)) & ~NUVOTON_SMBTXF_STS_TX_THST) | FIELD_PREP(NUVOTON_SMBTXF_STS_TX_THST, 1 ), NUVOTON_SMBTXF_STS(bus)); //clear status bit
#ifdef SMB_STALL_TIMEOUT_SUPPORT
							bus->stall_counter = 0;
#endif
						}
					}
				} else if (bus->operation == SMB_READ_OPER) {
					u16 block_zero_bytes;
					/* Master read operation (pure read or following a write operation). */
					NPCM7XX_I2C_EVENT_LOG(NPCM7XX_I2C_EVENT_READ);


					/* Initialize number of bytes to include only the first byte (presents a case where */
					/* number of bytes to read is zero); add PEC if applicable */
					block_zero_bytes = 1;
					if (bus->PEC_use == true)
						block_zero_bytes++;

					/* Perform master read, distinguishing between last byte and the rest of the  */
					/* bytes. The last byte should be read when the clock is stopped  */
					if ((bus->read_index < (bus->read_size - 1)) ||
					bus->fifo_use == true) {
						u8 data;

						/* byte to be read is not the last one  */
						/* Check if byte-before-last is about to be read */
						if ((bus->read_index == (bus->read_size - 2)) &&
						bus->fifo_use == false)

							/* Set nack before reading byte-before-last, so that nack will be generated */
							/* after receive of last byte*/
							nuvoton_smb_nack(bus);

						if (!FIELD_GET(NUVOTON_SMBST_SDAST , readb(NUVOTON_SMBST(bus)))) {
							/* No data available - reset state for new transaction*/
							bus->state = SMB_IDLE;

							/* Notify upper layer of transaction completion */
							nuvoton_smb_callback(bus, SMB_NO_DATA_IND, bus->read_index);
						} else if (bus->read_index == 0) {
							if (bus->read_size == SMB_BYTES_EXCLUDE_BLOCK_SIZE_FROM_BUFFER ||
							bus->read_size == SMB_BYTES_BLOCK_PROT) {
								(void)nuvoton_smb_read_byte(bus, &data);

								/* First byte indicates length in block protocol  */
								if (bus->read_size == SMB_BYTES_EXCLUDE_BLOCK_SIZE_FROM_BUFFER)
									bus->read_size = data;
								else {
									bus->read_data_buf[bus->read_index++] = data;
									bus->read_size = data + 1;
								}

								if (bus->PEC_use == true) {
									bus->read_size += 1;
									data += 1;
								}

								if (bus->fifo_use == true) {
									writeb(readb(NUVOTON_SMBRXF_STS(bus)) | NUVOTON_SMBRXF_STS_RX_THST, NUVOTON_SMBRXF_STS(bus));
									writeb(readb(NUVOTON_SMBTXF_STS(bus)) | NUVOTON_SMBTXF_STS_TX_THST, NUVOTON_SMBTXF_STS(bus));
									//writeb(readb(NUVOTON_SMBFIF_CTS(bus)) | NUVOTON_SMBFIF_CTS_CLR_FIFO, NUVOTON_SMBFIF_CTS(bus));
									writeb(readb(NUVOTON_SMBFIF_CTS(bus)) | NUVOTON_SMBFIF_CTS_RXF_TXE, NUVOTON_SMBFIF_CTS(bus));
									if (data > SMBUS_FIFO_SIZE)
										writeb((readb(NUVOTON_SMBRXF_CTL(bus)) & ~NUVOTON_SMBRXF_CTL_RX_THR) | FIELD_PREP(NUVOTON_SMBRXF_CTL_RX_THR, SMBUS_FIFO_SIZE), NUVOTON_SMBRXF_CTL(bus));
									else {
										if (data == 0)
											data = 1;

										/* clear the status bits*/
										writeb((readb(NUVOTON_SMBRXF_CTL(bus)) & ~NUVOTON_SMBRXF_CTL_RX_THR) | FIELD_PREP(NUVOTON_SMBRXF_CTL_RX_THR, (u8)data), NUVOTON_SMBRXF_CTL(bus));
										writeb(readb(NUVOTON_SMBRXF_CTL(bus)) | NUVOTON_SMBRXF_CTL_LAST_PEC, NUVOTON_SMBRXF_CTL(bus));
									}
								}
							} else {
								if (bus->fifo_use == false) {
									(void)nuvoton_smb_read_byte(bus, &data);
									bus->read_data_buf[bus->read_index++] = data;
								} else {
									writeb(readb(NUVOTON_SMBTXF_STS(bus)) | NUVOTON_SMBTXF_STS_TX_THST, NUVOTON_SMBTXF_STS(bus));
									nuvoton_smb_master_fifo_read(bus);
								}
							}

						} else {
							if (bus->fifo_use == true) {   // FIFO in used.
								if ((bus->read_size == block_zero_bytes) && (bus->read_block_use == true)) {
#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
									/* Enable "End of Busy" interrupt */
									writeb(readb(NUVOTON_SMBCTL1(bus)) | NUVOTON_SMBCTL1_EOBINTE, NUVOTON_SMBCTL1(bus));
#endif
									nuvoton_smb_master_stop(bus);
									nuvoton_smb_read_from_fifo(bus, FIELD_GET(NUVOTON_SMBRXF_CTL_RX_THR, readb(NUVOTON_SMBRXF_CTL(bus))));


#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
									bus->state = SMB_STOP_PENDING;
									bus->stop_indication = SMB_MASTER_BLOCK_BYTES_ERR_IND;
#else
									/* Reset state for new transaction*/
									bus->state = SMB_IDLE;

									/* Notify upper layer of transaction completion*/
									nuvoton_smb_callback(bus, SMB_MASTER_BLOCK_BYTES_ERR_IND, bus->read_index);
#endif
								} else
									nuvoton_smb_master_fifo_read(bus);
							} else {
								(void)nuvoton_smb_read_byte(bus, &data);
								bus->read_data_buf[bus->read_index++] = data;
							}
						}
					} else {
						/* last byte is about to be read - end of transaction. */
						/* Stop should be set before reading last byte.  */
						u8 data;
						SMB_STATE_IND_T ind = SMB_MASTER_DONE_IND;

#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
						/* Enable "End of Busy" interrupt.*/
						writeb(readb(NUVOTON_SMBCTL1(bus)) | NUVOTON_SMBCTL1_EOBINTE, NUVOTON_SMBCTL1(bus));
#endif
						nuvoton_smb_master_stop(bus);

						(void)nuvoton_smb_read_byte(bus, &data);

						if ((bus->read_size == block_zero_bytes) && (bus->read_block_use == true))
							ind = SMB_MASTER_BLOCK_BYTES_ERR_IND;
						else {
							bus->read_data_buf[bus->read_index++] = data;
							if (nuvoton_smb_get_PEC(bus) != 0)
								ind = SMB_MASTER_PEC_ERR_IND;
						}

#ifdef SMB_CAPABILITY_END_OF_BUSY_SUPPORT
						bus->state = SMB_STOP_PENDING;
						bus->stop_indication = ind;
#else
						/* Reset state for new transaction*/
						bus->state = SMB_IDLE;

						/* Notify upper layer of transaction completion  */
						nuvoton_smb_callback(bus, ind, bus->read_index);
#endif
					} /* last read byte */
				} /* read operation */
				else {
					printf("NPCM7XX I2C: unknown operation state.\n");
				}
				break;
			default:
				printf("master sda set error on state machine\n");
				BUG();
		} // End of master operation: SDA status is set - transmit or receive.
	} //SDAST
}

////////////////    END OF MASTER_HANDLER



static void nuvoton_smb_reset(nuvoton_i2c_bus_t *bus)
{
	/* Save NUVOTON_SMBCTL1 relevant bits. It is being cleared when the module is disabled */
	u8 smbctl1 = readb(NUVOTON_SMBCTL1(bus)) & (NUVOTON_SMBCTL1_GCMEN |NUVOTON_SMBCTL1_INTEN | NUVOTON_SMBCTL1_NMINTE);

	/* Disable the SMB module */
	writeb((readb(NUVOTON_SMBCTL2(bus)) & ~SMBCTL2_ENABLE) | FIELD_PREP(SMBCTL2_ENABLE, DISABLE), NUVOTON_SMBCTL2(bus));

	/* Enable the SMB module */
	(void)nuvoton_smb_enable(bus);

	/* Restore NUVOTON_SMBCTL1 status */
	writeb(smbctl1, NUVOTON_SMBCTL1(bus));

	/* Reset driver status */
	bus->state = SMB_IDLE;
	/*-----------------------------------------------------------------------------------------------------*/
	/* Configure FIFO disabled mode so slave will not use fifo (master will set it on if supported)	*/
	/*-----------------------------------------------------------------------------------------------------*/
	writeb(readb(NUVOTON_SMBFIF_CTL(bus)) & ~NUVOTON_SMBFIF_CTL_FIFO_EN, NUVOTON_SMBFIF_CTL(bus));
	bus->fifo_use = false;
}


static void nuvoton_smb_master_abort(nuvoton_i2c_bus_t *bus)
{
	I2C_DEBUG("\n");

	/* Only current master is allowed to issue Stop Condition*/
	if (FIELD_GET(NUVOTON_SMBST_MASTER , readb(NUVOTON_SMBST(bus))))
		nuvoton_smb_abort_data(bus);

	nuvoton_smb_reset(bus);

	return;
}

static void nuvoton_smb_int_enable(nuvoton_i2c_bus_t *bus, bool enable)
{
	writeb((readb(NUVOTON_SMBCTL1(bus)) & ~NUVOTON_SMBCTL1_INTEN) | FIELD_PREP(NUVOTON_SMBCTL1_INTEN, enable), NUVOTON_SMBCTL1(bus));
}

static const u8 crc8_table[256] = {
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
	0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
	0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
	0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
	0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
	0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
	0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
	0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
	0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
	0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
	0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
	0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
	0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
	0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
	0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
	0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
	0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
	0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
	0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
	0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
	0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
	0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
	0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
	0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
	0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
	0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
	0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
	0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
	0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
	0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
	0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
	0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

static u8 nuvoton_smb_calc_crc8(u8 crc_data, u8 data)
{
	u8 tmp = crc_data ^ data;

	crc_data = crc8_table[tmp];

	return crc_data;
}

static void nuvoton_smb_calc_PEC(nuvoton_i2c_bus_t *bus, u8 data)
{
	if (bus->PEC_use)
		bus->crc_data = nuvoton_smb_calc_crc8(bus->crc_data, data);
}

static inline u8 nuvoton_smb_get_PEC(nuvoton_i2c_bus_t *bus)
{
	if (bus->PEC_use)
#if defined SMB_CAPABILITY_HW_PEC_SUPPORT
		return readb(NUVOTON_SMBPEC(bus));
#else
		return bus->crc_data;
#endif
	else
		return 0;
}

static inline void nuvoton_smb_write_PEC(nuvoton_i2c_bus_t *bus)
{
	if (bus->PEC_use)
	{
		// get PAC value and write to the bus:
		nuvoton_smb_write_byte(bus, nuvoton_smb_get_PEC(bus));
	}
	return;
}


#ifdef SMB_STALL_TIMEOUT_SUPPORT
static void nuvoton_smb_set_stall_threshhold(nuvoton_i2c_bus_t *bus, u8 threshold)
{
	bus->stall_threshold = threshold;
}

static void nuvoton_smb_stall_handler(nuvoton_i2c_bus_t *bus)
{
	if ((bus->state == SMB_IDLE) ||
	(bus->state == SMB_DISABLE) ||
	(bus->master_or_slave == SMB_SLAVE))
		; // ignore this bus
	else {
		/* increase timeout counter*/
		bus->stall_counter++;

		/* time expired, execute recovery */
		if ((bus->stall_counter) >= bus->stall_threshold) {
			nuvoton_smb_master_abort(bus);
			nuvoton_smb_callback(bus, SMB_BUS_ERR_IND, nuvoton_smb_get_index(bus));
			return;
		}
	}
}
#endif

#ifdef TBD
static void nuvoton_smb_re_enable_module(nuvoton_i2c_bus_t *bus)
{
	/* Enable SMB interrupt and New Address Match interrupt source */
	writeb((readb(NUVOTON_SMBCTL1(bus)) & ~NUVOTON_SMBCTL1_NMINTE) | FIELD_PREP(NUVOTON_SMBCTL1_NMINTE, ENABLE), NUVOTON_SMBCTL1(bus));
	writeb((readb(NUVOTON_SMBCTL1(bus)) & ~NUVOTON_SMBCTL1_INTEN) | FIELD_PREP(NUVOTON_SMBCTL1_INTEN, ENABLE), NUVOTON_SMBCTL1(bus));
}

static bool nuvoton_smb_interrupt_is_pending(void)
{
	nuvoton_i2c_bus_t *bus;
	bool InterruptIsPending = false;

	for (bus = 0; bus < SMB_NUM_OF_MODULES; bus++)
		InterruptIsPending |= INTERRUPT_PENDING(SMB_INTERRUPT_PROVIDER,
	SMB_INTERRUPT(bus));

	return InterruptIsPending;
}
#endif

#ifdef SMB_CAPABILITY_FORCE_SCL_SDA
static void nuvoton_smb_set_SCL(nuvoton_i2c_bus_t *bus, SMB_LEVEL_T level)
{
	unsigned long flags;

	/* Select Bank 0 to access NUVOTON_SMBCTL4 */
	spin_lock_irqsave(&bus->lock, flags);
	nuvoton_smb_select_bank(bus, SMB_BANK_0);

	/* Set SCL_LVL, SDA_LVL bits as Read/Write (R/W) */
	writeb(readb(NUVOTON_SMBCTL4(bus)) | SMBCTL4_LVL_WE, NUVOTON_SMBCTL4(bus));

	/* Set level */
	writeb((readb(NUVOTON_SMBCTL3(bus)) & ~SMBCTL3_SCL_LVL) | FIELD_PREP(SMBCTL3_SCL_LVL, level), NUVOTON_SMBCTL3(bus));

	/* Set SCL_LVL, SDA_LVL bits as Read Only (RO) */
	writeb(readb(NUVOTON_SMBCTL4(bus)) & ~SMBCTL4_LVL_WE, NUVOTON_SMBCTL4(bus));

	/* Return to Bank 1 */
	nuvoton_smb_select_bank(bus, SMB_BANK_1);
	spin_unlock_irqrestore(&bus->lock, flags);
}

static void nuvoton_smb_set_SDA(nuvoton_i2c_bus_t *bus, SMB_LEVEL_T level)
{
	unsigned long flags;

	/* Select Bank 0 to access NUVOTON_SMBCTL4 */
	spin_lock_irqsave(&bus->lock, flags);
	nuvoton_smb_select_bank(bus, SMB_BANK_0);

	/* Set SCL_LVL, SDA_LVL bits as Read/Write (R/W) */
	writeb(readb(NUVOTON_SMBCTL4(bus)) | SMBCTL4_LVL_WE, NUVOTON_SMBCTL4(bus));

	/* Set level */
	writeb((readb(NUVOTON_SMBCTL3(bus)) & ~SMBCTL3_SDA_LVL) | FIELD_PREP(SMBCTL3_SDA_LVL, level), NUVOTON_SMBCTL3(bus));

	/* Set SCL_LVL, SDA_LVL bits as Read Only (RO) */
	writeb(readb(NUVOTON_SMBCTL4(bus)) & ~SMBCTL4_LVL_WE, NUVOTON_SMBCTL4(bus));

	/* Return to Bank 1 */
	nuvoton_smb_select_bank(bus, SMB_BANK_1);
	spin_unlock_irqrestore(&bus->lock, flags);
}
#endif // SMB_CAPABILITY_FORCE_SCL_SDA



static void nuvoton_smb_callback(nuvoton_i2c_bus_t *bus, SMB_STATE_IND_T op_status, u16 info)
{
	struct i2c_msg *msgs = bus->msgs;
	int msgs_num = bus->msgs_num;


	I2C_DEBUG("\t\t=>\tend bus%d status %d info %d\n", bus->module__num, op_status, info);
	switch (op_status) {
	case SMB_MASTER_DONE_IND:
		// Master transaction finished and all transmit bytes were sent
		// info: number of bytes actually received after the Master receive operation
		//	 (if Master didn't issue receive it should be 0)
				// Notify that not all data was received on Master or Slave
		// info:
		//	on receive: number of actual bytes received
		//				when PEC is used even if 'info' is the expected number of bytes,
		//					it means that PEC error occured.
		{
			if (msgs[0].flags & I2C_M_RD)
				msgs[0].len = info;
			else if (msgs_num == 2 && msgs[1].flags & I2C_M_RD)
				msgs[1].len = info;


			I2C_DEBUG("I2C%d done, wrote %d bytes, read %d bytes\n",
						bus->module__num, bus->write_index, bus->read_index);

			bus->cmd_err = 0;
			bus->cmd_complete = 1;
		}
		break;

	case SMB_NO_DATA_IND:
		// Notify that not all data was received on Master or Slave
		// info:
		//	on receive: number of actual bytes received
		//				when PEC is used even if 'info' is the expected number of bytes,
		//					it means that PEC error occured.
		{
			if (msgs[0].flags & I2C_M_RD)
				msgs[0].len = info;
			else if (msgs_num == 2 && msgs[1].flags & I2C_M_RD)
				msgs[1].len = info;

			printf("I2C%d BER, wrote %d bytes, read %d bytes\n",
				bus->module__num, bus->write_index, bus->read_index);

			bus->cmd_err = -EFAULT;
			bus->cmd_complete = 1;
		}
		break;
	case SMB_NACK_IND:
		// MASTER transmit got a NAK before transmitting all bytes
		// info: number of transmitted bytes
		printf("I2C%d NACK, wrote %d bytes, read %d bytes\n",
				bus->module__num, bus->write_index, bus->read_index);
		bus->cmd_err = -EAGAIN;
		bus->cmd_complete = 1;
		break;
	case SMB_BUS_ERR_IND:
		// Bus error occured
		// info: has no meaning
		printf("I2C%d BER, wrote %d bytes, read %d bytes\n",
				bus->module__num, bus->write_index, bus->read_index);
		bus->cmd_err = -EIO;
		bus->cmd_complete = 1;
		break;
	case SMB_WAKE_UP_IND:
		// SMBus wake up occured
		// info: has no meaning
		break;
	default:
		break;
	}
}


static int __nuvoton_i2c_init(struct nuvoton_i2c_bus *bus)
{
	int ret;

	/* Initialize the internal data structures */
	bus->state = SMB_DISABLE;
	bus->master_or_slave = SMB_SLAVE;
#ifdef SMB_STALL_TIMEOUT_SUPPORT
	bus->stall_counter   = 0;
	bus->stall_threshold = DEFAULT_STALL_COUNT;
#endif

	I2C_DEBUG("I2C%d:  : clk_freq = %d\n", bus->module__num, bus->freq);

	ret = nuvoton_smb_init_module(bus, SMB_MASTER, bus->freq / 1000);
	if (ret == false) {
		printk("I2C: nuvoton_smb_init_module() failed\n");
		return -1;
	}
#if defined (SMB_CAPABILITY_TIMEOUT_SUPPORT)
	nuvoton_smb_enable_timeout(bus, true);
#endif //SMB_CAPABILITY_TIMEOUT_SUPPORT


	return 0;
}


static HAL_STATUS nuvoton_i2c_bus_irq(UINT32 param)
{
	struct nuvoton_i2c_bus *bus = (struct nuvoton_i2c_bus *)param;

	bus->int_cnt++;

#ifdef SMB_SW_BYPASS_HW_ISSUE_SMB_STOP
	CLK_GetTimeStamp(bus->interrupt_time_stamp);
#endif
	if (bus->master_or_slave == SMB_MASTER)	{
		nuvoton_smb_int_master_handler(bus);
		return HAL_ERROR;
	}

	printf("int unknown on bus%d\n", bus->module__num);

	return HAL_OK;
}


static int  nuvoton_i2c_master_xfer(struct udevice* dev,
				struct i2c_msg *msgs, int num)
{
	struct nuvoton_i2c_bus *bus = dev_get_priv(dev);
	struct i2c_msg *msg0, *msg1;
	volatile long time_left;
	unsigned long flags;
	u16 nwrite, nread;
	u8 *write_data, *read_data;
	u8 slave_addr;
	int ret = 0;

	spin_lock_irqsave(&bus->lock, flags);
	bus->cmd_err = -EPERM; // restart error to unused value by this driver.
	bus->int_cnt = 0;

	AIC_RegisterHandler(bus->irq, nuvoton_i2c_bus_irq,  (UINT32)bus);
	AIC_EnableInt(bus->irq);

	writeb(0xFF, NUVOTON_SMBST(bus));
	//writeb(NUVOTON_SMBCST_BB, NUVOTON_SMBCST(bus));

	if (num > 2 || num < 1) {
		printf("I2C command not supported, num of msgs = %d\n", num);
		spin_unlock_irqrestore(&bus->lock, flags);
		return -EINVAL;
	}

	msg0 = &msgs[0];
	slave_addr = msg0->addr;
	if (msg0->flags & I2C_M_RD) { // read
		if (num == 2) {
			printf(" num = 2 but first msg is read instead of "
			"write.\n");
			spin_unlock_irqrestore(&bus->lock, flags);
			return -EINVAL;
		}
		nwrite = 0;
		write_data = NULL;
		if (msg0->flags & I2C_M_RECV_LEN)
			nread = SMB_BYTES_BLOCK_PROT;
		else
			nread = msg0->len;

		read_data = msg0->buf;

	} else { // write
		nwrite = msg0->len;
		write_data = msg0->buf;
		nread = 0;
		read_data = NULL;
		if (num == 2) {
			msg1 = &msgs[1];
			if (slave_addr != msg1->addr) {
				printf(" slave_addr == %02x but msg1->addr == "
				"%02x\n", slave_addr, msg1->addr);
				spin_unlock_irqrestore(&bus->lock, flags);
				return -EINVAL;
			}
			if ((msg1->flags & I2C_M_RD) == 0) {
				printf(" num = 2 but both msg are write.\n");
				spin_unlock_irqrestore(&bus->lock, flags);
				return -EINVAL;
			}
			if (msg1->flags & I2C_M_RECV_LEN)
				nread = SMB_BYTES_BLOCK_PROT;
			else
				nread = msg1->len;

			read_data = msg1->buf;
		}
	}

	bus->msgs = msgs;
	bus->msgs_num = num;

	if (nwrite == 0 && nread == 0)
		nwrite = nread = SMB_BYTES_QUICK_PROT;

	bus->cmd_complete = 0;
	spin_lock_irqsave(&bus->lock, flags);
	if (nuvoton_smb_master_start_xmit(bus, slave_addr, nwrite, nread, write_data,
				read_data, 0) == false)
			ret =  -(EBUSY);

	time_left = 0x10000000;
	while ((bus->cmd_complete == 0) && (--time_left > 0));


	if (bus->cmd_complete == 0){
		printk("\tI2C%d timeout! SA=0x%x error %d, nwrite=%d, nread=%d, state %d, op=%d, ind=%d, int_cnt=%d, ret=%d, log=0x%x\n",
			bus->module__num, slave_addr, bus->cmd_err,
			nwrite, nread, bus->state, bus->operation, bus->stop_indication,
			bus->int_cnt, ret, bus->event_log);

		if (bus->msgs[0].flags & I2C_M_RD)
			nread = bus->msgs[0].len;
		else if (bus->msgs_num == 2 && bus->msgs[1].flags & I2C_M_RD)
			nread = bus->msgs[1].len;

		if (nwrite && nwrite != SMB_BYTES_QUICK_PROT) {
			int i;
			char str[32 * 3 + 4];
			char *s = str;

			for (i = 0; (i < nwrite && i < 32); i++)
				s += sprintf(s, "%02x ", write_data[i]);

			printf("write_data  = %s\n", str);
		}

		if (nread && nread != SMB_BYTES_QUICK_PROT) {
			int i;
			char str[32 * 3 + 4];
			char *s = str;

			for (i = 0; (i < nread && i < 32); i++)
				s += sprintf(s, "%02x ", read_data[i]);

			printf("read_data  = %s\n", str);
		}

		nuvoton_smb_master_abort(bus);
		ret = -ETIMEDOUT;
	}
	else
		ret = bus->cmd_err;



#ifdef CONFIG_NPCM750_I2C_DEBUG
	if (bus->msgs[0].flags & I2C_M_RD)
		nread = bus->msgs[0].len;
	else if (bus->msgs_num == 2 && bus->msgs[1].flags & I2C_M_RD)
		nread = bus->msgs[1].len;
	if (nread && nread != SMB_BYTES_QUICK_PROT) {
		int i;
		char str[32 * 3 + 4];
		char *s = str;

		for (i = 0; (i < nread && i < 32); i++)
			s += sprintf(s, "%02x ", read_data[i]);

		printf("read_data  = %s\n", str);
	}
#endif


	bus->msgs = NULL;
	bus->msgs_num = 0;
	spin_unlock_irqrestore(&bus->lock, flags);

	return ret;
}




static int  nuvoton_i2c_probe_bus(struct udevice *dev)
{
	struct nuvoton_i2c_bus *bus;
	fdt_addr_t addr;
	fdt_size_t size;
	u32 cell[3];
	u32 val = 0;

	bus = dev_get_priv(dev);

	bus->module__num = dev->seq;

	bus->apb_clk = 50000000;  // CLK_GetAPBFreq(2)

	/* Read properties from FDT */
	addr = fdtdec_get_addr_size(gd->fdt_blob, dev->of_offset, "reg", &size);

	bus->base = map_sysmem(addr, size);

	if (!bus->base)
		return -ENOMEM;

	val = fdtdec_get_int_array(gd->fdt_blob, dev->of_offset, "bus-frequency", cell, 1);
	if (val < 0) {
		printf("I2C%d platform_get_frequency error: %d\n", bus->module__num, val);
		return -EINVAL;
	}
	bus->freq = cell[0];

	val = fdtdec_get_int_array(gd->fdt_blob, dev->of_offset, "interrupts", cell, 3);
	if (val < 0) {
		printf("\tI2C%d platform_get_irq error: %d\n", bus->module__num, val);
		return -EINVAL;
	}
	bus->irq = cell[1] + NPCM750_IRQ_GIC_START;
	I2C_DEBUG("\tI2C bus%d irq = %d, speed=%d, base=%d\n", bus->module__num, bus->irq, bus->freq, (uint)bus->base);

	AIC_RegisterHandler(bus->irq, nuvoton_i2c_bus_irq,  (UINT32)bus);
	AIC_EnableInt(bus->irq);

	val = __nuvoton_i2c_init(bus);
	if (val < 0){
		printf("I2C%d: __nuvoton_i2c_init failed error %d\n", bus->module__num, val);
		return val;
	}

	printf("I2C bus%d ready. irq = %d, speed=%d, base=0x%x\n", bus->module__num, bus->irq, bus->freq, (uint)bus->base);


	// Common registers
	nuvoton_smb_select_bank(bus, 0);
	I2C_DEBUG("SMBSDA(bus)		0x%x \n",	 readb(  NUVOTON_SMBSDA(bus)	));
	I2C_DEBUG("SMBST(bus)		0x%x \n",	 readb(  NUVOTON_SMBST(bus) 	));
	I2C_DEBUG("SMBCST(bus)		0x%x \n",	 readb(  NUVOTON_SMBCST(bus)	));
	I2C_DEBUG("SMBCTL1(bus) 	0x%x \n",	 readb(  NUVOTON_SMBCTL1(bus)	));
	I2C_DEBUG("SMBADDR1(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR1(bus)	));
	I2C_DEBUG("SMBCTL2(bus) 	0x%x \n",	 readb(  NUVOTON_SMBCTL2(bus)	));
	I2C_DEBUG("SMBADDR2(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR2(bus)	));
	I2C_DEBUG("SMBCTL3(bus) 	0x%x \n",	 readb(  NUVOTON_SMBCTL3(bus)	));
	I2C_DEBUG("SMBCST2(bus) 	0x%x \n",	 readb(  NUVOTON_SMBCST2(bus)	));
	I2C_DEBUG("SMBCST3(bus) 	0x%x \n",	 readb(  NUVOTON_SMBCST3(bus)	));
	I2C_DEBUG("SMB_VER(bus) 	0x%x \n",	 readb(  SMB_VER(bus)	));
	I2C_DEBUG("SMBADDR3(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR3(bus)	));
	I2C_DEBUG("SMBADDR7(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR7(bus)	));
	I2C_DEBUG("SMBADDR4(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR4(bus)	));
	I2C_DEBUG("SMBADDR8(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR8(bus)	));
	I2C_DEBUG("SMBADDR5(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR5(bus)	));
	I2C_DEBUG("SMBADDR9(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR9(bus)	));
	I2C_DEBUG("SMBADDR6(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR6(bus)	));
	I2C_DEBUG("SMBADDR10(bus)	0x%x \n",	 readb(  NUVOTON_SMBADDR10(bus) ));
	I2C_DEBUG("SMBCTL4(bus) 	0x%x \n",	 readb(  NUVOTON_SMBCTL4(bus)	));
	I2C_DEBUG("SMBCTL5(bus) 	0x%x \n",	 readb(  NUVOTON_SMBCTL5(bus)	));
	I2C_DEBUG("SMBSCLLT(bus)	0x%x \n",	 readb(  NUVOTON_SMBSCLLT(bus)	));
	I2C_DEBUG("SMBFIF_CTL(bus)	0x%x \n",	 readb(  NUVOTON_SMBFIF_CTL(bus)));
	I2C_DEBUG("SMBSCLHT(bus)	0x%x \n",	 readb(  NUVOTON_SMBSCLHT(bus)	));
	I2C_DEBUG("SMBADDR(bus, 0)	0x%x \n",	 readb(  NUVOTON_SMBADDR(bus, 0)));

	return 0;
}


static int  nuvoton_i2c_remove_bus(struct udevice *dev)
{
	struct nuvoton_i2c_bus *bus = dev_get_priv(dev);
	/* Disable everything. */
	nuvoton_smb_disable(bus);

	unmap_sysmem(bus->base);

	return 0;
}


static int nuvoton_i2c_set_bus_speed(struct udevice *dev,
						unsigned int speed)
{
		int ret;
		struct nuvoton_i2c_bus *bus = dev_get_priv(dev);

		ret = nuvoton_smb_init_clk(bus, SMB_MASTER, (u16)bus->freq);

		return ret;
}


static const struct dm_i2c_ops nuvoton_i2c_ops = {
	.xfer		    = nuvoton_i2c_master_xfer,
	.set_bus_speed	= nuvoton_i2c_set_bus_speed,
};

static const struct udevice_id nuvoton_i2c_of_match[] = {
	{ .compatible = "nuvoton,npcm750-i2c-bus" },
	{}
};

U_BOOT_DRIVER(npcm750_i2c_bus) = {
	.name = "npcm750-i2c",
	.id = UCLASS_I2C,
	.of_match = nuvoton_i2c_of_match,
	.probe = nuvoton_i2c_probe_bus,
	.remove = nuvoton_i2c_remove_bus,
	.priv_auto_alloc_size = sizeof(struct nuvoton_i2c_bus),
	.ops = &nuvoton_i2c_ops,
};



