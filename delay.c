#include "delay.h"

//functions prototype
void _delay_ms(int delayIn_MS);
void _delay_us(int delayIn_uS);

//function to give delay of X Milli-seconds
void _delay_ms(int delayIn_MS)
{	int i;
	double j=0;
	for (i=0; i<delayIn_MS; i++)
		for (j=0; j<MS_DelayCnt; j++);
}

//function to give delay of X micro-seconds
void _delay_us(int delayIn_uS)
{	int i;
	double j=0;
	for (i=0; i<delayIn_uS; i++)
		for (j=0; j<uS_DelayCnt; j++);
}
