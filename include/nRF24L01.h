/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#ifndef __NRF24L01_H__
#define __NRF24L01_H__

/* SPI commands */

#define R_REGISTER		0x00
#define W_REGISTER		0x20
#define REGISTER_MASK		0x1F
#define R_RX_PL_WID		0x60
#define R_RX_PAYLOAD		0x61
#define W_TX_PAYLOAD		0xA0
#define W_TX_PAYLOAD_NOACK	0xB0
#define W_ACK_PAYLOAD		0xA8
#define FLUSH_TX		0xE1
#define FLUSH_RX		0xE2
#define REUSE_TX_PL		0xE3
#define NOP			0xFF

/* From nRF24L01 datasheet:
 * The R_RX_PL_WID, W_ACK_PAYLOAD, and W_TX_PAYLOAD_NOACK features registers are initially
 * in a deactivated state; a write has no effect, a read only results in zeros on MISO.
 * To activate these registers, use the ACTIVATE command followed by data 0x73.
 * Then they can be accessed as any other register in nRF24L01.
 * Use the same command and data to deactivate the registers again.
 */

#define ACTIVATE		0x50
#define ACTIVATE_DATA		0x73

/* Register map table */

#define CONFIG			0x00
#define EN_AA			0x01
#define EN_RXADDR		0x02
#define SETUP_AW		0x03
#define SETUP_RETR		0x04
#define RF_CH			0x05
#define RF_SETUP		0x06
#define STATUS			0x07
#define OBSERVE_TX		0x08

/* From nRF24L01+ datasheet:
 * The register is called CD in the nRF24L01. The name is different in nRF24L01+
 * due to the different input power level threshold for this bit.
 */
#define RPD			0x09
#define CD			0x09

#define RX_ADDR_P0		0x0A
#define RX_ADDR_P1		0x0B
#define RX_ADDR_P2		0x0C
#define RX_ADDR_P3		0x0D
#define RX_ADDR_P4		0x0E
#define RX_ADDR_P5		0x0F

#define RX_ADDR_BASE		0x0A
#define RX_ADDR_P(n)		(RX_ADDR_BASE + (n))

#define TX_ADDR			0x10

#define RX_PW_P0		0x11
#define RX_PW_P1		0x12
#define RX_PW_P2		0x13
#define RX_PW_P3		0x14
#define RX_PW_P4		0x15
#define RX_PW_P5		0x16

#define RX_PW_BASE		0x11
#define RX_PW_P(n)		(RX_PW_BASE + (n))

#define FIFO_STATUS		0x17
#define DYNPD			0x1C
#define FEATURE			0x1D

/* Register bits */

#define CONFIG_MASK_RX_DR	(1 << 6)
#define CONFIG_MASK_TX_DS	(1 << 5)
#define CONFIG_MASK_MAX_RT	(1 << 4)
#define CONFIG_EN_CRC		(1 << 3)
#define CONFIG_CRCO		(1 << 2)
#define CONFIG_PWR_UP		(1 << 1)
#define CONFIG_PRIM_RX		(1 << 0)

#define EN_AA_ENAA_P5		(1 << 5)
#define EN_AA_ENAA_P4		(1 << 4)
#define EN_AA_ENAA_P3		(1 << 3)
#define EN_AA_ENAA_P2		(1 << 2)
#define EN_AA_ENAA_P1		(1 << 1)
#define EN_AA_ENAA_P0		(1 << 0)

#define EN_AA_ENAA_ALL		(0x3F)
#define EN_AA_ENAA_P(n)		(1 << (n))

#define EN_RXADDR_ERX_P5	(1 << 5)
#define EN_RXADDR_ERX_P4	(1 << 4)
#define EN_RXADDR_ERX_P3	(1 << 3)
#define EN_RXADDR_ERX_P2	(1 << 2)
#define EN_RXADDR_ERX_P1	(1 << 1)
#define EN_RXADDR_ERX_P0	(1 << 0)

#define EN_RXADDR_ERX_P(n)	(1 << (n))

#define SETUP_AW_MASK		0x3
#define SETUP_AW_SHIFT		0
#define SETUP_AW_3_BYTES	(0x1 << SETUP_AW_SHIFT)
#define SETUP_AW_4_BYTES	(0x2 << SETUP_AW_SHIFT)
#define SETUP_AW_5_BYTES	(0x3 << SETUP_AW_SHIFT)

#define SETUP_RETR_ARD_SHIFT	4
#define SETUP_RETR_ARD_MASK	0xf
#define SETUP_RETR_ARD_VAL(x)	((x) << SETUP_RETR_ARD_SHIFT)

#define SETUP_RETR_ARC_SHIFT	0
#define SETUP_RETR_ARC_MASK	0xf
#define SETUP_RETR_ARC_VAL(x)	((x) << SETUP_RETR_ARC_SHIFT)

#define RF_CH_SHIFT		0
#define RF_CH_MASK		0x7F

#define RF_SETUP_CONT_WAVE	(1 << 7)
#define RF_SETUP_RF_DR_LOW	(1 << 5)
#define RF_SETUP_PLL_LOCK	(1 << 4)
#define RF_SETUP_RF_DR_HIGH	(1 << 3)

#define RF_SETUP_RF_PWR_SHIFT	1
#define RF_SETUP_RF_PWR_MASK	0x3
#define RF_SETUP_RF_PWR_VAL(x)	((x) << RF_SETUP_RF_PWR_SHIFT)

#define STATUS_RX_DR		(1 << 6)
#define STATUS_TX_DS		(1 << 5)
#define STATUS_MAX_RT		(1 << 4)
#define STATUS_TX_FULL		(1 << 0)
#define STATUS_CLEAR		(STATUS_RX_DR | STATUS_TX_DS | STATUS_MAX_RT)

#define STATUS_RX_P_NO_SHIFT	1
#define STATUS_RX_P_NO_MASK	0x7

#define OBS_TX_PLOS_CNT_SHIFT	4
#define OBS_TX_PLOS_CNT_MASK	0xf
#define OBS_TX_PLOS_CNT_VAL(x)	((x) << OBS_TX_PLOS_CNT_SHIFT)

#define OBS_TX_ARC_CNT_SHIFT	0
#define OBS_TX_ARC_CNT_MASK	0xf
#define OBS_TX_ARC_CNT_VAL(x)	((x) << OBS_TX_ARC_CNT_SHIFT)

#define RPD_RPD			(1 << 0)

#define RX_PW_PX_MASK		0x3F

#define FIFO_STATUS_TX_REUSE	(1 << 6)
#define FIFO_STATUS_TX_FULL	(1 << 5)
#define FIFO_STATUS_TX_EMPTY	(1 << 4)
#define FIFO_STATUS_RX_FULL	(1 << 1)
#define FIFO_STATUS_RX_EMPTY	(1 << 0)

#define DYNPD_DPL_P5		(1 << 5)
#define DYNPD_DPL_P4		(1 << 4)
#define DYNPD_DPL_P3		(1 << 3)
#define DYNPD_DPL_P2		(1 << 2)
#define DYNPD_DPL_P1		(1 << 1)
#define DYNPD_DPL_P0		(1 << 0)

#define DYNPD_DPL_P(n)		(1 << (n))
#define DYNPD_DPL_ALL		0x3F

#define FEATURE_EN_DPL		(1 << 2)
#define FEATURE_EN_ACK_PAY	(1 << 1)
#define FEATURE_EN_DYN_ACK	(1 << 0)
#define FEATURE_EN_ALL		0x7

#endif /* __NRF24L01_H__ */
