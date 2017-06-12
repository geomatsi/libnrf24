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

/* mock mbyte spi xfer using sbyte xfers to reuse spi_tests */
static int mock_spi_xfer_mbyte(uint8_t *tx, uint8_t *rx, int len)
{
	int pos;

	for(pos = 0; pos < len; pos++) {
		*(rx + pos) = mock_spi_xfer_sbyte(*(tx + pos));
	}

	return 0;
}

struct mock_ops u_mock_ops = {
	.mock_csn		= mock_csn,
	.mock_ce		= mock_ce,
	.mock_spi_xfer_sbyte	= NULL,
	.mock_spi_xfer_mbyte	= mock_spi_xfer_mbyte,
};
