/**
 * @file dm_package_buffer.c
 * @brief module to buffer data_packages until requested
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/dm/dm_package_buffer.h>

void _data_package_fifo_init(data_package_fifo_t* data_package_fifo,
		data_package_stored_t* storage_array, int array_size) {
	data_package_fifo->storage_array = storage_array;
	data_package_fifo->array_size = array_size;

	data_package_fifo->next_read = 0;
	data_package_fifo->next_write = 0;
	data_package_fifo->cur_size = 0;
}

static void data_package_wrap_pointers(data_package_fifo_t* data_package_fifo) {
	if (data_package_fifo->next_write >= data_package_fifo->array_size)
		data_package_fifo->next_write -= data_package_fifo->array_size;
	if (data_package_fifo->next_read >= data_package_fifo->array_size)
		data_package_fifo->next_read -= data_package_fifo->array_size;
}

void data_package_fifo_put(data_package_fifo_t* data_package_fifo,
		const data_package_t* const package) {
	data_package_stored_t* element = data_package_fifo->storage_array
			+ data_package_fifo->next_write;  //get element to write

	//store package with data and re-arrange pointers in package to our copy
	element->package = *package;
	int i;
	for (i = 0; i < data_type_byte_sizes[package->type]; i++) {  //copy data from package
		element->data[i] = *((unsigned char*) (package->data + i));
	}
	element->package.data = &element->data;
	element->timestamp = *package->timestamp;
	element->package.timestamp = &element->timestamp;

	data_package_fifo->next_write++;  //increase write pointer

	if (data_package_fifo->cur_size < data_package_fifo->array_size) {
		data_package_fifo->cur_size++;  //increase current fifo size
	} else {
		data_package_fifo->next_read++;  //fifo full, data loss, move read pointer to new first element
	}

	data_package_wrap_pointers(data_package_fifo);  //re-adjust the pointers (wrap-around, ring-buffer)
}

data_package_t* data_package_fifo_get(data_package_fifo_t* data_package_fifo) {
	if (data_package_fifo->cur_size == 0)
		return 0;

	data_package_stored_t* element = data_package_fifo->storage_array
			+ data_package_fifo->next_read;  //get element to read

	data_package_fifo->cur_size--;
	data_package_fifo->next_read++;
	data_package_wrap_pointers(data_package_fifo);  //re-adjust the pointers (wrap-around, ring-buffer)

	return &element->package;
}
