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

        // BlockRAM interface
        input                   mem_clk,        //BRAM clk
        input                   mem_access,
        input                   store_access,
        input                   store_access_low,
	input                   store_access_high,
        input            [7:0]  addr_high,

        //*** io interface ***
        input wire sd_miso,
        output wire sd_mosi,
        output wire sd_clk,
        output wire sd_cs,
        
        output wire ts_dummy
);

  parameter BASE_ADR = 10'h0;
  parameter DMA_BASE_ADR = 18'h19c00;     //1024 addresses below IO ports
  parameter CLOCK_FREQUENCY = 16000000;
  
  assign ts_dummy = 1'b1;
  
  wire [17:0] di_peri_dma;

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

  //delay read signal by one cycle (for new pipeline), data is already generated in this cycle by sdcard module
  reg reg_read;
  always @(posedge clk_peri) begin
    reg_read <= select & !wr_peri;
  end
  
  wire [17:0] sd_dat_out;
  
  reg [17:0] sd_dat_out_last;
  
  wire [17:0] sd_dat_out_spmc;
  assign sd_dat_out_spmc = sd_dat_out_last;
  assign di_peri = (reg_read ? sd_dat_out_spmc : 18'b0) | di_peri_dma;
  
  always @(posedge clk_peri) begin
    sd_dat_out_last <= sd_dat_out;
  end
  
  spiMaster sdcard(
    .clk_i(clk_peri),
    .rst_i(reset),
    .address_i(addr_peri[2:0]),
    .data_i(do_peri[17:0]),
    .data_o(sd_dat_out),
    .select(select),
    .we_i(wr_peri),
    
    // SPI logic clock
    .spiSysClk(clk_peri),

    //SPI bus
    .spiClkOut(sd_clk),
    .spiDataIn(sd_miso),
    .spiDataOut(sd_mosi),
    .spiCS_n(sd_cs),
    
    .do_peri(do_peri),
    .di_peri(di_peri_dma),
    .addr_peri(addr_peri),
    .mem_clk(mem_clk),
    .mem_access(mem_access),
    .store_access(store_access | store_access_low),
    .addr_high(addr_high)
  );
  
  defparam sdcard.SDCARD_CLOCK = CLOCK_FREQUENCY;
  defparam sdcard.u_readWriteSDBlock.SDCARD_CLOCK = CLOCK_FREQUENCY;
  defparam sdcard.u_spiMasterSpmcBI.SDCARD_CLOCK = CLOCK_FREQUENCY;
  defparam sdcard.DMA_ADR = DMA_BASE_ADR;
  
endmodule
