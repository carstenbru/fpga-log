#include "fpga-log/device/device_contrast_box.h"

static void device_contrast_box_send_data(
		void* const _contrast_box, const unsigned int id,
		const timestamp_t* const timestamp);


//unsigned int pwm[8] = {0,0,0,0,0,0,0,0};	//Save old PWM_ON_VALUES
unsigned char number_of_boxes = 0;	//Save the number of active boxes

void device_contrast_box_init(
		device_contrast_box_t* const contrast_box,
		contrast_box_regs_t* contrast_box_peri, const unsigned int id) {
	datastream_source_init(&contrast_box->super, id);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	contrast_box->super.send_data = device_contrast_box_send_data;

	contrast_box->contrast_box = contrast_box_peri;

	contrast_box->data_out = &data_port_dummy;

	//Enable Contrast Box
	contrast_box->contrast_box->control |= CONTRAST_BOX_ENABLE;
	number_of_boxes = contrast_box->contrast_box->control>>15;	//The Number of Boxes specified by the user
}

void device_contrast_box_set_data_out(
		device_contrast_box_t* const contrast_box,
		const data_port_t* const data_port_out) {
	contrast_box->data_out = data_port_out;
}

static void device_contrast_box_send_data(
		void* const _contrast_box, const unsigned int id,
		const timestamp_t* const timestamp) {

	device_contrast_box_t* contrast_box = (device_contrast_box_t*) _contrast_box;

	data_package_t package = { contrast_box->super.id, "PWM_ON_VALUE_0",
			DATA_TYPE_INT, (void*) &contrast_box->contrast_box->pwm_on_value[0] , timestamp };

	/*if(pwm[0] != contrast_box->contrast_box->pwm_on_value[0]){
		contrast_box->data_out->new_data(contrast_box->data_out->parent, &package);
		pwm[0] = contrast_box->contrast_box->pwm_on_value[0];
	}*/
	char name[15] = {"PWM_ON_VALUE_X\0"};
	for(unsigned char i = 0 ; i < number_of_boxes; i = i + 1){
		//if(pwm[i] != contrast_box->contrast_box->pwm_on_value[i]){
			package.data =  (void*) &(contrast_box->contrast_box->pwm_on_value[i]);
			name[13] = i+'0';
			package.val_name = name;
			contrast_box->data_out->new_data(contrast_box->data_out->parent, &package);

		//	pwm[i] = contrast_box->contrast_box->pwm_on_value[i];
		//}
	}

}

