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

#include <avr/common.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "spi.h"

#define DDR_SPI		DDRB
#define DD_MOSI		DDB3
#define DD_MISO		DDB4
#define DD_SCK		DDB5

void spi_master_init( void )
{
	DDR_SPI &= ~_BV(DD_MISO);
	DDR_SPI |= (_BV(DD_MOSI) | _BV(DD_SCK));
	/* 19.5.1 SPCR – SPI Control Register
	 *
	 * • Bit 7 – SPIE: SPI Interrupt Enable
	 * This bit causes the SPI interrupt to be executed if SPIF bit in the SPSR Register is set and the if the Global
	 * Interrupt Enable bit in SREG is set.
	 *
	 * • Bit 6 – SPE: SPI Enable
	 * When the SPE bit is written to one, the SPI is enabled. This bit must be set to enable any SPI operations.
	 *
	 * • Bit 5 – DORD: Data Order
	 * When the DORD bit is written to one, the LSB of the data word is transmitted first.
	 * When the DORD bit is written to zero, the MSB of the data word is transmitted first.
	 *
	 * • Bit 4 – MSTR: Master/Slave Select
	 * This bit selects Master SPI mode when written to one, and Slave SPI mode when written logic zero. If SS is
	 * configured as an input and is driven low while MSTR is set, MSTR will be cleared, and SPIF in SPSR will become
	 * set. The user will then have to set MSTR to re-enable SPI Master mode.
	 *
	 * • Bit 3 – CPOL: Clock Polarity
	 * When this bit is written to one, SCK is high when idle. When CPOL is written to zero, SCK is low when idle.
	 *
	 * • Bits 1, 0 – SPR1, SPR0: SPI Clock Rate Select 1 and 0
	 * These two bits control the SCK rate of the device configured as a Master. SPR1 and SPR0 have no effect on
	 * the Slave. The relationship between SCK and the Oscillator Clock frequency fosc is shown in the following table:
	 *
	 * Table 19-5 Relationship Between SCK and Oscillator Frequency
	 * SPI2X | SPR1 | SPR0 | SCK Frequency
	 * ===================================
	 *   0   |   0  |   0  | fosc/4
	 *   0   |   0  |   1  | fosc/16
	 *   0   |   1  |   0  | fosc/64
	 *   0   |   1  |   1  | fosc/128
	 *   1   |   0  |   0  | fosc/2
	 *   1   |   0  |   1  | fosc/8
	 *   1   |   1  |   0  | fosc/32
	 *   1   |   1  |   1  | fosc/64
	 */
	SPCR = (0 << SPIE) |
	       (1 << SPE)  |
	       (0 << DORD) |
	       (1 << MSTR) |
	       (0 << CPOL) |
	       (0 << SPR1) | (0 << SPR0);
	/* 19.5.2   SPSR – SPI Status Register
	 *
	 * • Bit 7 – SPIF: SPI Interrupt Flag
	 * When a serial transfer is complete, the SPIF Flag is set. An interrupt is generated if SPIE in SPCR is set and
	 * global interrupts are enabled. If SS is an input and is driven low when the SPI is in Master mode, this will also
	 * set the SPIF Flag. SPIF is cleared by hardware when executing the corresponding interrupt handling vector.
	 * Alternatively, the SPIF bit is cleared by first reading the SPI Status Register with SPIF set, then accessing the
	 * SPI Data Register (SPDR).
	 *
	 * • Bit 0 – SPI2X: Double SPI Speed Bit
	 * When this bit is written logic one the SPI speed (SCK Frequency) will be doubled when the SPI is in Master
	 * mode (see Table 19-5). This means that the minimum SCK period will be two CPU clock periods. When the SPI
	 * is configured as Slave, the SPI is only guaranteed to work at fosc/4 or lower. */
	SPSR |= _BV(SPI2X);
}

void spi_bulk_send( uint8_t *send_buffer, uint8_t count )
{
	while ( count-- ) {
		SPDR = *send_buffer++;
		loop_until_bit_is_set(SPSR, SPIF);
	}
}

void spi_send( uint8_t send_data )
{
	SPDR = send_data;
	loop_until_bit_is_set(SPSR, SPIF);
}

void spi_bulk_exchange( uint8_t *send_buffer, uint8_t *receive_buffer, uint8_t count )
{
	while ( count-- ) {
		SPDR = *send_buffer++;
		loop_until_bit_is_set(SPSR, SPIF);
		*receive_buffer++ = SPDR;
	}
}

uint8_t spi_exchange( uint8_t send_data )
{
	SPDR = send_data;
	loop_until_bit_is_set(SPSR, SPIF);
	return SPDR;
}
