#include <rf24_cmds.h>

uint8_t rf24_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
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

uint8_t rf24_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
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

uint8_t rf24_read_register(struct rf24 *r, uint8_t reg)
{
	uint8_t val;

	rf24_read_cmd(r, R_REGISTER | (REGISTER_MASK & reg), &val, 1);

	return val;
}

uint8_t rf24_write_register(struct rf24 *r, uint8_t reg, uint8_t val)
{
	uint8_t status;

	status = rf24_write_cmd(r, W_REGISTER | (REGISTER_MASK & reg), &val, 1);

	return status;
}

uint8_t rf24_write_payload(struct rf24 *r, const void *buf, int len)
{
	const uint8_t* curr = (uint8_t *)buf;
	uint8_t dat_len;
	uint8_t pad_len;
	uint8_t status;

	dat_len = min_t(uint8_t, len, rf24_payload_size(r));
	pad_len = rf24_is_dynamic_payload(r) ? 0 : (rf24_payload_size(r) - dat_len);

	r->csn(0);

	status = r->spi_xfer(W_TX_PAYLOAD);

	while (dat_len--)
		r->spi_xfer(*curr++);

	while (pad_len--)
		r->spi_xfer(0x0);

	r->csn(1);

	return status;
}

uint8_t rf24_read_payload(struct rf24 *r, const void *buf, int len)
{
	uint8_t* curr = (uint8_t*) buf;
	uint8_t dat_len;
	uint8_t pad_len;
	uint8_t status;

	dat_len = min_t(uint8_t, len, rf24_payload_size(r));
	pad_len = rf24_is_dynamic_payload(r) ? 0 : (rf24_payload_size(r) - dat_len);

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

uint8_t rf24_write_address(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len)
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
