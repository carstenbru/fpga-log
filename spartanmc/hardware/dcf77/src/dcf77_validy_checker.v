/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Projekt : FGLT TU Darmstadt
// Dateiname : dcf77_validy_checker.v
// Autor : Stefan Klir
// Modul : dcf77_validy_checker
//
// Beschreibung : DCF77 ist ein Hardwaremodul welches das DCF77 Funksignal
//		  decodiert und an die software sendet
////////////////////////////////////////////////////////////////////////////////////////////////////////

module dcf77_validy_checker #(parameter CLOCK_FREQUENCY = 16000000
			) ( 	
        input wire              clk,       	//Clock
        input wire              reset,          //Reset-Signal
        input wire [58:0] dcf_bits,

	output reg signal_valid
);  

  wire parity_min;
  wire parity_hour;
  wire parity_date;

  assign parity_min = (^dcf_bits[27:21] == dcf_bits[28]);
  assign parity_hour = (^dcf_bits[34:29] == dcf_bits[35]);
  assign parity_date = (^dcf_bits[57:36] == dcf_bits[58]);

  //assign signal_valid = (parity_min && parity_hour && parity_date && (dcf_bits[0] == 1'b0) && (dcf_bits[20] == 1'b1));
  //TODO signal nur für 1 takt auf 1 danach wieder 0
  always@(posedge clk or posedge reset) begin
	if(reset) signal_valid <= 1'b0;
	else begin
		if(parity_min && parity_hour && parity_date && (dcf_bits[0] == 1'b0) && (dcf_bits[20] == 1'b1)) signal_valid <= 1'b1;
		else signal_valid <= 1'b0;
	end
  end
		

  //TODO redundace check with previous data
endmodule