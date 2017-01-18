#ifndef __NRF24_OPS_H__
#define __NRF24_OPS_H__

#include <stdint.h>

#include <RF24.h>

uint8_t rf24_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len);
uint8_t rf24_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len);

#endif /* __NRF24_OPS_H__ */
