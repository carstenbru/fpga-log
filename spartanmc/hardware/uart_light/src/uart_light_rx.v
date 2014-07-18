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
// Create Date:    13:45:33 09/20/2011 
// Module Name:    uart_light_rx 
//////////////////////////////////////////////////////////////////////////////////
module uart_light_rx
#(
	parameter SAMPLING_COUNT = 15,
	parameter SC_SIZE = 4,
	
	parameter  WORD_SIZE = 8,
	parameter  FIFO_ADDR_BITS = 5
)(
	input 	wire reset,
	input   wire clk_peri,
	input 	wire clk_rx,	
	input   wire read_ready,
	input	wire rx,
	output	wire [WORD_SIZE-1:0] word_rx,
	output	wire fifo_rx_empty,
	output	wire fifo_rx_full
  );
  
  wire fifo_full,fifo_empty,bit_eq_0,sc_full,frame_done,sc_halb,sc_inc,sc_clr,bc_inc,bc_clr,shift,fifo_write_enable,fifo_read_enable;
  
  uart_light_rx_ctrl
  rx_ctrl0
  (
	.reset(reset),
	.clk_rx(clk_rx),
	.read_ready(read_ready),
	.fifo_rx_full(fifo_rx_full),
	.fifo_rx_empty(fifo_rx_empty),
	.fifo_full(fifo_full),
	.fifo_empty(fifo_empty),
	
	.bit_eq_0(bit_eq_0),
	.sc_halb(sc_halb),
	.sc_full(sc_full),
	.frame_done(frame_done),
	.sc_inc(sc_inc),
	.sc_clr(sc_clr),
	.bc_inc(bc_inc),
	.bc_clr(bc_clr),
	.shift(shift),
	.fifo_write_enable(fifo_write_enable),
	.fifo_read_enable(fifo_read_enable)
  );
  
  uart_light_rx_dp
  #(
   .SAMPLING_COUNT(SAMPLING_COUNT),
   .SC_SIZE(SC_SIZE),
   .FIFO_ADDR_BITS(FIFO_ADDR_BITS)
  )
  rx_dp0
  (
	.reset(reset),
	.clk_peri(clk_peri),
	.clk_rx(clk_rx),
	.rx(rx),
	.word_rx(word_rx),
	
	.sc_inc(sc_inc),
	.sc_clr(sc_clr),
	.bc_inc(bc_inc),
	.bc_clr(bc_clr),
	.shift(shift),
	.fifo_write_enable(fifo_write_enable),
	.bit_eq_0(bit_eq_0),
	.sc_halb(sc_halb),
	.sc_full(sc_full),
	.frame_done(frame_done),
	.fifo_read_enable(fifo_read_enable),
	.fifo_full(fifo_full),
	.fifo_empty(fifo_empty)
  );

endmodule
