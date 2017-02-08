#include <CppUTest/TestHarness.h>
#include "CppUTestExt/MockSupport.h"

#include <cmd_mock_rf24.h>

TEST_GROUP(usecases)
{
	struct rf24 *pnrf24;

	void setup()
	{
		mock().disable();

		pnrf24 = &mock_rf24;
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

/* rf24_setup_ptx: capture PTX setup sequence */

TEST(usecases, setup_ptx_static)
{
	uint8_t ptx_addr[] = "ADDR0";
	int status_ok = 0xe;
	int pkt_size = 20;

	mock()
		.expectOneCall("rf24_write_address")
		.withParameter("reg", RX_ADDR_P0)
		.withParameter("len", RF24_MAX_ADDR_LEN)
		.andReturnValue(status_ok);

	mock()
		.expectOneCall("rf24_write_address")
		.withParameter("reg", TX_ADDR)
		.withParameter("len", RF24_MAX_ADDR_LEN)
		.andReturnValue(status_ok);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", RX_PW_P0)
		.withParameter("val", pkt_size);

	rf24_set_payload_size(pnrf24, pkt_size);
	rf24_setup_ptx(pnrf24, ptx_addr);

	MEMCMP_EQUAL(ptx_addr, pnrf24->p0_tx_addr, RF24_MAX_ADDR_LEN);
	mock().checkExpectations();
}

TEST(usecases, setup_ptx_dynamic)
{
	uint8_t ptx_addr[] = "ADDR0";
	int status_ok = 0xe;

	mock()
		.expectOneCall("rf24_write_address")
		.withParameter("reg", RX_ADDR_P0)
		.withParameter("len", RF24_MAX_ADDR_LEN)
		.andReturnValue(status_ok);

	mock()
		.expectOneCall("rf24_write_address")
		.withParameter("reg", TX_ADDR)
		.withParameter("len", RF24_MAX_ADDR_LEN)
		.andReturnValue(status_ok);

	/* don't verify dynamic payload setup, there are separate tests for it;
	 * this test is focused on checking dynamic payload transfer
	 */
	mock().disable();
	rf24_enable_dyn_payload(pnrf24);
	mock().enable();

	rf24_setup_ptx(pnrf24, ptx_addr);

	MEMCMP_EQUAL(ptx_addr, pnrf24->p0_tx_addr, RF24_MAX_ADDR_LEN);
	mock().checkExpectations();
}

/* rf24_setup_prx: capture PRX setup sequence */

TEST(usecases, setup_prx_static)
{
	uint8_t *pipe_addr[] = {
		(uint8_t *)"ADDR0",
		(uint8_t *)"ADDR1",
		(uint8_t *)"2",
		(uint8_t *)"3",
		(uint8_t *)"4",
		(uint8_t *)"5",
	};

	int status_ok = 0xe;
	int pkt_size = 20;
	int pipe = 0;

	for (pipe = 0; pipe < 2; pipe++) {
		mock()
			.expectOneCall("rf24_write_address")
			.withParameter("reg", RX_ADDR_P(pipe))
			.withParameter("len", RF24_MAX_ADDR_LEN)
			.andReturnValue(status_ok);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", RX_PW_P(pipe))
			.withParameter("val", pkt_size);

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", EN_RXADDR)
			.andReturnValue(0x0);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", EN_RXADDR)
			.withParameter("val", EN_RXADDR_ERX_P(pipe));
	}

	for (pipe = 2; pipe < 6; pipe++) {
		mock()
			.expectOneCall("rf24_write_address")
			.withParameter("reg", RX_ADDR_P(pipe))
			.withParameter("len", 1)
			.andReturnValue(status_ok);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", RX_PW_P(pipe))
			.withParameter("val", pkt_size);

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", EN_RXADDR)
			.andReturnValue(0x0);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", EN_RXADDR)
			.withParameter("val", EN_RXADDR_ERX_P(pipe));
	}


	rf24_set_payload_size(pnrf24, pkt_size);
	rf24_setup_prx(pnrf24, 0, pipe_addr[0]);
	rf24_setup_prx(pnrf24, 1, pipe_addr[1]);
	rf24_setup_prx(pnrf24, 2, pipe_addr[2]);
	rf24_setup_prx(pnrf24, 3, pipe_addr[3]);
	rf24_setup_prx(pnrf24, 4, pipe_addr[4]);
	rf24_setup_prx(pnrf24, 5, pipe_addr[5]);

	MEMCMP_EQUAL(pipe_addr[0], pnrf24->p0_rx_addr, RF24_MAX_ADDR_LEN);
	mock().checkExpectations();
}

TEST(usecases, setup_prx_dynamic)
{
	uint8_t *pipe_addr[] = {
		(uint8_t *)"ADDR0",
		(uint8_t *)"ADDR1",
		(uint8_t *)"2",
		(uint8_t *)"3",
		(uint8_t *)"4",
		(uint8_t *)"5",
	};

	int status_ok = 0xe;
	int pipe = 0;

	for (pipe = 0; pipe < 2; pipe++) {
		mock()
			.expectOneCall("rf24_write_address")
			.withParameter("reg", RX_ADDR_P(pipe))
			.withParameter("len", RF24_MAX_ADDR_LEN)
			.andReturnValue(status_ok);

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", EN_RXADDR)
			.andReturnValue(0x0);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", EN_RXADDR)
			.withParameter("val", EN_RXADDR_ERX_P(pipe));
	}

	for (pipe = 2; pipe < 6; pipe++) {
		mock()
			.expectOneCall("rf24_write_address")
			.withParameter("reg", RX_ADDR_P(pipe))
			.withParameter("len", 1)
			.andReturnValue(status_ok);

		mock()
			.expectOneCall("rf24_read_register")
			.withParameter("reg", EN_RXADDR)
			.andReturnValue(0x0);

		mock()
			.expectOneCall("rf24_write_register")
			.withParameter("reg", EN_RXADDR)
			.withParameter("val", EN_RXADDR_ERX_P(pipe));
	}

	/* don't verify dynamic payload setup, there are separate tests for it;
	 * this test is focused on checking dynamic payload transfer
	 */
	mock().disable();
	rf24_enable_dyn_payload(pnrf24);
	mock().enable();

	rf24_setup_prx(pnrf24, 0, pipe_addr[0]);
	rf24_setup_prx(pnrf24, 1, pipe_addr[1]);
	rf24_setup_prx(pnrf24, 2, pipe_addr[2]);
	rf24_setup_prx(pnrf24, 3, pipe_addr[3]);
	rf24_setup_prx(pnrf24, 4, pipe_addr[4]);
	rf24_setup_prx(pnrf24, 5, pipe_addr[5]);

	MEMCMP_EQUAL(pipe_addr[0], pnrf24->p0_rx_addr, RF24_MAX_ADDR_LEN);
	mock().checkExpectations();
}

TEST(usecases, rf24_setup_prx_invalid_pipe)
{
	uint8_t addr[] = "ADDR0";

	mock().expectNoCall("rf24_write_address");
	mock().expectNoCall("rf24_write_register");
	mock().expectNoCall("rf24_read_register");

	rf24_setup_prx(pnrf24, -1, addr);
	rf24_setup_prx(pnrf24, 6, addr);

	mock().checkExpectations();
}


/* rf24_start_ptx: capture PTX startup sequence */

TEST(usecases, start_ptx)
{
	int status_ok = 0xe;

	/* make sure ce=0 */

	mock()
		.expectOneCall("ce")
		.withParameter("level", 0);

	/* power_down */

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(0xFF);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", (uint8_t)(~CONFIG_PWR_UP));

	/* prx disable */

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(0xFF);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", (uint8_t)(~CONFIG_PRIM_RX));

	/* reset current status */

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_CLEAR);

	/* restore tx and rx0 addrs */

	mock()
		.expectOneCall("rf24_write_address")
		.withParameter("reg", RX_ADDR_P0)
		.withParameter("len", RF24_MAX_ADDR_LEN)
		.andReturnValue(status_ok);

	mock()
		.expectOneCall("rf24_write_address")
		.withParameter("reg", TX_ADDR)
		.withParameter("len", RF24_MAX_ADDR_LEN)
		.andReturnValue(status_ok);

	/* power up */

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", (uint8_t)CONFIG_PWR_UP);

	mock()
		.expectOneCall("delay_ms")
		.withParameter("msec", 2);

	/* make sure ce=0 */

	mock()
		.expectOneCall("ce")
		.withParameter("level", 0);

	rf24_start_ptx(pnrf24);

	mock().checkExpectations();
}

/* rf24_start_prx: capture PRX startup sequence */

TEST(usecases, start_prx)
{
	int status_ok = 0xe;

	/* make sure ce=0 */

	mock()
		.expectOneCall("ce")
		.withParameter("level", 0);

	/* power_down */

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(0xFF);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", (uint8_t)(~CONFIG_PWR_UP));

	/* prx enable */

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", (uint8_t)CONFIG_PRIM_RX);

	/* reset current status */

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_CLEAR);

	/* restore rx0 addr */

	mock()
		.expectOneCall("rf24_write_address")
		.withParameter("reg", RX_ADDR_P0)
		.withParameter("len", RF24_MAX_ADDR_LEN)
		.andReturnValue(status_ok);

	/* power up */

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", CONFIG)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", CONFIG)
		.withParameter("val", (uint8_t)CONFIG_PWR_UP);

	mock()
		.expectOneCall("delay_ms")
		.withParameter("msec", 2);

	/* enable ce=1 */

	mock()
		.expectOneCall("ce")
		.withParameter("level", 1);

	/* RX setting */

	mock()
		.expectOneCall("delay_us")
		.withParameter("usec", 200);

	rf24_start_prx(pnrf24);

	mock().checkExpectations();
}

/*
 * nRF24L01/nRF24L01+ spec. 7.9.1
 * Figure 15: single transaction with ACK packets and IRQs
 */

/* sunny cases: full rx/tx paths */

TEST(usecases, single_ack_transaction_rx)
{
	void *buf = (void *)0x1;	/* fake buffer */
	int pkt_len  = 20;		/* fixed packet size */
	int mock_pipe = 3;
	int pipe;
	enum rf24_rx_status ret;

	/* mock sequence setup */

	for (int i = 0; i < 5; i++) {
		mock()
			.expectOneCall("rf24_write_cmd")
			.withParameter("cmd", NOP)
			.andReturnValue(0x0);
	}

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", NOP)
		.andReturnValue(STATUS_RX_DR | (mock_pipe << STATUS_RX_P_NO_SHIFT));

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FIFO_STATUS)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("rf24_read_payload")
		.withParameter("len", pkt_len)
		.andReturnValue(STATUS_RX_DR);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_RX_DR);

	/* example rx code: static payload */

	rf24_set_payload_size(pnrf24, pkt_len);

	while(!rf24_rx_ready(pnrf24, &pipe)) {
		/* sleep some time before next rx check */
	}

	ret = rf24_rx_pipe_check(pnrf24, pipe);

	if (ret == RF24_RX_OK)
		ret = rf24_recv(pnrf24, buf, pkt_len);

	CHECK_EQUAL(mock_pipe, pipe);
	CHECK_EQUAL(RF24_RX_OK, ret);
	mock().checkExpectations();
}

TEST(usecases, single_ack_transaction_tx)
{
	void *buf = (void *)0x1;	/* fake buffer */
	int pkt_len  = 20;		/* fixed packet size */
	enum rf24_tx_status ret;

	/* mock sequence setup */

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FIFO_STATUS)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("rf24_write_payload")
		.withParameter("len", pkt_len);

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FIFO_STATUS)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("ce")
		.withParameter("level", 1);

	mock()
		.expectOneCall("delay_us")
		.withParameter("usec", 20);

	mock()
		.expectOneCall("ce")
		.withParameter("level", 0);

	for (int i = 0; i < 4; i++) {
		mock()
			.expectOneCall("rf24_write_cmd")
			.withParameter("cmd", NOP)
			.andReturnValue(0x0);
	}

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", NOP)
		.andReturnValue(STATUS_TX_DS);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_TX_DS);

	/* example tx code */

	ret = rf24_send(pnrf24, buf, pkt_len);

	CHECK_EQUAL(RF24_TX_OK, ret);
	mock().checkExpectations();
}

/* error cases: only path of rx/tx paths relevant to error checking/handling */

TEST(usecases, tx_rx_empty_buffer)
{
	void *buf = (void *)0x0;
	int pkt_len = 20;
	enum rf24_tx_status tx_ret;
	enum rf24_rx_status rx_ret;

	/* example rx code: static payload */

	tx_ret = rf24_send_async(pnrf24, buf, pkt_len);
	CHECK_EQUAL(RF24_TX_EINVAL, tx_ret);

	tx_ret = rf24_send(pnrf24, buf, pkt_len);
	CHECK_EQUAL(RF24_TX_EINVAL, tx_ret);

	rx_ret = rf24_recv(pnrf24, buf, pkt_len);
	CHECK_EQUAL(RF24_RX_EINVAL, rx_ret);
}

TEST(usecases, single_ack_transaction_tx_max_retransmissions)
{
	enum rf24_tx_status ret;

	/* mock sequence setup */

	for (int i = 0; i < 4; i++) {
		mock()
			.expectOneCall("rf24_write_cmd")
			.withParameter("cmd", NOP)
			.andReturnValue(0x0);
	}

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", NOP)
		.andReturnValue(STATUS_MAX_RT);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_MAX_RT);

	/* example tx code */

	ret = rf24_tx_done(pnrf24);

	CHECK_EQUAL(RF24_TX_MAX_RT, ret);
	mock().checkExpectations();
}

TEST(usecases, single_ack_transaction_rx_pipe_fifo_empty)
{
	int mock_pipe = RF24_MAX_PIPE + 2;	/* invalid pipe */
	enum rf24_rx_status ret;
	int pipe;

	/* mock sequence setup */

	for (int i = 0; i < 5; i++) {
		mock()
			.expectOneCall("rf24_write_cmd")
			.withParameter("cmd", NOP)
			.andReturnValue(0x0);
	}

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", NOP)
		.andReturnValue(STATUS_RX_DR | (mock_pipe << STATUS_RX_P_NO_SHIFT));

	/* example rx code */

	while(!rf24_rx_ready(pnrf24, &pipe)) {
		/* sleep some time before next rx check */
	}

	ret = rf24_rx_pipe_check(pnrf24, pipe);

	CHECK_EQUAL(mock_pipe, pipe);
	CHECK_EQUAL(RF24_RX_EMPTY, ret);
	mock().checkExpectations();
}

TEST(usecases, single_ack_transaction_rx_fifo_empty)
{
	void *buf = (void *)0x1;	/* fake buffer */
	int pkt_len  = 20;		/* fixed packet size */
	int mock_pipe = 3;
	int pipe;
	enum rf24_rx_status ret;

	/* mock sequence setup */

	for (int i = 0; i < 5; i++) {
		mock()
			.expectOneCall("rf24_write_cmd")
			.withParameter("cmd", NOP)
			.andReturnValue(0x0);
	}

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", NOP)
		.andReturnValue(STATUS_RX_DR | (mock_pipe << STATUS_RX_P_NO_SHIFT));

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FIFO_STATUS)
		.andReturnValue(FIFO_STATUS_RX_EMPTY);

	/* example rx code */

	rf24_set_payload_size(pnrf24, pkt_len);

	while(!rf24_rx_ready(pnrf24, &pipe)) {
		/* sleep some time before next rx check */
	}

	ret = rf24_rx_pipe_check(pnrf24, pipe);

	if (ret == RF24_RX_OK)
		ret = rf24_recv(pnrf24, buf, pkt_len);

	CHECK_EQUAL(mock_pipe, pipe);
	CHECK_EQUAL(RF24_RX_EMPTY, ret);
	mock().checkExpectations();
}

/*
 * nRF24L01/nRF24L01+ spec. 7.9.4
 * Figure 18: single transaction with ACK payload packet
 */

/* sunny cases: full tx path */

TEST(usecases, single_ack_payload_transaction_tx)
{
	void *rx_buf = (void *)0x1;	/* fake buffer */
	void *tx_buf = (void *)0x2;	/* fake buffer */
	enum rf24_tx_status tx_ret;
	enum rf24_rx_status rx_ret;
	int tx_len = 20;
	int exp_rx_len = 5;
	int rx_len;

	/* mock sequence setup */

	/* rf24_send */
	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FIFO_STATUS)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("rf24_write_payload")
		.withParameter("len", tx_len);

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FIFO_STATUS)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("ce")
		.withParameter("level", 1);

	mock()
		.expectOneCall("delay_us")
		.withParameter("usec", 20);

	mock()
		.expectOneCall("ce")
		.withParameter("level", 0);

	for (int i = 0; i < 4; i++) {
		mock()
			.expectOneCall("rf24_write_cmd")
			.withParameter("cmd", NOP)
			.andReturnValue(0x0);
	}

	mock()
		.expectOneCall("rf24_write_cmd")
		.withParameter("cmd", NOP)
		.andReturnValue(STATUS_TX_DS | STATUS_RX_DR);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_TX_DS);

	/* rf24_get_dyn_payload */

	mock().setData("read", exp_rx_len);

	mock()
		.expectOneCall("rf24_read_cmd")
		.withParameter("cmd", R_RX_PL_WID);

	/* rf24_recv */

	mock()
		.expectOneCall("rf24_read_register")
		.withParameter("reg", FIFO_STATUS)
		.andReturnValue(0x0);

	mock()
		.expectOneCall("rf24_read_payload")
		.withParameter("len", exp_rx_len)
		.andReturnValue(STATUS_RX_DR);

	mock()
		.expectOneCall("rf24_write_register")
		.withParameter("reg", STATUS)
		.withParameter("val", STATUS_RX_DR);

	/* example tx code: skip async send check */

	mock().disable();
	rf24_enable_dyn_payload(pnrf24);
	rf24_enable_ack_payload(pnrf24);
	mock().enable();

	tx_ret = rf24_send(pnrf24, tx_buf, tx_len);
	CHECK_EQUAL(RF24_TX_ACK_OK, tx_ret);

	rx_len = rf24_get_dyn_payload_size(pnrf24);

	rx_ret = rf24_recv(pnrf24, rx_buf, rx_len);
	CHECK_EQUAL(RF24_RX_OK, rx_ret);

	mock().checkExpectations();
}
