
#ifndef CONTROL_PROTOCOL_OPENBCI_H_
#define CONTROL_PROTOCOL_OPENBCI_H_

#include <fpga-log/sink/control_protocol.h>

typedef struct {
	control_protocol_t super; /**< super-"class": control_protocol_t */

} control_protocol_open_bci_t;

void control_protocol_open_bci_init(
		control_protocol_open_bci_t* const control_protocol_open_bci);

#endif
