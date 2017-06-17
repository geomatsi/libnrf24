#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <cmd_mock_rf24.h>

static uint8_t rf24_mock_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t *buf, uint8_t len)
{
	mock()
		.actualCall("rf24_write_cmd")
		.withParameter("cmd", cmd);

	return mock().intReturnValue();

}

static uint8_t rf24_mock_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t *buf, uint8_t len)
{
	uint8_t data = (uint8_t)mock().getData("read").getIntValue();

	mock()
		.actualCall("rf24_read_cmd")
		.withParameter("cmd", cmd);

	buf[0] = data;

	return mock().intReturnValue();
}

static uint8_t rf24_mock_read_register(struct rf24 *r, uint8_t reg)
{
	mock()
		.actualCall("rf24_read_register")
		.withParameter("reg", reg);

	return mock().intReturnValue();
}

static uint8_t rf24_mock_write_register(struct rf24 *r, uint8_t reg, uint8_t val)
{
	mock()
		.actualCall("rf24_write_register")
		.withParameter("reg", reg)
		.withParameter("val", val);

	return mock().intReturnValue();
}

static uint8_t rf24_mock_write_payload(struct rf24 *r, const void *buf, int len)
{
	mock()
		.actualCall("rf24_write_payload")
		.withParameter("len", len);

	return mock().intReturnValue();
}

static uint8_t rf24_mock_write_ack_payload(struct rf24 *r, int pipe, const void *buf, int len)
{
	mock()
		.actualCall("rf24_write_ack_payload")
		.withParameter("pipe", pipe);

	return mock().intReturnValue();
}

static uint8_t rf24_mock_read_payload(struct rf24 *r, void *buf, int len)
{
	mock()
		.actualCall("rf24_read_payload")
		.withParameter("len", len);

	return mock().intReturnValue();
}

static uint8_t rf24_mock_write_address(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len)
{
	mock()
		.actualCall("rf24_write_address")
		.withParameter("reg", reg)
		.withParameter("len", len);

	return mock().intReturnValue();
}

struct rf24_ops u_mock_rf24_ops = {
	.write_cmd		= rf24_mock_write_cmd,
	.read_cmd		= rf24_mock_read_cmd,
	.read_register		= rf24_mock_read_register,
	.write_register		= rf24_mock_write_register,
	.write_payload		= rf24_mock_write_payload,
	.write_ack_payload	= rf24_mock_write_ack_payload,
	.read_payload		= rf24_mock_read_payload,
	.write_address		= rf24_mock_write_address,
};

