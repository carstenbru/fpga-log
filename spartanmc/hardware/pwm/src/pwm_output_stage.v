/*
  module for a simple PWM generator stage
*/
module pwm_output_stage (clk, reset, sel, adr, data, pwm_out);
  parameter REG_WIDTH = 32; //width of internal registers and counter, determines min frequency
  
  parameter ADR_FREQ = 2'b00; //the different register adresses
  parameter ADR_ON = 2'b01;
  parameter ADR_COUNTER = 2'b10;
  
  input clk;
  input reset;
  input sel;
  input [1:0] adr;
  input [REG_WIDTH-1:0] data;
  
  output pwm_out;
  
  reg [REG_WIDTH-1:0] cycle;
  reg [REG_WIDTH-1:0] on_time;
  
  reg [REG_WIDTH-1:0] counter;
  
  assign pwm_out = (counter < on_time);
  
  always @(posedge clk) begin
    if (reset) begin
	    cycle <= {(REG_WIDTH){1'b0}}; //initialize period with 0
	    on_time <= {(REG_WIDTH){1'b0}}; //initialize on_time with 0 -> output disabled
		
	    counter <= {(REG_WIDTH){1'b0}}; //reset counter to 0
	 end else begin
	   if (sel) begin
	     case (adr) //store data to register addressed with "adr" line
		    ADR_FREQ: cycle <= data;
		    ADR_ON: on_time <= data;
		    ADR_COUNTER: counter <= data;
		  endcase
	   end
		
		if (!(sel && adr[1])) begin //no write action to counter register
	  	  if (counter == cycle)
		    counter <= {(REG_WIDTH){1'b0}}; //reset counter if max value reached
		  else
		    counter <= counter + 1;
		end
	 end
  end
  
endmodule
