	component system is
		port (
			clk_clk           : in  std_logic := 'X'; -- clk
			reset_reset_n     : in  std_logic := 'X'; -- reset_n
			i2c_serial_sda_in : in  std_logic := 'X'; -- sda_in
			i2c_serial_scl_in : in  std_logic := 'X'; -- scl_in
			i2c_serial_sda_oe : out std_logic;        -- sda_oe
			i2c_serial_scl_oe : out std_logic         -- scl_oe
		);
	end component system;

	u0 : component system
		port map (
			clk_clk           => CONNECTED_TO_clk_clk,           --        clk.clk
			reset_reset_n     => CONNECTED_TO_reset_reset_n,     --      reset.reset_n
			i2c_serial_sda_in => CONNECTED_TO_i2c_serial_sda_in, -- i2c_serial.sda_in
			i2c_serial_scl_in => CONNECTED_TO_i2c_serial_scl_in, --           .scl_in
			i2c_serial_sda_oe => CONNECTED_TO_i2c_serial_sda_oe, --           .sda_oe
			i2c_serial_scl_oe => CONNECTED_TO_i2c_serial_scl_oe  --           .scl_oe
		);

