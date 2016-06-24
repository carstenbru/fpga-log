/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Projekt : FGLT TU Darmstadt
// Dateiname : spmc_dcf77.v
// Autor : Stefan Klir
// Modul : spmc_dcf77
//
// Beschreibung : DCF77 ist ein Hardwaremodul welches das DCF77 Funksignal
//		  decodiert und an die software sendet
////////////////////////////////////////////////////////////////////////////////////////////////////////

module spmc_dcf77 #(parameter BASE_ADR = 10'h0,
                    parameter CLOCK_FREQUENCY = 16000000
 			) ( 
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
        input wire dcf77_non_inverted,

	output wire signal_valid
);  

  parameter ALL_PORTS = 8;		//Reserves the space in memory for control,t,mon,j,h,min,s
  parameter DECODSIM_DEB = "NO";

  parameter CONTROL_ADR = 0;
  parameter DAY_ADR = 1;
  parameter MONTH_ADR = 2;
  parameter YEAR_ADR = 3;
  parameter HOUR_ADR = 4;
  parameter MINUTE_ADR = 5;
  parameter WEEKDAY_ADR = 6;
  parameter STARTBITS_ADR = 7;


// Dekodierung der Adressen ********************** START *************************************
// chipselect fuer die Statusregister (ohne Modemsignale)
wire					rd_day;			// Lesen
wire					rd_month;		// Lesen
wire					rd_year;		// Lesen
wire					rd_hour;		// Lesen
wire					rd_minute;		// Lesen
wire					rd_weekday;		// Lesen
wire					rd_start_bits_19_1;	// Lesen
wire					wr_control;		// Schreiben

wire					select;
wire		[ALL_PORTS-1:0]		reg_select;
wire		[ALL_PORTS-1:0]		reg_read;
wire		[ALL_PORTS-1:0]		reg_write;

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

assign  wr_control		= reg_write[CONTROL_ADR];		// Schreiben
assign  rd_day			= reg_read[DAY_ADR];			// Lesen
assign  rd_month		= reg_read[MONTH_ADR];			// Lesen
assign  rd_year			= reg_read[YEAR_ADR];			// Lesen
assign  rd_hour			= reg_read[HOUR_ADR];			// Lesen
assign  rd_minute		= reg_read[MINUTE_ADR];			// Lesen
assign  rd_weekday		= reg_read[WEEKDAY_ADR];		// Lesen
assign  rd_start_bits_19_1	= reg_read[STARTBITS_ADR];		// Lesen

// Dekodierung der Adressen ********************** END **************************************
  
  //Config register/wires
  reg sw_reset;
  reg enable_intr;
  wire dcf77_reset_enable;
  wire [58:0] dcf_bits;
  wire dcf_signal_valid;
  wire dcf_new_sec;

  reg [18:0] start_bits_19_1;
  reg [3:0] minute_1;
  reg [2:0] minute_10;
  reg [3:0] hour_1;
  reg [1:0] hour_10;
  reg [3:0] day_1;
  reg [1:0] day_10;
  reg [2:0] weekDay;
  reg [3:0] month_1;
  reg month_10;
  reg [3:0] year_1;
  reg [3:0] year_10;

  dcf77_encoder #(.CLOCK_FREQUENCY(CLOCK_FREQUENCY)) dcf_enc ( 	
        .clk(clk_peri),
        .reset(dcf77_reset_enable),
        .dcf77_non_inverted(dcf77_non_inverted),
	.dcf_sec(dcf_new_sec),
	.dcf_outputbits(dcf_bits)
  ); 

  dcf77_validy_checker #(.CLOCK_FREQUENCY(CLOCK_FREQUENCY)) dcf_valid ( 	
        .clk(clk_peri),
        .reset(dcf77_reset_enable),
        .dcf_bits(dcf_bits),
	.dcf_new_sec(dcf_new_sec),
	.signal_valid(dcf_signal_valid)
  );  

  assign dcf77_reset_enable = reset || sw_reset;
  assign signal_valid = (enable_intr) ? dcf_signal_valid : 1'b0;

  always@(posedge clk_peri or posedge reset) begin
	if(reset) begin
		minute_1 <= 4'b0;
		minute_10 <= 3'b0;
		hour_1 <= 4'b0;
		hour_10 <= 2'b0;
		day_1 <= 4'b0;
		day_10 <= 2'b0;
		weekDay <= 3'b0;
		month_1 <= 4'b0;
		month_10 <= 1'b0;
		year_1 <= 4'b0;	
		year_10 <= 4'b0;
		start_bits_19_1 <= 19'b0;
	end else begin
		if(dcf_signal_valid) begin
			minute_1 <= dcf_bits[24:21];
			minute_10 <= dcf_bits[27:25];
			hour_1 <= dcf_bits[32:29];
			hour_10 <= dcf_bits[34:33];
			day_1 <= dcf_bits[39:36];
			day_10 <= dcf_bits[41:40];
			weekDay <= dcf_bits[44:42];
			month_1 <= dcf_bits[48:45];
			month_10 <= dcf_bits[49];
			year_1 <= dcf_bits[53:50];
			year_10 <= dcf_bits[57:54];
			start_bits_19_1 <= dcf_bits[19:1];
		end
	end
  end

  //SpartanMC peripheral interface logic
  //CONFIG Data Input from SpartanMC
  always @(posedge clk_peri or posedge reset) begin
    if (reset) begin
        sw_reset <= 1'b0;
	enable_intr <= 1'b1;
    end else begin
      if (wr_control) begin
        sw_reset <= do_peri[0];
	enable_intr <= do_peri[1];
      end
    end
  end

  //Send values to spartan MC
  assign di_peri = (rd_minute) ? {7'b0, minute_10, 4'b0, minute_1}  : 
		   (rd_hour) ? {8'b0, hour_10, 4'b0, hour_1} : 
		   (rd_day) ? {8'b0, day_10, 4'b0, day_1} : 
		   (rd_month) ? {9'b0, month_10, 4'b0, month_1} : 
		   (rd_year) ? {6'b0, year_10, 4'b0, year_1} : 
		   (rd_weekday) ? {start_bits_19_1[18], 14'b0, weekDay} : 
		   (rd_start_bits_19_1) ? {start_bits_19_1[17:0]} : 
		   18'b0;  


endmodule