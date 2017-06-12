#ifndef __NRF24_CMDS_H__
#define __NRF24_CMDS_H__

#include <nRF24L01.h>
#include <RF24.h>

/* */

#if !defined(SPI_SINGLE_BYTE) && !defined(SPI_MULTI_BYTE)
#error "SPI_SINGLE_BYTE or SPI_MULTI_BYTE or both flags shall be specified"
#endif

/* */

struct rf24_ops {
	uint8_t (*write_cmd)(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len);
	uint8_t (*read_cmd)(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len);

	uint8_t (*read_register)(struct rf24 *r, uint8_t reg);
	uint8_t (*write_register)(struct rf24 *r, uint8_t reg, uint8_t val);

	uint8_t (*write_payload)(struct rf24 *r, const void *buf, int len);
	uint8_t (*write_ack_payload)(struct rf24 *r, int pipe, const void *buf, int len);
	uint8_t (*read_payload)(struct rf24 *r, void *buf, int len);

	uint8_t (*write_address)(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len);
};

static inline uint8_t
rf24_write_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
{
	struct rf24_ops *ops = (struct rf24_ops *)(r->rf24_ops);

	return ops->write_cmd(r, cmd, buf, len);
}

static inline uint8_t
rf24_read_cmd(struct rf24 *r, uint8_t cmd, uint8_t* buf, uint8_t len)
{
	struct rf24_ops *ops = (struct rf24_ops *)(r->rf24_ops);

	return ops->read_cmd(r, cmd, buf, len);
}

static inline uint8_t
rf24_read_register(struct rf24 *r, uint8_t reg)
{
	struct rf24_ops *ops = (struct rf24_ops *)(r->rf24_ops);

	return ops->read_register(r, reg);
}

static inline uint8_t
rf24_write_register(struct rf24 *r, uint8_t reg, uint8_t val)
{
	struct rf24_ops *ops = (struct rf24_ops *)(r->rf24_ops);

	return ops->write_register(r, reg, val);
}

static inline uint8_t
rf24_write_payload(struct rf24 *r, const void *buf, int len)
{
	struct rf24_ops *ops = (struct rf24_ops *)(r->rf24_ops);

	return ops->write_payload(r, buf, len);
}

static inline uint8_t
rf24_write_ack_payload(struct rf24 *r, int pipe, const void *buf, int len)
{
	struct rf24_ops *ops = (struct rf24_ops *)(r->rf24_ops);

	return ops->write_ack_payload(r, pipe, buf, len);
}

static inline uint8_t
rf24_read_payload(struct rf24 *r, void *buf, int len)
{
	struct rf24_ops *ops = (struct rf24_ops *)(r->rf24_ops);

	return ops->read_payload(r, buf, len);
}

static inline uint8_t
rf24_write_address(struct rf24 *r, uint8_t reg, const uint8_t *buf, int len)
{
	struct rf24_ops *ops = (struct rf24_ops *)(r->rf24_ops);

	return ops->write_address(r, reg, buf, len);
}

#endif /* __NRF24_CMDS_H__ */
