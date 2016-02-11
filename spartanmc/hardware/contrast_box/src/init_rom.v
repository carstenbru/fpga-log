module init_ROM#(
    parameter ROM_WIDTH = 10,
    parameter ROM_DEPTH = 10,
    parameter EXPONENT = 3 ) (
    input [ROM_DEPTH-1:0] addr,

    output wire [ROM_WIDTH-1:0] data_out
);

  reg [ROM_WIDTH-1:0] rom [0:(2**ROM_DEPTH)-1];

  assign data_out = rom[addr];

  integer i;
  initial begin
	for(i = 0; i < (2**ROM_DEPTH); i = i + 1) begin
		rom[i] = (i**EXPONENT)/(2**(ROM_WIDTH*(EXPONENT-1)));
	end
  end

endmodule