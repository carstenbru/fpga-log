module spmc_timestamp_gen #(
            parameter SOURCES = 2,        //number of timestamp capture sources
            parameter PIN_SOURCES = 2,        //number of timestamp capture pin sources
            parameter INVERTED_SOURCES_MASK = 0,  //mask if sources to invert (so trigger on a negative edge instead of a positive edge)
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

        //*** io interface ***
        input wire [SOURCES-1:0] internal_source,
        input wire [PIN_SOURCES-1:0] pin_source
);
  //register addresses of the module
  parameter LPT_LOW = 3'b000; //low precision timestamp low register
  parameter LPT_HIGH = 3'b001; //low precision timestamp high register
  parameter HPT_LOW = 3'b010; //high precision timestamp low register
  parameter HPT_HIGH = 3'b011; //high precision timestamp high register
  parameter TSR = 3'b100; //timestamp source register
  parameter CONTROL_ADR = 3'b101; //control register
  parameter STATUS_ADR = 3'b110; //status register
  
  parameter SOURCES_SUM = SOURCES + PIN_SOURCES;
  
  wire [SOURCES_SUM-1:0] source_inv;
  assign source_inv = INVERTED_SOURCES_MASK;
  wire [SOURCES_SUM-1:0] source;
  assign source = ({ pin_source, internal_source } ^ source_inv);

  wire select;
  // Address decoder generates the select sinal out of the upper part of the peripheral address.
  pselect iCSL (
                .addr           (       addr_peri[9:3]  ),
                .activ_peri     (       access_peri     ),
                .select         (       select          )
                );
  defparam iCSL.ADDR_WIDTH  = 7;
  defparam iCSL.BASE_WIDTH  = 7;
  defparam iCSL.BASE_ADDR   = BASE_ADR >> 3;      //BASE_ADR has to be divisible by 8
  
  reg [15:0] sw_source_change;
  
  //fifo registers
  reg [6:0] fifo_ts_read;
  reg [6:0] fifo_ts_write;
  reg [35:0] fifo_in_dat;
  wire [35:0] fifo_out_dat;
  wire fifo_n_empty;
  
  reg [17:0] fifo_top [0:7];
  reg [1:0] fifo_top_read_state;
  
  //timestamp capture line registers
  reg [SOURCES_SUM-1:0] source_last;
  wire [SOURCES_SUM-1:0] source_change;
  reg capture;
  reg [17:0] tsr_tmp;
  reg [1:0] capture_state;
  reg [17:0] tsr_capture;
  reg [35:0] hpt_capture;  
  wire capture_ready;
  
  //counter registers
  reg [35:0] lpt_counter;
  reg [35:0] hpt_counter;
  
  wire [17:0] dat_out;
  //SpartanMC peripheral interface read logic
  assign dat_out = (addr_peri[2:0] == STATUS_ADR) ? {17'd0, fifo_n_empty} : fifo_top[addr_peri[2:0]];
  assign di_peri = (select & !wr_peri) ? dat_out : 18'b0;
  
  assign source_change = (~source_last & source);
  assign fifo_n_empty = (fifo_ts_read != fifo_ts_write);
  
  assign capture_ready = capture_state == 2'b11;
  
  //SpartanMC peripheral interface write logic
  always @(posedge clk_peri) begin
    if (reset) begin
      fifo_ts_read <= 7'd0;
    end else begin
      if (select & wr_peri & (addr_peri[2:0] == CONTROL_ADR)) begin
        if (do_peri[17]) begin
	  fifo_ts_read <= fifo_ts_write; //clear fifo
	end else
	if (do_peri[16] && fifo_n_empty) begin
	  fifo_ts_read <= fifo_ts_read + 1; //next fifo element
	end
	
	sw_source_change <= do_peri[15:0];
      end else
        if (capture_ready)
          sw_source_change <= 16'd0;
    end
  end
  
  //counter logic
  always @(posedge clk_peri) begin
    if (reset) begin
      hpt_counter <= 18'd0; //reset counters to 0
      lpt_counter <= 18'd0;
    end else begin
      if (hpt_counter == CLOCK_FREQUENCY-1) begin
	hpt_counter <= 18'd0; //reset high precision counter if max value reached
	lpt_counter <= lpt_counter + 1; //increment low precision counter
      end else
	hpt_counter <= hpt_counter + 1; //otherwise increment high precision counter
    end
  end
  
  //store last levels of input signals
  always @(posedge clk_peri) begin
    if (reset) begin
      source_last <= {(SOURCES_SUM){1'b1}};
    end else begin
      if (capture_ready)
        source_last <= source;
    end
  end
  
  //find changed timestamp capture signal
  integer i;
  always @(*) begin
    capture = 1'b0;
    tsr_tmp = 0;
    for (i = 0; i < SOURCES_SUM; i = i + 1) begin
      if(source_change[i]) begin
        tsr_tmp = i+1;
        capture = 1'b1;
      end
    end
  end
  
  //fifo write logic (after capture)
  always @(posedge clk_peri) begin
    if (reset) begin
      capture_state <= 2'b11;
      fifo_ts_write <= 7'd0;
    end else begin
      if (capture_state == 2'b11) begin
        if (sw_source_change) begin
          tsr_capture <= sw_source_change;
	  hpt_capture <= hpt_counter;
	  capture_state <= 2'b00;
        end else
	if (capture) begin
	  tsr_capture <= tsr_tmp;
	  hpt_capture <= hpt_counter;
	  capture_state <= 2'b00;
	end
      end else begin
	if (capture_state == 2'b10)
	  fifo_ts_write <= fifo_ts_write + 1;
	
	capture_state <= capture_state + 1;
      end
    end
  end
  
  always @(*) begin
    case (capture_state)
      2'b00: fifo_in_dat = lpt_counter;
      2'b01: fifo_in_dat = hpt_capture;
      2'b10: fifo_in_dat = {18'd0, tsr_capture};
      default: fifo_in_dat = 36'd0;
    endcase
  end
  
  //fifo top read logic (to register)
  always @(posedge clk_peri) begin
    case (fifo_top_read_state)
    2'b00: begin
	     fifo_top[6] <= fifo_out_dat[17:0];
	     fifo_top[7] <= fifo_out_dat[35:18];
	   end
    2'b01: begin
	     fifo_top[0] <= fifo_out_dat[17:0];
	     fifo_top[1] <= fifo_out_dat[35:18];
	   end
    2'b10: begin
	     fifo_top[2] <= fifo_out_dat[17:0];
	     fifo_top[3] <= fifo_out_dat[35:18];
	   end
    2'b11: begin
	     fifo_top[4] <= fifo_out_dat[17:0];
	     fifo_top[5] <= fifo_out_dat[35:18];
	   end
    endcase
    fifo_top_read_state <= fifo_top_read_state + 1;
  end
  
RAMB16_S36_S36 TIMESTAMP_FIFO	(
		.ADDRA(		{fifo_ts_read[6:0], fifo_top_read_state[1:0]}	),
		.ENA(		1'b1						),
		.WEA(		1'b0						),
		.SSRA(		1'b0						),
		.CLKA(		clk_peri					),
		.DOA(		fifo_out_dat[31:0]				),
		.DOPA(		fifo_out_dat[35:32]				),
		
		.DIB(		fifo_in_dat[31:0]				),
		.DIPB(		fifo_in_dat[35:32]				),
		.ADDRB(		{fifo_ts_write[6:0], capture_state[1:0]}	),
		.ENB(		!capture_ready					),
		.WEB(		1'b1						),
		.SSRB(		1'b1						),
		.CLKB(		clk_peri					)
		);
  
endmodule