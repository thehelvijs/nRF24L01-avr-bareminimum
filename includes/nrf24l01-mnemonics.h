/*  Copyright (c) 2015 B. Sidhipong <bsidhipong@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef __NORDIC_NRF24L01_RADIO_H__
#define __NORDIC_NRF24L01_RADIO_H__
#include <avr/common.h>

/* SPI commands */
#define REGISTER_MASK		0b00011111
#define R_REGISTER		0b00000000 /* 000A AAAA | AAAAA = 5-bit register map address */
#define W_REGISTER		0b00100000 /* 001A AAAA */
#define R_RX_PAYLOAD		0b01100001
#define W_TX_PAYLOAD		0b10100000
#define FLUSH_TX		0b11100001
#define FLUSH_RX		0b11100010
#define REUSE_TX_PL		0b11100011
#define ACTIVATE		0b01010000 
#define R_RX_PL_WID		0b01100000
#define ACK_PAYLOAD_MASK	0b00000111
#define W_ACK_PAYLOAD		0b10101000 /* 1010 1PPP | PPP = pipe number */
#define W_TX_PAYLOAD_NOACK	0b10110000
#define NOP			0b11111111

/* CONFIG: configuration register */
#define CONFIG			0x00
#define MASK_RX_DR		6
#define MASK_TX_DS		5
#define MASK_MAX_RT		4
#define EN_CRC			3
#define CRC0			2
#define PWR_UP			1
#define PRIM_RX			0

/* EN_AA: Enhanced ShockBurst™
 * Enable 'Auto Acknowledgment' function.  Disable this functionality
 * to be compatible with nRF2401. */
#define EN_AA			0x01
#define ENAA_P5			5
#define ENAA_P4			4
#define ENAA_P3			3
#define ENAA_P2			2
#define ENAA_P1			1
#define ENAA_P0			0

/* EN_RXADDR: enable RX addresses */
#define EN_RXADDR		0x02
#define ERX_P5			5
#define ERX_P4			4
#define ERX_P3			3
#define ERX_P2			2
#define ERX_P1			1
#define ERX_P0			0

/* SETUP_AW: seup of address widths */
#define SETUP_AW		0x03
#define AW			0 /* 1:0 */

/* SETUP_RETR: setup of automatic retransmission */
#define SETUP_RETR		0x04
#define ARD			4 /* 7:4 */
#define ARC			0 /* 3:0 */

#define RF_CH			0x05

/* RF_SETUP: RF setup register */
#define RF_SETUP		0x06
#define CONT_WAVE		7
#define RF_DR_LOW		5
#define PLL_LOCK		4
#define RF_DR_HIGH		3				/* RF_DR:RF_DR_LOW = RF data rate */
#define RF_DR_250KBPS	0b00100000		/* available on nRF24L01+ */
#define RF_DR_1MBPS		0b00000000
#define RF_DR_2MBPS		0b00001000
#define RF_PWR			1				/* 2:1 */
#define POWER_MIN		0b00000000
#define POWER_LOW		0b00000010
#define POWER_HIGH		0b00000100
#define POWER_MAX		0b00000110

/* STATUS: status register */
#define STATUS			0x07
#define RX_DR			6
#define TX_DS			5
#define MAX_RT			4
#define RX_P_NO			1	/* 3:1 */
#define TX_FULL			0

/* OBSERVE_TX: transmit observe register */
#define OBSERVE_TX		0x08
#define PLOS_CNT		4	/* 7:4 */
#define ARC_CNT			0	/* 3:0 */

#define RPD			0x09
#define RX_ADDR_P0		0x0A
#define RX_ADDR_P1		0x0B
#define RX_ADDR_P2		0x0C
#define RX_ADDR_P3		0x0D
#define RX_ADDR_P4		0x0E
#define RX_ADDR_P5		0x0F
#define TX_ADDR			0x10
#define RX_PW_P0		0x11
#define RX_PW_P1		0x12
#define RX_PW_P2		0x13
#define RX_PW_P3		0x14
#define RX_PW_P4		0x15
#define RX_PW_P5		0x16

/* FIFO_STATUS: FIFO status register */
#define FIFO_STATUS		0x17
#define TX_REUSE		6
#define FIFO_FULL		5
#define TX_EMPTY		4
#define RX_FULL			1
#define RX_EMPTY		0

/* DYNPD: enable dynamic payload length */
#define DYNPD			0x1C
#define DPL_P5			5
#define DPL_P4			4
#define DPL_P3			3
#define DPL_P2			2
#define DPL_P1			1
#define DPL_P0			0

/* FEATURE: */
#define FEATURE			0x1D
#define EN_DPL			2
#define EN_ACK_PAY		1
#define EN_DYN_ACK		0

#endif /* __NORDIC_NRF24L01_RADIO_H__ */
