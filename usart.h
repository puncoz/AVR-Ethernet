/*
 * uart.h
 *
 * Created: 8/2/2014 2:51:05 AM
 *  Author: rohit
 */ 


#ifndef UART_H_
#define UART_H_

#define F_CPU	12000000UL	//12MHz Clock
#include<avr/io.h>
#include <avr/interrupt.h>

//Set-up BAUD Rate
#define USART_BAUDRATE 9600	//BAUD Rate for USART
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1 )	//Calculating Actual BAUD Rate


/************************************************************************/
/*		Function to initialize the USART
		-->	The initialization process normally consists
			of setting the baud rate, setting frame format
			and enabling the Transmitter or the Receiver
			depending on the usage.                                     */
/************************************************************************/
void Init_USART(uint16_t ubrr);

/************************************************************************/
/*     Function that Sends Frames with 5 to 8 Data Bit
		-->	It simply waits for the transmit buffer to be empty 
			by checking the UDRE Flag, before loading it with new 
			data to be transmitted. If the Data Register Empty Interrupt 
			is utilized, the interrupt routine writes the data 
			into the buffer.											*/
/************************************************************************/
void USART_Send8BitData( unsigned char data );

/************************************************************************/
/*     Function that Sends Frames with 9 Data Bit
		-->	If 9-bit characters are used (UCSZ = 7), the ninth bit 
			must be written to the TXB8 bit in UCSRB before the low 
			byte of the character is written to UDR.
			
			The ninth bit can be used for indicating an address frame 
			when using multi processor communication mode or for other 
			protocol handling as for example synchronization.			*/
/************************************************************************/
void USART_Send9BitData( unsigned int data );

/************************************************************************/
/*     Function that Receives Frames with 5 to 8 Data Bits
		-->	It simply waits for data to be present in the receive 
			buffer by checking the RXC Flag, before reading the 
			buffer and returning the value.								*/
/************************************************************************/
unsigned char USART_Receive8BitData( void );

/************************************************************************/
/*     Function that Receives Frames with 9 Data Bits
		-->	If 9 bit characters are used (UCSZ=7) the ninth bit must be 
			read from the RXB8 bit in UCSRB before reading the low bits 
			from the UDR. This rule applies to the FE, DOR and PE 
			Status Flags as well. Read status from UCSRA, then data 
			from UDR. Reading the UDR I/O location will change the state 
			of the receive buffer FIFO and consequently the TXB8, FE, 
			DOR and PE bits, which all are stored in the FIFO, will 
			change.
			
			This function reads all the I/O Registers into the 
			Register File before any computation is done. 
			This gives an optimal receive buffer utilization 
			since the buffer location read will be free to accept 
			new data as early as possible.								*/
/************************************************************************/
unsigned int USART_Receive9BitData( void );

/************************************************************************/
/*     Function that Flush the Receive Buffer
		-->	The receiver buffer FIFO will be flushed when the 
			Receiver is disabled, that is, the buffer will be emptied 
			of its contents. Unread data will be lost. If the buffer 
			has to be flushed during normal operation, due to for 
			instance an error condition, read the UDR I/O location 
			until the RXC Flag is cleared.								*/
/************************************************************************/
void USART_Flush( void );

/************************************************************************/
/*     Function that sends the string via USART							*/
/************************************************************************/
void USART_SendString( char *str);



#endif /* UART_H_ */
