// MIT License
//
// Copyright (c) 2018 Helvijs Adams
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//	Set clock frequency
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//	nRF24L01+ include files
#include "nrf24l01.h"
#include "nrf24l01-mnemonics.h"
#include "spi.h"

//	Settings
uint8_t rx_address[5] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 };	// Read pipe address
uint8_t tx_address[5] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 };	// Write pipe address
#define READ_PIPE		0									// Number of read pipe
//
//	-AUTO_ACK can be disabled when running on 2MBPS @ <= 32 byte messages.
//	-250KBPS and 1MBPS with AUTO_ACK disabled lost many packets
//	if the packet size was bigger than 4 bytes.
//	-If AUTO_ACK is enabled, tx_address = rx_address.
//
#define AUTO_ACK		false								// Auto acknowledgment
#define DATARATE		RF_DR_2MBPS							// 250kbps, 1mbps, 2mbps
#define POWER			POWER_MAX							// Set power (MAX 0dBm..HIGH -6dBm..LOW -12dBm.. MIN -18dBm)
#define CHANNEL			0x76								// 2.4GHz-2.5GHz channel selection (0x01 - 0x7C)
#define DYN_PAYLOAD		true								// Dynamic payload enabled
#define CONTINUOUS		false								// Continuous carrier transmit mode (not tested)
//
//	ISR(INT0_vect) is triggered depending on config (only one can be true)
//
#define RX_INTERRUPT	true								// Interrupt when message is received (RX)
#define TX_INTERRUPT	false								// Interrupt when message is sent (TX)
#define RT_INTERRUPT	false								// Interrupt when maximum re-transmits are reached (MAX_RT)
//
//	-PIN map.
//	-If CE or CSN is changed to different PIN e.g. PC0
//	then change DDRB -> DDRC, PORTB -> PORTC and so on
//
//	CE
#define CE_DDR		DDRB
#define CE_PORT		PORTB
#define CE_PIN		DDB1									// CE connected to PB1
//	CSN
#define CSN_DDR		DDRB
#define CSN_PORT	PORTB
#define CSN_PIN		DDB2									// CSN connected to PB2
//	IRQ
#define IRQ_DDR		DDRD
#define IRQ_PORT	PORTD
#define IRQ_PIN		DDD2									// IRQ connected to PD2
//	MOSI
#define MOSI_DDR	DDRB
#define MOSI_PORT	PORTB
#define MOSI_PIN	DDB3
//	MISO
#define MISO_DDR	DDRB
#define MISO_PORT	PORTB
#define MISO_PIN	DDB4
//	SCK
#define SCK_DDR		DDRB
#define SCK_PORT	PORTB
#define SCK_PIN		DDB5

//	PIN toggling
#define setbit(port, bit) (port) |= (1 << (bit))
#define clearbit(port, bit) (port) &= ~(1 << (bit))
#define ce_low clearbit(CE_PORT,CE_PIN)
#define ce_high setbit(CE_PORT,CE_PIN)
#define csn_low clearbit(CSN_PORT,CSN_PIN)
#define csn_high setbit(CSN_PORT,CSN_PIN)

//	Used to store SPI commands
uint8_t data;

uint8_t nrf24_send_spi(uint8_t register_address, void *data, unsigned int bytes)
{
	uint8_t status;
	csn_low;
	status = spi_exchange(register_address);
	for (unsigned int i = 0; i < bytes; i++)
	((uint8_t*)data)[i] = spi_exchange(((uint8_t*)data)[i]);
	csn_high;
	return status;
}

uint8_t nrf24_write(uint8_t register_address, uint8_t *data, unsigned int bytes)
{
	return nrf24_send_spi(W_REGISTER | register_address, data, bytes);
}

uint8_t nrf24_read(uint8_t register_address, uint8_t *data, unsigned int bytes)
{
	return nrf24_send_spi(R_REGISTER | register_address, data, bytes);
}

void nrf24_init(void)
{
	//	Interrupt on falling edge of INT0 (PD2) from IRQ pin
	cli();					//	Disable interrupts
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);
	sei();					//	Enable interrupts
	
	//	CSN and CE as outputs and initial states
	setbit(CE_DDR,CE_PIN);
	setbit(CSN_DDR,CSN_PIN);
	csn_high;
	ce_low;
	
	//	Initialize SPI
	spi_master_init();
	_delay_ms(100);				//	Power on reset 100ms
	
	//	Start nRF24L01+ config
	data =
	(!(RX_INTERRUPT) << MASK_RX_DR) |	//	IRQ interrupt on RX (0 = enabled)
	(!(TX_INTERRUPT) << MASK_TX_DS) |	//	IRQ interrupt on TX (0 = enabled)
	(!(RT_INTERRUPT) << MASK_MAX_RT) |	//	IRQ interrupt on auto retransmit counter overflow (0 = enabled)
	(1 << EN_CRC) |						//	CRC enable
	(1 << CRC0) |						//	CRC scheme
	(1 << PWR_UP) |						//	Power up
	(1 << PRIM_RX);						//	TX/RX select
	nrf24_write(CONFIG,	&data,1);
	
	//	Auto-acknowledge on all pipes
	data =
	(AUTO_ACK << ENAA_P5) |
	(AUTO_ACK << ENAA_P4) |
	(AUTO_ACK << ENAA_P3) |
	(AUTO_ACK << ENAA_P2) |
	(AUTO_ACK << ENAA_P1) |
	(AUTO_ACK << ENAA_P0);
	nrf24_write(EN_AA,&data,1);
	
	//	Set retries
	data = 0xF0;				//	Delay 4000us with 1 re-try (will be added in settings)
	nrf24_write(SETUP_RETR,&data,1);
	
	//	Disable RX addresses
	data = 0;
	nrf24_write(EN_RXADDR, &data, 1);
	
	//	Set channel
	data = CHANNEL;
	nrf24_write(RF_CH,&data,1);
	
	//	Setup
	data =
	(CONTINUOUS << CONT_WAVE) |					//	Continuous carrier transmit
	((DATARATE >> RF_DR_HIGH) << RF_DR_HIGH) |	//	Data rate
	((POWER >> RF_PWR) << RF_PWR);				//	PA level
	nrf24_write(RF_SETUP,&data,1);
	
	//	Status - clear TX/RX FIFO's and MAX_RT by writing 1 into them
	data =
	(1 << RX_DR) |								//	RX FIFO
	(1 << TX_DS) |								//	TX FIFO
	(1 << MAX_RT);								//	MAX RT
	nrf24_write(STATUS,&data,1);
	
	//	Dynamic payload on all pipes
	data =
	(DYN_PAYLOAD << DPL_P0) |
	(DYN_PAYLOAD << DPL_P1) |
	(DYN_PAYLOAD << DPL_P2) |
	(DYN_PAYLOAD << DPL_P3) |
	(DYN_PAYLOAD << DPL_P4) |
	(DYN_PAYLOAD << DPL_P5);
	nrf24_write(DYNPD, &data,1);

	//	Enable dynamic payload
	data =
	(DYN_PAYLOAD << EN_DPL) |
	(AUTO_ACK << EN_ACK_PAY) |
	(AUTO_ACK << EN_DYN_ACK);
	nrf24_write(FEATURE,&data,1);
	
	// 	Flush TX/RX
	//	Clear interrupts
	uint8_t data = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
	nrf24_write(FLUSH_RX,0,0);
	nrf24_write(FLUSH_TX,0,0);
	
	//	Open pipes
	nrf24_write(RX_ADDR_P0 + READ_PIPE,rx_address,5);
	nrf24_write(TX_ADDR,tx_address,5);
	nrf24_write(EN_RXADDR,&data,1);
	data |= (1 << READ_PIPE);
	nrf24_write(EN_RXADDR,&data,1);
}

void nrf24_write_ack(void)
{
	const void *ack = "A";
	unsigned int length = 1;
	csn_low;
	spi_send(W_ACK_PAYLOAD);
	while (length--) spi_send(*(uint8_t *)ack++);
	csn_high;
}

void nrf24_state(uint8_t state)
{
	uint8_t config_register;
	nrf24_read(CONFIG,&config_register,1);
	
	switch (state)
	{
		case POWERUP:
		//	Check if already powered up
		if (!(config_register & (1 << PWR_UP)))
		{
			data = config_register | (1 << PWR_UP);
			nrf24_write(CONFIG,&data,1);
			// 1.5ms from POWERDOWN to start up
			_delay_ms(2);
		}
		break;
		case POWERDOWN:
		data = config_register & ~(1 << PWR_UP);
		nrf24_write(CONFIG,&data,1);
		break;
		case RECEIVE:
		data = config_register | (1 << PRIM_RX);
		nrf24_write(CONFIG,&data,1);
		//	Clear STATUS register
		data = (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT);
		nrf24_write(STATUS,&data,1);
		break;
		case TRANSMIT:
		data = config_register & ~(1 << PRIM_RX);
		nrf24_write(CONFIG,&data,1);
		break;
		case STANDBY1:
		ce_low;
		break;
		case STANDBY2:
		data = config_register & ~(1 << PRIM_RX);
		nrf24_write(CONFIG,&data,1);
		ce_high;
		_delay_us(150);
		break;
	}
}

void nrf24_start_listening(void)
{
	nrf24_state(RECEIVE);				//	Receive mode
	//if (AUTO_ACK) nrf24_write_ack();	//	Write acknowledgment
	ce_high;
	_delay_us(150);						//	Settling time
}

uint8_t nrf24_send_message(const void *tx_message)
{
	//	Message length
	uint8_t length = strlen(tx_message);

	//	Transmit mode
	nrf24_state(TRANSMIT);

	// Flush TX/RX and clear TX interrupt
	nrf24_write(FLUSH_RX,0,0);
	nrf24_write(FLUSH_TX,0,0);
	data = (1 << TX_DS);
	nrf24_write(STATUS,&data,1);
	
	// Disable interrupt on RX
	nrf24_read(CONFIG,&data,1);
	data |= (1 << MASK_RX_DR);
	nrf24_write(CONFIG,&data,1);
	
	// Start SPI, load message into TX_PAYLOAD
	csn_low;
	if (AUTO_ACK) spi_send(W_TX_PAYLOAD);
	else spi_send(W_TX_PAYLOAD_NOACK);
	while (length--) spi_send(*(uint8_t *)tx_message++);
	spi_send(0);
	csn_high;
	
	//	Send message by pulling CE high for more than 10us
	ce_high;
	_delay_us(15);
	ce_low;
	
	//	Wait for message to be sent (TX_DS flag raised)
	nrf24_read(STATUS,&data,1);
	while(!(data & (1 << TX_DS))) nrf24_read(STATUS,&data,1);
	
	// Enable interrupt on RX
	nrf24_read(CONFIG,&data,1);
	data &= ~(1 << MASK_RX_DR);
	nrf24_write(CONFIG,&data,1);

	//	Continue listening
	nrf24_start_listening();
	
	return 1;
}

unsigned int nrf24_available(void)
{
	uint8_t config_register;
	nrf24_read(FIFO_STATUS,&config_register,1);
	if (!(config_register & (1 << RX_EMPTY))) return 1;
	return 0;
}

const char * nrf24_read_message(void)
{
	// Message placeholder
	static char rx_message[32];
	memset(rx_message,0,32);
	
	// Write ACK message
	if (AUTO_ACK) nrf24_write_ack();
	
	// Get length of incoming message
	nrf24_read(R_RX_PL_WID,&data,1);
	
	// Read message
	if (data > 0) nrf24_send_spi(R_RX_PAYLOAD,&rx_message,data+1);

	// Check if there is message in array
	if (strlen(rx_message) > 0)
	{
		//	Clear RX interrupt
		data = (1 << RX_DR);
		nrf24_write(STATUS,&data,1);
		
		return rx_message;
	}
	
	//	Clear RX interrupt
	data = (1 << RX_DR);
	nrf24_write(STATUS,&data,1);
	
	return "failed";
}
