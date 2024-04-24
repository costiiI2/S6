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


        input_s : neuron_input_t;
        weights_s : neuron_weights_t;
        next_input_ready : std_logic := '0';
        data_ready : std_logic := '0';
        
    function neuron_compute(input_s : std_logic_vector; weights_s : std_logic_vector; COMMA_POS : integer) return std_logic_vector is
        variable sum : signed(input_s'length-1 downto 0) := (others => '0');
        variable product : signed((input_s'length*2)-1 downto 0);
    begin
        for i in 0 to input_s'length/COMMA_POS-1 loop
            product := signed(input_s(i*COMMA_POS downto (i+1)*COMMA_POS-1)) * signed(weights_s(i*COMMA_POS downto (i+1)*COMMA_POS-1));
            sum := sum + product; 
        end loop;
        return std_logic_vector(sum(input_s'length-1-COMMA_POS downto COMMA_POS));
    end neuron_compute;
        
begin

    save_input: process(clk_i, rst_i)
        begin
            if rst_i = '1' then
                input_s <= (others => '0');
                data_ready <= '0';
            elsif rising_edge(clk_i) then

                if(valid_o = '1' and ready_i = '1')then
                    -- les données ont été consommées on peut recommencer
                    data_ready <= '0';
                end if;

                if(next_input_ready = '1') then
                    for i in 0 to NBINPUTS-1 loop
                        -- on ne sauvegarde que les données valides
                        if valid_i(i) = '1' then
                            input_s(i) <= inputs_i(i);
                        end if;
                    end loop;
                    -- si toutes les données sont prêtes on peut commencer le calcul
                    if(valid_i = (others => '1')) then
                        data_ready <= '1';
                    end if;
                end if;
                
                
            end if;
        end process;

    next_input_ready :process(clk_i, rst_i)
        begin
            if rst_i = '1' then
                next_input_ready <= '0';
            elsif rising_edge(clk_i) then
                if (ready_i = '1'and valid_o = '1') then
                    -- les données ont été consommées on peut les effacer
                    next_input_ready <= '1';
                else
                    -- on attend que les données soient consommées
                    next_input_ready <= '0';
                end if;
            end if;
        end process;

    compute_neuron :process(clk_i, rst_i)
        begin
            if rst_i = '1' then
                result_s <= (others => '0');
                ready_o <= (others => '0');
                valid_o <= '0';
            elsif rising_edge(clk_i) then
                if data_ready = '1' then
                    -- on attend que tous les inputs soient valides
                    result_o <= neuron_compute(input_i, weights_i, COMMA_POS);
                    -- on indique que les données sont traitées
                    ready_o <= (others => '1');
                    -- on indique que les données sont valides
                    valid_o <= '1';
                else
                    -- on indique que les données ne sont pas valides
                    valid_o <= '0';
                    -- on indique que les données ne sont pas traitées
                    ready_o <= (others => '0');
                end if;
            end if;


end sequential;
