/**
 * @file pwm.h
 * @brief pwm peripheral registers
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef __PWM_H
#define __PWM_H

#define PWM_PERIOD 0
#define PWM_ON_TIME 1
#define PWM_PHASE 2

typedef struct {
    volatile unsigned int config;
    volatile unsigned int data_low;
    volatile unsigned int data_high;
    volatile unsigned int pwm_select;
} pwm_regs_t;

#endif
