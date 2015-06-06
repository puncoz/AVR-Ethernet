/*
 * timer.h
 *
 * Created: 8/23/2013 10:14:43 PM
 *  Author: Puncoz Nepal
 */ 


#ifndef TIMER_H_
#define TIMER_H_


#include<avr/io.h>
#include<avr/interrupt.h>

/********************HeartBeat CONFIG**************************/

#define heartBeatDDR	DDRC
#define heartBeatPORT	PORTC
#define heartBeatPIN	PINC0

/******************************************************/

//function to initialize Heart-Beat
void initTimer_heartBeat();


#endif /* TIMER_H_ */
