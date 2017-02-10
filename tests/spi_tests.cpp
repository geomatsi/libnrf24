#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <cstring>

#include <spi_mock_rf24.h>

TEST_GROUP(rf24_cmds)
{
	struct rf24 *pnrf24;
	struct rf24 nrf24;

	void setup()
	{
		mock().disable();

		memset(&nrf24, 0x0, sizeof(nrf24));
		nrf24.csn = mock_csn;
		nrf24.ce = mock_ce;
		nrf24.spi_xfer = mock_spi_xfer_sbyte;
		nrf24.spi_multi_xfer = mock_spi_xfer_mbyte;

		pnrf24 = &nrf24;
		rf24_init(pnrf24);

		mock().enable();
		mock().clear();
		mock().strictOrder();
	}

	void teardown()
	{
		mock().clear();
	}
};

/* tests: rf24_write_cmd */

TEST(rf24_cmds, cmd_write_nop)
{
	uint8_t status;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", NOP)
		.andReturnValue(0xe);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_cmd(pnrf24, NOP, NULL, 0);
	CHECK_EQUAL(0xe, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, cmd_write_byte)
{
	uint8_t byte = 0xa;
	uint8_t status;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_TX_PAYLOAD)
		.andReturnValue(0xe);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", byte);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_cmd(pnrf24, W_TX_PAYLOAD, &byte, 1);
	CHECK_EQUAL(0xe, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, cmd_write_bytes)
{
	uint8_t tx[] = {0x1, 0x2, 0x3, 0x4, 0x5};
	uint8_t status;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_TX_PAYLOAD)
		.andReturnValue(0xe);

	for (unsigned int i = 0; i < sizeof(tx); i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", tx[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_cmd(pnrf24, W_TX_PAYLOAD, tx, sizeof(tx));
	CHECK_EQUAL(0xe, status);

	mock().checkExpectations();
}

/* tests: rf24_read_cmd */

TEST(rf24_cmds, cmd_read_nop)
{
	uint8_t status;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", NOP)
		.andReturnValue(0xe);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_read_cmd(pnrf24, NOP, NULL, 0);
	CHECK_EQUAL(0xe, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, cmd_read_byte)
{
	uint8_t hw_byte = 0xa;
	uint8_t status;
	uint8_t byte;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", NOP)
		.andReturnValue(0xe);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xff)
		.andReturnValue(hw_byte);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_read_cmd(pnrf24, NOP, &byte, 1);
	CHECK_EQUAL(0xe, status);
	CHECK_EQUAL(hw_byte, byte);

	mock().checkExpectations();
}

TEST(rf24_cmds, cmd_read_bytes)
{
	uint8_t hw[] = {0x1, 0x2, 0x3, 0x4, 0x5};
	uint8_t rx[] = {0x0, 0x0, 0x0, 0x0};
	uint8_t status;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", NOP)
		.andReturnValue(0xe);

	for (unsigned int i = 0; i < sizeof(rx) - 1; i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", 0xff)
			.andReturnValue(hw[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_read_cmd(pnrf24, NOP, rx, sizeof(rx) - 1);
	CHECK_EQUAL(0xe, status);
	CHECK_EQUAL(0x0, rx[sizeof(rx) - 1]);
	MEMCMP_EQUAL(hw, rx, sizeof(rx) - 1);

	mock().checkExpectations();
}

/* tests: rf24_read_register */

TEST(rf24_cmds, cmd_read_register)
{
	uint8_t reg = RF_CH;
	uint8_t ret = 0x14;
	uint8_t val;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_REGISTER | (REGISTER_MASK & reg));

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0xff)
		.andReturnValue(ret);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	val = rf24_read_register(pnrf24, reg);
	CHECK_EQUAL(ret, val);

	mock().checkExpectations();
}

/* tests: rf24_write_register */

TEST(rf24_cmds, cmd_write_register)
{
	uint8_t reg = RF_CH;
	uint8_t val = 0x30;
	uint8_t ret = 0xe;
	uint8_t status;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_REGISTER | (REGISTER_MASK & reg))
		.andReturnValue(ret);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", val);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_register(pnrf24, reg, val);
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

/* fixed payload tests: rf24_write_payload */

TEST(rf24_cmds, write_payload_fixed_1)
{
	uint8_t buf[] = {0xa, 0xb, 0xc, 0xd, 0xe};
	uint8_t ret = 0xe;
	uint8_t status;

	/* usecase I: buffer length == configured payload length */

	rf24_set_payload_size(pnrf24, sizeof(buf));

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_TX_PAYLOAD)
		.andReturnValue(ret);

	for (unsigned int i = 0; i < sizeof(buf); i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", buf[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_payload(pnrf24, buf, sizeof(buf));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, write_payload_fixed_noack)
{
	uint8_t buf[] = {0xa, 0xb, 0xc, 0xd, 0xe};
	uint8_t ret = 0xe;
	uint8_t status;


	mock().disable();
	rf24_enable_tx_noack(pnrf24);
	mock().enable();

	rf24_set_payload_size(pnrf24, sizeof(buf));

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_TX_PAYLOAD_NOACK)
		.andReturnValue(ret);

	for (unsigned int i = 0; i < sizeof(buf); i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", buf[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_payload(pnrf24, buf, sizeof(buf));
	CHECK(rf24_is_tx_noack(pnrf24));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, write_payload_fixed_2)
{
	uint8_t buf[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t ret = 0xe;
	uint8_t status;

	/* usecase II: buffer length > configured payload length */

	rf24_set_payload_size(pnrf24, sizeof(buf) - 1);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_TX_PAYLOAD)
		.andReturnValue(ret);

	for (unsigned int i = 0; i < sizeof(buf) - 1; i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", buf[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_payload(pnrf24, buf, sizeof(buf));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, write_payload_fixed_3)
{
	uint8_t buf[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t ret = 0xe;
	uint8_t status;

	/* usecase III: buffer length < configured payload length */
	rf24_set_payload_size(pnrf24, sizeof(buf) + 1);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_TX_PAYLOAD)
		.andReturnValue(ret);

	for (unsigned int i = 0; i < sizeof(buf); i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", buf[i]);
	}

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", 0x0);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_payload(pnrf24, buf, sizeof(buf));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

/* dynamic payload tests: rf24_write_payload */

TEST(rf24_cmds, write_payload_dynamic)
{
	uint8_t buf[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t ret = 0xe;
	uint8_t status;

	/* don't verify dynamic payload setup, there are separate tests for it;
	 * this test is focused on checking dynamic payload transfer
	 */
	mock().disable();
	rf24_enable_dyn_payload(pnrf24);
	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_TX_PAYLOAD)
		.andReturnValue(ret);

	for (unsigned int i = 0; i < sizeof(buf); i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", buf[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_payload(pnrf24, buf, sizeof(buf));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, write_payload_dynamic_boundary)
{
	uint8_t buf[RF24_MAX_PAYLOAD_SIZE * 2] = {0};
	uint8_t ret = 0xe;
	uint8_t status;

	for (unsigned int i = 0; i < sizeof(buf); i++)
		buf[i] = (i + 1);

	/* don't verify dynamic payload setup, there are separate tests for it;
	 * this test is focused on checking dynamic payload transfer
	 */
	mock().disable();
	rf24_enable_dyn_payload(pnrf24);
	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_TX_PAYLOAD)
		.andReturnValue(ret);

	for (unsigned int i = 0; i < RF24_MAX_PAYLOAD_SIZE; i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", buf[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_payload(pnrf24, buf, sizeof(buf));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

/* ack payload tests: rf24_write_ack_payload */

TEST(rf24_cmds, write_ack_payload_ok)
{
	uint8_t buf[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t ret = 0xe;
	int pipe;
	uint8_t status;

	for (pipe = 0; pipe < 6; pipe++) {
		mock()
			.expectOneCall("csn")
			.withParameter("level", 0);

		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", W_ACK_PAYLOAD | (pipe & ACK_PAYLOAD_MASK))
			.andReturnValue(ret);

		for (unsigned int i = 0; i < sizeof(buf); i++) {
			mock()
				.expectOneCall("spi_xfer_sbyte")
				.withParameter("dat", buf[i]);
		}

		mock()
			.expectOneCall("csn")
			.withParameter("level", 1);
	}

	for (pipe = 0; pipe < 6; pipe++) {
		status = rf24_write_ack_payload(pnrf24, pipe, buf, sizeof(buf));
		CHECK_EQUAL(ret, status);
	}

	mock().checkExpectations();
}

TEST(rf24_cmds, write_ack_payload_exceed_max_length)
{
	uint8_t buf[RF24_MAX_PAYLOAD_SIZE + 5] = {0};
	uint8_t ret = 0xe;
	uint8_t status;
	int pipe = 4;

	for (unsigned int i = 0; i < sizeof(buf); i++)
		buf[i] = (i + 1);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_ACK_PAYLOAD | (pipe & ACK_PAYLOAD_MASK))
		.andReturnValue(ret);

	for (unsigned int i = 0; i < RF24_MAX_PAYLOAD_SIZE; i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", buf[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_ack_payload(pnrf24, pipe, buf, sizeof(buf));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();

}

TEST(rf24_cmds, write_ack_payload_invalid_pipe)
{
	uint8_t buf[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t fail = 0xff;
	int invalid_pipe = 6;
	uint8_t status;

	mock().expectNoCall("csn");
	mock().expectNoCall("spi_xfer_sbyte");

	status = rf24_write_ack_payload(pnrf24, invalid_pipe, buf, sizeof(buf));
	CHECK_EQUAL(fail, status);

	mock().checkExpectations();
}

/* fixed payload tests: rf24_read_payload */

TEST(rf24_cmds, read_payload_fixed_1)
{
	uint8_t tx[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t rx[] = {0x0, 0x0, 0x0, 0x0};
	uint8_t ret = 0xe;
	uint8_t status;
	int len;

	/* usecase I: buffer length == configured payload length */

	len = sizeof(rx);
	rf24_set_payload_size(pnrf24, len);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_RX_PAYLOAD)
		.andReturnValue(ret);

	for (int i = 0; i < len; i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", 0xff)
			.andReturnValue(tx[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_read_payload(pnrf24, rx, len);
	MEMCMP_EQUAL(tx, rx, len);
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, read_payload_fixed_2)
{
	uint8_t tx[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t rx[] = {0x0, 0x0, 0x0, 0x0};
	uint8_t ret = 0xe;
	uint8_t status;
	int len;

	/* usecase II: buffer length > configured payload length */

	len = sizeof(rx) - 1;
	rf24_set_payload_size(pnrf24, len);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_RX_PAYLOAD)
		.andReturnValue(ret);

	for (int i = 0; i < len; i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", 0xff)
			.andReturnValue(tx[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_read_payload(pnrf24, rx, len);
	MEMCMP_EQUAL(tx, rx, len);
	CHECK_EQUAL(0x0, rx[len]);
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, read_payload_fixed_3)
{
	uint8_t tx[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t rx[] = {0x0, 0x0, 0x0, 0x0};
	uint8_t ret = 0xe;
	uint8_t status;
	int rx_len;
	int tx_len;

	/* usecase II: buffer length < configured payload length */

	rx_len = sizeof(rx);
	tx_len = sizeof(rx);
	rf24_set_payload_size(pnrf24, tx_len);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_RX_PAYLOAD)
		.andReturnValue(ret);

	for (int i = 0; i < rx_len; i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", 0xff)
			.andReturnValue(tx[i]);
	}

	for (int i = 0; i < tx_len - rx_len; i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", 0xff);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_read_payload(pnrf24, rx, rx_len);
	MEMCMP_EQUAL(tx, rx, rx_len);
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

/* dynamic payload tests: rf24_read_payload */

TEST(rf24_cmds, read_payload_dynamic)
{
	uint8_t tx[] = {0xa, 0xb, 0xc, 0xd};
	uint8_t rx[] = {0x0, 0x0, 0x0, 0x0};
	uint8_t ret = 0xe;
	uint8_t status;

	/* don't verify dynamic payload setup, there are separate tests for it;
	 * this test is focused on checking dynamic payload transfer
	 */
	mock().disable();
	rf24_enable_dyn_payload(pnrf24);
	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_RX_PAYLOAD)
		.andReturnValue(ret);

	for (unsigned int i = 0; i < sizeof(rx); i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", 0xff)
			.andReturnValue(tx[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_read_payload(pnrf24, rx, sizeof(rx));
	MEMCMP_EQUAL(tx, rx, sizeof(rx));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

TEST(rf24_cmds, read_payload_dynamic_boundary)
{
	uint8_t rx[RF24_MAX_PAYLOAD_SIZE + 1] = {0};
	uint8_t tx[RF24_MAX_PAYLOAD_SIZE];
	uint8_t ret = 0xe;
	uint8_t status;

	for (unsigned int i = 0; i < sizeof(tx); i++)
		tx[i] = (i + 1);

	/* don't verify dynamic payload setup, there are separate tests for it;
	 * this test is focused on checking dynamic payload transfer
	 */
	mock().disable();
	rf24_enable_dyn_payload(pnrf24);
	mock().enable();

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", R_RX_PAYLOAD)
		.andReturnValue(ret);

	for (unsigned int i = 0; i < sizeof(tx); i++) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", 0xff)
			.andReturnValue(tx[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_read_payload(pnrf24, rx, sizeof(rx));
	CHECK_EQUAL(0x0, rx[RF24_MAX_PAYLOAD_SIZE]);
	MEMCMP_EQUAL(tx, rx, sizeof(tx));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}

/* set rx/tx address */

TEST(rf24_cmds, rf24_write_address_long)
{
	const char *addr = "HELLO";
	uint8_t ret = 0xe;
	uint8_t status;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_REGISTER | (REGISTER_MASK & RX_ADDR_P0))
		.andReturnValue(ret);

	for (int i = strlen(addr) - 1; i > -1; i--) {
		mock()
			.expectOneCall("spi_xfer_sbyte")
			.withParameter("dat", addr[i]);
	}

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_address(pnrf24, RX_ADDR_P0, (uint8_t *)addr, strlen(addr));
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();

}

TEST(rf24_cmds, rf24_write_address_short)
{
	const char *addr = "E";
	uint8_t ret = 0xe;
	uint8_t status;

	mock()
		.expectOneCall("csn")
		.withParameter("level", 0);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", W_REGISTER | (REGISTER_MASK & RX_ADDR_P5))
		.andReturnValue(ret);

	mock()
		.expectOneCall("spi_xfer_sbyte")
		.withParameter("dat", addr[0]);

	mock()
		.expectOneCall("csn")
		.withParameter("level", 1);

	status = rf24_write_address(pnrf24, RX_ADDR_P5, (uint8_t *)addr, 1);
	CHECK_EQUAL(ret, status);

	mock().checkExpectations();
}
