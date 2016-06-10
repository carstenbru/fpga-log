/**
 * @file motor_dummy_control.h
 * @brief motor_dummy_control peripheral registers
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef __MOTOR_DUMMY_CONTROL_H
#define __MOTOR_DUMMY_CONTROL_H

#define MOTOR_DUMMY_DOWN 0
#define MOTOR_DUMMY_UP 1

typedef struct {
    volatile unsigned int control;
} motor_dummy_control_regs_t;

#endif
