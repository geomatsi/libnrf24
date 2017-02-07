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
#define RF24_MAX_ADDR_LEN	5

#define RF24_TX_NOACK		BIT(2)
#define RF24_DYN_PAYLOAD	BIT(3)
#define RF24_ACK_PAYLOAD	BIT(4)

struct rf24 {
	void    (*csn)(int level);
	void    (*ce)(int level);
	uint8_t (*spi_xfer)(uint8_t dat);

	uint8_t flags;
	uint8_t payload_size;

	/* cached p0 rx/tx address to switch between PRX and PTX modes */
	uint8_t p0_tx_addr[RF24_MAX_ADDR_LEN];
	uint8_t p0_rx_addr[RF24_MAX_ADDR_LEN];
};

#define rf24_is_tx_noack(r)	((r)->flags & RF24_TX_NOACK)
#define rf24_is_dyn_payload(r)	((r)->flags & RF24_DYN_PAYLOAD)
#define rf24_is_ack_payload(r)	((r)->flags & RF24_ACK_PAYLOAD)

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

enum rf24_rx_status {
	RF24_RX_OK	= 0,
	RF24_RX_EINVAL,
};

enum rf24_tx_status {
	RF24_TX_OK	= 0,
	RF24_TX_EINVAL,
	RF24_TX_FULL,
	RF24_TX_MAX_RT,
	RF24_TX_EIO,
};

void rf24_init(struct rf24 *r);
void rf24_enable_dyn_payload(struct rf24 *r);
void rf24_set_payload_size(struct rf24 *r, int len);
uint8_t rf24_get_dyn_payload_size(struct rf24 *r);

uint8_t rf24_get_status(struct rf24 *r);
uint8_t rf24_get_carrier(struct rf24 *r);

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

void rf24_enable_tx_noack(struct rf24 *r);
void rf24_disable_tx_noack(struct rf24 *r);
void rf24_enable_ack_payload(struct rf24 *r);
void rf24_set_auto_ack_all(struct rf24 *r, int enable);
void rf24_set_auto_ack_pipe(struct rf24 *r, int pipe, int enable);


void rf24_setup_ptx(struct rf24 *r, uint8_t *addr);
void rf24_start_ptx(struct rf24 *r);

void rf24_setup_prx(struct rf24 *r, int pipe, uint8_t *addr);
void rf24_start_prx(struct rf24 *r);

int rf24_rx_ready(struct rf24 *r, int *ppipe);
enum rf24_rx_status rf24_recv(struct rf24 *r, void *buf, int len);

enum rf24_tx_status rf24_send_async(struct rf24 *r, void *buf, int len);
enum rf24_tx_status rf24_send(struct rf24 *r, void *buf, int len);
enum rf24_tx_status rf24_tx_done(struct rf24 *r);

#endif /* __NRF24_H__ */
