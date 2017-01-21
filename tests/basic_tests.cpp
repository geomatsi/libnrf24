#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <mock_rf24.h>

TEST_GROUP(basic)
{
	struct rf24 *pnrf24 = &mock_rf24;

	void setup()
	{
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

TEST(basic, dynamic_payload)
{
	CHECK_FALSE(rf24_is_dynamic_payload(pnrf24));
	rf24_enable_dynamic_payload(pnrf24);
	CHECK_TRUE(rf24_is_dynamic_payload(pnrf24));
	CHECK_EQUAL(RF24_MAX_PAYLOAD_SIZE, rf24_payload_size(pnrf24));
}
