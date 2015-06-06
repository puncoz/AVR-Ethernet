/*
 * timer.c
 *
 * Created: 8/23/2013 10:14:53 PM
 *  Author: Puncoz Nepal
 */ 
#include "timer.h"

//global variables
volatile uint8_t count;
uint8_t flag;

//functions prototypes
void initTimer_heartBeat();

//function to initialize Heart-Beat
void initTimer_heartBeat()
 {
	 /************************************************************************/
	 /*		CS02	CS01	CS00
			0		0		0		-->		Timer Stop
			0		0		1		-->		FCPU
			0		1		0		-->		FCPU/8
			0		1		1		-->		FCPU/64
			1		0		0		-->		FCPU/256
			1		0		1		-->		FCPU/1024
			1		1		0		-->		External Clock Source on PIN T0.
											Clock On Falling Edge
			1		1		1		-->		External Clock Source on PIN T0.
											Clock On Rising Edge			 */
	 /************************************************************************/
	 
	// Prescaler = FCPU/1024
	TCCR0 |= (1 << CS02) | (1 << CS00);
   
	//Enable Overflow Interrupt Enable
	TIMSK |= 1 << TOIE0;
   
	//Initialize Counter
	TCNT0 = 0;
   
	//Initialize our variable
	count = 0;
	flag = 0;
   
	//Set Port as output
	heartBeatDDR |= (1 << heartBeatPIN);

	sei();
 }
 
//Interrupt Service Routine for TIMER0 Overflow Interrupt
ISR(TIMER0_OVF_vect)
{
	//This is the interrupt service routine for TIMER0 OVERFLOW Interrupt.
	//CPU automatically call this when TIMER0 overflows.

	//Increment our variable
	count++;
	if(count == 16)	//toggle in every 500ms
	{
		count = 0;
		if (flag == 0)
		{
			heartBeatPORT |= (1 << heartBeatPIN);
			flag = 1;
		}
		else if (flag == 1)
		{
			heartBeatPORT &= ~(1 << heartBeatPIN);
			flag = 0;
		}
	}//end of if
	
	/************************************************************************/
	/* Frequency of overflow = 8MHz / 1024 = 7812.5Hz (i.e. 1 Sec = 7812.5 Hz)
		
		count = 7812.5 / 256 = 30.51 (for 1 sec)
	
	Interrupts occur at around 31 Hz (actually 7812.5 / 256 = 30.51 Hz) 
		so in 1 sec there are 31 interrupts. Therefore counter is set to 0 
		every 1 sec.that means heartBeatPORT will toggled every 1 sec. 
		That means it is ON for 1 sec and OFF for 1 sec. So time period 
		is 2 sec and freq is 1/2 = 0.5Hz									*/
	/************************************************************************/
}//end of ISR
