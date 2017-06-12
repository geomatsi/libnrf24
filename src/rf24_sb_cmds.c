#include <rf24_cmds.h>
#include <rf24_std.h>

#if defined(SPI_SINGLE_BYTE)

static uint8_t
rf24_single_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
{
	uint8_t status;
	int i;

	r->csn(0);
	status = r->spi_xfer(cmd);

	for(i = 0; i < len; i++)
		r->spi_xfer(buf[i]);

	r->csn(1);
	return status;
}

static uint8_t
rf24_single_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
{
	uint8_t status;
	int i;

	r->csn(0);
	status = r->spi_xfer(cmd);

	for(i = 0; i < len; i++)
		buf[i] = r->spi_xfer(0xff);

	r->csn(1);
	return status;
}

static uint8_t
rf24_single_read_register(struct rf24 *r, uint8_t reg)
{
	uint8_t val;

	rf24_single_read_cmd(r, R_REGISTER | (REGISTER_MASK & reg), &val, 1);

	return val;
}

static uint8_t
rf24_single_write_register(struct rf24 *r, uint8_t reg, uint8_t val)
{
	uint8_t status;

	status = rf24_single_write_cmd(r, W_REGISTER | (REGISTER_MASK & reg), &val, 1);

	return status;
}

static uint8_t
rf24_single_write_payload(struct rf24 *r, const void *buf, int len)
{
	const uint8_t *curr = (uint8_t *)buf;
	uint8_t dat_len;
	uint8_t pad_len;
	uint8_t status;

	dat_len = min_t(uint8_t, len, rf24_payload_size(r));
	pad_len = rf24_is_dyn_payload(r) ? 0 : (rf24_payload_size(r) - dat_len);

	r->csn(0);

	if (rf24_is_tx_noack(r))
		status = r->spi_xfer(W_TX_PAYLOAD_NOACK);
	else
		status = r->spi_xfer(W_TX_PAYLOAD);

	while (dat_len--)
		r->spi_xfer(*curr++);

	while (pad_len--)
		r->spi_xfer(0x0);

	r->csn(1);

	return status;
}

static uint8_t
rf24_single_write_ack_payload(struct rf24 *r, int pipe, const void *buf, int len)
{
	const uint8_t *curr = (uint8_t *)buf;
	uint8_t data_len;
	uint8_t status;

	/* invalid pipe number: do nothing */
	if ((pipe < 0) || (pipe > 5))
		return 0xFF;

	data_len = min_t(uint8_t, len, rf24_payload_size(r));

	r->csn(0);

	status = r->spi_xfer(W_ACK_PAYLOAD | (pipe & ACK_PAYLOAD_MASK));
	while (data_len--)
		r->spi_xfer(*curr++);

	r->csn(1);

	return status;
}

static uint8_t
rf24_single_read_payload(struct rf24 *r, void *buf, int len)
{
	uint8_t *curr = (uint8_t*) buf;
	uint8_t dat_len;
	uint8_t pad_len;
	uint8_t status;

	dat_len = min_t(uint8_t, len, rf24_payload_size(r));
	pad_len = rf24_is_dyn_payload(r) ? 0 : (rf24_payload_size(r) - dat_len);

	r->csn(0);

	status = r->spi_xfer(R_RX_PAYLOAD);

	while (dat_len--)
		*curr++ = r->spi_xfer(0xff);

	/* NB: output buffer length is smaller than configured fixed payload,
	 * but we have to read all the FIFO content to avoid corruption
	 */
	while (pad_len--)
		r->spi_xfer(0xff);

	r->csn(1);

	return status;
}

static uint8_t
rf24_single_write_address(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len)
{
	const uint8_t *addr = &buf[len];
	uint8_t status;

	r->csn(0);

	status = r->spi_xfer(W_REGISTER | (REGISTER_MASK & reg));

	while (len--)
		r->spi_xfer(*(--addr));

	r->csn(1);

	return status;
}

static struct rf24_ops rf24_sb_ops = {
	.write_cmd		= rf24_single_write_cmd,
	.read_cmd		= rf24_single_read_cmd,
	.read_register		= rf24_single_read_register,
	.write_register		= rf24_single_write_register,
	.write_payload		= rf24_single_write_payload,
	.write_ack_payload	= rf24_single_write_ack_payload,
	.read_payload		= rf24_single_read_payload,
	.write_address		= rf24_single_write_address,
};

struct rf24_ops *rf24_sb_ops_p = &rf24_sb_ops;

#else	/* SPI_SINGLE_BYTE */

struct rf24_ops *rf24_sb_ops_p = NULL;

#endif	/* SPI_SINGLE_BYTE */
