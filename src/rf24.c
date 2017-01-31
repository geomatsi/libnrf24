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

	len = rf24_read_cmd(r, R_RX_PL_WID, (uint8_t *)&len, 1);

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

void rf24_set_crc_mode(struct rf24 *r, enum rf24_crc_mode mode)
{
	uint8_t reg;

	reg = rf24_read_register(r, CONFIG);
	reg &= ~(CONFIG_EN_CRC | CONFIG_CRCO);

	/* use 'if' instead of 'switch' to save memory */
	if (mode == RF24_CRC_NONE) {
		/* ready to go */
	} else if (mode == RF24_CRC_8_BITS) {
		reg |= CONFIG_EN_CRC;
	} else if (mode == RF24_CRC_16_BITS) {
		reg |= (CONFIG_EN_CRC | CONFIG_CRCO);
	} else {
		/* should not be here: do nothing */
		return;
	}

	rf24_write_register(r, CONFIG, reg);
}

enum rf24_crc_mode rf24_get_crc_mode(struct rf24 *r)
{
	uint8_t reg;

	reg = rf24_read_register(r, CONFIG);

	if ((reg & CONFIG_EN_CRC) == 0)
		return RF24_CRC_NONE;

	if ((reg & CONFIG_CRCO) == 0)
		return RF24_CRC_8_BITS;

	return RF24_CRC_16_BITS;
}

void rf24_set_retries(struct rf24 *r, uint8_t ard, uint8_t arc)
{
	uint8_t val = 0;

	val |= SETUP_RETR_ARC_VAL(arc & SETUP_RETR_ARC_MASK);
	val |= SETUP_RETR_ARD_VAL(ard & SETUP_RETR_ARD_MASK);

	rf24_write_register(r, SETUP_RETR, val);
}

void rf24_power_up(struct rf24 *r)
{
	uint8_t reg;

	reg = rf24_read_register(r, CONFIG);

	if ((reg & CONFIG_PWR_UP) == 0) {
		reg |= CONFIG_PWR_UP;
		rf24_write_register(r, CONFIG, reg);

		/* nRF24L01+ spec: max start-up time 1.5ms */
		delay_ms(2);
	}
}

void rf24_power_down(struct rf24 *r)
{
	uint8_t reg;

	reg = rf24_read_register(r, CONFIG);

	if (reg & CONFIG_PWR_UP) {
		reg &= ~CONFIG_PWR_UP;
		rf24_write_register(r, CONFIG, reg);
	}
}

/* Feature activation/deactivation is relevant for nRF24L01 only:
 *   - R_RX_PL_WID
 *   - W_ACK_PAYLOAD
 *   - W_TX_PAYLOAD_NOACK
 *
 * Note: this command should be executed in
 * power down or stand by modes only.
 */

void rf24_activate_features(struct rf24 *r)
{
	uint8_t data = ACTIVATE_DATA;
	uint8_t v1, v2;

	v1 = rf24_read_register(r, FEATURE);
	rf24_write_register(r, FEATURE, (~v1) & FEATURE_EN_ALL);
	v2 = rf24_read_register(r, FEATURE);

	if (v1 == v2) {
		/* activate features */
		rf24_write_cmd(r, ACTIVATE, &data, 1);
	} else {
		/* features are already active: restore original value */
		rf24_write_register(r, FEATURE, v1);
	}
}

void rf24_deactivate_features(struct rf24 *r)
{
	uint8_t data = ACTIVATE_DATA;
	uint8_t v1, v2;

	v1 = rf24_read_register(r, FEATURE);
	rf24_write_register(r, FEATURE, (~v1) & FEATURE_EN_ALL);
	v2 = rf24_read_register(r, FEATURE);

	if (v1 != v2) {
		/* reset and deactivate features */
		rf24_write_register(r, FEATURE, 0x0);
		rf24_write_cmd(r, ACTIVATE, &data, 1);
	}
}
