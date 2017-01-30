#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <cmd_mock_rf24.h>

TEST_GROUP(core)
{
	struct rf24 *pnrf24;

	void setup()
	{
		pnrf24 = &mock_rf24;
		rf24_init(pnrf24);
	}

	void teardown()
	{
		mock().clear();
	}
};

TEST(core, init)
{
	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, mock_rf24.payload_size);

	POINTERS_EQUAL(mock_spi_xfer_sbyte, pnrf24->spi_xfer);
	POINTERS_EQUAL(mock_csn, pnrf24->csn);
	POINTERS_EQUAL(mock_ce, pnrf24->ce);
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

	CHECK_FALSE(rf24_is_dynamic_payload(pnrf24));
	rf24_enable_dynamic_payload(pnrf24);
	CHECK_TRUE(rf24_is_dynamic_payload(pnrf24));
	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, rf24_payload_size(pnrf24));
}

TEST(core, get_dynamic_payload_sunny)
{
	int expected_len;
	int actual_len;

	/* sunny case: read valid length from nRF24 chip */

	expected_len = 10;

	mock()
		.expectOneCall("rf24_read_cmd")
		//.withParameter("r", pnrf24)
		.withParameter("cmd", R_RX_PL_WID)
		.andReturnValue(expected_len);

	actual_len = rf24_get_dynamic_payload_size(pnrf24);
	CHECK_EQUAL(expected_len, actual_len);

	mock().checkExpectations();
}

TEST(core, get_dynamic_payload_error)
{
	int expected_len;
	int actual_len;

	/* error case: read invalid length from nRF24 chip */

	expected_len = RF24_MAX_PAYLOAD_SIZE * 2;

	mock()
		.expectOneCall("rf24_read_cmd")
		.withParameter("cmd", R_RX_PL_WID)
		.andReturnValue(expected_len);

	actual_len = rf24_get_dynamic_payload_size(pnrf24);
	CHECK_EQUAL(-1, actual_len);

	mock().checkExpectations();
}

TEST(core, rf24_fluxh_rx)
{
	int expected_status = 0xe;
	int actual_status;

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
