#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

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
		mock().clear();
	}
};

TEST(rf24_cmds, cmd_nop)
{
	uint8_t status;

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", NOP)
		.andReturnValue(0xe);

	status = rf24_read_cmd(pnrf24, NOP, NULL, 0);
	CHECK_EQUAL(0xe, status);

	mock().checkExpectations();
}


TEST(rf24_cmds, cmd_read_byte)
{
	uint8_t status;
	uint8_t rx[1] = {0};

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", NOP)
		.andReturnValue(0xe);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xff)
		.andReturnValue(0xa);

	status = rf24_read_cmd(pnrf24, NOP, rx, 1);
	CHECK_EQUAL(0xe, status);
	CHECK_EQUAL(0xa, rx[0]);

	mock().checkExpectations();
}

TEST(rf24_cmds, cmd_read_bytes)
{
	uint8_t status;
	uint8_t rx[3] = {0x1, 0x2, 0x3};

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", NOP)
		.andReturnValue(0xe);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xff)
		.andReturnValue(0xa);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xff)
		.andReturnValue(0xb);

	status = rf24_read_cmd(pnrf24, NOP, rx, 2);
	CHECK_EQUAL(0xe, status);
	CHECK_EQUAL(0xa, rx[0]);
	CHECK_EQUAL(0xb, rx[1]);
	CHECK_EQUAL(0x3, rx[2]);

	mock().checkExpectations();
}

#if 0
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
#endif
