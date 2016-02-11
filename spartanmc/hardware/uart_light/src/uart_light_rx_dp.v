/*
_______________________________________________________________________________

 Copyright (c) 2012 TU Dresden, Chair for Embedded Systems
 (http://www.mr.inf.tu-dresden.de) All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 3. All advertising materials mentioning features or use of this software
    must display the following acknowledgement: "This product includes
    software developed by the TU Dresden Chair for Embedded Systems and
    its contributors."

 4. Neither the name of the TU Dresden Chair for Embedded Systems nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY TU DRESDEN CHAIR FOR EMBEDDED SYSTEMS AND
 CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
_______________________________________________________________________________
*/

//////////////////////////////////////////////////////////////////////////////////
// Create Date:    09:15:09 09/22/2011 
// Module Name:    uart_light_rx_dp 
//////////////////////////////////////////////////////////////////////////////////
module uart_light_rx_dp
#(
	parameter WORD_SIZE = 8,
	parameter BC_SIZE = 3, //2^BC_SIZE >= WORD_SIZE	
	parameter SAMPLING_COUNT = 15, 
	parameter SC_SIZE = 4,
	parameter FIFO_ADDR_BITS = 5
)(
	input wire reset,
	input wire clk_peri,
	input wire clk_rx,
	input wire rx,
	
	//Interfaces zum SPMC
	output wire [WORD_SIZE-1:0] word_rx,
	
	//Interfaces zum Controller-Module, welche durch clk_rx getrieben werden
	input wire sc_inc,
	input wire sc_clr,
	input wire bc_inc,
	input wire bc_clr,
	input wire shift,
	input wire fifo_write_enable,
	output wire bit_eq_0,
	output wire sc_halb,
	output wire sc_full,
	output wire frame_done,
	
	//Interfaces zum Controller-Module, welche durch clk_peri getrieben werden
	input wire fifo_read_enable,
	output wire fifo_full,
	output wire fifo_empty
   );

	reg rx0, rx1;		// zur Synchronisierung von rx mit clk_rx
	reg [WORD_SIZE-1:0] shfreg_rx;
	reg [BC_SIZE:0]     bit_counter;
	reg [SC_SIZE-1:0]   sampling_counter;
	
	assign bit_eq_0   = (rx1 == 1'b0);
	assign sc_halb    = (sampling_counter == ((SAMPLING_COUNT-1)/2) - 1'b1);
	assign sc_full    = (sampling_counter == SAMPLING_COUNT - 1'b1);
	assign frame_done = (bit_counter == WORD_SIZE);
	
	always @(posedge clk_rx, posedge reset) begin
		if(reset==1'b1) begin
			bit_counter      <= {(BC_SIZE+1){1'b0}};
			sampling_counter <= {(SC_SIZE){1'b0}};
			shfreg_rx        <= {(WORD_SIZE){1'b0}};
			rx0		 <= 1'b1;
			rx1		 <= 1'b1;
		end
		else begin
			rx0		 <= rx;
			rx1		 <= rx0;
			if(sc_clr)
				sampling_counter <= {(SC_SIZE){1'b0}};
			else if(sc_inc) 
				sampling_counter <= sampling_counter + 1'b1;
			
			if(bc_clr)
				bit_counter <= {(BC_SIZE+1){1'b0}};
			else if(bc_inc)
				bit_counter <= bit_counter + 1'b1;
			
			if(shift)
				shfreg_rx <= {rx1,shfreg_rx[WORD_SIZE-1:1]};			
		end
	end


	yaf #
	(
		.DSIZE (WORD_SIZE),
		.ASIZE (FIFO_ADDR_BITS)
	)
	yaf_rx0
	(
		.rdata(		word_rx			),	//Daten zum MC
		.wfull(					),	//FIFO voll mit wclk synchronisiert
		.rfull(					),	//FIFO voll mit rclk synchronisiert
		.rwfull(	fifo_full		),	//FIFO voll = rfull | wfull
		.wempty(				),	//FIFO leer mit wclk synchronisiert
		.rempty(				),	//FIFO leer mit rclk synchronisiert
		.rwempty(	fifo_empty		),	//FIFO leer = rempty & wempty
		.fill(					),	//Output wird nicht verwendet.
		.wdata(		shfreg_rx[WORD_SIZE-1:0]),	//Daten vom Transceiver lesen
		.winc(		fifo_write_enable	),	//Transceiver schreibt Daten
		.wclk(		clk_rx			),
		.wrst_n(	~reset			),
		.rinc(		fifo_read_enable	),	//MC liest Daten solange read=1 ist.
		.rclk(		clk_peri		),  
		.rrst_n	(	~reset			),
		.rptr_upd(	1'b0			),
		.rptr_rest(	1'b0			),
		.wptr_upd(	1'b0			),
		.wptr_rest(	1'b0			)
	);

endmodule
