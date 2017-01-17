/*
 * uart.c
 *
 * Created: 7.12.2016 15:17:39
 *  Author: jbbja
 */ 

#include "uart.h"

// viljum 2 bit stop bits 8 bit data, asynchronous mode, parity mode off,
void uart_init(void)
{
	/* Configure peripheral clock UART. */
	PMC->PMC_PCER0 = (1 << ID_UART);
	/* Enable receiver and transmitter */
	UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
	/* Turn parity mode off */
	UART->UART_MR = UART_MR_PAR_NO;	
	/* Baud rate generator register */
	UART->UART_BRGR = CLOCKDIVISION;
	/* Reset interrupt enable register */
	UART->UART_IDR = 0xFFFF;
}

// Transmission interrupt enable to minimize calculation downtime
void uart_tx_interrupt_init(void)
{
	//UART->UART_IER = UART_IER_TXRDY;
	
	enable_interrupt_vector(UART_IRQn, UART_PRIORITY);
	
	// Initialize Transmit buffer character array
	for (uint16_t i = 0; i < TXBUFFERSIZE; i++)
	{
		TxString[i] = NULL;
		RxString[i] = NULL;
	}
	// Initialize Transmit/receive buffer iterator for array
	TxStringHead = TxStringTail = 0;
	RxStringHead = RxStringTail = 0;
}

void uart_interrupt_transfer(char * str)
{
	uint16_t i = 0;
	while(str[i] != NULL && TxStringHead < TXBUFFERSIZE)
		TxString[TxStringHead++] = str[i++];
	UART->UART_IER = UART_IER_TXRDY;
}

void uart_rx_interrupt_init(void)
{
	enable_interrupt_vector(UART_IRQn, UART_PRIORITY);
	UART->UART_IER = UART_IER_RXRDY;
}

void uart_rx_read_buffer(void)
{
	if (RxStringTail == RxStringHead)
	{
		RxStringTail = RxStringHead = 0;
		return;
	}
	uint8_t read = RxString[RxStringTail++];
	if (read == 'A')
	{
		uart_interrupt_transfer("ER");
	}
}

void uart_transfer(uint8_t transmit)
{
	while (!(UART->UART_SR & UART_SR_TXRDY));
	UART->UART_THR = transmit;
}
//Send Integers over to terminal
void uart_print_int(uint32_t data)
{
	uint32_t div = 1000000000;		// Divider to divide data with
	uint8_t start = 0;
	uint8_t cnt = 10;
	for (int i = 1; i <= cnt; i++)
	{
		uint8_t send = data / div + 48; // calculate the Ascii for each number
		if(send != 48 || start == 1 || i == cnt)
		{
			//Wait until hardware is ready to send data, UDRE0 = Data register empty
			uart_transfer(send);
			start = 1;
		}
		data %= div;
		div /= 10;
	}
	//uart_new_line();
}
void uart_new_line(void)
{
	uart_transfer(10);
}

void uart_print_string(char * data)
{
	int i = 0;
	// loop which print strings till null is reached or more than 20 characters (for safety)
	while(data[i] != 0 || i > 20)
		uart_transfer(data[i++]);
	uart_transfer(32); // space
}

void UART_Handler(void)
{
	// Check the interrupt status, transmit or receive ?
	uint32_t status = UART->UART_SR;
	
	// Transmit data from buffer if the buffer contains data (TxStringHead > 0) 
	if (TxStringHead && (status & UART_SR_TXRDY))
	{
		// Load the buffer character to transmitting register
		UART->UART_THR = TxString[TxStringTail];
		// Increment buffer tail iterator 
		TxStringTail += 1;
		// Check if the message has been fully transmitted
		if (TxStringTail >= TxStringHead)
		{
			// Reset iterators
			TxStringTail = TxStringHead = 0;
			// Disable transmit interrupts
			UART->UART_IDR = UART_IDR_TXRDY;
		}
	}
	if (status & UART_SR_RXRDY)
	{
		
		// The bit RXRDY is automatically cleared when the receive holding register UART_RHR is read
		uint8_t receive = UART->UART_RHR;
		// If UART_RHR has not been read by the sofware since the last transfer, overrun bit
		// If there was a parity error during the receive, reset in control register
		if (status & UART_SR_OVRE || status & UART_SR_PARE)
		{
			// TODO: Send an error to computer
			// Clear the overrun bit in control register
			UART->UART_CR = UART_CR_RSTSTA;
			return;
		}
		
		RxString[RxStringHead++] = receive;
		if (RxStringHead >= RXBUFFERSIZE)
		{
			RxStringHead = 0;
		}
	}
}