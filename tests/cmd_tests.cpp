#include <CppUTest/TestHarness.h>

#include <stub_nrf24.h>

TEST_GROUP(rf24_ops)
{
	struct rf24 *ps24;

	void setup()
	{
		ps24 = &stub_nrf24;
		rf24_init(ps24);
	}

	void teardown()
	{

	}
};

TEST(rf24_ops, cmd_read)
{
	uint8_t status;

	status = rf24_read_cmd(&stub_nrf24, NOP, NULL, 0);
	CHECK_EQUAL(0xe, status);
}

TEST(rf24_ops, cmd_write)
{
	uint8_t status;

	status = rf24_write_cmd(&stub_nrf24, NOP, NULL, 0);
	CHECK_EQUAL(0xe, status);
}
