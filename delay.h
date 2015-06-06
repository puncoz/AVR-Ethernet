#ifndef _DELAY_H_
#define _DELAY_H_


#define MS_DelayCnt	54		//delay count of 8MHz for Milli-seconds
#define uS_DelayCnt	10		//delay count of 8MHz for micro-seconds

//function to give delay of X Milli-seconds
void _delay_ms(int delayIn_MS);

//function to give delay of X micro-seconds
void _delay_us(int delayIn_uS);



#endif
