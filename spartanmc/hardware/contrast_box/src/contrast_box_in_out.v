module contrast_box_in_out #(parameter CLOCK_FREQUENCY = 16000000,
			     parameter PWM_FREQ = 1000,
			     parameter AUTOMATIC_INCREASE = "FALSE",
                             parameter INCREASE_VALUE = 11,
                             parameter TIME_TO_INCREASE = 50,		//time_to_increase_clk_cnt limits the time: 0ms - 2097 ms 
                             parameter DECREASE_VALUE = 11,
			     parameter PWM_REG_WIDTH = 10,
			     parameter PWM_CYCLE = 1023,		//max 2^PWM_REG_WIDTH-1
			     parameter DEBOUNCE_CNTR_WIDTH = 17) ( 	
        input wire              clk,       	//Clock
        input wire              reset,          //Reset-Signal
	input wire		switch_up,		//Switch to increase the value
	input wire		switch_down,		//Switch to decrease the value

	output reg pwm_on_value_changed,	//1 if the pwm value changed, 0 otherwise
        output wire pwm,
	output reg [PWM_REG_WIDTH-1:0] pwm_on_time
);  

  parameter PWM_CLK_CYCLES = (CLOCK_FREQUENCY/PWM_FREQ)/(PWM_CYCLE+1);
  parameter TIME_TO_INCREASE_CYCLE = CLOCK_FREQUENCY*TIME_TO_INCREASE/1000;
  parameter MAX_PWM_ON_VALUE = PWM_CYCLE - INCREASE_VALUE + 1;

  //control registers
  reg [PWM_REG_WIDTH-1:0] pwm_counter;
  wire switch_down_state;
  wire switch_down_state_continuous;
  wire switch_up_state_continuous;
  wire pwm_clk;
  reg [PWM_REG_WIDTH-1:0] pwm_clk_cnt;	//BIT BREITE MUSS ANGEPASST WERDEN
  reg [24:0] time_to_increase_clk_cnt; 
  reg time_to_increase_clk;

  //Button debounce => active High switch
  //Variate the COUNTER_WIDTH for longer or shorter debounce time
  PushButton_Debouncer #(.COUNTER_WIDTH(DEBOUNCE_CNTR_WIDTH)) debounce_down (
	.clk(clk),
	.PB(switch_down),
	.PB_state(switch_down_state_continuous),
	.PB_down(),
	.PB_up(switch_down_state));

  PushButton_Debouncer #(.COUNTER_WIDTH(DEBOUNCE_CNTR_WIDTH)) debounce_up (
	.clk(clk),
	.PB(switch_up),
	.PB_state(switch_up_state_continuous),
	.PB_down(),
	.PB_up());




  //automatic increase pwm duty cycle
  always @(posedge clk or posedge reset) begin
	if(reset) begin
		time_to_increase_clk_cnt <= {(25){1'b0}};
		time_to_increase_clk <= 1'b0;
	end else begin
		if((AUTOMATIC_INCREASE == "FALSE") && (switch_down_state_continuous ^ switch_up_state_continuous) || (AUTOMATIC_INCREASE == "TRUE")) begin
			//Either a button is pressed or the automatic increase is on

			if(time_to_increase_clk_cnt < TIME_TO_INCREASE_CYCLE) time_to_increase_clk_cnt <= time_to_increase_clk_cnt + 1;
			else time_to_increase_clk_cnt <= {(PWM_REG_WIDTH){1'b0}};
			
			if(time_to_increase_clk_cnt == TIME_TO_INCREASE_CYCLE) time_to_increase_clk <= 1'b1;
			else time_to_increase_clk <= 1'b0;
		end else begin
			time_to_increase_clk_cnt <= {(25){1'b0}};
			time_to_increase_clk <= 1'b0;
		end
	end	
  end

//Button logic
  always @(posedge clk) begin
	if(AUTOMATIC_INCREASE == "FALSE") begin
  		//Continous case
  		//increase or decrease the pwm value if the button is pressed

		if(switch_down_state_continuous ^ switch_up_state_continuous) begin
			if(switch_down_state_continuous) begin
				if(time_to_increase_clk && (pwm_on_time > (DECREASE_VALUE-1))) begin
					pwm_on_time <= pwm_on_time - DECREASE_VALUE;
					pwm_on_value_changed <= 1'b1;
				end else pwm_on_value_changed <= 1'b0;
			end else begin
				if(time_to_increase_clk && (pwm_on_time < MAX_PWM_ON_VALUE)) begin
					pwm_on_time <= pwm_on_time + INCREASE_VALUE;
					pwm_on_value_changed <= 1'b1;
				end else pwm_on_value_changed <= 1'b0;
			end
		end
	end else begin
		//PWM_On_Value calculation with automatic increase and button pressed decrease

		if(switch_down_state) begin
			if(pwm_on_time > (DECREASE_VALUE-1)) begin
				pwm_on_time <= pwm_on_time - DECREASE_VALUE;
				pwm_on_value_changed <= 1'b1;
			end else pwm_on_value_changed <= 1'b0;
		end else begin
			if(time_to_increase_clk && pwm_on_time < MAX_PWM_ON_VALUE) begin
				pwm_on_time <= pwm_on_time + INCREASE_VALUE;
				pwm_on_value_changed <= 1'b1;
			end else pwm_on_value_changed <= 1'b0;
		end
	end
  end


  //PWM logic

  //Generate PWM Clock
  assign pwm_clk = pwm_clk_cnt < (PWM_CLK_CYCLES>>2); 
  always @(posedge clk or posedge reset) begin
	if(reset) begin
		pwm_clk_cnt <= {(PWM_REG_WIDTH){1'b0}}; //reset counter to 0
	end else begin
		if(pwm_clk_cnt < PWM_CLK_CYCLES) pwm_clk_cnt <= pwm_clk_cnt + 1;
		else pwm_clk_cnt <= {(PWM_REG_WIDTH){1'b0}};
	end
  end

  assign pwm = (pwm_counter < pwm_on_time);	
  always @(posedge pwm_clk or posedge reset) begin
    if(reset) begin
	pwm_counter <= {(PWM_REG_WIDTH){1'b0}}; //reset counter to 0
    end else begin
      if (pwm_counter == PWM_CYCLE) begin
        pwm_counter <= {(PWM_REG_WIDTH){1'b0}}; //reset counter if max value reached
      end else begin
        pwm_counter <= pwm_counter + 1;
      end
    end
  end
 
endmodule