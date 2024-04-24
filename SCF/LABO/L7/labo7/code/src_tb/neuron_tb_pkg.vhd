--| Library |-------------------------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
use work.neuron_pkg.all;
--------------------------------------------------------------------------------

--| Package |-------------------------------------------------------------------
package neuron_tb_pkg is

    --| Constants |------------------------------------------------------------
    constant CLK_PERIOD : time := 2 us;
    constant TIMEOUT    : time := 20 * CLK_PERIOD;
    ---------------------------------------------------------------------------

    --| Types |-----------------------------------------------------------------
    type neuron_stimulis_t is record
        inputs  : neuron_input_t;
        weights : neuron_weights_t;
        valid   : std_logic_vector;
        ready   : std_logic;
    end record;

    type neuron_observed_t is record
          result : std_logic_vector;
          valid  : std_logic;
          ready  : std_logic_vector;
    end record;

    type neuron_test_type_t is (
        EACH_CLK,
        PAUSE,
        FOLLOW
    );
    ----------------------------------------------------------------------------

     --| Procedures |------------------------------------------------------------
    procedure cycle_fall(signal clk      : in std_logic;
                                nb_cycle : in integer := 1);

    procedure cycle_rise(signal clk      : in std_logic;
                                nb_cycle : in integer := 1);

    procedure calc_ref(signal   stimulis  : in  neuron_stimulis_t;
                       variable reference : out std_logic_vector;
                       constant COMMA_POS : in  integer);
    ----------------------------------------------------------------------------

end neuron_tb_pkg;
--------------------------------------------------------------------------------


--| Package body |--------------------------------------------------------------
package body neuron_tb_pkg is

    --| Procedures |------------------------------------------------------------
    procedure cycle_fall(signal clk      : in std_logic;
                                nb_cycle : in integer := 1) is
    begin
        for i in 1 to nb_cycle loop
            wait until falling_edge(clk);
        end loop;
    end cycle_fall;

    procedure cycle_rise(signal clk      : in std_logic;
                                nb_cycle : in integer := 1) is
    begin
        for i in 1 to nb_cycle loop
            wait until rising_edge(clk);
        end loop;
    end cycle_rise;

    procedure calc_ref(signal   stimulis  : in  neuron_stimulis_t;
                       variable reference : out std_logic_vector;
                       constant COMMA_POS : in  integer) is
                        constant DATASIZE : integer := reference'length;
        variable product_v : unsigned(2*DATASIZE - 1 downto 0) := (others => '0');
    begin
        product_v := (others => '0');
        reference := (reference'range => '0');
        tab_loop1: for i in 0 to stimulis.inputs'high loop
            product_v := (unsigned(stimulis.inputs(i)) * unsigned(stimulis.weights(i)));

            product_v(product_v'high downto product_v'high-COMMA_POS) := (others => '0');
            product_v(product_v'high-COMMA_POS downto 0) := product_v(product_v'high downto COMMA_POS);

            reference := std_logic_vector(unsigned(reference) + product_v(reference'range));
        end loop;
    end calc_ref;
    ----------------------------------------------------------------------------

end neuron_tb_pkg;
--------------------------------------------------------------------------------