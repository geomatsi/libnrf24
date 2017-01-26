#include <nRF24L01.h>
#include <RF24.h>

#include <rf24_cmds.h>
#include <rf24_std.h>

#include <rf24delay.h>
#include <rf24log.h>

static struct rf24 *nrf;

void rf24_init(struct rf24 *r)
{
	r->payload_size = RF24_MAX_PAYLOAD_SIZE;
	r->flags = 0;

	nrf = r;
}

void rf24_set_payload_size(struct rf24 *r, int len)
{
	/* invalid input: keep former value */
	if (len < 0)
		return;

	r->payload_size = min_t(uint8_t, len, RF24_MAX_PAYLOAD_SIZE);
}

void rf24_enable_dynamic_payload(struct rf24 *r)
{
	uint8_t val;

	/* FIXME: check ACTIVATE command in legacy write_feature */

	val = rf24_read_register(r, FEATURE);
	rf24_write_register(r, FEATURE, val | FEATURE_EN_DPL);

	/* enable dynamic payload on all pipes at once */
	/* FIXME: do we need mixed pipe configuration ? */

	val = rf24_read_register(r, DYNPD);
	rf24_write_register(r, DYNPD, val | DYNPD_DPL_ALL);

	r->flags |= RF24_DYNAMIC_PAYLOAD;

	/* set max payload size for boundary checks */
	r->payload_size = RF24_MAX_PAYLOAD_SIZE;
}

int rf24_get_dynamic_payload_size(struct rf24 *r)
{
	int len;

	r->csn(0);
	r->spi_xfer(R_RX_PL_WID);
	len = r->spi_xfer(0xff);
	r->csn(1);

	/* sanity check */
	if ((len < 0) || (len > 32))
		len = -1;

	return len;
}

uint8_t rf24_get_status(struct rf24 *r)
{
	return rf24_write_cmd(r, NOP, 0, 0);
}

uint8_t rf24_flush_rx(struct rf24 *r)
{
	return rf24_write_cmd(r, FLUSH_RX, 0, 0);
}

uint8_t rf24_flush_tx(struct rf24 *r)
{
	return rf24_write_cmd(r, FLUSH_TX, 0, 0);
}

void rf24_set_channel(struct rf24 *r, uint8_t channel)
{
	/* invalid input: keep former value */
	if (channel > RF24_MAX_CHANNEL)
		return;

	rf24_write_register(r, RF_CH, channel);
}
