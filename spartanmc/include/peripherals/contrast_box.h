/**
 * @file contrast_box.h
 * @brief contrast_box peripheral registers
 * 
 * @author Stefan Klir (klir@lichttechnik.tu-darmstadt.de)
 */

#ifndef __CONTRAST_BOX_H
#define __CONTRAST_BOX_H

//Control BITs
#define CONTRAST_BOX_DISABLE 0	//Bit 0
#define CONTRAST_BOX_ENABLE 1	//Bit 0

#define CONTRAST_BOX_RESET 2	//Bit 1

//Not all pwm_ib_values are used. Only use the number of values which is specified in NUMBER_OF_BOXES!!!
typedef struct {
	//CONTROL Register:
	//Bit 0: Enable/Disable
	//Bit 1: Reset
	//Bit 2-14: nicht definiert
	//Bit 15: Number of Boxes Bit 0
	//Bit 16: Number of Boxes Bit 1
	//Bit 17: Number of Boxes Bit 2
	volatile unsigned int control;
	volatile unsigned int pwm_on_value[8];
} contrast_box_regs_t;

#endif
