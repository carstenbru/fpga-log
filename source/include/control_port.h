/**
 * @file control_port.h
 * @brief control_port structure to realize control interface to device drivers
 * 
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */


#ifndef CONTROL_PORT_H_
#define CONTROL_PORT_H_

/**
 * @brief struct defining a control destination
 */
typedef struct {
  void* parent;		/**< pointer to the parent struct, e.g. a device module */
  //TODO multiple function pointers for contol functions
} control_port_t;

#endif