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

#define RF24_DYNAMIC_PAYLOAD	BIT(3)

struct rf24 {
	void    (*csn)(int level);
	void    (*ce)(int level);
	uint8_t (*spi_xfer)(uint8_t dat);

	uint8_t flags;
	uint8_t payload_size;
};

#define rf24_is_dynamic_payload(r)	((r)->flags & RF24_DYNAMIC_PAYLOAD)
#define rf24_payload_size(r)		((r)->payload_size)

enum rf24_crc_mode {
	RF24_CRC_NONE = 0,
	RF24_CRC_8_BITS,
	RF24_CRC_16_BITS,
};

void rf24_init(struct rf24 *r);
void rf24_enable_dynamic_payload(struct rf24 *r);
void rf24_set_payload_size(struct rf24 *r, int len);
int rf24_get_dynamic_payload_size(struct rf24 *r);

uint8_t rf24_get_status(struct rf24 *r);

uint8_t rf24_flush_rx(struct rf24 *r);
uint8_t rf24_flush_tx(struct rf24 *r);

void rf24_set_channel(struct rf24 *r, uint8_t channel);
void rf24_set_retries(struct rf24 *r, uint8_t ard, uint8_t arc);

void rf24_set_crc_mode(struct rf24 *r, enum rf24_crc_mode mode);
enum rf24_crc_mode rf24_get_crc_mode(struct rf24 *r);

void rf24_power_down(struct rf24 *r);
void rf24_power_up(struct rf24 *r);

#endif /* __NRF24_H__ */
