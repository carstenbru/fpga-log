module spmc_pwm(
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

  parameter BASE_ADR = 10'h0;
  parameter CHANNELS = 2;  //Number of PWM channels
  parameter PWM_REG_WIDTH = 32; //width of internal registers and counter, determines min frequency, 18-36 bit
  
  parameter CONFIG_ADR = 0;
  parameter DATA_LOW_ADR = 1;
  parameter DATA_HIGH_ADR = 2;
  parameter PWM_SEL_ADR = 3;

  wire select;
  // Address decoder generates the select sinal out of the upper part of the peripheral address.
  pselect iCSL (
                .addr           (       addr_peri[9:2]  ),
                .activ_peri     (       access_peri     ),
                .select         (       select          )
                );
  defparam iCSL.ADDR_WIDTH  = 8;
  defparam iCSL.BASE_WIDTH  = 8;
  defparam iCSL.BASE_ADDR   = BASE_ADR >> 2;      //BASE_ADR have to be divisible by 4
  
  
  
  
  reg [1:0] pwm_adr;
  reg [PWM_REG_WIDTH:0] pwm_data;
  reg [CHANNELS-1:0] pwm_sel;
  
  genvar i;
  generate //generate specified number of PWM stages
    for (i = 0; i < CHANNELS; i = i + 1) begin : output_stage_gen_loop
      pwm_output_stage #(.REG_WIDTH(PWM_REG_WIDTH))
		       pwm(.clk(clk_peri), 
		           .reset(reset), 
			   .sel(pwm_sel[i]), 
			   .adr(pwm_adr), 
			   .data(pwm_data), 
			   .pwm_out(pwm[i]));
    end
  endgenerate
  
  always @(posedge clk_peri) begin
    if (reset) begin
      pwm_adr <= 2'b0;
      pwm_data <= {(PWM_REG_WIDTH){1'b0}};
      pwm_sel <=  {(CHANNELS){1'b0}};
    end else begin
      if (select & wr_peri) begin
        case (addr_peri[2:0])
          CONFIG_ADR:    pwm_adr <= do_peri[2:0];
          DATA_LOW_ADR:  pwm_data[17:0] <= do_peri[17:0];
          DATA_HIGH_ADR: pwm_data[PWM_REG_WIDTH-1:18] <= do_peri[PWM_REG_WIDTH-19:0];
          PWM_SEL_ADR:   pwm_sel[CHANNELS-1:0] <= do_peri[CHANNELS-1:0];
        endcase
      end else begin
        pwm_sel <=  {(CHANNELS){1'b0}};
      end
    end
  end
  
endmodule