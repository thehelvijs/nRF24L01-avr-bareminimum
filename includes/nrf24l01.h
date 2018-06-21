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

#ifndef _NRF24L01_H
#define _NRF24L01_H

//	States
#define POWERUP		1
#define POWERDOWN	2
#define RECEIVE		3
#define TRANSMIT	4
#define STANDBY1	5
#define STANDBY2	6

//	Forward declarations
uint8_t nrf24_send_spi(uint8_t register_address, void *data, unsigned int bytes);
uint8_t nrf24_write(uint8_t register_address, uint8_t *data, unsigned int bytes);
uint8_t nrf24_read(uint8_t register_address, uint8_t *data, unsigned int bytes);
void nrf24_init(void);
void nrf24_state(uint8_t state);
void nrf24_start_listening(void);
unsigned int nrf24_available(void);
const char * nrf24_read_message(void);
uint8_t nrf24_send_message(const void *tx_message);

#endif /*_NRF24L01_H*/
