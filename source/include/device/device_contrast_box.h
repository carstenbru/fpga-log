/**
 * @file device_contrast_box.h
 * @brief Contrast Box LED/Switch output/input
 *
 * @author Stefan Klir (klir@lichttechnik.tu-darmstadt.de)
 */

#ifndef CONTRAST_BOX_H_
#define CONTRAST_BOX_H_

#include <fpga-log/datastream_source.h>
#include <fpga-log/data_port.h>

#include <peripherals/contrast_box.h>

typedef struct {
	datastream_source_t super; /**< super-"class": datastream_object_t*/

	const data_port_t* data_out; /**< data output (on request) destination */

	contrast_box_regs_t* contrast_box;

} device_contrast_box_t;

void device_contrast_box_init(device_contrast_box_t* const contrast_box,
		contrast_box_regs_t* contrast_box_peri, const unsigned int id);

void device_contrast_box_set_data_out(
		device_contrast_box_t* const contrast_box,
		const data_port_t* const data_out);

#endif
