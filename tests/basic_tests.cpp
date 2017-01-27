#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <mock_rf24.h>

TEST_GROUP(basic)
{
	struct rf24 *pnrf24;

	void setup()
	{
		pnrf24 = &mock_rf24;
		rf24_init(pnrf24);

		/* here mock is explicitely enabled where it is needed */
		mock().disable();
	}

	void teardown()
	{
		mock().clear();
	}
};

TEST(basic, init)
{
	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, mock_rf24.payload_size);

	POINTERS_EQUAL(mock_spi_xfer_sbyte, pnrf24->spi_xfer);
	POINTERS_EQUAL(mock_csn, pnrf24->csn);
	POINTERS_EQUAL(mock_ce, pnrf24->ce);
}

TEST(basic, fixed_payload)
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

TEST(basic, enable_dynamic_payload)
{
	CHECK_FALSE(rf24_is_dynamic_payload(pnrf24));
	rf24_enable_dynamic_payload(pnrf24);
	CHECK_TRUE(rf24_is_dynamic_payload(pnrf24));
	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, rf24_payload_size(pnrf24));
}

TEST(basic, get_dynamic_payload_sunny)
{
	int expected_len;
	int actual_len;

	mock().enable();

	/* sunny case: read valid length from nRF24 chip */

	expected_len = 10;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_RX_PL_WID);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xff)
		.andReturnValue(expected_len);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	actual_len = rf24_get_dynamic_payload_size(pnrf24);
	CHECK_EQUAL(expected_len, actual_len);

	mock().checkExpectations();
}

TEST(basic, get_dynamic_payload_error)
{
	int expected_len;
	int actual_len;

	mock().enable();

	/* error case: read invalid length from nRF24 chip */

	expected_len = RF24_MAX_PAYLOAD_SIZE * 2;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_RX_PL_WID);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xff)
		.andReturnValue(expected_len);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	actual_len = rf24_get_dynamic_payload_size(pnrf24);
	CHECK_EQUAL(-1, actual_len);

	mock().checkExpectations();
}

TEST(basic, rf24_fluxh_rx)
{
	int expected_status = 0xe;
	int actual_status;

	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", FLUSH_RX)
		.andReturnValue(expected_status);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	actual_status = rf24_flush_rx(pnrf24);
	CHECK_EQUAL(expected_status, actual_status);

	mock().checkExpectations();
}

TEST(basic, rf24_fluxh_tx)
{
	int expected_status = 0xe;
	int actual_status;

	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", FLUSH_TX)
		.andReturnValue(expected_status);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	actual_status = rf24_flush_tx(pnrf24);
	CHECK_EQUAL(expected_status, actual_status);

	mock().checkExpectations();
}

TEST(basic, rf24_get_status)
{
	int expected_status = 0xe;
	int actual_status;

	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", NOP)
		.andReturnValue(expected_status);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	actual_status = rf24_get_status(pnrf24);
	CHECK_EQUAL(expected_status, actual_status);

	mock().checkExpectations();
}

TEST(basic, rf24_set_channel_valid)
{
	int channel = 0x10;

	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_REGISTER | (REGISTER_MASK & RF_CH));

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", channel);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	rf24_set_channel(pnrf24, channel);

	mock().checkExpectations();
}

TEST(basic, rf24_set_channel_invalid)
{
	int channel = RF24_MAX_CHANNEL + 1;

	mock().enable();
	mock().expectNoCall("csn");
	mock().expectNoCall("spi_xfer_sbyte");

	rf24_set_channel(pnrf24, channel);

	mock().checkExpectations();
}

TEST(basic, rf24_set_crc_mode_none)
{
	uint8_t config = 0xFF;

	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_REGISTER | (REGISTER_MASK & CONFIG));

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xFF)
		.andReturnValue(config);

	config &= ~(CONFIG_EN_CRC | CONFIG_CRCO);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_REGISTER | (REGISTER_MASK & CONFIG));

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", config);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	rf24_set_crc_mode(pnrf24, RF24_CRC_NONE);

	mock().checkExpectations();
}

TEST(basic, rf24_set_crc_mode_8_bits)
{
	uint8_t config = 0x0;

	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_REGISTER | (REGISTER_MASK & CONFIG));

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xFF)
		.andReturnValue(config);

	config |= CONFIG_EN_CRC;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_REGISTER | (REGISTER_MASK & CONFIG));

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", config);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	rf24_set_crc_mode(pnrf24, RF24_CRC_8_BITS);

	mock().checkExpectations();
}

TEST(basic, rf24_set_crc_mode_16_bits)
{
	uint8_t config = 0x0;

	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_REGISTER | (REGISTER_MASK & CONFIG));

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xFF)
		.andReturnValue(config);

	config |= (CONFIG_EN_CRC | CONFIG_CRCO);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_REGISTER | (REGISTER_MASK & CONFIG));

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", config);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	rf24_set_crc_mode(pnrf24, RF24_CRC_16_BITS);

	mock().checkExpectations();
}

TEST(basic, rf24_get_crc_mode)
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
			.expectOneCall("csn")
			.withParameter("level", 0);

		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", R_REGISTER | (REGISTER_MASK & CONFIG));

		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", 0xFF)
			.andReturnValue(reg[i]);

		mock()
			.expectOneCall("csn")
			.withParameter("level", 1);

		ret = rf24_get_crc_mode(pnrf24);

		CHECK_EQUAL(mode[i], ret);
		mock().checkExpectations();
		mock().clear();
	}
}
