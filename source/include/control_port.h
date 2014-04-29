 /*
 * control_port structure to realize control interface to device drivers
 * 
 * Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */


#ifndef CONTROL_PORT_H_
#define CONTROL_PORT_H_

/*
 * struct defining a control destination
 */
typedef struct {
  void* parent;		/* pointer to the parent struct, e.g. a device module */
  //TODO multiple function pointers for contol functions
} control_port;

#endif