#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <spi_mock_rf24.h>

void delay_ms(int msec)
{

}

void delay_us(int usec)
{

}

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
	mock().actualCall("spi_xfer_sbyte").withParameter("dat", dat);
	return mock().intReturnValue();
}

int mock_spi_xfer_mbyte(uint8_t *tx, uint8_t *rx, int len)
{
	return 0;
}
