#include <CppUTest/TestHarness.h>

#include <stub_nrf24.h>

TEST_GROUP(rf24_cmds)
{
	struct rf24 *pnrf24 = &stub_nrf24;

	void setup()
	{
		rf24_init(pnrf24);
	}

	void teardown()
	{

	}
};

TEST(rf24_cmds, cmd_read)
{
	uint8_t status;

	status = rf24_read_cmd(pnrf24, NOP, NULL, 0);
	CHECK_EQUAL(0xe, status);
}

TEST(rf24_cmds, cmd_write)
{
	uint8_t status;

	status = rf24_write_cmd(pnrf24, NOP, NULL, 0);
	CHECK_EQUAL(0xe, status);
}
