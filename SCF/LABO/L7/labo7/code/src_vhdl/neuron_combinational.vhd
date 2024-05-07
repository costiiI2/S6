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

architecture combinational of neuron is

    begin   

    -- calcul combinatoire de la sortie
    process(all)
        -- on aloue 2 * datasize pour la somme
        variable sum : unsigned((DATASIZE*2)-1 downto 0) := (others => '0');
    begin
        sum := (others => '0');

        

            for i in 0 to NBINPUTS-1 loop
                sum := sum + unsigned(inputs_i(i)) * unsigned(weights_i(i));
            end loop;
            -- on mets la virgule au bon endroit par rapport a data size et notre calcul
            -- la virgule se trouve a la position COMMA_POS on doit donc tronquer les bits de COMMA_POS taille a droite et a gauche
            result_o <= std_logic_vector(sum(DATASIZE+COMMA_POS-1 downto COMMA_POS));
      
    end process;

    ready_o <= valid_i;
    valid_o <= ready_i;

end combinational;
