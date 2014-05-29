module spmc_pwm #(
            parameter CHANNELS = 2,        //number of PWM channels, 1 - 1.179.648
            parameter BASE_ADR = 10'h0) ( 
        //*** Connections to SpartanMC Core (do not change) ***
        input wire              clk_peri,       //System-Clock
        input wire      [17:0]  do_peri,        //Data Bus  from MC
        output          [17:0]  di_peri,        //Data Bus  to MC
        input wire      [9:0]   addr_peri,      //Address Bus from MC
        input wire              access_peri,    //Peripheral Access Signal
        input wire              wr_peri,        //Write Enable Signal

        //*** Connections to SpartanMC Core which can be changed ***
        input wire              reset,          //Reset-Signal (could be external)

        //*** io interface ***
        output wire [CHANNELS-1:0] pwm
);
  parameter PWM_REG_WIDTH = 32; //width of internal registers and counter, determines min frequency, 18-36 bit
  
  //register addresses of the module
  parameter CONFIG_ADR = 2'b00;
  parameter DATA_LOW_ADR = 2'b01;
  parameter DATA_HIGH_ADR = 2'b10;
  parameter PWM_SEL_ADR = 2'b11;
  
  //addresses of output stages
  parameter ADR_FREQ = 2'b00;
  parameter ADR_COUNTER = 2'b10;
  
  parameter PWM_BANKS = ((CHANNELS-1) / 18) + 1;
  parameter PWM_SEL_WIDTH = (CHANNELS < 18) ? CHANNELS : 18;
  parameter PWM_BANK_WIDTH = PWM_BANKS < 2  ? 1:
			     PWM_BANKS < 4  ? 2:
			     PWM_BANKS < 8  ? 3:
			     PWM_BANKS < 16 ? 4: 
			     PWM_BANKS < 32  ? 5:
			     PWM_BANKS < 64  ? 6:
			     PWM_BANKS < 128  ? 7:
			     PWM_BANKS < 256  ? 8:
			     PWM_BANKS < 512  ? 9:
			     PWM_BANKS < 1024  ? 10:
			     PWM_BANKS < 2048  ? 11:
			     PWM_BANKS < 4096  ? 12:
			     PWM_BANKS < 8192  ? 13:
			     PWM_BANKS < 16384  ? 14:
			     PWM_BANKS < 32768  ? 15:
			     16;

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
  
  //data register
  reg [PWM_REG_WIDTH:0] data_reg;
  
  //bank register
  reg [PWM_BANK_WIDTH-1:0] pwm_bank;
  
  //output stages signals
  reg [1:0] pwm_adr;
  reg [PWM_REG_WIDTH:0] pwm_data;
  reg [PWM_SEL_WIDTH-1:0] pwm_sel;
  
  //reference signal registers
  reg [PWM_REG_WIDTH-1:0] phase_counter;
  reg [PWM_REG_WIDTH-1:0] phase_cycle;
  
  reg [PWM_REG_WIDTH:0] phase_calc_tmp;
  
  genvar i;
  generate //generate specified number of PWM stages
    for (i = 0; i < CHANNELS; i = i + 1) begin : output_stage_gen_loop
      pwm_output_stage #(.REG_WIDTH(PWM_REG_WIDTH))
		       pwm(.clk(clk_peri), 
		           .reset(reset), 
			   .sel(pwm_sel[i % 18] && (pwm_bank == i/18)), 
			   .adr(pwm_adr), 
			   .data(pwm_data), 
			   .pwm_out(pwm[i]));
    end
  endgenerate
  
  //SpartanMC peripheral interface logic
  always @(posedge clk_peri) begin
    if (reset) begin
      pwm_adr <= 2'b00;
      data_reg <= {(PWM_REG_WIDTH){1'b0}};
      pwm_sel <=  {(PWM_SEL_WIDTH){1'b0}};
    end else begin
      if (select & wr_peri) begin
        case (addr_peri[2:0])
          CONFIG_ADR:    begin
			   pwm_adr <= do_peri[1:0];
			   pwm_bank[PWM_BANK_WIDTH-1:0] <= do_peri[PWM_BANK_WIDTH+1:2];
			 end
          DATA_LOW_ADR:  data_reg[17:0] <= do_peri[17:0];
          DATA_HIGH_ADR: data_reg[PWM_REG_WIDTH-1:18] <= do_peri[PWM_REG_WIDTH-19:0];
          PWM_SEL_ADR:   begin
			   pwm_sel[PWM_SEL_WIDTH-1:0] <= do_peri[PWM_SEL_WIDTH-1:0];
			   if (pwm_adr == ADR_FREQ)
			     phase_cycle <= data_reg;
			 end
        endcase
      end else begin
        pwm_sel <=  {(PWM_SEL_WIDTH){1'b0}}; //reset pwm stage select signals after a write action
      end
    end
  end
  
  //reference phase generation logic
  always @(posedge clk_peri) begin
    if (reset) begin
      phase_counter <= {(PWM_REG_WIDTH){1'b0}}; //reset counter to 0
    end else begin
      if (phase_counter >= phase_cycle)
	phase_counter <= {(PWM_REG_WIDTH){1'b0}}; //reset counter if max value reached
      else
	phase_counter <= phase_counter + 1; //otherwise increment counter
    end
  end
  
  //phase calculation logic
  always @(*) begin
    if (pwm_adr == ADR_COUNTER) begin //when phase is set..
      phase_calc_tmp = data_reg + phase_counter; //add current phase of refrenece signal to desired phase
      if (phase_calc_tmp > phase_cycle) //perform modulo
        pwm_data = phase_calc_tmp - phase_cycle;
      else
         pwm_data = phase_calc_tmp;
    end else //normally just pass data
      pwm_data = data_reg;
  end
  
endmodule