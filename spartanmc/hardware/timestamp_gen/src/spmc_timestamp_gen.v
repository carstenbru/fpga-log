module spmc_timestamp_gen #(
            parameter SOURCES = 2,        //number of timestamp capture sources
            parameter PIN_SOURCES = 2,        //number of timestamp capture pin sources
            parameter INVERTED_SOURCES_MASK = 0,  //mask if sources to invert (so trigger on a negative edge instead of a positive edge)
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

        //*** timestamp counter input ***
        input wire [35:0] lpt_counter, //low precision counter input
        input wire [35:0] hpt_counter, //high precision counter input
        
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
  
  reg [PIN_SOURCES-1:0] pin_source_sync;
  
  wire [SOURCES_SUM-1:0] source_inv;
  assign source_inv = INVERTED_SOURCES_MASK;
  wire [SOURCES_SUM-1:0] source;
  assign source = ({ pin_source_sync, internal_source } ^ source_inv);

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
  
  //timestamp capture line registers
  reg [SOURCES_SUM-1:0] source_last;
  wire [SOURCES_SUM-1:0] source_change;
  reg capture;
  reg [17:0] tsr_tmp;
  reg [1:0] capture_state;
  reg [17:0] tsr_capture;
  reg [35:0] hpt_capture;  
  wire capture_ready;
  
  wire [17:0] dat_out;
  
  //delay read signals by one cycle (for new pipeline)
  reg reg_read;
  reg status_read;
  reg read_addr_lsb;
  always @(posedge clk_peri) begin
    reg_read <= select & !wr_peri;
    status_read <= addr_peri[2:0] == STATUS_ADR;
    read_addr_lsb <= addr_peri[0];
  end
  
  //SpartanMC peripheral interface read logic
  assign dat_out = status_read ? {17'd0, fifo_n_empty} : (read_addr_lsb ? fifo_out_dat[35:18] : fifo_out_dat[17:0]);
  assign di_peri = (reg_read) ? dat_out : 18'b0;
  
  assign source_change = (~source_last & source);
  assign fifo_n_empty = (fifo_ts_read != fifo_ts_write);
  
  assign capture_ready = capture_state == 2'b11;
  
  //synchronize external pin sources to clk_peri
  always @(posedge clk_peri) begin
    pin_source_sync <= pin_source;
  end
  
  //SpartanMC peripheral interface write logic
  always @(posedge clk_peri) begin
    if (reset) begin
      fifo_ts_read <= 7'd0;
    end else begin
      if (select & wr_peri) begin
        case (addr_peri[2:0])
          CONTROL_ADR: begin
            if (do_peri[17]) begin
	      fifo_ts_read <= fifo_ts_write; //clear fifo
	    end else
	    if (do_peri[16] && fifo_n_empty) begin
	      fifo_ts_read <= fifo_ts_read + 1; //next fifo element
	    end
	
	    sw_source_change <= do_peri[15:0];
	  end
        endcase
      end else begin
        if (capture_ready)
          sw_source_change <= 16'd0;
      end
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
  
RAMB16_S36_S36 TIMESTAMP_FIFO	(
		.ADDRA(		{fifo_ts_read[6:0], addr_peri[2:1]}		),
		.ENA(		1'b1						),
		.WEA(		1'b0						),
		.SSRA(		1'b0						),
		.CLKA(		clk_peri					),
		.DOA(		{fifo_out_dat[33:18],fifo_out_dat[15:0]}	),
		.DOPA(		{fifo_out_dat[35:34],fifo_out_dat[17:16]}	),
		
		.DIB(		{fifo_in_dat[33:18],fifo_in_dat[15:0]}		),
		.DIPB(		{fifo_in_dat[35:34],fifo_in_dat[17:16]}		),
		.ADDRB(		{fifo_ts_write[6:0], capture_state[1:0]}	),
		.ENB(		!capture_ready					),
		.WEB(		1'b1						),
		.SSRB(		1'b1						),
		.CLKB(		clk_peri					)
		);
  
endmodule
