# Lightweight library for nRF24L01/nRF24L01+ running on AVR architecture

Bare minimum software developed for nRF24L01/nRF24L01+ RF chip. Software was tested on ATmega328P and ATmega328PB (PB needs few changes in SPI). Can be easily ported to any MCU by adjusting SPI library.

## Hardware

```
nRF24L01+ | ATmega328P  | Arduino UNO
-----------------------------------
  CE      |     PB1     |    D9
  CSN     |     PB2     |    D10
  IRQ     |     PD2     |    D2
  MOSI    |     PB3     |    D11
  MISO    |     PB4     |    D12
  SCK     |     PB5     |    D13
```

## Software

For testing I used RPi as receiver/sender as well. Follow this tutorial to set up Raspberry Pi http://invent.module143.com/daskal_tutorial/raspberry-pi-3-wireless-pi-to-arduino-communication-with-nrf24l01/

### Listen to incomming messages

```
nrf24_start_listening();
```
By default when there is something on RX register, this ISR gets triggered:
```
ISR(INT0_vect) 
```
### Send message

After message is sent, it goes back to listening mode.

```
status = nrf24_send_message(tx_message);
```
Where status returns '1' on successful send and 'tx_message' is a string set with <string.h>:
```
strcpy(tx_message,"Your message here");
```
'tx_message' has to be 32 bytes or smaller

## Settings

If auto-acknowledgment is disabled, keep in mind that using lower data rates such as 250kbps and 1mbps will lose packets if for example payload exceeds 4 bytes for 250kbps therefore 2mbps should be used. With auto-acknowledgment enabled 250kbps transmits 32 bytes with no problem.

```
#define DATARATE		RF_DR_2MBPS							// 250kbps, 1mbps, 2mbps
#define POWER			POWER_MAX							// Set power (MAX 0dBm..HIGH -6dBm..LOW -12dBm.. MIN -18dBm)
#define CHANNEL			0x76								// 2.4GHz-2.5GHz channel selection (0x01 - 0x7C)
uint8_t rx_address[5] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 };	// Read pipe address
uint8_t tx_address[5] = { 0xe7, 0xe7, 0xe7, 0xe7, 0xe7 };	// Write pipe address
#define READ_PIPE		0									// Number of read pipe
#define MESSAGE_LENGTH	32									// 0-32 bytes
//
//	-AUTO_ACK can be disabled when running on 2MBPS @ <= 32 byte messages.
//	-250KBPS and 1MBPS with AUTO_ACK disabled lost many packets
//	if the packet size was bigger than 4 bytes.
//	-If AUTO_ACK is enabled, tx_address = rx_address.
//
#define AUTO_ACK		true								// Auto acknowledgment
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
```

## Compiled with

Atmel Studio 7 (Version: 7.0.1417)


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

