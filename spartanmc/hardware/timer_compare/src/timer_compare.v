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

module timer_compare(
	clk_peri,
	do_peri,
	di_peri,
	addr_peri,
	access_peri,
	wr_peri,
	reset,
	intr,
	timer_data,
	pin_out
	);

// Der Parameter fuer die Adresse wird
// in der system.v durch jConfig eingestellt.
parameter CTRL_ADR	= 0;
parameter DAT_ADR	= 1;
parameter BASE_ADR	= 10'h0;

`define COMP_OUT

// inputs & outputs des Systembus
input		clk_peri;
input	[9:0]	addr_peri;		// Adressbus
input	[17:0]	do_peri;		// Datenbus
output	[17:0]	di_peri;		// Datenbus
input		access_peri;
input		wr_peri;
input		reset;
output		intr;			// Interrupt flag
// Signale des Modul
input	[17:0]	timer_data;		// Zaehlerstand des Timers
output		pin_out;		// Ausgangspin	(falls konfiguriert)

reg	[17:0]	comp_reg; 		// 18 bit Compare-Register
reg	[4:0]	control;		// Steuerregister

wire		en = control[0];	// compare enable
wire 		ir_en = control[1];	// interrupt enable 
wire	[2:0]	mode_sel = control[4:2];
					// Betriebsmodi :	x00 = gleich
					//			001 = setzen
					//			010 = zuruecksetzen
					//			011 = negieren
					//			101 = 1 bei CMP und 0 bei NULL
					//			110 = 0 bei CMP und 1 bei NULL
					//			111 = 1 bei CMP und 0 bei NULL
 
reg		ir_sig;			// interrupt register
reg	[17:0]	c_reg;			// Puffer fuer Timerwert

assign	intr	= ir_en?ir_sig:1'b0;
reg		out;			// Datenregister Ausgangspin


wire		select;

// chipselect fuer die Status- und Steuerregister
pselect p1	(
			.addr(		addr_peri[9:3]	),
			.activ_peri(	access_peri	),
			.select(	select		)
		);
defparam p1.ADDR_WIDTH	= 7;
defparam p1.BASE_WIDTH	= 7;
defparam p1.BASE_ADDR	= BASE_ADR >> 3;


// Ausgangspin wird mit jConfig festgelegt und in die UCF Datei eingetragen.
`ifdef COMP_OUT
reg		oe;		// output enable
wire		pin_out;	// Ausgangspinsignal
assign pin_out	= oe?out:1'bz;	// output tristate
`endif

assign di_peri[17:0]	= (addr_peri[2:0]==CTRL_ADR && select)?{`ifdef COMP_OUT out, oe, `endif control}:
			  (addr_peri[2:0]==DAT_ADR && select)?comp_reg:18'b0;

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
	if(addr_peri[2:0]==CTRL_ADR && select && wr_peri)
		begin
		control <= do_peri[4:0];		// Steuerregister
`ifdef COMP_OUT
		oe	<= do_peri[5];
//		out	<= do_peri[6];
`endif
		end
	if(addr_peri[2:0]==DAT_ADR && select && wr_peri)
		comp_reg <= do_peri;			// Compareregister
	end


// In diesem Block wird bei Erreichen des eingestellten Wertes das
// Interruptsignal erzeugt und der Ausgang entsprechend der Programmierung
// gesetzt.
always@(posedge clk_peri)
if(reset)
   begin
   c_reg	= 18'b0;
   ir_sig	= 1'b0;
   out		= 0;
   end
else
   begin
	// Interrupt und out zuruecksetzen bei einem Portzugriff
   if(addr_peri[2:0]==CTRL_ADR && select || addr_peri[2:0]==DAT_ADR && select)
	begin
	ir_sig = 1'b0;
	// Ausgangswerte von out genau auf gegenteiligen Wert setzen!
        case(mode_sel[2:0])
		3'b001: out = 1'b0;	// setzen
		3'b010: out = 1'b1;	// zuruecksetzen
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
         ir_sig = 1'b1;
         	casex(mode_sel[2:0])
			3'b?00: out = out;	// gleichbleibend
			3'b?01: out = 1'b1;	// setzen
			3'b?10: out = 1'b0;	// zuruecksetzen
			3'b011: out = ~out;	// invertieren
			3'b111: out = 1'b1;	// setzen
		endcase
         end
	else
	if(en && timer_data == 18'h00000 && c_reg != timer_data && mode_sel[2])
	// Aktionen beim Nulldurchgang des Timer
         begin         
         	case(mode_sel[1:0])
			2'b00: out = out;	// gleichbleibend
			2'b01: out = 1'b0;	// zuruecksetzen
			2'b10: out = 1'b1;	// setzen
			2'b11: out = 1'b0;	// zuruecksetzen
		endcase
         end
         
         // Timerwert zwischenspeichern, um Aenderung erkennen zu koennen
         c_reg = timer_data;
     end 
   end 						

endmodule
