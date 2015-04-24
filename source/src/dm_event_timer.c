/**
 * @file dm_event_timer.c
 * @brief event timer module
 *
 * This module can be used to generate control commands with various timings.
 *
 * @author Carsten Bruns (bruns@lichttechnik.tu-darmstadt.de)
 */

#include <fpga-log/dm/dm_event_timer.h>

#include <fpga-log/peripheral_funcs/timer_funcs.h>
#include <fpga-log/peripheral_funcs/compare_funcs.h>

/**
 * @brief event timer update function
 *
 * @param	_event_timer	pointer to the timer
 */
static void dm_event_timer_update(void* const _event_timer);

static void dm_event_timer_clear_event_list(dm_event_timer_t* const event_timer);

void dm_event_timer_init(dm_event_timer_t* const event_timer,
		dm_event_timer_mode mode, timer_regs_t* const timer_regs,
		compare_regs_t* const compare_regs) {
	datastream_object_init(&event_timer->super);  //call parents init function
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	event_timer->super.update = dm_event_timer_update;

	event_timer->control_out = &control_port_dummy;

	event_timer->timer = timer_regs;
	event_timer->compare = compare_regs;

	event_timer->mode = mode;

	event_timer->peridic_next = 0;
	event_timer->start_delay = 0;
	event_timer->repetitions = -1;

	dm_event_timer_clear_event_list(event_timer);

	timer_enable(timer_regs);  //TODO enable timer only when needed?
	//timer_set_interval_ms(timer_regs, 1000);

	compare_init_mode_set(compare_regs, 0);
}

void dm_event_timer_set_control_out(dm_event_timer_t* const event_timer,
		const control_port_t* const control_in) {
	event_timer->control_out = control_in;
}

static int dm_event_timer_find_free_list_element(
		dm_event_timer_t* const event_timer) {
	int i;
	for (i = 0; i < DM_EVENT_TIMER_MAX_EVENTS; i++) {
		if (event_timer->event_list[i].control_action == 0)
			return i;
	}
	return -1;
}

void dm_event_timer_add_event(dm_event_timer_t* const event_timer,
		uint36_t delay, control_action_t* const control_action) {
	int free = dm_event_timer_find_free_list_element(event_timer);  //search a free place in the list
	if (free == -1)
		return;  //error: list is full!!!

	int* pos_last = &event_timer->first_event;
	if (*pos_last != -1) {  //empty list test
		while (delay > event_timer->event_list[*pos_last].delay) {  //iterate over list, find place for new element (by delay or list end)
			delay -= event_timer->event_list[*pos_last].delay;
			pos_last = &event_timer->event_list[*pos_last].next;
			if (*pos_last == -1)
				break;
		}
	}

	if (pos_last == &event_timer->first_event) {  //insertion as first element: timer is affected!
		timer_set_interval_ms(event_timer->timer, delay + event_timer->start_delay);
	}

	int pos = *pos_last;  //insert element
	event_timer->event_list[free].next = pos;
	*pos_last = free;
	event_timer->event_list[free].control_action = control_action;
	event_timer->event_list[free].delay = delay;

	if (pos != -1) {  //decrement delay of all following elements
		event_timer->event_list[pos].delay -= delay;
	}
}

static void dm_event_timer_clear_event_list(dm_event_timer_t* const event_timer) {
	event_timer->first_event = -1;
	int i;
	for (i = 0; i < DM_EVENT_TIMER_MAX_EVENTS; i++) {
		event_timer->event_list[i].next = -1;
		event_timer->event_list[i].control_action = 0;
	}
}

static void dm_event_timer_update(void* const _event_timer) {
	dm_event_timer_t* event_timer = (dm_event_timer_t*) _event_timer;

	if (compare_check_and_reset_flag(event_timer->compare)) {
		int pos;
		if (event_timer->mode != EVENT_TIMER_PERIODIC) {
			pos = event_timer->first_event;
		} else {
			pos = event_timer->peridic_next;
		}

		if (pos != -1) {
			//execute control action
			event_timer->event_list[pos].control_action->execute(
					event_timer->event_list[pos].control_action,
					event_timer->control_out);

			int pos_next = event_timer->event_list[pos].next;
			if (event_timer->mode != EVENT_TIMER_PERIODIC) {
				event_timer->first_event = pos_next;  //remove event from list
				event_timer->event_list[pos].control_action = 0;
			} else {
				if (pos_next == -1) {
					if (event_timer->repetitions > 0) {
						event_timer->repetitions--;
					}
					if (event_timer->repetitions != 0) {
						pos_next = event_timer->first_event;
					}
				}
				event_timer->peridic_next = pos_next;
			}

			if (pos_next != -1)
				timer_set_interval_ms(event_timer->timer,
						event_timer->event_list[pos_next].delay);
		}
	}
}

void dm_event_timer_set_start_delay(dm_event_timer_t* const event_timer,
		int36_t start_delay) {
	event_timer->start_delay = start_delay;
}

void dm_event_timer_set_repetitions(dm_event_timer_t* const event_timer,
		int repetitions) {
	event_timer->repetitions = repetitions;
}
