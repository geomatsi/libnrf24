#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <cmd_mock_rf24.h>

uint8_t rf24_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
{
	mock().actualCall("rf24_write_cmd")
		//.withParameter("r", r)
		.withParameter("cmd", cmd);
		//.withParameter("buf", buf)
		//.withParameter("len", len);
		return mock().intReturnValue();

}

uint8_t rf24_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
{
	mock().actualCall("rf24_read_cmd")
		//.withParameter("r", r)
		.withParameter("cmd", cmd);
		//.withParameter("buf", buf)
		//.withParameter("len", len);
		return mock().intReturnValue();
}

uint8_t rf24_read_register(struct rf24 *r, uint8_t reg)
{
	mock().actualCall("rf24_read_register")
		//.withParameter("r", r)
		.withParameter("reg", reg);
		return mock().intReturnValue();
}

uint8_t rf24_write_register(struct rf24 *r, uint8_t reg, uint8_t val)
{
	mock().actualCall("rf24_write_register")
		//.withParameter("r", r)
		.withParameter("reg", reg)
		.withParameter("val", val);
		return mock().intReturnValue();
}

uint8_t rf24_write_payload(struct rf24 *r, const void *buf, int len)
{
	mock().actualCall("rf24_write_payload");
		//.withParameter("r", r)
		//.withParameter("buf", buf)
		//.withParameter("len", len);
		return mock().intReturnValue();
}

uint8_t rf24_read_payload(struct rf24 *r, const void *buf, int len)
{
	mock().actualCall("rf24_ready_payload");
		//.withParameter("r", r)
		//.withParameter("buf", buf)
		//.withParameter("len", len);
		return mock().intReturnValue();
}

uint8_t rf24_write_address(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len)
{
	mock().actualCall("rf24_ready_payload")
		//.withParameter("r", r)
		.withParameter("reg", reg);
		//.withParameter("buf", buf)
		//.withParameter("len", len);
		return mock().intReturnValue();
}

void delay_ms(int msec)
{
	mock().actualCall("delay_ms")
		.withParameter("msec", msec);
}

void delay_us(int usec)
{
	mock().actualCall("delay_us")
		.withParameter("usec", usec);
}

/* empty stubs to fill spi ops in rf24 structure */

void mock_csn(int level)
{

}

void mock_ce(int level)
{

}

uint8_t mock_spi_xfer_sbyte(uint8_t dat)
{
	return 0;
}

int mock_spi_xfer_mbyte(uint8_t *tx, uint8_t *rx, int len)
{
	return 0;
}

struct rf24 mock_rf24 = {
	mock_csn,
	mock_ce,
	mock_spi_xfer_sbyte,
	0,
	0
};
