#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <spi_mock_rf24.h>

void delay_ms(int msec)
{

}

void delay_us(int usec)
{

}

static void mock_csn(int level)
{
	mock().actualCall("csn").withParameter("level", level);
}

static void mock_ce(int level)
{
	mock().actualCall("ce").withParameter("level", level);
}

static uint8_t mock_spi_xfer_sbyte(uint8_t dat)
{
	mock().actualCall("spi_xfer_sbyte").withParameter("dat", dat);
	return mock().intReturnValue();
}

struct mock_ops u_mock_ops = {
	.mock_csn		= mock_csn,
	.mock_ce		= mock_ce,
	.mock_spi_xfer_sbyte	= mock_spi_xfer_sbyte,
	.mock_spi_xfer_mbyte	= NULL,
};
