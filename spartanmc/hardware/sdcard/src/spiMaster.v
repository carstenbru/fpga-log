//////////////////////////////////////////////////////////////////////
////                                                              ////
//// spiMaster.v                                                    ////
////                                                              ////
//// This file is part of the spiMaster opencores effort.
//// <http://www.opencores.org/cores//>                           ////
////                                                              ////
//// Module Description:                                          ////
////  Top level module
//// 
////  
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
`include "spiMaster_defines.v"

module spiMaster(
  clk_i,
  rst_i,
  address_i,
  data_i,
  data_o,
  strobe_i,
  we_i,
  ack_o,

  // SPI logic clock
  spiSysClk,

  //SPI bus
  spiClkOut,
  spiDataIn,
  spiDataOut,
  spiCS_n
);
parameter SDCARD_CLOCK = 16000000;

//Wishbone bus
input clk_i;
input rst_i;
input [7:0] address_i;
input [7:0] data_i;
output [7:0] data_o;
input strobe_i;
input we_i;
output ack_o;

// SPI logic clock
input spiSysClk;

//SPI bus
output spiClkOut;
input spiDataIn;
output spiDataOut;
output spiCS_n;

// local wires and regs
wire spiSysClk;
wire [7:0] spiClkDelay;
wire rstSyncToSpiClk;
wire [7:0] rxDataFromRWSPIWireData;
wire rxDataRdySetFromRWSPIWireData;
wire txDataFullFromSpiTxRxData;
wire txDataFullClrFromRWSPIWireData;
wire [7:0] txDataToRWSPIWireData;
wire rxDataRdyClrFromRWSDBlock;
wire rxDataRdyClrFromSendCmd;
wire [7:0] rxDataFromSpiTxRxData;
wire rxDataRdy;
wire [7:0] txDataFromRWSDBlock;
wire txDataWenFromRWSDBlock;
wire [7:0] txDataFromSendCmd;
wire txDataWenFromSendCmd;
wire [7:0] dataFromCtrlStsReg;
wire [7:0] dataFromTxFifo;
wire [7:0] dataFromRxFifo;
wire [1:0] spiTransType;
wire [7:0] spiDirectAccessTxData;
wire [1:0] readWriteSDBlockReq;
wire [1:0] SDWriteError;
wire [1:0] SDReadError;
wire [7:0] sendCmdRespByte;
wire [7:0] cmdByteFromRWSDBlock;
wire [7:0] dataByte1FromRWSDBlock;
wire [7:0] dataByte2FromRWSDBlock;
wire [7:0] dataByte3FromRWSDBlock;
wire [7:0] dataByte4FromRWSDBlock;
wire [7:0] checkSumByteFromRWSDBlock;
wire [7:0] txFifoDataOut;
wire [7:0] rxFifoDataIn;
wire [31:0] SDAddr;
wire [7:0] spiClkDelayFromCtrlStsReg;
wire spiCS_nFromRWSDBlock;
wire spiCS_nFromSpiCtrl;
wire SDInitReq;


assign spiCS_n = spiCS_nFromRWSDBlock & spiCS_nFromSpiCtrl;
assign spiClkDelay = SDInitReq ? `SLOW_SPI_CLK : spiClkDelayFromCtrlStsReg;

// -----------------------------------
// Instance of Module: wishBoneBI
// -----------------------------------
spiMasterWishBoneBI u_spiMasterWishBoneBI(
  .ack_o(               ack_o                 ),
  .address(             address_i             ),
  .clk(                 clk_i                 ),
  .ctrlStsRegSel(       ctrlStsRegSel         ),
  .dataFromCtrlStsReg(  dataFromCtrlStsReg    ),
  .dataFromRxFifo(      dataFromRxFifo        ),
  .dataFromTxFifo(      dataFromTxFifo        ),
  .dataIn(              data_i                ),
  .dataOut(             data_o                ),
  .rst(                 rst_i                 ),
  .rxFifoSel(           rxFifoSel             ),
  .strobe_i(            strobe_i              ),
  .txFifoSel(           txFifoSel             ),
  .writeEn(             we_i                  )
	);

// -----------------------------------
// Instance of Module: ctrlStsRegBI
// -----------------------------------
ctrlStsRegBI u_ctrlStsRegBI(
  .busClk(              clk_i                 ),
  .spiSysClk(           spiSysClk             ),
  .rstSyncToBusClkOut(  rstSyncToBusClk       ),
  .rstSyncToSpiClkOut(  rstSyncToSpiClk       ),
  .rstFromWire(         rst_i                 ),
  .address(             address_i             ),
  .strobe_i(            strobe_i              ),
  .dataIn(              data_i                ),
  .dataOut(             dataFromCtrlStsReg    ),
  .ctrlStsRegSel(       ctrlStsRegSel         ),
  .spiTransType(        spiTransType          ),
  .spiTransCtrl(        spiTransCtrl          ), 
  .spiTransStatus(      spiTransSts           ),
  .spiDirectAccessTxData(spiDirectAccessTxData),
  .spiDirectAccessRxData(rxDataFromSpiTxRxData), 
  .writeEn(             we_i                  ),
  .SDWriteError(        SDWriteError          ),
  .SDReadError(         SDReadError           ),
  .SDInitError(         `INIT_NO_ERROR        ),
  .SDAddr(              SDAddr                ),
  .spiClkDelay(         spiClkDelayFromCtrlStsReg)
	);

// -----------------------------------
// Instance of Module: spiCtrl
// -----------------------------------
spiCtrl u_spiCtrl(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .SDInitReq(           SDInitReq             ),
  .readWriteSDBlockReq( readWriteSDBlockReq   ),
  .readWriteSDBlockRdy( readWriteSDBlockRdy   ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdyClr(        rxDataRdyClrFromSpiCtrl),
  .spiTransType(        spiTransType          ),
  .spiTransCtrl(        spiTransCtrl          ),
  .spiTransSts(         spiTransSts           ),
  .txDataWen(           txDataWenFromSpiCtrl  ),
  .spiCS_n(             spiCS_nFromSpiCtrl    )
	);

// -----------------------------------
// Instance of Module: readWriteSDBlock
// -----------------------------------
readWriteSDBlock u_readWriteSDBlock(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .readWriteSDBlockReq( readWriteSDBlockReq   ),
  .readWriteSDBlockRdy( readWriteSDBlockRdy   ),
  .cmdByte(             cmdByteFromRWSDBlock  ),
  .dataByte1(           dataByte1FromRWSDBlock),
  .dataByte2(           dataByte2FromRWSDBlock),
  .dataByte3(           dataByte3FromRWSDBlock),
  .dataByte4(           dataByte4FromRWSDBlock),
  .checkSumByte(        checkSumByteFromRWSDBlock),
  .readError(           SDReadError             ),
  .respByte(            sendCmdRespByte       ),
  .respTout(            sendCmdRespTout       ),
  .rxDataIn(            rxDataFromSpiTxRxData ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdyClr(        rxDataRdyClrFromRWSDBlock),
  .sendCmdRdy(          sendCmdRdy            ),
  .sendCmdReq(          sendCmdReqFromRWSDBlock),
  .spiCS_n(             spiCS_nFromRWSDBlock ),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataEmpty(         txDataEmptyFromRWSPIWireData),
  .txDataOut(           txDataFromRWSDBlock   ),
  .txDataWen(           txDataWenFromRWSDBlock),
  .txFifoData(          txFifoDataOut         ),
  .txFifoRen(           txFifoRE              ),
  .rxFifoData(          rxFifoDataIn          ),
  .rxFifoWen(           rRxFifoWE             ),
  .writeError(          SDWriteError          ),
  .blockAddr(           SDAddr                )

	);

// -----------------------------------
// Instance of Module: sendCmd
// -----------------------------------
sendCmd u_sendCmd(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .sendCmdReq_in(         sendCmdReqFromRWSDBlock),
  .sendCmdRdy(          sendCmdRdy            ),
  .cmdByte_in(           cmdByteFromRWSDBlock  ),
  .dataByte1_in(         dataByte1FromRWSDBlock),
  .dataByte2_in(         dataByte2FromRWSDBlock),
  .dataByte3_in(         dataByte3FromRWSDBlock),
  .dataByte4_in(         dataByte4FromRWSDBlock),
  .checkSumByte_in(      checkSumByteFromRWSDBlock),
  .respByte(            sendCmdRespByte       ),
  .respTout(            sendCmdRespTout       ),
  .rxDataIn(            rxDataFromSpiTxRxData ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdyClr(        rxDataRdyClrFromSendCmd),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataEmpty(         txDataEmptyFromRWSPIWireData),
  .txDataOut(           txDataFromSendCmd     ),
  .txDataWen(           txDataWenFromSendCmd  )
	);

// -----------------------------------
// Instance of Module: spiTxRxData
// -----------------------------------
spiTxRxData u_spiTxRxData(
  .clk(                 spiSysClk             ),
  .rst(                 rstSyncToSpiClk       ),
  .rx1DataRdyClr(       rxDataRdyClrFromRWSDBlock),
  .rx2DataRdyClr(       rxDataRdyClrFromSendCmd),
  .rx3DataRdyClr(       rxDataRdyClrFromSpiCtrl),
  .rxDataIn(            rxDataFromRWSPIWireData),
  .rxDataOut(           rxDataFromSpiTxRxData ),
  .rxDataRdy(           rxDataRdyFromSpiTxRxData),
  .rxDataRdySet(        rxDataRdySetFromRWSPIWireData),
  .tx1DataIn(           txDataFromRWSDBlock   ),
  .tx1DataWEn(          txDataWenFromRWSDBlock),
  .tx2DataIn(           txDataFromSendCmd     ),
  .tx2DataWEn(          txDataWenFromSendCmd  ),
  .tx3DataIn(           spiDirectAccessTxData ),
  .tx3DataWEn(          txDataWenFromSpiCtrl  ),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataFullClr(       txDataFullClrFromRWSPIWireData),
  .txDataOut(           txDataToRWSPIWireData )
	);

// -----------------------------------
// Instance of Module: readWriteSPIWireData
// -----------------------------------
readWriteSPIWireData u_readWriteSPIWireData(
  .clk(                 spiSysClk             ),
  .clkDelay(            spiClkDelay           ),
  .rst(                 rstSyncToSpiClk       ),
  .rxDataOut(           rxDataFromRWSPIWireData),
  .rxDataRdySet(        rxDataRdySetFromRWSPIWireData),
  .spiClkOut(           spiClkOut             ),
  .spiDataIn(           spiDataIn             ),
  .spiDataOut(          spiDataOut            ),
  .txDataFull(          txDataFullFromSpiTxRxData),
  .txDataFullClr(       txDataFullClrFromRWSPIWireData),
  .txDataIn(            txDataToRWSPIWireData ),
  .txDataEmpty(         txDataEmptyFromRWSPIWireData)
	);

sm_TxFifo #(`TX_FIFO_DEPTH, `TX_FIFO_ADDR_WIDTH) u_sm_txFifo (
  .spiSysClk(spiSysClk), 
  .busClk(clk_i), 
  .rstSyncToBusClk(rstSyncToBusClk), 
  .rstSyncToSpiClk(rstSyncToSpiClk), 
  .fifoREn(txFifoRE), 
  .fifoEmpty(hostTxFifoEmpty),
  .busAddress(address_i[2:0]), 
  .busWriteEn(we_i), 
  .busStrobe_i(strobe_i),
  .busFifoSelect(txFifoSel),
  .busDataIn(data_i), 
  .busDataOut(dataFromTxFifo),
  .fifoDataOut(txFifoDataOut) );


sm_RxFifo #(`RX_FIFO_DEPTH, `RX_FIFO_ADDR_WIDTH) u_sm_rxFifo(
  .spiSysClk(spiSysClk), 
  .busClk(clk_i),
  .rstSyncToBusClk(rstSyncToBusClk), 
  .rstSyncToSpiClk(rstSyncToSpiClk), 
  .fifoWEn(rRxFifoWE), 
  .fifoFull(hostRxFifoFull),
  .busAddress(address_i[2:0]), 
  .busWriteEn(we_i), 
  .busStrobe_i(strobe_i),
  .busFifoSelect(rxFifoSel),
  .busDataIn(data_i), 
  .busDataOut(dataFromRxFifo),
  .fifoDataIn(rxFifoDataIn)  );

endmodule

