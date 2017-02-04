/*
 * Copyright (C) 2017 Sergey Matyukevich <geomatsi@gmail.com>
 * Copyright (C) 2013 Andrew Andrianov <andrew@ncrmnt.org>
 * Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

#ifndef __NRF24_H__
#define __NRF24_H__

#include <rf24_std.h>

#define RF24_MAX_PAYLOAD_SIZE	32
#define RF24_MAX_CHANNEL	127

#define RF24_DYN_PAYLOAD	BIT(3)

struct rf24 {
	void    (*csn)(int level);
	void    (*ce)(int level);
	uint8_t (*spi_xfer)(uint8_t dat);

	uint8_t flags;
	uint8_t payload_size;
};

#define rf24_is_dyn_payload(r)	((r)->flags & RF24_DYN_PAYLOAD)
#define rf24_payload_size(r)	((r)->payload_size)

enum rf24_crc_mode {
	RF24_CRC_NONE = 0,
	RF24_CRC_8_BITS,
	RF24_CRC_16_BITS,
};

enum rf24_data_rate {
	RF24_RATE_1M = 0,
	RF24_RATE_2M,
	RF24_RATE_250K,
	RESERVED,
};

enum rf24_pa_level {
	RF24_PA_MIN	= 0,
	RF24_PA_M18DBM	= 0,
	RF24_PA_M12DBM	= 1,
	RF24_PA_M06DBM	= 2,
	RF24_PA_M00DBM	= 3,
	RF24_PA_MAX	= 3,
};

void rf24_init(struct rf24 *r);
void rf24_enable_dyn_payload(struct rf24 *r);
void rf24_set_payload_size(struct rf24 *r, int len);
int rf24_get_dyn_payload_size(struct rf24 *r);

uint8_t rf24_get_status(struct rf24 *r);

uint8_t rf24_flush_rx(struct rf24 *r);
uint8_t rf24_flush_tx(struct rf24 *r);

void rf24_set_channel(struct rf24 *r, uint8_t channel);
void rf24_set_retries(struct rf24 *r, uint8_t ard, uint8_t arc);

void rf24_set_crc_mode(struct rf24 *r, enum rf24_crc_mode mode);
enum rf24_crc_mode rf24_get_crc_mode(struct rf24 *r);

void rf24_power_down(struct rf24 *r);
void rf24_power_up(struct rf24 *r);

void rf24_activate_features(struct rf24 *r);
void rf24_deactivate_features(struct rf24 *r);

void rf24_set_data_rate(struct rf24 *r, enum rf24_data_rate rate);
enum rf24_data_rate rf24_get_data_rate(struct rf24 *r);

void rf24_set_pa_level(struct rf24 *r, enum rf24_pa_level level);
enum rf24_pa_level rf24_get_pa_level(struct rf24 *r);

#endif /* __NRF24_H__ */
