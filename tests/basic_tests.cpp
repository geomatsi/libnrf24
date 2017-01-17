#include <CppUTest/TestHarness.h>

#include <stub_nrf24.h>

TEST_GROUP(basic)
{
	void setup()
	{
		rf24_init(&stub_nrf24);
	}

	void teardown()
	{

	}
};

TEST(basic, register_ops)
{
	FAIL("TODO");
}

TEST(basic, init)
{
	CHECK_EQUAL(0, stub_nrf24.ack_payload_length);
	CHECK_EQUAL(32, stub_nrf24.payload_size);

	CHECK_FALSE(stub_nrf24.spi_set_speed == NULL);
	CHECK_FALSE(stub_nrf24.spi_xfer == NULL);
	CHECK_FALSE(stub_nrf24.csn == NULL);
	CHECK_FALSE(stub_nrf24.ce == NULL);
}
