/**
 * @file peripherals.h
 * @brief peripheral definitions to simulate the SpartanMC
 *
 * Some peripherals are simulated using pipes (fifo's).
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef PERIPHERAL_PIPES_H_
#define PERIPHERAL_PIPES_H_

/*
 * number of pipes (one per peripheral)
 */
#define PIPE_COUNT 2

/*
 * pipe numbers of the different peripherals in the system
 */
#define UART_LIGHT_PC 0
#define UART_LIGHT_1  1


/*
 * number of timer and compare peripherals
 */
#define TIMER_COUNT 1
#define COMPARE_COUNT 1
/*
 * Timer and timer units numbers
 *
 * Use the same number on a compare unit and a timer
 * to assign the compare to the timer output.
 */
#define TIMER_0 	0
#define COMPARE_0 0

#endif
