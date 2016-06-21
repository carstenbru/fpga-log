/**
 * @file dcf77.h
 * @brief DCF77 peripheral registers
 * 
 * @author Stefan Klir (s.klir@gmx.de)
 */

#ifndef __DCF77_H
#define __DCF77_H

//Control BITs
#define DCF77_RESET 0				//Bit 0
#define DCF77_ENABLE_INTERRUPT 1	//Bit 1

//Not all pwm_ib_values are used. Only use the number of values which is specified in NUMBER_OF_BOXES!!!
typedef struct {
	//CONTROL Register:
	//Bit 0: Reset
	//Bit 1-17: nicht definiert
	volatile unsigned int control;

	volatile unsigned int day;
	volatile unsigned int month;
	volatile unsigned int year;
	volatile unsigned int hour;
	volatile unsigned int minute;
	volatile unsigned int weekDay;	//17: is bit 19 auf dcf signal + 2-0: Wochentag
	volatile unsigned int start_bits_19_1;	//dcf bits 18-1
} dcf77_regs_t;

#endif
