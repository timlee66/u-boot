#ifndef _POLEG_OTP_H_
#define _POLEG_OTP_H_

typedef enum {
	NPCM750_KEY_SA    = 0,
	NPCM750_FUSE_SA   = 1,
	NPCM750_NUM_OF_SA = 2
} poleg_otp_storage_array;

struct poleg_otp_regs {
	unsigned int fst;
	unsigned int faddr;
	unsigned int fdata;
	unsigned int fcfg;
	unsigned int fustrap_fkeyind;
	unsigned int fctl;
};

#define FST_RDY                 (1 << 0)
#define FST_RDST                (1 << 1)
#define FST_RIEN                (1 << 2)

#define FADDR_BYTEADDR(addr)    ((addr) << 0)
#define FADDR_BITPOS(pos)       ((pos) << 10)
#define FADDR_VAL(addr, pos)    (FADDR_BYTEADDR(addr) | FADDR_BITPOS(pos))

#define FDATA_MASK              (0xff)


// Program cycle initiation values (sequence of two adjacent writes)
#define PROGRAM_ARM             0x1
#define PROGRAM_INIT            0xBF79E5D0

// Read cycle initiation value
#define READ_INIT               0x02

// Value to clean FDATA contents
#define FDATA_CLEAN_VALUE       0x01


#define NPCM750_OTP_ARR_BYTE_SIZE        1024
#define MIN_PROGRAM_PULSES               4
#define MAX_PROGRAM_PULSES               20


void fuse_nibble_parity_ecc_encode(u8 *datain, u8 *dataout, u32 size);
void fuse_majority_rule_ecc_encode(u8 *datain, u8 *dataout, u32 size);

#endif
