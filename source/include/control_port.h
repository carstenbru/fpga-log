/**
 * @file control_port.h
 * @brief control_port structure to realize control interface to device drivers
 * 
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#ifndef CONTROL_PORT_H_
#define CONTROL_PORT_H_

typedef int parameter_type_t;

typedef enum {
	PARAMETER_A = 'A',
	PARAMETER_B = 'B',
	PARAMETER_C = 'C',
	PARAMETER_D = 'D',
	PARAMETER_E = 'E',
	PARAMETER_F = 'F',
	PARAMETER_G = 'G',
	PARAMETER_H = 'H',
	PARAMETER_I = 'I',
	PARAMETER_J = 'J',
	PARAMETER_K = 'K',
	PARAMETER_L = 'L',
	PARAMETER_M = 'M',
	PARAMETER_N = 'N',
	PARAMETER_O = 'O',
	PARAMETER_P = 'P',
	PARAMETER_Q = 'Q',
	PARAMETER_R = 'R',
	PARAMETER_S = 'S',
	PARAMETER_T = 'T',
	PARAMETER_U = 'U',
	PARAMETER_V = 'V',
	PARAMETER_W = 'W',
	PARAMETER_X = 'X',
	PARAMETER_Y = 'Y',
	PARAMETER_Z = 'Z',
	PARAMETER_a = 'a',
	PARAMETER_b = 'b',
	PARAMETER_c = 'c',
	PARAMETER_d = 'd',
	PARAMETER_e = 'e',
	PARAMETER_f = 'f',
	PARAMETER_g = 'g',
	PARAMETER_h = 'h',
	PARAMETER_i = 'i',
	PARAMETER_j = 'j',
	PARAMETER_k = 'k',
	PARAMETER_l = 'l',
	PARAMETER_m = 'm',
	PARAMETER_n = 'n',
	PARAMETER_o = 'o',
	PARAMETER_p = 'p',
	PARAMETER_q = 'q',
	PARAMETER_r = 'r',
	PARAMETER_s = 's',
	PARAMETER_t = 't',
	PARAMETER_u = 'u',
	PARAMETER_v = 'v',
	PARAMETER_w = 'w',
	PARAMETER_x = 'x',
	PARAMETER_y = 'y',
	PARAMETER_z = 'z',
} parameters_cpt;

/**
 * @brief struct holding incoming data
 */
typedef struct {
	parameter_type_t type; /**< the type of the control parameter */
	int value; /**< parameter value */
} control_parameter_t;

/**
 * @brief control parameter init function
 *
 * @param control_parameter	pointer to the control parameter
 * @param type							type of the parameter
 * @param value							value of the parameter
 */
void control_parameter_init(control_parameter_t* const control_parameter,
		parameter_type_t type, int value);

/**
 * @brief struct defining a control destination
 */
typedef struct {
	void* parent; /**< pointer to the parent struct, e.g. a device module */

	/**
	 * @brief pointer to the control message function of the destination
	 *
	 * @param count				amount of passed parameter structs
	 * @param parameters	pointer to paramter structures, see @ref control_parameter_t
	 */
	void (*new_control_message)(void* const parent, unsigned int count,
			const control_parameter_t* parameters);
} control_port_t;

/**
 * @brief a control_port dummy
 *
 * can be set when no real control_port is assigned
 * or before real values are assigned so that no function pointer is null
 */
extern const control_port_t control_port_dummy;

#endif
