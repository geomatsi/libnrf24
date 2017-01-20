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
