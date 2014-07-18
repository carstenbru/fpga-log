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
// Create Date:    13:33:17 09/20/2011 
// Module Name:    uart_light 
//////////////////////////////////////////////////////////////////////////////////
module uart_light
#(
	parameter  FIFO_ADDR_BITS_RX = 5,
	parameter  FIFO_ADDR_BITS_TX = 5,
	
	parameter SAMPLING_COUNT = 15,
	parameter SC_SIZE = 4,
	
	parameter BR_DIVISOR_TX = 0,
	parameter BR_DIVISOR_RX = 0,
	parameter BRD_SIZE_TX = 6,
	parameter BRD_SIZE_RX = 3,
	parameter WORD_SIZE = 8
)(
	input  wire reset,
	input  wire clk_peri,
	
	//tx
	input  wire word_ready,
	input  wire [WORD_SIZE-1:0] word_tx,
	output wire fifo_tx_full,
	output wire fifo_tx_empty,
	output wire fifo_tx_read,
	output wire tx,
	
	//rx
	input  wire read_ready,
	input  wire rx,
	output wire [WORD_SIZE-1:0] word_rx,
	output wire fifo_rx_full,
	output wire fifo_rx_empty,
	
	output wire clk_rx_out
   );

	
	wire clk_rx;
	wire clk_tx;
	
	assign clk_rx_out = clk_rx;
	
	uart_light_clk_gen
	#(
	.BR_DIVISOR_TX(BR_DIVISOR_TX),
	.BR_DIVISOR_RX(BR_DIVISOR_RX),
	.BRD_SIZE_TX(BRD_SIZE_TX),
	.BRD_SIZE_RX(BRD_SIZE_RX)
	)
	clock_generator0(
	.clk_rx(clk_rx),
	.clk_tx(clk_tx),
	.clk_peri(clk_peri),
	.reset(reset)
	);
	
	uart_light_rx
	#(
	.SAMPLING_COUNT(SAMPLING_COUNT),
	.SC_SIZE(SC_SIZE),
	.WORD_SIZE(WORD_SIZE),
	.FIFO_ADDR_BITS(FIFO_ADDR_BITS_RX)
	)
	uart_rx0(
	.reset(reset),
	.clk_peri(clk_peri),
	.clk_rx(clk_rx),
	.read_ready(read_ready),
	.rx(rx),
	.word_rx(word_rx),
	.fifo_rx_full(fifo_rx_full),
	.fifo_rx_empty(fifo_rx_empty)
	);
	
	uart_light_tx
	#(
	.WORD_SIZE(WORD_SIZE),
	.FIFO_ADDR_BITS(FIFO_ADDR_BITS_TX)
	)
	uart_tx0(
	.reset(reset),
	.clk_peri(clk_peri),	
	.clk_tx(clk_tx),
	.word_ready(word_ready),
	.word_tx(word_tx),
	.tx(tx),
	.fifo_tx_full(fifo_tx_full),
	.fifo_tx_empty(fifo_tx_empty),
	.fifo_tx_read(fifo_tx_read)
	);

endmodule
