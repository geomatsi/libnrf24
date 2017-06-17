#ifndef __SPI_STUB_NRF24_H__
#define __SPI_STUB_NRF24_H__

extern "C"
{
#include "nRF24L01.h"
#include "RF24.h"

#include "rf24_cmds.h"
#include "rf24_std.h"
#include "rf24_log.h"
}

struct mock_ops {
	void (*mock_delay_ms)(int);
	void (*mock_delay_us)(int);
	void (*mock_csn)(int level);
	void (*mock_ce)(int level);
	uint8_t (*mock_spi_xfer_sbyte)(uint8_t dat);
	int (*mock_spi_xfer_mbyte)(uint8_t *tx, uint8_t *rx, int len);
};

#endif /* __SPI_STUB_NRF24_H__ */
