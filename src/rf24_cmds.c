#include <rf24_cmds.h>

uint8_t rf24_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
{
	return -1;
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
