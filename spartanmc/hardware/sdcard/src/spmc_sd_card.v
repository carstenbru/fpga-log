module spmc_sd_card(
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
        input wire sd_miso,
        output wire sd_mosi,
        output wire sd_clk,
        output wire sd_cs,
        
        output wire ts_dummy
);

  parameter BASE_ADR = 10'h0;
  parameter CLOCK_FREQUENCY = 16000000;
  
  assign ts_dummy = 1'b1;

  wire select;
  // Address decoder generates the select sinal out of the upper part of the peripheral address.
  pselect iCSL (
                .addr           (       addr_peri[9:6]  ),
                .activ_peri     (       access_peri     ),
                .select         (       select          )
                );
  defparam iCSL.ADDR_WIDTH  = 4;
  defparam iCSL.BASE_WIDTH  = 4;
  defparam iCSL.BASE_ADDR   = BASE_ADR >> 6;      //BASE_ADR has to be divisible by 64

  //delay read signal by one cycle (for new pipeline), data is already generated in this cycle by sdcard module
  reg reg_read;
  always @(posedge clk_peri) begin
    reg_read <= select & !wr_peri;
  end
  
  wire [17:0] sd_dat_out;
  
  wire wishbone_ack;
  reg [17:0] sd_dat_out_last;
  
  wire [17:0] sd_dat_out_spmc;
  assign sd_dat_out_spmc = wishbone_ack ? sd_dat_out : sd_dat_out_last;
  assign di_peri = reg_read ? sd_dat_out_spmc : 18'b0;
  
  always @(posedge clk_peri) begin
    sd_dat_out_last <= sd_dat_out;
  end
  
  spiMaster sdcard(
    .clk_i(clk_peri),
    .rst_i(reset),
    .address_i(addr_peri[5:0]),
    .data_i(do_peri[17:0]),
    .data_o(sd_dat_out),
    .strobe_i(select),
    .we_i(wr_peri),
    .ack_o(wishbone_ack),

    // SPI logic clock
    .spiSysClk(clk_peri),

    //SPI bus
    .spiClkOut(sd_clk),
    .spiDataIn(sd_miso),
    .spiDataOut(sd_mosi),
    .spiCS_n(sd_cs)
  );
  
  defparam sdcard.SDCARD_CLOCK = CLOCK_FREQUENCY;
  defparam sdcard.u_readWriteSDBlock.SDCARD_CLOCK = CLOCK_FREQUENCY;
  defparam sdcard.u_ctrlStsRegBI.SDCARD_CLOCK = CLOCK_FREQUENCY;
  
endmodule
