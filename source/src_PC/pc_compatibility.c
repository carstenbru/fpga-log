/**
 * @file pc_compatibility.c
 * @brief code needed to run and simulate the code natively on a PC
 *
 * @author Carsten Bruns (carst.bruns@gmx.de)
 */

#include <peripherals/compare.h>

#include <fpga-log/pc_compatibility.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

file_pipe_t pipes[PIPE_COUNT];
unsigned char fifo_first[PIPE_COUNT];
unsigned int fifo_first_valid[PIPE_COUNT];
int fifo_irq_enabled[PIPE_COUNT];

io_descr_t stdio_descr = { };

timestamp_gen_regs_t* timestamp_gen;
unsigned int timestamp_gen_capture_signals[] = TIMESTAMP_CAPTURE_SIGNALS;
struct timeval timestamp_gen_start;

unsigned int software_timestamp_count;
unsigned int software_timestamp_ids[MAX_SOFTWARE_TIMESTAMPS];

/**
 * @brief initializes the pipe peripheral simulation
 */
void pipes_init(void) {
	char buffer[1024];

	int i;
	for (i = 0; i < PIPE_COUNT; i++) {
		sprintf(buffer, "fifo_in_%d", i);  //SpartanMC stdio.h overwrites normal c-header, but sprintf is there
		mkfifo(buffer, S_IRWXU | S_IRWXG);
		pipes[i].in = open(buffer, O_RDWR);
		int flags = fcntl(pipes[i].in, F_GETFL, 0);
		fcntl(pipes[i].in, F_SETFL, flags | O_NONBLOCK);

		sprintf(buffer, "fifo_out_%d", i);
		mkfifo(buffer, S_IRWXU | S_IRWXG);
		pipes[i].out = open(buffer, O_RDWR);
	}
}

void fifo_enable_irq(unsigned int fifo) {
	fifo_irq_enabled[fifo] = 1;
}

int fifo_read(unsigned int fifo, unsigned char* data) {
	if (fifo_first_valid[fifo]) {
		*data = fifo_first[fifo];
		fifo_first_valid[fifo] = 0;
		return 1;
	} else {
		return read(pipes[fifo].in, data, 1);
	}
}

int fifo_write(unsigned int fifo, unsigned char data) {
	return write(pipes[fifo].out, &data, 1);
}

static void timestamp_gen_set_timestamp(timestamp_gen_regs_t* timestamp_gen,
		unsigned int id) {
	time_t t = time(0);
	timestamp_gen->timestamp.lpt_low = t;
	timestamp_gen->timestamp.lpt_high = t >> (sizeof(unsigned int) * 8);

	struct timeval now;
	gettimeofday(&now, NULL);
	timestamp_gen->timestamp.hpt_low = now.tv_usec;
	timestamp_gen->timestamp.hpt_high = now.tv_usec >> (sizeof(unsigned int) * 8);

	timestamp_gen->tsr = id;
}

unsigned int timestamp_gen_not_empty(timestamp_gen_regs_t* timestamp_gen) {
	int i;
	unsigned char b;
	if (software_timestamp_count) {
		timestamp_gen_set_timestamp(timestamp_gen,
				software_timestamp_ids[--software_timestamp_count]);
		return 1;
	}
	for (i = 0; i < TIMESTAMP_CAPTURE_SIGNALS_COUNT; i++) {
		if (fifo_irq_enabled[i]) {
			if (!fifo_first_valid[i] && read(pipes[i].in, &b, 1) == 1) {
				fifo_first_valid[i] = 1;
				fifo_first[i] = b;

				timestamp_gen_set_timestamp(timestamp_gen, i+1);

				return 1;
			}
		}
	}
	return 0;
}

void timestamp_gen_generate_software_timestamp(
		timestamp_gen_regs_t* timestamp_gen, unsigned int id) {
	software_timestamp_ids[software_timestamp_count++] = id;
}

void sys_init(void) {
	pipes_init();

	timestamp_gen = malloc(sizeof(timestamp_gen_regs_t));
	software_timestamp_count = 0;
}

unsigned long int get_peri_clock(void) {
	return 100000000;
}

timestamp_gen_regs_t* get_timestamp_gen(void) {
	return timestamp_gen;
}
