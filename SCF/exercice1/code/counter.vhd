library ieee;
use ieee.std_logic_1164.all;

entity counter is
generic (
    SIZE : integer := 8
);
port (
    clk_i       : in  std_logic;
    rst_i       : in  std_logic;
    enable_i    : in  std_logic;
    up_nDown_i  : in  std_logic;
    equal_val_i : in  std_logic_vector(SIZE-1 downto 0);
    equals_o    : out std_logic;
    value_o     : out std_logic_vector(SIZE-1 downto 0);
    nbincr_o    : out std_logic_vector(SIZE-1 downto 0);
    nbdecr_o    : out std_logic_vector(SIZE-1 downto 0)
);
end counter;


architecture rtl of counter is
	variable dec_s : std_logic_vector(SIZE-1 downto 0);
	variable inc_s : std_logic_vector(SIZE-1 downto 0);
	equal_vin_s : std_logic_vector(SIZE-1 downto 0);
	val_s : std_logic_vector(value_o'range);
	equals_s : std_logic;
	
begin
	process(rst_i,clk_i) is
	begin
		if (rst_i = '1' ) then
			val_s = (others => '0');
			equals_s = '0';
			dec_s := (others => '0');
			inc_s := (others => '0');
		elsif rising_edge(clk_i) then
			equal_vin_s = equal_val_i;
		end if;

	end process;	
				
	process(all)
		variable new_val : std_logic_vector(SIZE-1 downto 0);
	begin

		if(enable_i = '1') then
			if(up_nDown_i = '1') then 
			    -- inc
			    new_val := std_logic_vector(unsigned(val_s) + 1);
			    inc_s := std_logic_vector(unsigned(inc_s) + 1);
			elsif(up_nDown_i = '0') then
			    --dec
			    new_val := std_logic_vector(unsigned(val_s) - 1);
			    dec_s := std_logic_vector(unsigned(dec_s) + 1);
			end if;
		end if;

		-- equals
		if(new_val == equal_vin_s)
			equals_s = 1;
		else
			equals_s = 0;
		end if;	

		val_s <= new_val;

	end process ;
		
	value_o = val_s;
	nbincr_o = inc_s;
	nbdecr_o = dec_s;
	equals_o = equals_s;

end
