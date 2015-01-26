/**
 * @file dm_package_buffer.h
 * @brief module to buffer data_packages until requested
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

//TODO build a module around this!!!
#ifndef DM_PACKAGE_BUFFER_H_
#define DM_PACKAGE_BUFFER_H_

#include <fpga-log/data_port.h>

typedef struct {
	data_package_t package; /**< the normal package structure */

	unsigned char data[DATA_TYPE_MAX_BYTES]; /**< package data memory */
	timestamp_t timestamp; /**< timestamp storage */
} data_package_stored_t;

typedef struct {
	data_package_stored_t* storage_array; /**< array for storing the actual data */
	int array_size; /**< size of the storage_arry */

	int next_read; /**< ringbuffer: indicates the position of the next package to read */
	int next_write; /**< ringbuffer: indicates the position where the next package should be stored */
	int cur_size; /**< current number of elements in the fifo */
} data_package_fifo_t;

void _data_package_fifo_init(data_package_fifo_t* data_package_fifo,
		data_package_stored_t* storage_array, int array_size);

void data_package_fifo_put(data_package_fifo_t* data_package_fifo,
		const data_package_t* const package);

data_package_t* data_package_fifo_get(data_package_fifo_t* data_package_fifo);

#endif
