#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <cmd_mock_rf24.h>

uint8_t rf24_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t *buf, uint8_t len)
{
	mock()
		.actualCall("rf24_write_cmd")
		.withParameter("cmd", cmd);

	return mock().intReturnValue();

}

uint8_t rf24_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t *buf, uint8_t len)
{
	uint8_t data = (uint8_t)mock().getData("read").getIntValue();

	mock()
		.actualCall("rf24_read_cmd")
		.withParameter("cmd", cmd);

	buf[0] = data;

	return mock().intReturnValue();
}

uint8_t rf24_read_register(struct rf24 *r, uint8_t reg)
{
	mock()
		.actualCall("rf24_read_register")
		.withParameter("reg", reg);

	return mock().intReturnValue();
}

uint8_t rf24_write_register(struct rf24 *r, uint8_t reg, uint8_t val)
{
	mock()
		.actualCall("rf24_write_register")
		.withParameter("reg", reg)
		.withParameter("val", val);

	return mock().intReturnValue();
}

uint8_t rf24_write_payload(struct rf24 *r, const void *buf, int len)
{
	mock()
		.actualCall("rf24_write_payload")
		.withParameter("len", len);

	return mock().intReturnValue();
}

uint8_t rf24_write_ack_payload(struct rf24 *r, int pipe, const void *buf, int len)
{
	mock()
		.actualCall("rf24_write_ack_payload")
		.withParameter("pipe", pipe);

	return mock().intReturnValue();
}

uint8_t rf24_read_payload(struct rf24 *r, const void *buf, int len)
{
	mock()
		.actualCall("rf24_read_payload")
		.withParameter("len", len);

	return mock().intReturnValue();
}

uint8_t rf24_write_address(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len)
{
	mock()
		.actualCall("rf24_write_address")
		.withParameter("reg", reg)
		.withParameter("len", len);

	return mock().intReturnValue();
}

void delay_ms(int msec)
{
	mock().actualCall("delay_ms").withParameter("msec", msec);
}

void delay_us(int usec)
{
	mock().actualCall("delay_us").withParameter("usec", usec);
}

/* empty stubs to fill spi ops in rf24 structure */

void mock_csn(int level)
{
	mock().actualCall("csn").withParameter("level", level);
}

void mock_ce(int level)
{
	mock().actualCall("ce").withParameter("level", level);
}

uint8_t mock_spi_xfer_sbyte(uint8_t dat)
{
	return 0;
};
