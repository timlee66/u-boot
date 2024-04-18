// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2024 Nuvoton Technology Corp.
 */

#include <common.h>
#include <dm.h>
#include <hash.h>
#include <malloc.h>
#include <asm/io.h>

#define SHA_BLOCK_LENGTH        (512 / 8)
#define SHA512_BLOCK_LENGTH     (1024 / 8)
#define SHA_TIMEOUT             100

/* Register fields */
#define HASH_CTR_STS_SHA_EN             BIT(0)
#define HASH_CTR_STS_SHA_BUSY           BIT(1)
#define HASH_CTR_STS_SHA_RST            BIT(2)
#define HASH_CFG_SHA1_SHA2              BIT(0)
#define SHA512_CMD_SHA_512		BIT(3)
#define SHA512_CMD_INTERNAL_ROUND	BIT(2)
#define SHA512_CMD_WRITE		BIT(1)
#define SHA512_CMD_READ			BIT(0)

/* SHA type */
enum npcm_sha_type {
	npcm_sha_type_sha2 = 0,
	npcm_sha_type_sha1,
	npcm_sha_type_sha512,
};

struct npcm_sha_reg_map {
	u32 hash_data_in;
	u8 hash_ctr_sts;
	u8 reserved_0[0x03];
	u8 hash_cfg;
	u8 reserved_1[0x03];
	u8 hash_ver;
	u8 reserved_2[0x03];
	u32 sha512_data_in;
	u8 sha512_ctr_sts;
	u8 reserved_3[0x03];
	u8 sha512_cmd;
	u8 reserved_4[0x03];
	u32 sha512_data_out;
	u32 hash_dig[8];
};

struct npcm_sha_regs {
	u32 *data_in;
	u32 *data_out;
	u8 *ctr_sts;
	u8 *hash_cfg;
	u8 *sha512_cmd;
};

struct hash_info {
	u32 block_sz;
	u32 digest_len;
	u8 length_bytes;
	u8 type;
};

struct message_block {
	u64 length[2];
	u64 nonhash_sz;
	u8 buffer[SHA512_BLOCK_LENGTH * 2];
};

struct npcm_sha_priv {
	struct npcm_sha_reg_map *map;
	struct npcm_sha_regs regs;
	struct message_block block;
	struct hash_info hash;
	bool internal_round;
	bool support_sha512;
};

static struct npcm_sha_priv *sha_priv;

static int npcm_sha_wait_busy(void)
{
	struct npcm_sha_regs *regs = &sha_priv->regs;
	u32 waits = SHA_TIMEOUT;
	u8 val;

	while (waits--) {
		val = readb(regs->ctr_sts);
		if ((val & HASH_CTR_STS_SHA_BUSY) == 0)
			return 0;
	}

	return -ETIMEDOUT;
}

static int npcm_sha_init(struct npcm_sha_priv *priv, u8 type)
{
	struct npcm_sha_reg_map *map = priv->map;
	struct npcm_sha_regs *regs = &priv->regs;
	struct message_block *block = &priv->block;
	struct hash_info *hash = &priv->hash;

	switch (type) {
	case npcm_sha_type_sha1:
		hash->type = npcm_sha_type_sha1;
		hash->block_sz = SHA_BLOCK_LENGTH;
		hash->digest_len = 160;
		hash->length_bytes = 8;
		regs->ctr_sts = &map->hash_ctr_sts;
		regs->data_in = &map->hash_data_in;
		regs->data_out = &map->hash_dig[0];
		regs->hash_cfg = &map->hash_cfg;
		break;
	case npcm_sha_type_sha2:
		hash->type = npcm_sha_type_sha2;
		hash->block_sz = SHA_BLOCK_LENGTH;
		hash->digest_len = 256;
		hash->length_bytes = 8;
		regs->ctr_sts = &map->hash_ctr_sts;
		regs->data_in = &map->hash_data_in;
		regs->data_out = &map->hash_dig[0];
		regs->hash_cfg = &map->hash_cfg;
		break;
	case npcm_sha_type_sha512:
		if (!priv->support_sha512)
			return -ENOTSUPP;
		hash->type = npcm_sha_type_sha512;
		hash->block_sz = SHA512_BLOCK_LENGTH;
		hash->digest_len = 512;
		hash->length_bytes = 16;
		regs->ctr_sts = &map->sha512_ctr_sts;
		regs->data_in = &map->sha512_data_in;
		regs->data_out = &map->sha512_data_out;
		regs->sha512_cmd = &map->sha512_cmd;
		break;
	default:
		return -ENOTSUPP;
	}
	block->length[0] = 0;
	block->length[1] = 0;
	block->nonhash_sz = 0;
	sha_priv->internal_round = false;

	return 0;
}

static void npcm_sha_reset(void)
{
	struct npcm_sha_regs *regs = &sha_priv->regs;
	struct hash_info *hash = &sha_priv->hash;
	u8 val;

	if (hash->type == npcm_sha_type_sha1)
		writeb(HASH_CFG_SHA1_SHA2, regs->hash_cfg);
	else if (hash->type == npcm_sha_type_sha2)
		writeb(0, regs->hash_cfg);
	else if (hash->type == npcm_sha_type_sha512)
		writeb(SHA512_CMD_SHA_512, regs->sha512_cmd);

	val = readb(regs->ctr_sts) & ~HASH_CTR_STS_SHA_EN;
	writeb(val | HASH_CTR_STS_SHA_RST, regs->ctr_sts);
}

static void npcm_sha_enable(bool on)
{
	struct npcm_sha_regs *regs = &sha_priv->regs;
	u8 val;

	val = readb(regs->ctr_sts) & ~HASH_CTR_STS_SHA_EN;
	val |= on;
	writeb(val | on, regs->ctr_sts);
}

static int npcm_sha_flush_block(u8 *block)
{
	struct npcm_sha_regs *regs = &sha_priv->regs;
	struct hash_info *hash = &sha_priv->hash;
	u32 *blk_dw = (u32 *)block;
	u8 reg_val;
	int i;

	if (hash->type == npcm_sha_type_sha512) {
		reg_val = SHA512_CMD_SHA_512 | SHA512_CMD_WRITE;
		if (sha_priv->internal_round)
			reg_val |= SHA512_CMD_INTERNAL_ROUND;
		writeb(reg_val, regs->sha512_cmd);
	}
	for (i = 0; i < (hash->block_sz / sizeof(u32)); i++)
		writel(blk_dw[i], regs->data_in);

	sha_priv->internal_round = true;

	return npcm_sha_wait_busy();
}

static int npcm_sha_update_block(const u8 *in, u32 len)
{
	struct message_block *block = &sha_priv->block;
	struct hash_info *hash = &sha_priv->hash;
	u8 *buffer = &block->buffer[0];
	u32 block_sz = hash->block_sz;
	u32 hash_sz;

	hash_sz = (block->nonhash_sz + len) > block_sz ?
		(block_sz - block->nonhash_sz) : len;
	memcpy(buffer + block->nonhash_sz, in, hash_sz);
	block->nonhash_sz += hash_sz;
	block->length[0] += hash_sz;
	if (block->length[0] < hash_sz)
		block->length[1]++;

	if (block->nonhash_sz == block_sz) {
		block->nonhash_sz = 0;
		if (npcm_sha_flush_block(buffer))
			return -EBUSY;
	}

	return hash_sz;
}

static int npcm_sha_update(const u8 *input, u32 len)
{
	int hash_sz;

	while (len) {
		hash_sz = npcm_sha_update_block(input, len);
		if (hash_sz < 0) {
			printf("SHA512 module busy\n");
			return -EBUSY;
		}
		len -= hash_sz;
		input += hash_sz;
	}

	return 0;
}

static int npcm_sha_finish(u8 *out)
{
	struct npcm_sha_regs *regs = &sha_priv->regs;
	struct message_block *block = &sha_priv->block;
	struct hash_info *hash = &sha_priv->hash;
	u8 *buffer = &block->buffer[0];
	u32 block_sz = hash->block_sz;
	u32 *out32 = (u32 *)out;
	u32 zero_len;
	u64 *length;
	u32 *reg_data_out;
	int i;

	/* Padding, minimal padding size is length_bytes+1 */
	if ((block_sz - block->nonhash_sz) >= (hash->length_bytes + 1))
		zero_len = block_sz - block->nonhash_sz - (hash->length_bytes + 1);
	else
		zero_len = block_sz * 2 - block->nonhash_sz - (hash->length_bytes + 1);
	/* Last byte */
	buffer[block->nonhash_sz++] = 0x80;
	/* Zero bits padding */
	memset(&buffer[block->nonhash_sz], 0, zero_len);
	block->nonhash_sz += zero_len;
	/* Message length */
	length = (u64 *)&buffer[block->nonhash_sz];
	if (hash->length_bytes == 16) {
		*length++ = cpu_to_be64(block->length[1] << 3 | block->length[0] >> 61);
		block->nonhash_sz += 8;
	}
	*length = cpu_to_be64(block->length[0] << 3);
	block->nonhash_sz += 8;
	if (npcm_sha_flush_block(&block->buffer[0]))
		return -ETIMEDOUT;

	/* After padding, the last message may produce 2 blocks */
	if (block->nonhash_sz > block_sz) {
		if (npcm_sha_flush_block(&block->buffer[block_sz]))
			return -ETIMEDOUT;
	}
	/* Read digest */
	if (hash->type == npcm_sha_type_sha512)
		writeb(SHA512_CMD_SHA_512 | SHA512_CMD_READ, regs->sha512_cmd);

	reg_data_out = regs->data_out;
	for (i = 0; i < (hash->digest_len / 32); i++) {
		*out32 = readl(reg_data_out);
		out32++;
		if (hash->type == npcm_sha_type_sha1 ||
		    hash->type == npcm_sha_type_sha2)
			reg_data_out++;
	}

	return 0;
}

int npcm_sha_calc(const u8 *input, u32 len, u8 *output, u8 type)
{
	npcm_sha_init(sha_priv, type);
	npcm_sha_reset();
	npcm_sha_enable(true);
	npcm_sha_update(input, len);
	npcm_sha_finish(output);
	npcm_sha_enable(false);

	return 0;
}

void hw_sha512(const unsigned char *input, unsigned int len,
	       unsigned char *output, unsigned int chunk_sz)
{
	if (!sha_priv->support_sha512) {
		puts("sha512 not support\n");
		return;
	}
	puts("hw_sha512 using BMC HW accelerator\n");
	npcm_sha_calc(input, len, output, npcm_sha_type_sha512);
}

void hw_sha256(const unsigned char *input, unsigned int len,
	       unsigned char *output, unsigned int chunk_sz)
{
	puts("hw_sha256 using BMC HW accelerator\n");
	npcm_sha_calc(input, len, output, npcm_sha_type_sha2);
}

void hw_sha1(const unsigned char *input, unsigned int len,
	     unsigned char *output, unsigned int chunk_sz)
{
	puts("hw_sha1 using BMC HW accelerator\n");
	npcm_sha_calc(input, len, output, npcm_sha_type_sha1);
}

int hw_sha_init(struct hash_algo *algo, void **ctxp)
{
	if (!strcmp("sha1", algo->name)) {
		npcm_sha_init(sha_priv, npcm_sha_type_sha1);
	} else if (!strcmp("sha256", algo->name)) {
		npcm_sha_init(sha_priv, npcm_sha_type_sha2);
	} else if (!strcmp("sha512", algo->name)) {
		if (!sha_priv->support_sha512)
			return -ENOTSUPP;
		npcm_sha_init(sha_priv, npcm_sha_type_sha512);
	} else {
		return -ENOTSUPP;
	}

	printf("Using npcm SHA engine\n");
	*ctxp = sha_priv;
	npcm_sha_reset();
	npcm_sha_enable(true);

	return 0;
}

int hw_sha_update(struct hash_algo *algo, void *ctx, const void *buf,
		  unsigned int size, int is_last)
{
	return npcm_sha_update(buf, size);
}

int hw_sha_finish(struct hash_algo *algo, void *ctx, void *dest_buf,
		  int size)
{
	int ret;

	ret = npcm_sha_finish(dest_buf);
	npcm_sha_enable(false);

	return ret;
}

static int npcm_sha_bind(struct udevice *dev)
{
	sha_priv = calloc(1, sizeof(struct npcm_sha_priv));
	if (!sha_priv)
		return -ENOMEM;

	sha_priv->map = dev_read_addr_ptr(dev);
	if (!sha_priv->map) {
		printf("Cannot find sha reg address, binding failed\n");
		return -EINVAL;
	}

	if (IS_ENABLED(CONFIG_ARCH_NPCM8XX))
		sha_priv->support_sha512 = true;

	printf("SHA: NPCM SHA module bind OK\n");

	return 0;
}

static const struct udevice_id npcm_sha_ids[] = {
	{ .compatible = "nuvoton,npcm845-sha" },
	{ .compatible = "nuvoton,npcm750-sha" },
	{ }
};

U_BOOT_DRIVER(npcm_sha) = {
	.name = "npcm_sha",
	.id = UCLASS_MISC,
	.of_match = npcm_sha_ids,
	.priv_auto = sizeof(struct npcm_sha_priv),
	.bind = npcm_sha_bind,
};
