module spmc_timestamp_counter #(
            parameter CLOCK_FREQUENCY = 16000000, //input clock frequency
            parameter BASE_ADR = 10'h0) ( 
        //*** Connections to SpartanMC Core (do not change) ***
        input wire              clk_peri,       //System-Clock
        input wire      [17:0]  do_peri,        //Data Bus  from MC
        output wire     [17:0]  di_peri,        //Data Bus  to MC
        input wire      [9:0]   addr_peri,      //Address Bus from MC
        input wire              access_peri,    //Peripheral Access Signal
        input wire              wr_peri,        //Write Enable Signal

        //*** Connections to SpartanMC Core which can be changed ***
        input wire              reset,          //Reset-Signal (could be external)

        //*** timestamp counter output ***
        output reg [35:0] lpt_counter, //low precision counter input
        output reg [35:0] hpt_counter //high precision counter input
);
  //register addresses of the module
  parameter LPT_LOW = 2'b00; //low precision timestamp low register
  parameter LPT_HIGH = 2'b01; //low precision timestamp high register
  parameter HPT_LOW = 2'b10; //high precision timestamp low register
  parameter HPT_HIGH = 2'b11; //high precision timestamp high register

  wire select;
  // Address decoder generates the select sinal out of the upper part of the peripheral address.
  pselect iCSL (
                .addr           (       addr_peri[9:2]  ),
                .activ_peri     (       access_peri     ),
                .select         (       select          )
                );
  defparam iCSL.ADDR_WIDTH  = 8;
  defparam iCSL.BASE_WIDTH  = 8;
  defparam iCSL.BASE_ADDR   = BASE_ADR >> 2;      //BASE_ADR has to be divisible by 4
  
  //counter write register
  reg [35:0] lpt_counter_write;
  reg [35:0] hpt_counter_write;
  reg counter_write_req;
  
 
  //SpartanMC peripheral interface read logic -> no read
  assign di_peri = 18'b0;

  
  //SpartanMC peripheral interface write logic
  always @(posedge clk_peri) begin
    if (reset) begin
      counter_write_req <= 1'b0;
    end else begin
      if (select & wr_peri) begin
        case (addr_peri[1:0])
	  LPT_LOW: lpt_counter_write[17:0] = do_peri[17:0];
	  LPT_HIGH: lpt_counter_write[35:18] = do_peri[17:0];
	  HPT_LOW: hpt_counter_write[17:0] = do_peri[17:0];
	  HPT_HIGH: begin
	      hpt_counter_write[35:18] = do_peri[17:0]; 
	      counter_write_req <= 1'b1;
	    end
        endcase
      end else begin
        counter_write_req <= 1'b0;
      end
    end
  end
  
  //counter logic
  always @(posedge clk_peri) begin
    if (reset) begin
      hpt_counter <= 18'd0; //reset counters to 0
      lpt_counter <= 18'd0;
    end else begin
      if (counter_write_req == 1'b1)begin
        hpt_counter <= hpt_counter_write;
        lpt_counter <= lpt_counter_write;
      end else begin
        if (hpt_counter == CLOCK_FREQUENCY-1) begin
	  hpt_counter <= 18'd0; //reset high precision counter if max value reached
	  lpt_counter <= lpt_counter + 1; //increment low precision counter
        end else
	  hpt_counter <= hpt_counter + 1; //otherwise increment high precision counter
      end
    end
  end
  
endmodule
