/**
 * @file device_max31855.c
 * @brief MAX31855 thermocouple-to-digital device driver
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/device/device_max31855.h>
#include <fpga-log/sys_init.h>
#include <spi.h>

const char* max31855_fault_names[3] = MAX31855_FAULT_NAMES;

/**
 * @brief max31855 send data function
 *
 * This function sends the data packages to the devices output port.
 *
 * @param	_max31855		pointer to the max31855 device
 * @param id					the source id of the pending data
 * @param timestamp		pointer to the timestamp of the pending data
 */
static void device_max31855_send_data(void* const _max31855,
		const unsigned int id, const timestamp_t* const timestamp);

/**
 * @brief control message function of max31855 device
 *
 * @param	parent			the max31855 device
 * @param count				amount of passed parameter structs
 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
 */
static void device_max31855_control_message(void* const _max31855,
		unsigned int count, const control_parameter_t* parameters);

/**
 * @brief update function of the max31855 device
 *
 * @param _max31855	pointer to the max31855 device
 */
static void device_max31855_update(void* const _max31855);

void device_max31855_init(device_max31855_t* const max31855,
		spi_master_regs_t* const spi_master, const int id) {
	datastream_source_init(&max31855->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	max31855->super.super.update = device_max31855_update;
	max31855->super.send_data = device_max31855_send_data;

	max31855->spi_master = spi_master;

	max31855->control_in = control_port_dummy;
	max31855->control_in.parent = (void*) max31855;
	max31855->control_in.new_control_message = device_max31855_control_message;

	max31855->data_out = &data_port_dummy;
	max31855->error_out = &data_port_dummy;
	max31855->internal_temp_out = &data_port_dummy;

	int prescaler = 0;
	long int clk = get_peri_clock() >> 2;  //spi peripheral divides by 4
	while (clk > MAX31855_MAX_SPI_CLK) {
		clk >>= 1;
		prescaler++;
	}
	spi_set_div(spi_master, prescaler);
	//CPOL = 0, CPAH = 0 is default
	spi_enable(spi_master);
}

control_port_t* device_max31855_get_control_in(
		device_max31855_t* const max31855) {
	return &max31855->control_in;
}

void device_max31855_set_data_out(device_max31855_t* const max31855,
		const data_port_t* const data_in) {
	max31855->data_out = data_in;
}

void device_max31855_set_internal_temp_out(device_max31855_t* const max31855,
		const data_port_t* const data_in) {
	max31855->internal_temp_out = data_in;
}

void device_max31855_set_error_out(device_max31855_t* const max31855,
		const data_port_t* const data_in) {
	max31855->error_out = data_in;
}

static void device_max31855_control_message(void* const _max31855,
		unsigned int count, const control_parameter_t* parameters) {
	while (count--) {
		if (parameters->type == MAX31855_PARAMETER_READ) {
			device_max31855_t* max31855 = (device_max31855_t*) _max31855;
			if (max31855->spi_master->spi_control & SPI_MASTER_CTRL_TRANS_EMPTY) {
				datastream_source_generate_software_timestamp(
						(datastream_source_t*) _max31855);

				spi_activate(max31855->spi_master, 1);
				max31855->spi_master->spi_data_out = 0;  //start reading by write to data_out register
			}
		}
		parameters++;
	}
}

static void device_max31855_update(void* const _max31855) {
}

static void device_max31855_send_data(void* const _max31855,
		const unsigned int id, const timestamp_t* const timestamp) {
	device_max31855_t* max31855 = (device_max31855_t*) _max31855;

	while (!(max31855->spi_master->spi_control & SPI_MASTER_CTRL_TRANS_EMPTY))
		;
	int data = max31855->spi_master->spi_data_in;
	//start reading by write to data_out register
	//do this right now so peripheral will be very likely already finished when the data is needed
	//e.g. for 16MHz CPU clock and 4MHz SPI clock reading will need 64 clock cycles
	max31855->spi_master->spi_data_out = 0;

	int fault = data & 1;
	int val = data >> 2;
	data = val & 8191;
	if (data != val)
		val = -data;
	data_package_t package = { id, MAX31855_THERMOCOUPLE_TEMP_NAME, DATA_TYPE_INT,
			&val, timestamp };
	max31855->data_out->new_data(max31855->data_out->parent, &package);

	while (!(max31855->spi_master->spi_control & SPI_MASTER_CTRL_TRANS_EMPTY))
		;
	if (max31855->internal_temp_out != &data_port_dummy) {
		data = max31855->spi_master->spi_data_in;
		val = data >> 4;
		data = val & 2047;
		if (data != val)
			val = -data;

		data_package_t int_temp_package = { id, MAX31855_INTERNAL_TEMP_NAME,
				DATA_TYPE_INT, &val, timestamp };
		max31855->internal_temp_out->new_data(max31855->internal_temp_out->parent,
				&int_temp_package);
	}

	if (fault) {
		fault = max31855->spi_master->spi_data_in & 7;
		if (fault == 2)
			fault = 1;
		if (fault == 4)
			fault = 2;
		data_package_t fault_package = { id, max31855_fault_names[fault],
				DATA_TYPE_INT, &fault, timestamp };
		max31855->error_out->new_data(max31855->error_out->parent, &fault_package);
	}

	spi_deactivate(max31855->spi_master);
}