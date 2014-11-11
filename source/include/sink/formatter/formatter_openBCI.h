
#ifndef FORMATTER_OPENBCI_H_
#define FORMATTER_OPENBCI_H_

#include <fpga-log/sink/formatter/formatter.h>

#define OPEN_BCI_PCKT_START 0xA0
#define OPEN_BCI_PCKT_END 0xC0

typedef struct {
	formatter_t super; 	/**< super-"class": formatter_t */

	long count;
} formatter_open_bci_t;

void formatter_open_bci_init(formatter_open_bci_t* const formatter);

#endif 
