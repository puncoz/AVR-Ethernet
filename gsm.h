/*
 * gsm.h
 *
 * Created: 8/3/2014 4:11:23 AM
 *  Author: emBank
 */ 


#ifndef GSM_H_
#define GSM_H_

#include<avr/io.h>
#include "usart.h"

void Init_GSM();                                //function to initialize SIM300 GSM Modem
void GSM_SendMessage(char *number,char *msg);   //function to send message from SIM300 GSM Modem

#endif /* GSM_H_ */
