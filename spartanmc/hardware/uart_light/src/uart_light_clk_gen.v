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
// Create Date:    09:17:01 09/23/2011 
// Module Name:    uart_light_clk_gen 
//////////////////////////////////////////////////////////////////////////////////
module uart_light_clk_gen
#( 	//Die Default-Werte der folgenden Parameter werden für Baudrate 460800 genutzt
	parameter BR_DIVISOR_TX = 54,
	parameter BR_DIVISOR_RX = 5,
	parameter BRD_SIZE_TX = 6,
	parameter BRD_SIZE_RX = 3
)(
	output wire clk_rx,
	output wire clk_tx,
	input  wire clk_peri,
	input  wire reset
   );
	
	reg [BRD_SIZE_TX-1:0] divisor_tx;
	reg [BRD_SIZE_RX-1:0] divisor_rx;
	
	assign clk_tx = divisor_tx[BRD_SIZE_TX-1];
	assign clk_rx = divisor_rx[BRD_SIZE_RX-1];
	
	always @(posedge clk_peri, posedge reset) begin
		if(reset) begin 
			divisor_tx <= {(BRD_SIZE_TX){1'b0}};
			divisor_rx <= {(BRD_SIZE_RX){1'b0}};
		end
		else begin  
			if(divisor_tx == (BR_DIVISOR_TX - 1'b1))
				divisor_tx <= {(BRD_SIZE_TX){1'b0}};
			else
				divisor_tx <= divisor_tx + 1'b1;
			
			if(divisor_rx == (BR_DIVISOR_RX - 1'b1))
				divisor_rx <= {(BRD_SIZE_RX){1'b0}};
			else
				divisor_rx <= divisor_rx + 1'b1;
		end
	end

endmodule
