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
// Create Date:    17:06:33 09/20/2011 
// Module Name:    uart_light_tx_ctrl 
//////////////////////////////////////////////////////////////////////////////////
module uart_light_tx_ctrl
#(
	parameter STATE_COUNT = 2,
	parameter IDLE = 2'b00,
	parameter LOAD = 2'b01,
	parameter TRANSMITTING = 2'b10
)(
	input  wire reset,
	input  wire clk_tx,

	//Interfaces zum SPMC
	input  wire word_ready,
	output wire fifo_tx_full,
	output wire fifo_tx_empty,
	
	//Interfaces zum Datenpfad-Module, welche durch clk_tx getrieben werden
	input  wire frame_done,
	output reg  transmit,
	output reg  clear,
	output reg  fifo_read_enable,
	
	//Interfaces zum Datenpfad-Module, welche durch clk_peri getrieben werden
	input wire  fifo_full,
	input wire  fifo_empty,
	output wire fifo_write_enable

    );

	reg [STATE_COUNT-1:0] state_cur, state_next;
	
	assign fifo_tx_full      = fifo_full;
	assign fifo_tx_empty     = fifo_empty;
 	assign fifo_write_enable = word_ready & (~fifo_full);
	
	always @(*) begin
		transmit         = 1'b0;
		clear            = 1'b0;
		fifo_read_enable = 1'b0;
		state_next       = IDLE;
		
		case(state_cur)
			IDLE:
				if (fifo_empty == 1'b0) begin
					state_next       = LOAD;
				end
			LOAD:
				begin
					fifo_read_enable = 1'b1;
					state_next	 = TRANSMITTING;
				end
			TRANSMITTING:
				if(frame_done) begin
					clear      = 1'b1;
					state_next = IDLE;
				end
				else begin
					transmit   = 1'b1;
					state_next = TRANSMITTING;
				end
			default: state_next = IDLE;
		endcase
	end
	
	always @(posedge clk_tx, posedge reset) begin
		if(reset) begin
			state_cur <= IDLE;
		end
		else begin 
			state_cur <= state_next;
		end
	end
	
endmodule
