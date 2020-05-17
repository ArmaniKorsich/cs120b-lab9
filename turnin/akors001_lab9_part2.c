
/*	Author: armanikorsich
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdbool.h>

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
    
    TCCR1B = 0x0b;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void Timerff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}


void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }

		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; } 
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}


enum SM_States {START, sm_1, sm_2, sm_3} state;
bool b1 = false;

double notes[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
unsigned char count = 0;

void TickFct() {
	switch(state) {
		case START:
			if ((~PINA & 0x07) == 0x01) {
				state = sm_1;
				b1 = !b1;
			} else if ((~PINA & 0x07) == 0x02) {
				state = sm_2;
				if(count < 7) {
					count++;
				}
			} else if ((~PINA & 0x07) == 0x04) {
				state = sm_3;
				if (count > 0) {
					count--;
				}
			} else {
				state = START;
			}
			break;
		case sm_1:
			if ((~PINA & 0x07) == 0x01) {
				state = sm_1;
			} else {
				state = START;
			}	
			break;
		case sm_2:
			if ((~PINA & 0x07) == 0x02) {
				state = sm_2;
			} else {
				state = START;
			}
			break;
		case sm_3:
			if ((~PINA & 0x07) == 0x04) {
				state = sm_3;
			} else {
				state = START;
			}
			break;
		default:
			break;

	}

	switch(state) {
		case START:
		case sm_1:
		case sm_2:
		case sm_3:
			if(b1) {
				set_PWM(notes[count]);
			} else {
				set_PWM(0);
			}
			break;	
		default:
			set_PWM(0);
			break;
	}


}
int main(void) {
	/* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;
    	

    TimerSet(50);
    TimerOn();
    PWM_on();
	PORTB = 0x01;
    /* Insert your solution below */
    while (1) {
        while (!TimerFlag);
        TimerFlag = 0;
   	TickFct();
	}
    return 1;    

}



