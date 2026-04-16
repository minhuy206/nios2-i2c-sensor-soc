module sensor_status_regs (
    input  wire        iClk,
    input  wire        iReset_n,
    input  wire        iChipSelect_n,
    input  wire        iWrite_n,
    input  wire        iRead_n,
    input  wire [2:0]  iAddress,
    input  wire [31:0] iWriteData,
    output reg  [31:0] oReadData,
    output wire [7:0]  oDebugLeds
);

    reg [31:0] led_reg;
    reg [31:0] app_status_reg;
    reg [31:0] last_sample_reg;
    reg [31:0] sample_count_reg;
    reg [31:0] error_count_reg;

    assign oDebugLeds = led_reg[7:0];

    always @(posedge iClk or negedge iReset_n) begin
        if (~iReset_n) begin
            led_reg          <= 32'd0;
            app_status_reg   <= 32'd0;
            last_sample_reg  <= 32'd0;
            sample_count_reg <= 32'd0;
            error_count_reg  <= 32'd0;
        end else if (~iChipSelect_n && ~iWrite_n) begin
            case (iAddress)
                3'd0: led_reg          <= iWriteData;
                3'd1: app_status_reg   <= iWriteData;
                3'd2: last_sample_reg  <= iWriteData;
                3'd3: sample_count_reg <= iWriteData;
                3'd4: error_count_reg  <= iWriteData;
                default: begin
                    led_reg          <= led_reg;
                    app_status_reg   <= app_status_reg;
                    last_sample_reg  <= last_sample_reg;
                    sample_count_reg <= sample_count_reg;
                    error_count_reg  <= error_count_reg;
                end
            endcase
        end
    end

    always @(*) begin
        oReadData = 32'd0;

        if (~iChipSelect_n && ~iRead_n) begin
            case (iAddress)
                3'd0: oReadData = led_reg;
                3'd1: oReadData = app_status_reg;
                3'd2: oReadData = last_sample_reg;
                3'd3: oReadData = sample_count_reg;
                3'd4: oReadData = error_count_reg;
                default: oReadData = 32'd0;
            endcase
        end
    end

endmodule
