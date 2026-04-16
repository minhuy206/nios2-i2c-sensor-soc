module final_project (
    input  wire        CLOCK_50,
    input  wire [0:0]  KEY,

    inout  wire        i2c_sda,
    inout  wire        i2c_scl
);

//====================================================
// Internal wires
//====================================================
wire sda_in;
wire scl_in;
wire sda_oe;
wire scl_oe;

//====================================================
// Nios II System instance
//====================================================
system NIOS_system (
    .clk_clk              (CLOCK_50),
    .reset_reset_n        (KEY[0]),

    .i2c_serial_sda_in    (sda_in),
    .i2c_serial_scl_in    (scl_in),
    .i2c_serial_sda_oe    (sda_oe),
    .i2c_serial_scl_oe    (scl_oe)
);

//====================================================
// I2C open-drain implementation
//====================================================

// The Avalon I2C core drives *_oe high when the line should be pulled low.
assign i2c_sda = (sda_oe == 1'b1) ? 1'b0 : 1'bz;
assign sda_in  = i2c_sda;

// Same open-drain convention for SCL.
assign i2c_scl = (scl_oe == 1'b1) ? 1'b0 : 1'bz;
assign scl_in  = i2c_scl;

endmodule 
