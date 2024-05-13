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

architecture pipeline of neuron is

    begin
          
        process(clk_i, rst_i)
                variable result_s : unsigned((DATASIZE*2)-1 downto 0) := (others => '0');
                variable mult_s : unsigned((DATASIZE*2)-1 downto 0) := (others => '0');
                variable all_ready :  natural range 0 to NBINPUTS := 0;
                variable i : natural range 0 to NBINPUTS := 0;

            begin
                if rst_i = '1' then
    
                    for i in 0 to NBINPUTS-1 loop
                        ready_o(i) <= '0';
                    end loop;
                    valid_o <= '0';
                    result_s := (others => '0');
                    mult_s := (others => '0');
                    i :=0;
                    all_ready := 0;
    
                elsif rising_edge(clk_i) then
                    if(valid_o = '1' and ready_i = '1')then
                        valid_o <= '0';
                        result_s := (others => '0');
                        all_ready := 0;
                    end if;
    
                    if (valid_i(i) = '1' and valid_o /= '1') then
                        mult_s := unsigned(inputs_i(i)) * unsigned(weights_i(i));
                        result_s := result_s + mult_s;
                        ready_o(i) <= '1'; 
                        if all_ready /= NBINPUTS-1 then
                            all_ready := all_ready + 1; 
                        end if;
                        
                    else
                        ready_o(i) <= '0';
                    end if;
    
                    if (all_ready = NBINPUTS-1) then
                        result_o <= std_logic_vector(result_s(DATASIZE + COMMA_POS-1 downto COMMA_POS));
                        valid_o <= '1';
                    end if;
    
    
                    if (i = NBINPUTS-1) then
                        i := 0; 
                        else
                        i := i+1;
                    end if;
    
                end if;
    
            end process;

/*
# +--------------------+
# | FINAL REPORT       |
# |--------------------+
# | Nb warnings = 0
# | Nb errors   = 1
# |
# | Verbosity level is : note
# |
# | END OF SIMULATION
#    Time: 100 us  Iteration: 0  Instance: /neuron_tb/monitor
# +--------------------+
*/
end pipeline;
