//////////////////////////////////////////////////////////////////////
////                                                              ////
//// sm_fifoRTL.v                                                 ////
////                                                              ////
//// This file is part of the spiMaster opencores effort.
//// <http://www.opencores.org/cores//>                           ////
////                                                              ////
//// Module Description:                                          ////
////  parameterized dual clock domain fifo. 
////  fifo depth is restricted to 2^ADDR_WIDTH
////  No protection against over runs and under runs.
//// 
////                                                              ////
//// To Do:                                                       ////
//// 
////                                                              ////
//// Author(s):                                                   ////
//// - Steve Fielding, sfielding@base2designs.com                 ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2008 Steve Fielding and OPENCORES.ORG          ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE. See the GNU Lesser General Public License for more  ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from <http://www.opencores.org/lgpl.shtml>                   ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
`include "timescale.v"

module sm_fifoDMA(fifoClk, reset, dataIn, 
  dataOut, fifoWEn, fifoREn,
  forceEmptyWr, forceEmptyRd,
  do_peri, di_peri, addr_peri, mem_clk, mem_access, store_access, addr_high);
//FIFO_DEPTH = ADDR_WIDTH^2. Min = 2, Max = 66536
  parameter FIFO_DEPTH = 64; 
  parameter ADDR_WIDTH = 6;   
  
  parameter DMA_ADR = 18'h19c00;     //1024 addresses below IO ports
  
  parameter FIFO_WIDTH = 8;

// Two clock domains within this module, DMA and fifo clocks
input fifoClk;
input reset;
input [FIFO_WIDTH-1:0] dataIn;
input fifoWEn;
input forceEmptyWr;

output [FIFO_WIDTH-1:0] dataOut;
input fifoREn;
input forceEmptyRd;

//*** Connections to SpartanMC Core (do not change) ***

input           [17:0]  do_peri;        //Data Bus  from MC
output          [17:0]  di_peri;        //Data Bus  to MC
input           [9:0]   addr_peri;      //Address Bus from MC

// BlockRAM interface
input                   mem_clk;        //BRAM clk
input                   mem_access;
input                   store_access;
input            [7:0]  addr_high;


wire fifoClk;
wire reset;
wire [FIFO_WIDTH-1:0] dataIn;
reg [FIFO_WIDTH-1:0] dataOut;
wire fifoWEn;
wire fifoREn;
wire forceEmpty;
reg  [15:0]numElementsInFifo;


// local registers
reg  [ADDR_WIDTH:0]bufferInIndex; 
reg  [ADDR_WIDTH:0]bufferOutIndex;
reg  [ADDR_WIDTH-1:0]bufferIndexToMem;
reg  fifoREnDelayed;
wire [FIFO_WIDTH-1:0] dataFromMem;

always @(posedge fifoClk)
begin
  if (reset == 1'b1 || forceEmptyWr == 1'b1)
  begin
    bufferInIndex <= 0;
  end
    else
    begin
      if (fifoWEn == 1'b1) begin
        bufferInIndex <= bufferInIndex + 1'b1;
      end 
    end
end


always @(posedge fifoClk)
begin
  if (reset == 1'b1 || forceEmptyRd == 1'b1)
  begin
    bufferOutIndex <= 0;
    fifoREnDelayed <= 1'b0;
  end
    else
    begin
      fifoREnDelayed <= fifoREn;
      if (fifoREn == 1'b1 && fifoREnDelayed == 1'b0) begin
        dataOut <= dataFromMem;
        bufferOutIndex <= bufferOutIndex + 1'b1;
      end
    end
end


always @(bufferInIndex or bufferOutIndex or fifoWEn) begin
  if (fifoWEn) begin
    bufferIndexToMem <= bufferInIndex[ADDR_WIDTH-1:0];
  end else begin
    bufferIndexToMem <= bufferOutIndex[ADDR_WIDTH-1:0];
  end
end
  
wire            activ_dma0;

pselect        #(
                .ADDR_WIDTH     (8                         ),
                .BASE_WIDTH     (8                         ),
                .BASE_ADDR      (DMA_ADR >> 10      ) // using upper 8 bit of address
                )
        dmasel0 (
                .addr           (addr_high[7:0]            ),
                .activ_peri     (mem_access                ),
                .select         (activ_dma0                )
                );
  
RAMB16_S9_S9 fifo_mem (
// Port A, access from SD card module
                .DIA            (dataIn[7:0]                ),
                .DIPA           (1'd0                       ),
                .ADDRA          ({2'b0, bufferIndexToMem}   ),
                .ENA            (1'b1                       ), 
                .WEA            (fifoWEn                    ),
                .SSRA           (1'b0                       ),   
                .CLKA           (fifoClk                    ),
                .DOA            (dataFromMem[7:0]           ),
               // .DOPA           (dataFromMem[8]              ),
// Port B, access from SpartanMC
                .DIB            (do_peri[7:0]               ),
                .DIPB           (do_peri[8]                 ),
                .ADDRB          ({1'b0, addr_peri[9:0]}     ),
                .ENB            (1'b1                       ),
                .WEB            (store_access & activ_dma0  ),
                .SSRB           (! activ_dma0               ),
                .CLKB           (mem_clk                    ),
                .DOB            (di_peri[7:0]               ),
                .DOPB           (di_peri[8]                 )
                );

endmodule
