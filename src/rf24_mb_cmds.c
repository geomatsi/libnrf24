#include <rf24_cmds.h>
#include <rf24_std.h>

#if defined(SPI_MULTI_BYTE)

static uint8_t rx[RF24_MAX_PAYLOAD_SIZE + 1];
static uint8_t tx[RF24_MAX_PAYLOAD_SIZE + 1];

static uint8_t
rf24_multi_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t *buf, uint8_t len)
{
	memcpy(&tx[1], buf, len);
	tx[0] = cmd;

	r->csn(0);
	r->spi_multi_xfer(tx, rx, len + 1);
	r->csn(1);

	return rx[0];
}

static uint8_t
rf24_multi_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t *buf, uint8_t len)
{
	memset(tx, 0xff, sizeof(tx));
	tx[0] = cmd;

	r->csn(0);
	r->spi_multi_xfer(tx, rx, len + 1);
	r->csn(1);

	memcpy(buf, &rx[1], len);

	return rx[0];
}

static uint8_t
rf24_multi_read_register(struct rf24 *r, uint8_t reg)
{
	uint8_t val;

	rf24_multi_read_cmd(r, R_REGISTER | (REGISTER_MASK & reg), &val, 1);

	return val;
}

static uint8_t
rf24_multi_write_register(struct rf24 *r, uint8_t reg, uint8_t val)
{
	uint8_t status;

	status = rf24_multi_write_cmd(r, W_REGISTER | (REGISTER_MASK & reg), &val, 1);

	return status;
}

static uint8_t
rf24_multi_write_payload(struct rf24 *r, const void *buf, int len)
{
	uint8_t dat_len;
	uint8_t pad_len;

	dat_len = min_t(uint8_t, len, rf24_payload_size(r));
	pad_len = rf24_is_dyn_payload(r) ? 0 : (rf24_payload_size(r) - dat_len);

	memset(tx, 0x0, sizeof(tx));
	memcpy(&tx[1], buf, dat_len);

	if (rf24_is_tx_noack(r))
		tx[0] = W_TX_PAYLOAD_NOACK;
	else
		tx[0] = W_TX_PAYLOAD;

	r->csn(0);
	r->spi_multi_xfer(tx, rx, dat_len + pad_len + 1);
	r->csn(1);

	return rx[0];
}

static uint8_t
rf24_multi_write_ack_payload(struct rf24 *r, int pipe, const void *buf, int len)
{
	uint8_t dat_len;

	/* invalid pipe number: do nothing */
	if ((pipe < 0) || (pipe > 5))
		return 0xFF;

	dat_len = min_t(uint8_t, len, rf24_payload_size(r));

	memcpy(&tx[1], buf, dat_len);
	tx[0] = W_ACK_PAYLOAD | (pipe & ACK_PAYLOAD_MASK);

	r->csn(0);
	r->spi_multi_xfer(tx, rx, dat_len + 1);
	r->csn(1);

	return rx[0];
}

static uint8_t
rf24_multi_read_payload(struct rf24 *r, void *buf, int len)
{
	uint8_t dat_len;
	uint8_t pad_len;

	dat_len = min_t(uint8_t, len, rf24_payload_size(r));
	pad_len = rf24_is_dyn_payload(r) ? 0 : (rf24_payload_size(r) - dat_len);

	memset(tx, 0xff, sizeof(tx));
	tx[0] = R_RX_PAYLOAD;

	r->csn(0);
	r->spi_multi_xfer(tx, rx, dat_len + pad_len + 1);
	r->csn(1);

	memcpy(buf, &rx[1], dat_len);

	return rx[0];
}

static uint8_t
rf24_multi_write_address(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len)
{
	const uint8_t *addr = &buf[len];
	int pos;

	tx[0] = W_REGISTER | (REGISTER_MASK & reg);

	for (pos = 0; pos < len; pos++)
		tx[pos + 1] = *(--addr);

	r->csn(0);
	r->spi_multi_xfer(tx, rx, len + 1);
	r->csn(1);

	return rx[0];
}

struct rf24_ops rf24_mb_ops = {
	.write_cmd		= rf24_multi_write_cmd,
	.read_cmd		= rf24_multi_read_cmd,
	.read_register		= rf24_multi_read_register,
	.write_register		= rf24_multi_write_register,
	.write_payload		= rf24_multi_write_payload,
	.write_ack_payload	= rf24_multi_write_ack_payload,
	.read_payload		= rf24_multi_read_payload,
	.write_address		= rf24_multi_write_address,
};

#endif	/* SPI_MULTI_BYTE */
