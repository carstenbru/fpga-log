module motor_dummy_control #(parameter BASE_ADR = 10'h0,
                             parameter CLOCK_FREQUENCY = 16000000,
                             parameter MAX_SPEED = 100,
                             parameter PWM_FREQ = 100,
                             parameter SOFT_START_LENGTH = 5000,
                             parameter SOFT_START_EXPONENTIAL = "FALSE",
                             parameter END_SW_DETECTION_LENGTH = 128) ( 
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
        output reg active,
        output reg direction,
        input wire end_sw_down,
        input wire end_sw_up,
        
        output reg dest_reached
);  

  parameter PWM_REG_WIDTH = 32;

  wire select;
  // Address decoder generates the select sinal out of the upper part of the peripheral address.
  pselect iCSL (
                .addr           (       addr_peri[9:0]  ),
                .activ_peri     (       access_peri     ),
                .select         (       select          )
                );
  defparam iCSL.ADDR_WIDTH  = 10;
  defparam iCSL.BASE_WIDTH  = 10;
  defparam iCSL.BASE_ADDR   = BASE_ADR;  
  
  //control registers
  reg destination;
  wire active_int;
  
  wire end_sw_down_wire;
  wire end_sw_up_wire;
  
  reg [END_SW_DETECTION_LENGTH-1:0] end_sw_down_regs;
  reg [END_SW_DETECTION_LENGTH-1:0] end_sw_up_regs;
  
  reg [PWM_REG_WIDTH-1:0] pwm_counter;
  reg [PWM_REG_WIDTH-1:0] pwm_on_time;
  wire pwm;
  
  assign pwm = (pwm_counter < pwm_on_time);
  
  //delay read signals by one cycle (for new pipeline)
  reg read_access;
  always @(posedge clk_peri) begin
    read_access <= (select & !wr_peri);
  end
  assign di_peri = read_access ? {16'd0, direction, active_int} : 18'b0;
  
  assign active_int = (destination ? end_sw_up_wire : end_sw_down_wire) && !dest_reached;
  
  always @(posedge clk_peri) begin
    direction <= destination;
    active <= (active_int & pwm) ^ direction;
  end
  
  //SpartanMC peripheral interface logic
  always @(posedge clk_peri) begin
    if (reset) begin
      destination <= 1'b0;
      dest_reached <= 1'b1;
    end else begin
      if (select & wr_peri) begin
        destination <= do_peri[0];
        dest_reached <= 1'b0;
      end else begin
        dest_reached <= !active_int;
      end
    end
  end
  
  assign end_sw_down_wire = !(end_sw_down_regs == {(END_SW_DETECTION_LENGTH){1'b1}}); //end sw is only pressed when all recorded values in shift register are 1
  assign end_sw_up_wire = !(end_sw_up_regs == {(END_SW_DETECTION_LENGTH){1'b1}});
  
  always @(posedge clk_peri) begin
    end_sw_down_regs <= {end_sw_down_regs[END_SW_DETECTION_LENGTH-2:0], end_sw_down}; //shift register, shift end_sw_down in
    end_sw_up_regs <= {end_sw_up_regs[END_SW_DETECTION_LENGTH-2:0], end_sw_up}; //shift register, shift end_sw_up in
  end
  
  parameter PWM_CYCLE = CLOCK_FREQUENCY / PWM_FREQ;
  parameter PWM_MAX_ON_TIME = PWM_CYCLE * MAX_SPEED / 100;
  parameter SOFT_START_INCREASE = PWM_MAX_ON_TIME * 1000 / (SOFT_START_LENGTH * PWM_FREQ);
  
  //PWM logic
  always @(posedge clk_peri) begin
    if (active_int == 0) begin
      pwm_counter <= {(PWM_REG_WIDTH){1'b0}};
      pwm_on_time <= {(PWM_REG_WIDTH){1'b0}};
    end else begin
      if (pwm_counter == PWM_CYCLE) begin
        pwm_counter <= {(PWM_REG_WIDTH){1'b0}};
        if (pwm_on_time < PWM_MAX_ON_TIME) begin //increase on_time if smaller than max value
          if (SOFT_START_EXPONENTIAL == "FALSE") begin
            pwm_on_time <= pwm_on_time + SOFT_START_INCREASE;
          end else begin
            pwm_on_time <= pwm_on_time + 1 + (pwm_on_time >> SOFT_START_LENGTH); //+1 for start phase, otherwise 0+0
          end
        end
      end else begin
        pwm_counter <= pwm_counter + 1;
      end
    end
  end
 
endmodule