#ifndef __NRF24_CMDS_H__
#define __NRF24_CMDS_H__

#include "nRF24L01.h"
#include <RF24.h>

uint8_t rf24_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len);
uint8_t rf24_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len);

uint8_t rf24_read_register(struct rf24 *r, uint8_t reg);
uint8_t rf24_write_register(struct rf24 *r, uint8_t reg, uint8_t val);

uint8_t rf24_write_payload(struct rf24 *r, const void *buf, int len);

#endif /* __NRF24_CMDS_H__ */
