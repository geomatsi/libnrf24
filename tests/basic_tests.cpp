#include <CppUTest/TestHarness.h>

#include <mock_rf24.h>

TEST_GROUP(basic)
{
	struct rf24 *pnrf24 = &mock_rf24;

	void setup()
	{
		rf24_init(pnrf24);
	}

	void teardown()
	{

	}
};

TEST(basic, init)
{
	CHECK_EQUAL(32, mock_rf24.payload_size);

	CHECK_FALSE(mock_rf24.spi_xfer == NULL);
	CHECK_FALSE(mock_rf24.csn == NULL);
	CHECK_FALSE(mock_rf24.ce == NULL);
}
