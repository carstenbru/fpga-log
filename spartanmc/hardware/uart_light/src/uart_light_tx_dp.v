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
// Create Date:    17:07:11 09/20/2011 
// Module Name:    uart_light_tx_dp 
//////////////////////////////////////////////////////////////////////////////////
module uart_light_tx_dp
#(
	parameter WORD_SIZE = 8,
	parameter BC_SIZE = 3, //2^BC_SIZE >= WORD_SIZE
	parameter FIFO_ADDR_BITS = 5
)(
	input wire reset,
	input wire clk_peri,
	input wire clk_tx,
	output wire tx,
	
	//Interfaces zum SPMC
	input wire [WORD_SIZE-1:0] word_tx,
	
	//Interfaces zum Controller-Module, welche durch clk_tx getrieben werden
	input wire transmit,
	input wire clear,
	input wire fifo_read_enable,
	output wire frame_done,
	
	//Interfaces zum Controller-Module, welche durch clk_peri getrieben werden
   	input wire fifo_write_enable,
	output wire fifo_full,
	output wire fifo_empty
	);
	
	wire[WORD_SIZE-1:0] fifo_out;
	reg [WORD_SIZE:0] shfreg_tx;
	reg [BC_SIZE:0] bit_count;
	
	assign tx = shfreg_tx[0];
	assign frame_done = (bit_count == WORD_SIZE);
	
	always @(posedge clk_tx, posedge reset) begin
		if(reset) begin 
			shfreg_tx <= {(WORD_SIZE+1){1'b1}};
			bit_count <= {(BC_SIZE+1){1'b0}};
		end
		else begin
			if(fifo_read_enable) 
				shfreg_tx <= {fifo_out,1'b0};
			else if(transmit) begin
				shfreg_tx <= {1'b1, shfreg_tx[WORD_SIZE:1]};
				bit_count <= bit_count + 1'b1;
			end
			else if(clear) begin
				shfreg_tx <= {(WORD_SIZE+1){1'b1}};
				bit_count <= {(BC_SIZE+1){1'b0}};
			end
		end
	end


	yaf #
	(
		.DSIZE (WORD_SIZE),
		.ASIZE (FIFO_ADDR_BITS)
	)
	yaf_tx0
	(
		.rdata(		fifo_out		),	//Sendedaten zum Tranceiver
		.wfull(					),	//Empfangs-FIFO voll mit wclk synchronisiert
		.rfull(					),	//Empfangs-FIFO voll mit rclk synchronisiert
		.rwfull(	fifo_full		),	//Sende-FIFO voll = rfull | wfull
		.wempty(				),	//Empfangs-FIFO leer mit wclk synchronisiert
		.rempty(				),	//Empfangs-FIFO leer mit rclk synchronisiert
		.rwempty(	fifo_empty		),	//Sende-FIFO leer = rempty & wempty
		.fill(					),	//Output wird nicht verwendet.
		.wdata(		word_tx			),	//Sendedatem vom MC
		.winc(		fifo_write_enable	),	//MC schreibt Daten solange write=1 ist.
		.wclk(		clk_peri		),
		.wrst_n(	~reset			),
		.rinc(		fifo_read_enable	),	//Transceiver liest Daten
		.rclk(		clk_tx			),  
		.rrst_n	(	~reset			),
		.rptr_upd(	1'b0			),
		.rptr_rest(	1'b0			),
		.wptr_upd(	1'b0			),
		.wptr_rest(	1'b0			)
	);

endmodule
