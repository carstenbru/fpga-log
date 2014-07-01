/**
 * @file logger_config.h
 * @brief configuration of the datalogger modules
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#ifndef LOGGER_CONFIG_H_
#define LOGGER_CONFIG_H_

/**
 * @brief calls the init function of all objects of the datalogger
 */
void init_objects(void);

/**
 * @brief connects the ports (control and data) of the datastream objects of the datalogger
 */
void connect_ports(void);

/**
 * @brief calls additional configuration fuctions of the objects of the datalogger
 */
void advanced_config(void);

#endif
