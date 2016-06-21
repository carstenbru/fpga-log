/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Projekt : FGLT TU Darmstadt
// Dateiname : spmc_dcf77.v
// Autor : Stefan Klir
// Modul : spmc_dcf77
//
// Beschreibung : DCF77 ist ein Hardwaremodul welches das DCF77 Funksignal
//		  decodiert und an die software sendet
////////////////////////////////////////////////////////////////////////////////////////////////////////

module dcf77_encoder #(parameter CLOCK_FREQUENCY = 16000000
			) ( 	
        input wire              clk,       	//Clock
        input wire              reset,          //Reset-Signal
        input wire dcf77_non_inverted,

	output reg dcf_sec,
	output reg [58:0] dcf_outputbits

);  

  reg [59:0] dcf_bits;
  //reg dcf_sec;
  reg [30:0] cnt;	//variabel
  reg [2:0] dcf_edge;

  parameter CNT_MAX = (11*CLOCK_FREQUENCY)/10;		//1.1 sec	//minute finished
  parameter CNT_SAMPLE = (15*CLOCK_FREQUENCY)/100;	//150 ms	// < 150ms = 0; else 1

  always@(posedge clk or posedge reset) begin
	if(reset) begin
		dcf_outputbits <= 60'b0;
		dcf_bits <= 60'b0;
		dcf_sec <= 1'b0;
		cnt <= 0;
		dcf_edge <= 3'b0;
	end else begin
		dcf_edge <= {dcf_edge[1:0], dcf77_non_inverted};
	
		if(cnt < CNT_MAX) cnt <= cnt + 1;
	
		if(dcf_edge[2:1] == 2'b01) begin	
			if(cnt == CNT_MAX) begin	//minute finished, long 0 detected
				dcf_sec <= 1'b1;
				dcf_outputbits <= dcf_bits[59:1];
				dcf_bits <= 0;
			end else begin
				dcf_sec <= 1'b0;
			end
			cnt <= 0;
		end else dcf_sec <= 1'b0;

		if(dcf_edge[2:1] == 2'b10) begin
			if(cnt < CNT_SAMPLE) begin
				dcf_bits <= {1'b0, dcf_bits[59:1]};	//check if cnt if < 150ms or above 150ms
			end
			else begin
				dcf_bits <= {1'b1, dcf_bits[59:1]};
			end	
		end
	end
	
  end
endmodule