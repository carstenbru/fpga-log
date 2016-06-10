/**
 * @file datastream_object.c
 * @brief datastream_object as super-class for sinks/devices, update function to update all datastream_objects
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <fpga-log/datastream_object.h>

int datastream_object_count = 0;
datastream_object_t* datastream_object_list[MAX_DATASTREAM_OBJECTS];

/**
 * adds an object to the list of datastream_object_t
 *
 * @param new_object	the object to add
 * @return	1 on success, 0 if something went wrong (e.g. list was full)
 */
static int datastream_list_add(datastream_object_t* const new_object) {
	if (datastream_object_count < MAX_DATASTREAM_OBJECTS) {
		datastream_object_list[datastream_object_count++] = new_object;
		return 1;
	} else {
		//TODO call some error code
		return 0;
	}
}

void datastream_objects_update(void) {
	int i;
	for (i = 0; i < datastream_object_count; i++) {
		datastream_object_t* obj = datastream_object_list[i];
		obj->update(obj);
	}
}

/**
 * @brief	dummy update function
 *
 * @param	object		pointer to the datastream_object
 */
static void datastream_object_update_summy(void* const object) {
}

void datastream_object_init(datastream_object_t* const object) {
	object->update = datastream_object_update_summy;

	datastream_list_add(object);
}
