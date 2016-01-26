/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Projekt : FGLT TU Darmstadt
// Dateiname : spmc_contrast_box.v
// Autor : Stefan Klir
// Modul : spmc_contrast_box
//
// Beschreibung : Contrast_box ist ein Hardwaremodul zum erzeugen einer 
//		  automatisch hellerwerdenden LED, welche durch einen Taster dunkler werden kann.
//                Sollte anstatt eines VCC Schaltenden Schalters verwendet werden
//		  muss dies in der Debounce.v datei geändert werden.
////////////////////////////////////////////////////////////////////////////////////////////////////////

module spmc_contrast_box #(parameter BASE_ADR = 10'h0,
                             parameter CLOCK_FREQUENCY = 16000000,
                             parameter NUMBER_OF_BOXES = 2,
			     parameter PWM_FREQ = 1000,
                             parameter INCREASE_VALUE_AUTOMATIC = 55,
                             parameter TIME_TO_INCREASE_AUTOMATIC = 11'd1000,
                             parameter INCREASE_VALUE_SWITCH = 11,
                             parameter TIME_TO_INCREASE_SWITCH = 50,
                             parameter DECREASE_VALUE_SWITCH = 11) ( 
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
        input wire [NUMBER_OF_BOXES-1:0] switch_up,
        input wire [NUMBER_OF_BOXES-1:0] switch_down,

        output wire [NUMBER_OF_BOXES-1:0] LED_PWM,
	output wire pwm_value_changed
);  

  parameter PWM_REG_WIDTH = 10;
  parameter PWM_CYCLE = 1023;	//max 2^PWM_REG_WIDTH-1
  parameter CONTROL_ADR = 0; //control register
  parameter PWM_ON_VALUE_START_ADR = 1; //PWM_ON_VALUE_0 REGISTER, ....

  parameter ALL_PORTS = 9;		//NUMBER_OF_BOXES + 1; reserviert den platz im speicher für maximal 8 boxen
  parameter DECODSIM_DEB = "NO";


// Dekodierung der Adressen ********************** START *************************************
// chipselect fuer die Statusregister (ohne Modemsignale)
wire		[NUMBER_OF_BOXES-1:0]	rd_pwm;			// Lesen
wire					rd_control;		// Lesen
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

genvar k;
generate //generate specified number of contrast box stages
  for (k = 0; k < NUMBER_OF_BOXES; k = k + 1) begin : read_wires_gen_loop
	assign	rd_pwm[k]		= reg_read[PWM_ON_VALUE_START_ADR+k];	// Lesen
  end
endgenerate

assign  rd_control		= reg_read[CONTROL_ADR];			// Lesen
assign	wr_control		= reg_write[CONTROL_ADR];			// Schreiben

// Dekodierung der Adressen ********************** END **************************************
  
  //Config register/wires
  reg sw_reset;
  reg enable;
  wire contrast_reset_enable;
  wire [(PWM_REG_WIDTH*NUMBER_OF_BOXES-1):0] pwm_on_time;
  wire [(PWM_REG_WIDTH*NUMBER_OF_BOXES-1):0] di_peri_single;
  wire [17:0] di_peri_control_single;
  wire [NUMBER_OF_BOXES-1:0] pwm_value_changed_single;

  genvar i;
  generate //generate specified number of contrast box stages
    for (i = 0; i < NUMBER_OF_BOXES; i = i + 1) begin : output_stage_gen_loop
      contrast_box_in_out #(.CLOCK_FREQUENCY(CLOCK_FREQUENCY),
			    .PWM_FREQ(PWM_FREQ),
			    .INCREASE_VALUE_AUTOMATIC(INCREASE_VALUE_AUTOMATIC),
			    .TIME_TO_INCREASE_AUTOMATIC(TIME_TO_INCREASE_AUTOMATIC),
			    .INCREASE_VALUE_SWITCH(INCREASE_VALUE_SWITCH),
			    .TIME_TO_INCREASE_SWITCH(TIME_TO_INCREASE_SWITCH),
			    .DECREASE_VALUE_SWITCH(DECREASE_VALUE_SWITCH),
			    .PWM_REG_WIDTH(PWM_REG_WIDTH),
			    .PWM_CYCLE(PWM_CYCLE),
			    .DEBOUNCE_CNTR_WIDTH(17))
	      contrastBox (.clk(clk_peri), 
		           .reset(contrast_reset_enable), 
			   .switch_up(switch_up[i]),
			   .switch_down(switch_down[i]), 
			   .pwm(LED_PWM[i]),
			   .pwm_on_value_changed(pwm_value_changed_single[i]),
			   .pwm_on_time(pwm_on_time[(PWM_REG_WIDTH-1+PWM_REG_WIDTH*i):PWM_REG_WIDTH*i]));
    end
  endgenerate

  assign contrast_reset_enable = reset || sw_reset || ~enable;

  //Generate timestamp if one of the pwm_on_values has changed
  assign pwm_value_changed = |pwm_value_changed_single;

  //SpartanMC peripheral interface logic
  //CONFIG Data Input from SpartanMC
  always @(posedge clk_peri or posedge reset) begin
    if (reset) begin
      sw_reset <= 1'b0;
      enable <= 1'b0;
    end else begin
      if (wr_control) begin
	enable <= do_peri[0];
        sw_reset <= do_peri[1];
      end
    end
  end

  //Send PWM_ON_TIME values to spartan MC
  //delay read signals by one cycle (for new pipeline)

  genvar l;
  generate //generate specified number of contrast box stages
    for (l = NUMBER_OF_BOXES; l > 0; l = l - 1) begin : data_out_gen_loop
	if(l == NUMBER_OF_BOXES) assign di_peri_single[(PWM_REG_WIDTH-1+PWM_REG_WIDTH*(l-1)):PWM_REG_WIDTH*(l-1)] = (rd_pwm[l-1]) ? {8'b0, pwm_on_time[(PWM_REG_WIDTH-1+PWM_REG_WIDTH*(l-1)):PWM_REG_WIDTH*(l-1)]} : 10'b0;
	else assign di_peri_single[(PWM_REG_WIDTH-1+PWM_REG_WIDTH*(l-1)):PWM_REG_WIDTH*(l-1)] = (rd_pwm[l-1]) ? {8'b0, pwm_on_time[(PWM_REG_WIDTH-1+PWM_REG_WIDTH*(l-1)):PWM_REG_WIDTH*(l-1)]} : di_peri_single[(PWM_REG_WIDTH-1+PWM_REG_WIDTH*(l)):PWM_REG_WIDTH*(l)];
    end
  endgenerate

  //di_peri_single[9:0] saves the hole if else tree
  assign di_peri_control_single = (rd_control) ?  {NUMBER_OF_BOXES, 15'b0} : {8'b0 ,di_peri_single[9:0]};  

  assign di_peri = {8'b0 ,di_peri_control_single};


endmodule