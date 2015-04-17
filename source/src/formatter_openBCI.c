#include <fpga-log/sink/formatter/formatter_openBCI.h>
#include <fpga-log/device/device_ads1299.h>

static void formatter_open_bci_format(void* const formatter,
		const data_package_t* const package);

void formatter_open_bci_init(formatter_open_bci_t* const formatter) {
	/*
	 * set method pointer(s) of super-"class" to sub-class function(s)
	 */
	formatter->super.format = formatter_open_bci_format;

	formatter->count = 0;
}

//little-endian byte order used in openBCI protocol
static void formatter_open_bci_format(void* const formatter,
		const data_package_t* const package) {
	formatter_t* f = (formatter_t*) formatter;
	formatter_open_bci_t* fobci = (formatter_open_bci_t*) formatter;

	if (package->type == DATA_TYPE_OPEN_BCI) {
		void (*write_byte)(void* const param, const unsigned char byte) =
				f->write_byte;
		void* const param = f->param;

		write_byte(param, OPEN_BCI_PCKT_START);

		write_byte(param, (1 + 8) * 4);  //package length
		long count = fobci->count++;  //message counter
		write_byte(param, count & 255);
		write_byte(param, (count >> 8) & 255);
		write_byte(param, (count >> 16) & 255);
		write_byte(param, count >> 24);

		//send samples
		const char* samples = package->data;
		samples += 4; //skip status register value
		int i;
		for (i = 0; i < 24; i += 3) {
			write_byte(param, samples[i+2]);
			write_byte(param, samples[i+1]);
			write_byte(param, samples[i]);
			write_byte(param, (samples[i] & 0x80) ? 0xFF : 0x00); //sign extend for the last byte
		}

		write_byte(param, OPEN_BCI_PCKT_END);
	}
}
