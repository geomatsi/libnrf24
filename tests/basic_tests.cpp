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
