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

    type mult_ss_t is array (0 to NBINPUTS-1) of std_logic_vector((DATASIZE*2)-1 downto 0);
    signal mult_ss : mult_ss_t;
    begin
        
        process(clk_i, rst_i)
        variable result_s : unsigned((DATASIZE*2)-1 downto 0) := (others => '0');
        variable j : natural range 0 to NBINPUTS := 0;
        variable sum_ready : natural range 0 to NBINPUTS := 0;
        begin
            if rst_i = '1' then
                result_s := (others => '0');
                j := 0;
                sum_ready := 0;
            elsif rising_edge(clk_i) then

                if(valid_o = '1' and ready_i = '1')then
                    result_s := (others => '0');
                end if;

                if (valid_i(j) = '1' and ready_o(j) = '1') then
                    result_s := result_s + unsigned(mult_ss(j));
                    if sum_ready /= NBINPUTS-1 then
                        sum_ready := sum_ready + 1; 
                    end if;
                end if;
                
                if (sum_ready = NBINPUTS-1) then
                    result_o <= std_logic_vector(result_s(DATASIZE + COMMA_POS-1 downto COMMA_POS));
                    valid_o <= '1';
                    sum_ready := 0;
                else 
                    valid_o <= '0';
                end if;

                if (j = NBINPUTS-1) then
                    j := 0; 
                    else
                    j := j+1;
                end if;

            end if;
        end process;
          
        process(clk_i, rst_i)
               
                variable mult_s : unsigned((DATASIZE*2)-1 downto 0) := (others => '0');
                variable i : natural range 0 to NBINPUTS := 0;

            begin
                if rst_i = '1' then
    
                    for i in 0 to NBINPUTS-1 loop
                        ready_o(i) <= '0';
                    end loop;
                    mult_s := (others => '0');
                    i :=0;
    
                elsif rising_edge(clk_i) then
                   
    
                    if (valid_i(i) = '1' and valid_o /= '1') then
                        mult_ss(i) <= std_logic_vector(unsigned(inputs_i(i)) * unsigned(weights_i(i)));
                        ready_o(i) <= '1';     
                    else
                        ready_o(i) <= '0';
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
