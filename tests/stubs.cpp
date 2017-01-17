#include <stub_nrf24.h>

void delay_ms(int msec)
{

}

void delay_us(int usec)
{

}

void stub_csn(int level)
{

}

void stub_ce(int level)
{

}

void stub_spi_set_speed(int khz)
{

}

uint8_t stub_spi_xfer(uint8_t dat)
{
	return 0;
}

struct rf24 stub_nrf24 = {
	.csn		= stub_csn,
	.ce		= stub_ce,
	.spi_set_speed	= stub_spi_set_speed,
	.spi_xfer	= stub_spi_xfer,
};
