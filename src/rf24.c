#include <nRF24L01.h>
#include <RF24.h>

#include <rf24_cmds.h>
#include <rf24_std.h>

#include <rf24delay.h>
#include <rf24log.h>

static struct rf24 *nrf;

static void no_pin(int level)
{

}

void rf24_init(struct rf24 *r)
{
	int i;

	nrf = r;

	if (!nrf->csn)
		nrf->csn = no_pin;

	if (!nrf->ce)
		nrf->ce = no_pin;

	nrf->payload_size = RF24_MAX_PAYLOAD_SIZE;
	nrf->flags = 0;

	for (i = 0; i < RF24_MAX_ADDR_LEN; i++) {
		nrf->p0_tx_addr[i] = 0;
		nrf->p0_rx_addr[i] = 0;
	}

	r->ce(0);
	delay_ms(10);

	rf24_activate_features(nrf);

	/* default settings:
	 *   - automatic retransmit delay: 1500 uS
	 *   - automatic retransmit count ARC 15
	 *   - data rate: 1 Mbs
	 *   - crc: 16 bits
	 *   - channel: 76
	 *   - fixed payload size: 32 bits (max payload size)
	 *   - auto ack enabled
	 *   - dynamic payload: disabled
	 */
	rf24_set_retries(nrf, BIN(0100), BIN(1111));
	rf24_set_pa_level(nrf, RF24_PA_MAX);
	rf24_set_data_rate(nrf, RF24_RATE_1M);
	rf24_set_crc_mode(nrf, RF24_CRC_16_BITS);
	rf24_set_channel(nrf, 76);
	rf24_set_payload_size(nrf, RF24_MAX_PAYLOAD_SIZE);
	/* disable dynamic payload */
	rf24_write_register(nrf, DYNPD, 0);

	/* reset current status */
	rf24_write_register(nrf, STATUS, STATUS_CLEAR);

	/* flush buffers */
	rf24_flush_rx(r);
	rf24_flush_tx(r);
}

void rf24_set_payload_size(struct rf24 *r, int len)
{
	/* invalid input: keep former value */
	if (len < 0)
		return;

	r->payload_size = min_t(uint8_t, len, RF24_MAX_PAYLOAD_SIZE);
}

void rf24_enable_dyn_payload(struct rf24 *r)
{
	uint8_t val;

	val = rf24_read_register(r, FEATURE);
	rf24_write_register(r, FEATURE, val | FEATURE_EN_DPL);

	/* enable dynamic payload on all pipes at once */
	/* FIXME: do we need mixed pipe configuration ? */

	val = rf24_read_register(r, DYNPD);
	rf24_write_register(r, DYNPD, val | DYNPD_DPL_ALL);

	r->flags |= RF24_DYN_PAYLOAD;

	/* set max payload size for boundary checks */
	r->payload_size = RF24_MAX_PAYLOAD_SIZE;
}

uint8_t rf24_get_dyn_payload_size(struct rf24 *r)
{
	uint8_t len;

	rf24_read_cmd(r, R_RX_PL_WID, &len, 1);

	/* sanity check */
	if (len > 32)
		len = 0xff;

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

void rf24_set_data_rate(struct rf24 *r, enum rf24_data_rate rate)
{
	uint8_t value;

	value = rf24_read_register(r, RF_SETUP) ;
	value &= ~(RF_SETUP_RF_DR_LOW | RF_SETUP_RF_DR_HIGH);

	/* use 'if' instead of 'switch' to save memory */

	if (rate == RF24_RATE_250K) {
		value |= RF_SETUP_RF_DR_LOW;
	} else if (rate == RF24_RATE_2M) {
		value |= RF_SETUP_RF_DR_HIGH;
	} else if (rate == RF24_RATE_1M) {
		/* do nothing: DR bits already cleared */
	} else {
		/* invalid input: keep former value */
		return;
	}

	rf24_write_register(r, RF_SETUP, value);
}

enum rf24_data_rate rf24_get_data_rate(struct rf24 *r)
{
	uint8_t reg;

	reg = rf24_read_register(r, RF_SETUP);
	reg &= (RF_SETUP_RF_DR_HIGH | RF_SETUP_RF_DR_LOW);

	if (reg == 0)
		return RF24_RATE_1M;

	if (reg ==  RF_SETUP_RF_DR_HIGH)
		return RF24_RATE_2M;

	if (reg == RF_SETUP_RF_DR_LOW)
		return RF24_RATE_250K;

	return RESERVED;
}

void rf24_set_pa_level(struct rf24 *r, enum rf24_pa_level level)
{
	uint8_t value;

	value = rf24_read_register(r, RF_SETUP) ;
	value &= ~(RF_SETUP_RF_PWR_MASK << RF_SETUP_RF_PWR_SHIFT);

	/* use 'if' instead of 'switch' to save memory */

	if (level == RF24_PA_M18DBM || level == RF24_PA_MIN) {
		value |= RF_SETUP_RF_PWR_VAL(0);
	} else if (level == RF24_PA_M12DBM) {
		value |= RF_SETUP_RF_PWR_VAL(1);
	} else if (level == RF24_PA_M06DBM) {
		value |= RF_SETUP_RF_PWR_VAL(2);
	} else if (level == RF24_PA_M00DBM || level == RF24_PA_MAX) {
		value |= RF_SETUP_RF_PWR_VAL(3);
	} else {
		/* invalid input: keep former value */
		return;
	}

	rf24_write_register(r, RF_SETUP, value);
}

enum rf24_pa_level rf24_get_pa_level(struct rf24 *r)
{
	uint8_t reg;

	reg = rf24_read_register(r, RF_SETUP);
	reg &= (RF_SETUP_RF_PWR_MASK << RF_SETUP_RF_PWR_SHIFT);

	if (reg == RF_SETUP_RF_PWR_VAL(0))
		return RF24_PA_M18DBM;

	if (reg == RF_SETUP_RF_PWR_VAL(1))
		return RF24_PA_M12DBM;

	if (reg == RF_SETUP_RF_PWR_VAL(2))
		return RF24_PA_M06DBM;

	/* remaining option: RF_SETUP_PWR_VAL(3)) */
	return RF24_PA_M00DBM;
}

void rf24_enable_tx_noack(struct rf24 *r)
{
	uint8_t val;

	val = rf24_read_register(r, FEATURE);
	val |= FEATURE_EN_DYN_ACK;
	rf24_write_register(r, FEATURE, val);

	r->flags |= RF24_TX_NOACK;
}

void rf24_disable_tx_noack(struct rf24 *r)
{
	uint8_t val;

	val = rf24_read_register(r, FEATURE);
	val &= ~FEATURE_EN_DYN_ACK;
	rf24_write_register(r, FEATURE, val);

	r->flags &= ~RF24_TX_NOACK;
}

/* From nRF24L01+ datasheet:
 * If ACK packet payload is activated, ACK packets have dynamic payload lengths
 * and the Dynamic Payload Length feature should be enabled for pipe 0 on the PTX and PRX.
 * This is to ensure that they receive the ACK packets with payloads.
 * If the ACK payload is more than 15 byte in 2Mbps mode the ARD must be 500μS or more,
 * and if the ACK payload is more than 5 byte in 1Mbps mode the ARD must be 500μS or more.
 * In 250kbps mode (even when the payload is not in ACK) the ARD must be 500μS or more.
 */
void rf24_enable_ack_payload(struct rf24 *r)
{
	uint8_t val;

	val = rf24_read_register(r, FEATURE);
	val |= (FEATURE_EN_ACK_PAY | FEATURE_EN_DPL);
	rf24_write_register(r, FEATURE, val);

	val = rf24_read_register(r, DYNPD);
	val |= (DYNPD_DPL_P0 | DYNPD_DPL_P1);
	rf24_write_register(r, DYNPD, val);

	r->flags |= RF24_ACK_PAYLOAD;
}

void rf24_set_auto_ack_all(struct rf24 *r, int enable)
{
	if (enable)
		rf24_write_register(r, EN_AA, EN_AA_ENAA_ALL);
	else
		rf24_write_register(r, EN_AA, 0);
}

void rf24_set_auto_ack_pipe(struct rf24 *r, int pipe, int enable)
{
	uint8_t val;

	if ((pipe >= 0) && (pipe <=5)) {
		val = rf24_read_register(r, EN_AA);

		if(enable)
			val |= EN_AA_ENAA_P(pipe);
		else
			val &= ~EN_AA_ENAA_P(pipe);

		rf24_write_register(r, EN_AA, val);
	}
}

uint8_t rf24_get_carrier(struct rf24 *r)
{
	return rf24_read_register(r, RPD);
}

void rf24_setup_ptx(struct rf24 *r, uint8_t *addr)
{
	int i;

	rf24_write_address(r, RX_ADDR_P0, addr, RF24_MAX_ADDR_LEN);
	rf24_write_address(r, TX_ADDR, addr, RF24_MAX_ADDR_LEN);

	/* cache tx address */
	for (i = 0; i < RF24_MAX_ADDR_LEN; i++)
		r->p0_tx_addr[i] = addr[i];

	if (!rf24_is_dyn_payload(r))
		rf24_write_register(r, RX_PW_P0, r->payload_size);
}

void rf24_start_ptx(struct rf24 *r)
{
	uint8_t reg;

	r->ce(0);
	rf24_power_down(r);

	reg = rf24_read_register(r, CONFIG);
	reg &= ~CONFIG_PRIM_RX;
	rf24_write_register(r, CONFIG, reg);

	/* reset current status */
	rf24_write_register(nrf, STATUS, STATUS_CLEAR);

	/* Setup proper TX and RX0 addresses when switch to PTX:
	 * it is possible that they could have been changed
	 * during switching between PTX and PRX.
	 */
	rf24_write_address(r, RX_ADDR_P0, r->p0_rx_addr, RF24_MAX_ADDR_LEN);
	rf24_write_address(r, TX_ADDR, r->p0_tx_addr, RF24_MAX_ADDR_LEN);

	rf24_power_up(r);
	r->ce(0);
}

void rf24_setup_prx(struct rf24 *r, int pipe, uint8_t *addr)
{
	uint8_t reg;
	int i;

	/* invalid pipe number: do nothing */
	if ((pipe < 0) || (pipe > 5))
		return;

	/* cache rx0 address */
	if (pipe == 0)
		for (i = 0; i < RF24_MAX_ADDR_LEN; i++)
			r->p0_rx_addr[i] = addr[i];

	/* From nRF24L01+ spec 7.7:
	 * Pipes 1-5 share 4 most significant address bytes.
	 * The LSByte must be unique for all six pipes.
	 */
	rf24_write_address(r, RX_ADDR_P(pipe), addr, (pipe < 2) ? RF24_MAX_ADDR_LEN : 1);

	if (!rf24_is_dyn_payload(r))
		rf24_write_register(r, RX_PW_P(pipe), r->payload_size);

	reg = rf24_read_register(r, EN_RXADDR);
	rf24_write_register(r, EN_RXADDR, reg | EN_RXADDR_ERX_P(pipe));
}

void rf24_start_prx(struct rf24 *r)
{
	uint8_t reg;

	r->ce(0);
	rf24_power_down(r);

	reg = rf24_read_register(r, CONFIG);
	rf24_write_register(r, CONFIG, reg | CONFIG_PRIM_RX);

	/* reset current status */
	rf24_write_register(nrf, STATUS, STATUS_CLEAR);

	/* Setup proper TX and RX0 addresses when switch to PRX:
	 * it is possible that they could have been changed
	 * during switching between PRX and PTX.
	 *
	 * Note: p1..p5 addresses are not modified in PTX mode.
	 */
	rf24_write_address(r, RX_ADDR_P0, r->p0_rx_addr, RF24_MAX_ADDR_LEN);


	rf24_power_up(r);
	r->ce(1);

	/* from nRF24L01+ spec. Fig.3 (Radio control state diagram):
	 * RX setting takes ~130us
	 */
	delay_us(200);
}

int rf24_rx_ready(struct rf24 *r, int *ppipe)
{
	uint8_t rx_ready;
	uint8_t status;

	status = rf24_get_status(r);
	rx_ready = (status & STATUS_RX_DR) ? 1 : 0;

	if (rx_ready && ppipe)
		*ppipe = STATUS_RX_P_NO(status);

	return rx_ready;
}

enum rf24_rx_status rf24_rx_pipe_check(struct rf24 *r, int pipe)
{
	if ((pipe >= 0) && (pipe <= RF24_MAX_PIPE))
		return RF24_RX_OK;

	if (pipe == (RF24_MAX_PIPE + 2))
		return RF24_RX_EMPTY;

	return RF24_RX_EINVAL;
}

/* FIXME: make sure that buf has enough space for dynamic payload */
enum rf24_rx_status rf24_recv(struct rf24 *r, void *buf, int len)
{
	uint8_t status;
	uint8_t reg;

	if (!buf)
		return RF24_RX_EINVAL;

	reg = rf24_read_register(r, FIFO_STATUS);
	if (reg & FIFO_STATUS_RX_EMPTY)
		return RF24_RX_EMPTY;

	status = rf24_read_payload(r, buf, len);

	/* clear RX_DR bit */
	if (status & STATUS_RX_DR)
		rf24_write_register(r, STATUS, STATUS_RX_DR);

	return RF24_RX_OK;
}

enum rf24_tx_status rf24_send_async(struct rf24 *r, void *buf, int len)
{
	uint8_t reg;

	if (!buf)
		return RF24_TX_EINVAL;

	reg = rf24_read_register(r, FIFO_STATUS);
	if (reg & FIFO_STATUS_TX_FULL)
		return RF24_TX_FULL;

	rf24_write_payload(r, buf, len);

	reg = rf24_read_register(r, FIFO_STATUS);
	if (reg & FIFO_STATUS_TX_EMPTY)
		return RF24_TX_EIO;

	/* From nRF24L01+ spec 6.1.5:
	 * The TX mode is an active mode for transmitting packets. To enter this
	 * mode, the nRF24L01+ must have PWR_UP bit set high, PRIM_RX bit set low,
	 * a payload in the TX FIFO and a high pulse on the CE for more than 10 us.
	 */
	r->ce(1);
	delay_us(20);
	r->ce(0);

	return RF24_TX_OK;

}

enum rf24_tx_status rf24_tx_done(struct rf24 *r)
{
	uint8_t status;

	do {
		status = rf24_get_status(r);
	} while (!(status & (STATUS_TX_DS | STATUS_MAX_RT)));

	/* clear TX_DS and/or MAX_RT bits */
	rf24_write_register(r, STATUS, status & (STATUS_TX_DS | STATUS_MAX_RT));

	/* TX failure: max retransmit count reached */
	if (status & STATUS_MAX_RT)
		return RF24_TX_MAX_RT;

	return RF24_TX_OK;
}

enum rf24_tx_status rf24_send(struct rf24 *r, void *buf, int len)
{
	enum rf24_tx_status ret;

	ret = rf24_send_async(r, buf, len);
	if (ret != RF24_TX_OK)
		return ret;

	return rf24_tx_done(r);
}
