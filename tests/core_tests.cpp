#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <cstring>

#include <cmd_mock_rf24.h>

TEST_GROUP(core)
{
	struct rf24 *pnrf24;
	struct rf24 nrf24;

	void setup()
	{
		mock().disable();

		memset(&nrf24, 0x0, sizeof(nrf24));
		nrf24.csn = mock_csn;
		nrf24.ce = mock_ce;
		nrf24.spi_xfer = mock_spi_xfer_sbyte;

		pnrf24 = &nrf24;
		rf24_init(pnrf24);

		mock().enable();
		mock().clear();
		mock().strictOrder();
	}

	void teardown()
	{
		mock().clear();
	}
};

TEST(core, init)
{
	mock().disable();

	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, pnrf24->payload_size);

	POINTERS_EQUAL(mock_spi_xfer_sbyte, pnrf24->spi_xfer);
	POINTERS_EQUAL(mock_csn, pnrf24->csn);
	POINTERS_EQUAL(mock_ce, pnrf24->ce);
}

TEST(core, init_empty_methods)
{
	struct rf24 empty;

	mock().disable();
	memset(&empty, 0x0, sizeof(empty));
	rf24_init(&empty);

	CHECK_FALSE(empty.csn == 0);
	CHECK_FALSE(empty.ce == 0);
	CHECK(empty.spi_xfer == 0);
}

TEST(core, fixed_payload)
{
	rf24_set_payload_size(pnrf24, -1);
	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, rf24_payload_size(pnrf24));

	rf24_set_payload_size(pnrf24, 0);
	CHECK_EQUAL(0, rf24_payload_size(pnrf24));

	rf24_set_payload_size(pnrf24, 1);
	CHECK_EQUAL(1, rf24_payload_size(pnrf24));

	rf24_set_payload_size(pnrf24, 20);
	CHECK_EQUAL(20, rf24_payload_size(pnrf24));

	rf24_set_payload_size(pnrf24, -100);
	CHECK_EQUAL(20, rf24_payload_size(pnrf24));

	rf24_set_payload_size(pnrf24, RF24_MAX_PAYLOAD_SIZE + 1);
	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, rf24_payload_size(pnrf24));
}

TEST(core, enable_dynamic_payload)
{
	mock().disable();

	CHECK_FALSE(rf24_is_dyn_payload(pnrf24));
	rf24_enable_dyn_payload(pnrf24);
	CHECK_TRUE(rf24_is_dyn_payload(pnrf24));
	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, rf24_payload_size(pnrf24));
}

TEST(core, get_dynamic_payload_sunny)
{
	int expected_len = 10;
	int actual_len;

	/* sunny case: read valid length from nRF24 chip */

	mock().setData("read", expected_len);

	mock()
		.expectOneCall("rf24_read_cmd")
		.withParameter("cmd", R_RX_PL_WID);

	actual_len = rf24_get_dyn_payload_size(pnrf24);
	CHECK_EQUAL(expected_len, actual_len);

	mock().checkExpectations();
}

TEST(core, get_dynamic_payload_error)
{
	int expected_len = RF24_MAX_PAYLOAD_SIZE * 2;
	int actual_len;

	/* error case: read invalid length from nRF24 chip */

	mock().setData("read", expected_len);

	mock()
		.expectOneCall("rf24_read_cmd")
		.withParameter("cmd", R_RX_PL_WID);

	actual_len = rf24_get_dyn_payload_size(pnrf24);
	CHECK_EQUAL(0xff, actual_len);

	mock().checkExpectations();
}

TEST(core, rf24_fluxh_rx)
{
	int expected_status = 0xe;
	int actual_status;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_RX_DR);

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", FLUSH_RX)
		.andReturnValue(expected_status);

	actual_status = rf24_flush_rx(pnrf24);
	CHECK_EQUAL(expected_status, actual_status);

	mock().checkExpectations();
}

TEST(core, rf24_fluxh_tx)
{
	int expected_status = 0xe;
	int actual_status;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_TX_DS | STATUS_MAX_RT);

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", FLUSH_TX)
		.andReturnValue(expected_status);

	actual_status = rf24_flush_tx(pnrf24);
	CHECK_EQUAL(expected_status, actual_status);

	mock().checkExpectations();
}

TEST(core, rf24_get_status)
{
	int expected_status = 0xe;
	int actual_status;

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", NOP)
		.andReturnValue(expected_status);

	actual_status = rf24_get_status(pnrf24);
	CHECK_EQUAL(expected_status, actual_status);

	mock().checkExpectations();
}

TEST(core, rf24_set_channel_valid)
{
	int channel = 0x10;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", RF_CH)
		.withParameter("val", channel)
		.andReturnValue(0xe);

	rf24_set_channel(pnrf24, channel);

	mock().checkExpectations();
}

TEST(core, rf24_set_channel_invalid)
{
	int channel = RF24_MAX_CHANNEL + 1;

	mock().expectNoCall("rf24_write_register");

	rf24_set_channel(pnrf24, channel);

	mock().checkExpectations();
}

TEST(core, rf24_get_channel_ok)
{
	uint8_t ech;
	uint8_t ach;

	for (ech = 0; ech <= RF24_MAX_CHANNEL; ech++) {
		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", RF_CH)
			.andReturnValue(ech);

		ach = rf24_get_channel(pnrf24);

		CHECK_EQUAL(ech, ach);
		mock().checkExpectations();
		mock().clear();
	}
}

TEST(core, rf24_get_channel_fail)
{
	uint8_t ech;
	uint8_t ach;

	for (ech = RF24_MAX_CHANNEL + 1; ech != 0; ech++) {
		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", RF_CH)
			.andReturnValue(ech);

		ach = rf24_get_channel(pnrf24);

		CHECK_EQUAL(0xff, ach);
		mock().checkExpectations();
		mock().clear();
	}
}

TEST(core, rf24_set_crc_mode_none)
{
	uint8_t config = 0xFF;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(config);

	config &= ~(CONFIG_EN_CRC | CONFIG_CRCO);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", config)
		.andReturnValue(0xe);

	rf24_set_crc_mode(pnrf24, RF24_CRC_NONE);

	mock().checkExpectations();
}

TEST(core, rf24_set_crc_mode_8_bits)
{
	uint8_t config = 0x0;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(config);

	config |= CONFIG_EN_CRC;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", config)
		.andReturnValue(0xe);

	rf24_set_crc_mode(pnrf24, RF24_CRC_8_BITS);

	mock().checkExpectations();
}

TEST(core, rf24_set_crc_mode_16_bits)
{
	uint8_t config = 0x0;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(config);

	config |= (CONFIG_EN_CRC | CONFIG_CRCO);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", config)
		.andReturnValue(0xe);

	rf24_set_crc_mode(pnrf24, RF24_CRC_16_BITS);

	mock().checkExpectations();
}

TEST(core, rf24_get_crc_mode)
{
	uint8_t reg[] = {
		0x0,
		CONFIG_CRCO,
		CONFIG_EN_CRC,
		CONFIG_EN_CRC | CONFIG_CRCO,
	};

	enum rf24_crc_mode  mode[] = {
		RF24_CRC_NONE,
		RF24_CRC_NONE,
		RF24_CRC_8_BITS,
		RF24_CRC_16_BITS,
	};

	enum rf24_crc_mode ret;

	// TODO: add compile time assert to check sizeof(reg) == sizeof(mode)

	for (unsigned int i = 0; i < sizeof(reg); i++) {

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", CONFIG)
			.andReturnValue(reg[i]);

		ret = rf24_get_crc_mode(pnrf24);

		CHECK_EQUAL(mode[i], ret);
		mock().checkExpectations();
		mock().clear();
	}
}

TEST(core, rf24_set_retries)
{
	uint8_t ard[] = {0x00, 0x01, 0x11, 0x01, 0x11};
	uint8_t arc[] = {0x00, 0x02, 0x02, 0x22, 0x22};
	uint8_t value;

	// TODO: add compile time assert to check sizeof(reg) == sizeof(mode)

	for (unsigned int i = 0; i < sizeof(ard); i++) {

		value = SETUP_RETR_ARC_VAL(arc[i] & SETUP_RETR_ARC_MASK) |
			SETUP_RETR_ARD_VAL(ard[i] & SETUP_RETR_ARD_MASK);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", SETUP_RETR)
			.withParameter("val", value);

		rf24_set_retries(pnrf24, ard[i], arc[i]);

		mock().checkExpectations();
		mock().clear();
	}
}

TEST(core, rf24_power_up_cold)
{
	uint8_t value = 0x0;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(value);

	value |= CONFIG_PWR_UP;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", value);

	mock()
		.expectOneCall("delay_ms")
		.withParameter("msec", 2);

	rf24_power_up(pnrf24);

	mock().checkExpectations();
}

TEST(core, rf24_power_up_warm)
{
	uint8_t value = CONFIG_PWR_UP;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(value);

	rf24_power_up(pnrf24);

	mock().checkExpectations();
}

TEST(core, rf24_power_down_1)
{
	uint8_t value = CONFIG_PWR_UP;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(value);

	value &= ~CONFIG_PWR_UP;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", value);

	rf24_power_down(pnrf24);

	mock().checkExpectations();
}

TEST(core, rf24_power_down_2)
{
	uint8_t value = 0x0;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(value);

	rf24_power_down(pnrf24);

	mock().checkExpectations();
}

TEST(core, rf24_activate_features_when_disabled)
{
	uint8_t v1 = 0x0;
	uint8_t v2;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(v1);

	v2 = (~v1) & FEATURE_EN_ALL;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", FEATURE)
		.withParameter("val", v2);

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(v1);

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", ACTIVATE);

	rf24_activate_features(pnrf24);

	mock().checkExpectations();
}

TEST(core, rf24_activate_features_when_enabled)
{
	uint8_t v1 = 0x1;
	uint8_t v2;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(v1);

	v2 = (~v1) & FEATURE_EN_ALL;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", FEATURE)
		.withParameter("val", v2);

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(v2);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", FEATURE)
		.withParameter("val", v1);

	rf24_activate_features(pnrf24);

	mock().checkExpectations();
}

TEST(core, rf24_deactivate_features_when_enabled)
{
	uint8_t v1 = 0x0;
	uint8_t v2;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(v1);

	v2 = (~v1) & FEATURE_EN_ALL;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", FEATURE)
		.withParameter("val", v2);

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(v2);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", FEATURE)
		.withParameter("val", 0x0);

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", ACTIVATE);

	rf24_deactivate_features(pnrf24);

	mock().checkExpectations();
}

TEST(core, rf24_deactivate_features_when_disabled)
{
	uint8_t v1 = 0x1;
	uint8_t v2;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(v1);

	v2 = (~v1) & FEATURE_EN_ALL;

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", FEATURE)
		.withParameter("val", v2);

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(v1);

	rf24_deactivate_features(pnrf24);

	mock().checkExpectations();
}

TEST(core, rf24_set_data_rate_valid)
{
	enum rf24_data_rate rate[] = {
		RF24_RATE_1M,
		RF24_RATE_2M,
		RF24_RATE_250K,
	};

	uint8_t reg[] = {
		0,
		RF_SETUP_RF_DR_HIGH,
		RF_SETUP_RF_DR_LOW,
	};

	uint8_t value;

	// TODO: add compile time assert to check sizeof(reg) == sizeof(mode)

	for (unsigned int i = 0; i < sizeof(reg); i++) {

		value = ~0;

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", RF_SETUP)
			.andReturnValue(value);

		value &= ~(RF_SETUP_RF_DR_LOW | RF_SETUP_RF_DR_HIGH);
		value |= reg[i];

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", RF_SETUP)
			.withParameter("val", value);

		rf24_set_data_rate(pnrf24, rate[i]);

		mock().checkExpectations();
		mock().clear();
	}
};

TEST(core, rf24_set_data_rate_invalid)
{
	/* invalid rate */
	enum rf24_data_rate rate = RF24_RATE_RES;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", RF_SETUP);

	rf24_set_data_rate(pnrf24, rate);

	mock().checkExpectations();
};

TEST(core, rf24_get_data_rate)
{
	enum rf24_data_rate expected_rate[] = {
		RF24_RATE_1M,
		RF24_RATE_2M,
		RF24_RATE_250K,
		RF24_RATE_RES,
	};

	uint8_t reg[] = {
		0,
		RF_SETUP_RF_DR_HIGH,
		RF_SETUP_RF_DR_LOW,
		RF_SETUP_RF_DR_HIGH | RF_SETUP_RF_DR_LOW,
	};

	enum rf24_data_rate actual_rate;

	// TODO: add compile time assert to check sizeof(reg) == sizeof(mode)

	for (unsigned int i = 0; i < sizeof(reg); i++) {

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", RF_SETUP)
			.andReturnValue(reg[i]);

		actual_rate = rf24_get_data_rate(pnrf24);

		CHECK_EQUAL(expected_rate[i], actual_rate);
		mock().checkExpectations();
		mock().clear();
	}
}

TEST(core, rf24_set_pa_level_valid)
{
	enum rf24_pa_level level[] = {
		RF24_PA_MIN,
		RF24_PA_M18DBM,
		RF24_PA_M12DBM,
		RF24_PA_M06DBM,
		RF24_PA_M00DBM,
		RF24_PA_MAX,
	};

	uint8_t reg[] = {
		RF_SETUP_RF_PWR_VAL(0),
		RF_SETUP_RF_PWR_VAL(0),
		RF_SETUP_RF_PWR_VAL(1),
		RF_SETUP_RF_PWR_VAL(2),
		RF_SETUP_RF_PWR_VAL(3),
		RF_SETUP_RF_PWR_VAL(3),
	};

	uint8_t value;

	// TODO: add compile time assert to check sizeof(reg) == sizeof(mode)

	for (unsigned int i = 0; i < sizeof(reg); i++) {

		value = ~0;

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", RF_SETUP)
			.andReturnValue(value);

		value &= ~(RF_SETUP_RF_PWR_MASK << RF_SETUP_RF_PWR_SHIFT);
		value |= reg[i];

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", RF_SETUP)
			.withParameter("val", value);

		rf24_set_pa_level(pnrf24, level[i]);

		mock().checkExpectations();
		mock().clear();
	}
};

TEST(core, rf24_set_pa_level_invalid)
{
	/* invalid rate */
	enum rf24_pa_level level = (enum rf24_pa_level)100;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", RF_SETUP);

	rf24_set_pa_level(pnrf24, level);

	mock().checkExpectations();
};

TEST(core, rf24_get_pa_level)
{
	enum rf24_pa_level expected_level[] = {
		RF24_PA_M18DBM,
		RF24_PA_M12DBM,
		RF24_PA_M06DBM,
		RF24_PA_M00DBM,
	};

	uint8_t reg[] = {
		RF_SETUP_RF_PWR_VAL(0),
		RF_SETUP_RF_PWR_VAL(1),
		RF_SETUP_RF_PWR_VAL(2),
		RF_SETUP_RF_PWR_VAL(3),
	};

	enum rf24_pa_level actual_level;

	// TODO: add compile time assert to check sizeof(reg) == sizeof(mode)

	for (unsigned int i = 0; i < sizeof(reg); i++) {

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", RF_SETUP)
			.andReturnValue(reg[i]);

		actual_level = rf24_get_pa_level(pnrf24);

		CHECK_EQUAL(expected_level[i], actual_level);
		mock().checkExpectations();
		mock().clear();
	}
}

TEST(core, enable_ack_payload)
{
	uint8_t val;

	val = 0;
	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FEATURE)
		.andReturnValue(val);

	val |= (FEATURE_EN_ACK_PAY | FEATURE_EN_DPL);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", FEATURE)
		.withParameter("val", val);

	val = 0;
	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", DYNPD)
		.andReturnValue(val);

	val |= (DYNPD_DPL_P0 | DYNPD_DPL_P1);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", DYNPD)
		.withParameter("val", val);

	CHECK_FALSE(rf24_is_ack_payload(pnrf24));
	rf24_enable_ack_payload(pnrf24);
	CHECK_TRUE(rf24_is_ack_payload(pnrf24));
	mock().checkExpectations();
}

TEST(core, set_auto_ack_all_enable)
{
	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", EN_AA)
		.withParameter("val", EN_AA_ENAA_ALL);

	rf24_set_auto_ack_all(pnrf24, 1);
	mock().checkExpectations();
}

TEST(core, set_auto_ack_all_disable)
{
	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", EN_AA)
		.withParameter("val", 0);

	rf24_set_auto_ack_all(pnrf24, 0);
	mock().checkExpectations();
}

TEST(core, set_auto_ack_pipe_enable)
{
	for (int i = 0; i < 6; i++) {
		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", EN_AA)
			.andReturnValue(0);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", EN_AA)
			.withParameter("val", EN_AA_ENAA_P(i));

		rf24_set_auto_ack_pipe(pnrf24, i, 1);
		mock().checkExpectations();
		mock().clear();
	}
}

TEST(core, set_auto_ack_pipe_disable)
{
	uint8_t val;

	for (int i = 0; i < 6; i++) {
		val = 0xff;

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", EN_AA)
			.andReturnValue(val);

		val &= ~EN_AA_ENAA_P(i);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", EN_AA)
			.withParameter("val", val);

		rf24_set_auto_ack_pipe(pnrf24, i, 0);
		mock().checkExpectations();
		mock().clear();
	}
}

TEST(core, set_auto_ack_pipe_out_of_range)
{
	mock().expectNoCall("rf24_read_register");
	mock().expectNoCall("rf24_write_register");

	rf24_set_auto_ack_pipe(pnrf24, -1, 0);
	rf24_set_auto_ack_pipe(pnrf24, 6, 0);
	rf24_set_auto_ack_pipe(pnrf24, 7, 0);

	mock().checkExpectations();
}

TEST(core, get_carrier)
{
	int expected_value = 0xa;
	int actual_value;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", RPD)
		.andReturnValue(expected_value);

	actual_value = rf24_get_carrier(pnrf24);
	CHECK_EQUAL(expected_value, actual_value);

	mock().checkExpectations();
}

TEST(core, get_register)
{
	int expected_value = 0xa;
	int actual_value;

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", STATUS)
		.andReturnValue(expected_value);

	actual_value = rf24_get_register(pnrf24, STATUS);
	CHECK_EQUAL(expected_value, actual_value);

	mock().checkExpectations();
}
