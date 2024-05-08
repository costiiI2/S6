library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neuron_pkg.all;

entity neuron is
    generic(
        COMMA_POS : integer := 0
    );
    port (
        clk_i     : in  std_logic;
        rst_i     : in  std_logic;
        inputs_i  : in  neuron_input_t;
        weights_i : in  neuron_weights_t;
        valid_i   : in  std_logic_vector;
        ready_o   : out std_logic_vector;
        result_o  : out std_logic_vector;
        ready_i   : in  std_logic;
        valid_o   : out std_logic
        );

    -- A nice VHDL possibility, to have constants available in each architecture
    constant NBINPUTS : integer := inputs_i'length;
    constant DATASIZE : integer := result_o'length;
end neuron;

architecture sequential of neuron is

    
begin

    process(clk_i, rst_i)
            variable result_s : unsigned((DATASIZE*2)-1 downto 0) := (others => '0');
            variable all_ready : unsigned := 0;
            signal i : std_logic_vector(NBINPUTS-1 downto 0) := (others => '0');
        begin
            if rst_i = '1' then

                for i in 0 to NBINPUTS-1 loop
                    ready_o(i) <= '0';
                end loop;
                valid_o <= '0';
                result_s := (others => '0');
                i <= (others => '0');
                all_ready := 0;

            elsif rising_edge(clk_i) then
            
                
                if valid_i(i) = '1' then
                    result_s := result_s + unsigned(inputs_i(i)) * unsigned(weights_i(i));
                    ready_o(i) <= '1'; 
                    all_ready := all_ready + 1; 
                else
                    ready_o(i) <= '0';
                end if;

                i <= i + 1;

                if (all_ready = NBINPUTS-1) then
                    result_o <= std_logic_vector(result_s(DATASIZE+COMMA_POS-1 downto COMMA_POS));
                    valid_o <= '1';
                end if;

                if(valid_o = '1' and ready_i = '1')then
                    -- les données ont été consommées on peut recommencer
                    valid_o <= '0';
                    result_s := (others => '0');
                    i <= (others => '0');
                    all_ready := 0;
                end if;
                

            end if;
        end process;
end sequential;
