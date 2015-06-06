/*
 * uart.c
 *
 * Created: 8/2/2014 2:50:33 AM
 *  Author: rohit
 */ 

#include "usart.h"

//Function to initialize the USART
void Init_USART(uint16_t ubrr){
	UCSRB =  (1 << RXEN) | (1 << TXEN);      // Enable receiver and transmitter              
   // UCSRC = (1 << URSEL) | (3 << UCSZ0);     // Set frame format: 8-bit data, 1-stop bit
	UCSRC |= (1 << URSEL ) | (1 << UCSZ0 ) | (1 << UCSZ1 ); // Use 8- bit character sizes
    // Set baud rate
	UBRRL = (uint8_t)BAUD_PRESCALE;          // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRRH = (uint8_t)(BAUD_PRESCALE >> 8);   // Load higher 8-bits of the baud rate value into the higher byte of the UBRR register                      
    //UCSRB |= (1 << RXCIE );                  // Enable the USART Recieve Complete interrupt ( USART_RXC )
    //sei ();                                  // Enable the Global Interrupt Enable flag so that interrupts can be processed
}

//Function that Sends Frames with 5 to 8 Data Bit
void USART_Send8BitData(unsigned char data){
	while ( !( UCSRA & (1<<UDRE)) );        //Wait for empty transmit buffer
	UDR = data;                             //Put data into buffer, sends the data
}

//Function that Sends Frames with 9 Data Bit
void USART_Send9BitData(unsigned int data){	
	while ( !( UCSRA & (1<<UDRE)) );       //Wait for empty transmit buffer
	UCSRB &= ~(1<<TXB8);                   //Copy 9th bit to TXB8
	if ( data & 0x0100 )
		UCSRB |= (1<<TXB8);	
	UDR = data;                           //Put data into buffer, sends the data
}

//Function that Receives Frames with 5 to 8 Data Bits
unsigned char USART_Receive8BitData(void){	
	while ( !(UCSRA & (1<<RXC)) );       //Wait for data to be received	
	return UDR;                          //Get and return received data from buffer
}

//Function that Receives Frames with 9 Data Bits
unsigned int USART_Receive9BitData(void){
	unsigned char status, resh, resl;	
	while ( !(UCSRA & (1<<RXC)) );        //Wait for data to be received	
	//Get status and 9th bit, then data from buffer
	status = UCSRA;
	resh = UCSRB;
	resl = UDR;	
	//If error, return -1
	if ( status & ((1<<FE)|(1<<DOR)|(1<<PE)) )
		return -1;
	//Filter the 9th bit, then return
	resh = (resh >> 1) & 0x01;
	return ((resh << 8) | resl);
}

//Function that Flush the Receive Buffer
void USART_Flush(void){
	unsigned char dummy;
	while ( UCSRA & (1<<RXC) ) 
	  dummy = UDR;
}

//Function that sends the string via USART
void USART_SendString(char *str) {
	int h;
	while(*str)         
		USART_Send8BitData(*str++);       // send message
	for(h=0;h<=200;h++); 
}

//ISR(USART_RXC_vect){
	// interrupt ISR code
//}
