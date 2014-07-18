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
// Create Date:    09:16:10 09/22/2011 
// Module Name:    uart_light_rx_ctrl 
//////////////////////////////////////////////////////////////////////////////////
module uart_light_rx_ctrl
#(
	 parameter STATE_COUNT = 3,
	 parameter IDLE = 3'b001,
	 parameter STARTING = 3'b010,
	 parameter RECEIVING = 3'b100
)(
	input  wire reset,
	input  wire clk_rx,
	
	//Interfaces zum SPMC
	input  wire read_ready,
	output wire fifo_rx_full,
	output wire fifo_rx_empty,
	
	//Interfaces zum Datenpfad-Module, welche durch clk_rx getrieben werden
	input  wire bit_eq_0,
	input  wire sc_halb,
	input  wire sc_full,
	input  wire frame_done,
	output reg  sc_inc,
	output reg  sc_clr,
	output reg  bc_inc,
	output reg  bc_clr,
	output reg  shift,
	output reg  fifo_write_enable,
	
	//Interfaces zum Datenpfad-Module, welche durch clk_peri getrieben werden
	input  wire fifo_full,
	input  wire fifo_empty,
	output wire fifo_read_enable
    );
	 	 
	 reg [STATE_COUNT-1:0] state_cur, state_next;
	 
	 assign fifo_rx_full     = fifo_full;
	 assign fifo_rx_empty    = fifo_empty;
	 assign fifo_read_enable = read_ready & (~fifo_empty);
	 
	 always @(state_cur, bit_eq_0, sc_halb, sc_full, frame_done) begin
		sc_inc = 1'b0;
		sc_clr = 1'b0;
		bc_inc = 1'b0;
		bc_clr = 1'b0;
		shift  = 1'b0;
		fifo_write_enable = 1'b0;
		state_next = IDLE;
		
		case(state_cur)
			IDLE:
				if(bit_eq_0 == 1'b1) begin
					state_next = STARTING;
				end
			STARTING:
				if(bit_eq_0 == 1'b0) begin
					sc_clr = 1'b1;
					state_next = IDLE;
				end
				else if(sc_halb == 1'b1) begin
					sc_clr = 1'b1;
					state_next = RECEIVING;
				end
				else begin
					sc_inc = 1'b1;
					state_next = STARTING;
				end
			RECEIVING:
				if(sc_full == 1'b1) begin
					sc_clr = 1'b1;
					if(frame_done) begin
						bc_clr = 1'b1;
						fifo_write_enable = 1'b1;
						state_next = IDLE;
					end
					else begin
						shift  = 1'b1;
						bc_inc = 1'b1;
						state_next = RECEIVING;
					end
				end
				else begin
					sc_inc = 1'b1;
					state_next = RECEIVING;
				end
			default: state_next = IDLE;
		endcase
	 end
	 
	 always @(posedge clk_rx, posedge reset) begin
		if(reset) begin
			state_cur <= IDLE;
		end
		else begin 
			state_cur <= state_next;
		end
	 end

endmodule
