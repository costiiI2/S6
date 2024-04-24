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
    process(inputs_i, weights_i)
        -- on aloue 2 * datasize pour le produit et la somme
        variable sum : unsigned((DATASIZE*2)-1 downto 0) := (others => '0');
        variable product : unsigned((DATASIZE*2)-1 downto 0);
    begin
        for i in 0 to NBINPUTS-1 loop
            product := unsigned(inputs_i(i)) * unsigned(weights_i(i));
            sum := sum + product;
        end loop;
        -- on mets la virgule au bon endroit par rapport a data size et notre calcul
        -- la virgule se trouve a la position COMMA_POS on doit donc tronquer les bits de COMMA_POS taille a droite et a gauche
        result_o <= sum((DATASIZE*2)-1-COMMA_POS downto COMMA_POS);
    end process;

    ready_o <= ready_i;
    valid_o <= valid_i(0);

end combinational;
