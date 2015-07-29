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

///////////////////////////////////////////////////////////////////////////////
// Projekt : SpartanMC
// Dateiname : timer_compare.v
// Autor : Steffen Kunze
// Modul : compare
//
// Beschreibung : Das Compareregister vergleicht den Timerwert mit einem 
//   		  gespeicherten Wert und loest bei Erreichen dieses Wertes
//  		  einen Interrupt oder ein Ereignis am Ausgangspin aus,
//                (falls konfiguriert).
///////////////////////////////////////////////////////////////////////////////

`define COMP_OUT

module timer_compare(
	clk_peri,
	do_peri,
	di_peri,
	addr_peri,
	access_peri,
	wr_peri,
	reset,
	intr,
	timer_data
`ifdef COMP_OUT
	,
	pin_out
`endif
	);

// Der Parameter fuer die Adresse wird
// in der system.v durch jConfig eingestellt.
parameter BASE_ADR	= 10'h0;

parameter CTRL_ADR	= 0;
parameter DATA_ADR	= 1;

parameter ALL_PORTS	= 2;
parameter DECODSIM_DEB	= "NO";

parameter PIN_IDLE_LEVEL = "1'bz";


// inputs & outputs des Systembus
input	wire		clk_peri;
input	wire	[9:0]	addr_peri;		// Adressbus
input	wire	[17:0]	do_peri;		// Datenbus
output	wire	[17:0]	di_peri;		// Datenbus
input	wire		access_peri;
input	wire		wr_peri;
input	wire		reset;
output	wire		intr;			// Interrupt flag
// Signale des Modul
input	wire	[17:0]	timer_data;		// Zaehlerstand des Timers
`ifdef COMP_OUT
output	wire		pin_out;		// Ausgangspin	(falls konfiguriert)
`endif

reg		[17:0]	comp_reg; 		// 18 bit Compare-Register
reg		[4:0]	control;		// Steuerregister

wire			en = control[0];	// compare enable
wire 			ir_en = control[1];	// interrupt enable 
wire		[2:0]	mode_sel = control[4:2];
						// Betriebsmodi :	x00 = gleich
						//			001 = setzen
						//			010 = zuruecksetzen
						//			011 = negieren
						//			101 = 1 bei CMP und 0 bei NULL
						//			110 = 0 bei CMP und 1 bei NULL
						//			111 = 1 bei CMP und 0 bei NULL

reg			ir_sig;			// interrupt register
reg		[17:0]	c_reg;			// Puffer fuer Timerwert

assign	intr	= ir_en? ir_sig: 1'b0;
reg			out;			// Datenregister Ausgangspin


// Dekodierung der Adressen ********************** START *************************************
// chipselect fuer die Statusregister (ohne Modemsignale)
wire				cs_tco_ctrl_rw;	// Lesen oder Schreiben
wire				cs_tco_data_rw;	// Lesen oder Schreiben
wire				cs_tco_ctrl_r;	// Lesen
wire				cs_tco_data_r;	// Lesen
wire				cs_tco_ctrl_w;	// Schreiben
wire				cs_tco_data_w;	// Schreiben

wire				select;
wire		[ALL_PORTS-1:0]	reg_select;
wire		[ALL_PORTS-1:0]	reg_read;
wire		[ALL_PORTS-1:0]	reg_write;

// Addressdekoder fuer alle Geraeteregister
reg_access_decoder	#(
		.REG_COUNT(	ALL_PORTS	),
		.BASE_ADR(	BASE_ADR	),
		.SIM_DEBUG(	DECODSIM_DEB	)
			)
decod1			(
		.reset(		reset		),
		.clk_peri(	clk_peri	),
		.addr_peri(	addr_peri	),
		.access_peri(	access_peri	),
		.wr_peri(	wr_peri		),

		.peri_select(	select		),
		.reg_select(	reg_select	),
		.reg_read(	reg_read	),
		.reg_write(	reg_write	)
			);

// chipselect fuer die FIFO-,Status- und Steuerregister
assign	cs_tco_ctrl_rw	= reg_select[CTRL_ADR];	// Lesen oder Schreiben
assign	cs_tco_data_rw	= reg_select[DATA_ADR];	// Lesen oder Schreiben
assign	cs_tco_ctrl_r	= reg_read[CTRL_ADR];	// Lesen
assign	cs_tco_data_r	= reg_read[DATA_ADR];	// Lesen
assign	cs_tco_ctrl_w	= reg_write[CTRL_ADR];	// Schreiben
assign	cs_tco_data_w	= reg_write[DATA_ADR];	// Schreiben

// Dekodierung der Adressen ********************** END **************************************


// Ausgangspin wird mit jConfig festgelegt und in die UCF Datei eingetragen.
`ifdef COMP_OUT
reg			oe;		// output enable
assign pin_out		= oe?out:PIN_IDLE_LEVEL;	// output tristate
`endif

//store "out" value for one clock cycle, otherwise it will be cleared before read with the new pipeline!
reg out_last;
always @ (posedge clk_peri) begin
  out_last <= out;
end

assign di_peri[17:0]	= (cs_tco_ctrl_r)?	{out_last, `ifdef COMP_OUT oe, `else 1'b0, `endif control}:
			  (cs_tco_data_r)?	comp_reg:	18'b0;

// In diesem Block werden Daten vom Bus in die Register uebernommen.
always @ (posedge clk_peri or posedge reset)
if(reset)
	begin	
	control		<=  5'b00000;
	comp_reg	<= 18'h00000;
`ifdef COMP_OUT
	oe		<= 1'b0;
`endif
	end
else
	begin
	if(cs_tco_ctrl_w)
		begin
		control <= do_peri[4:0];		// Steuerregister
`ifdef COMP_OUT
		oe	<= do_peri[5];
//		out	<= do_peri[6];
`endif
		end
	if(cs_tco_data_w)
		comp_reg <= do_peri;			// Compareregister
	end


// In diesem Block wird bei Erreichen des eingestellten Wertes das
// Interruptsignal erzeugt und der Ausgang entsprechend der Programmierung
// gesetzt.
always@(posedge clk_peri or posedge reset)
if(reset)
   begin
   c_reg	<= 18'b0;
   ir_sig	<= 1'b0;
   out		<= 0;
   end
else
   begin
	// Interrupt und out zuruecksetzen bei einem Portzugriff
   if(cs_tco_ctrl_rw || cs_tco_data_rw)
	begin
	ir_sig <= 1'b0;
	// Ausgangswerte von out genau auf gegenteiligen Wert setzen!
        case(mode_sel[2:0])
		3'b001: out <= 1'b0;	// setzen
		3'b010: out <= 1'b1;	// zuruecksetzen
	endcase
	end
   else
     begin
	// wenn der Zaehler den eingestellten Wert erreicht hat, wird Interrupt
	// gesetzt und der Ausgang geschalten. Da der Timer und das Comparereregister
	// unterschiedlich getaktet sind, darf das nur passieren, wenn sich der
	// Zaehlerstand zum letzten Takt geaendert hat.    
      if(en && comp_reg == timer_data && c_reg != timer_data)
         begin
         ir_sig <= 1'b1;
         	casex(mode_sel[2:0])
			3'b?00: out <= out;	// gleichbleibend
			3'b?01: out <= 1'b1;	// setzen
			3'b?10: out <= 1'b0;	// zuruecksetzen
			3'b011: out <= ~out;	// invertieren
			3'b111: out <= 1'b1;	// setzen
		endcase
         end
	else
	if(en && timer_data == 18'h00000 && c_reg != timer_data && mode_sel[2])
	// Aktionen beim Nulldurchgang des Timer
         begin         
         	case(mode_sel[1:0])
			2'b00: out <= out;	// gleichbleibend
			2'b01: out <= 1'b0;	// zuruecksetzen
			2'b10: out <= 1'b1;	// setzen
			2'b11: out <= 1'b0;	// zuruecksetzen
		endcase
         end
         
         // Timerwert zwischenspeichern, um Aenderung erkennen zu koennen
	 c_reg <= timer_data;
     end 
   end 						

endmodule
