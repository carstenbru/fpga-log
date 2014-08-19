module clk_div #(parameter DIVISION = 8000) ( 
        input wire                  clk_i,
        output reg                  clk_o
);

parameter COUNTER_WIDTH = 18;//$clog2(DIVISION);

reg [COUNTER_WIDTH-1:0] counter;

always @(posedge clk_i) begin
  if (counter == (DIVISION/2-1)) begin
    clk_o <= ~clk_o;
    counter <= {(COUNTER_WIDTH){1'b0}};
  end else begin
    counter <= counter + 1;
  end
end

  
endmodule