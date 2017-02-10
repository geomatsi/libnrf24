#ifndef __CMD_STUB_NRF24_H__
#define __CMD_STUB_NRF24_H__

extern "C"
{
#include "nRF24L01.h"
#include "RF24.h"

#include "rf24_cmds.h"
#include "rf24_std.h"
#include "rf24_delay.h"
#include "rf24_log.h"
}

extern uint8_t rf24_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len);
extern uint8_t rf24_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len);
extern uint8_t rf24_read_register(struct rf24 *r, uint8_t reg);
extern uint8_t rf24_write_register(struct rf24 *r, uint8_t reg, uint8_t val);

extern uint8_t rf24_write_payload(struct rf24 *r, const void *buf, int len);
extern uint8_t rf24_read_payload(struct rf24 *r, const void *buf, int len);

extern uint8_t rf24_write_address(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len);

extern void mock_csn(int level);
extern void mock_ce(int level);
extern uint8_t mock_spi_xfer_sbyte(uint8_t dat);
extern int mock_spi_xfer_mbyte(uint8_t *tx, uint8_t *rx, int len);

#endif /* __CMD_STUB_NRF24_H__ */
