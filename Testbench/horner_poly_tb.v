`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 30.08.2020 04:43:25
// Design Name: 
// Module Name: horner_poly_tb
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module horner_poly_tb();

reg clock, reset, start;

wire [31:0] arg_in;
wire [31:0] CN0;
wire [31:0] CN1;
wire [31:0] CN2;
wire [31:0] CN3;
wire [31:0] CN4;
wire [31:0] CN5;

wire ready_out; //result is ready
wire [31:30] dummy;
wire [31:0] result_out;

//expected result_out = 7 because only x^0 is different than zero and equals 7
assign arg_in = 10;
assign CN0 = 0;
assign CN1 = 0;
assign CN2 = 0;
assign CN3 = 0;
assign CN4 = 0;
assign CN5 = 7;


// Reset stimulus
 initial
 begin
 reset = 1'b1;
 #10 reset = 1'b0;
 end

// Clocks stimulus
 initial
 begin
 clock = 1'b0; //set clk to 0
 end
 always
 begin
 #5 clock = ~clock; //toggle clk every 5 time units
 end
 
 //Start stimulus
initial
  begin
  start = 1'b1; //set start to 1
  end
  always
  begin
  #30 start = ~start; //toggle start every 30 time units
  end

//Instantiate tested module
horner_poly_rtl horner_rtl_inst( clock, reset, start, arg_in, CN0, CN1, CN2, CN3, CN4, CN5, ready_out, dummy, result_out); 

endmodule
