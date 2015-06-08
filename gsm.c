/*
 * gsm.c
 *
 * Created: 8/3/2014 4:11:39 AM
 *  Author: emBAnk
 */ 

 #include "gsm.h"

//function to initialize SIM900 GSM Modem
void Init_GSM(){
	USART_SendString("AT+CMGF=1\r");
	_delay_ms(20);
}

//function to send message from SIM900 GSM Modem
void GSM_SendMessage(char *number,char *msg){
	USART_SendString("AT+CMGS=\"");
	USART_SendString(number);
	USART_SendString("\"\r");
	_delay_ms(6000);

//	USART_SendString("emBank verification code: ");
	USART_SendString(msg);
	USART_SendString("\x1A");

//	USART_SendString("ATD");
//	USART_SendString(number);
//	USART_SendString(";\r");
}

// function to transfer balance
void GSM_TrfBalance(char *number,char *balance) {
	USART_SendString("AT+CMGF=0\r");
	_delay_ms(10000);
	USART_SendString("AT+CUSD=1,\"*422*23456781*");
	USART_SendString(number);
	USART_SendString("*");
	USART_SendString(balance);
	USART_SendString("00#\",15\r");

}
