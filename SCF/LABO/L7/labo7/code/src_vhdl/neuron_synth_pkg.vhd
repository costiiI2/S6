library ieee;
use ieee.std_logic_1164.all;

package neuron_synth_pkg is

    -- Change these two constants to test various sizes
    constant DATASIZE_SYNTH : natural := 8;
    constant NBINPUTS_SYNTH : integer := 16;

    type neuron_input_synth_t is array (0 to NBINPUTS_SYNTH - 1) of std_logic_vector(DATASIZE_SYNTH - 1 downto 0);

    type neuron_weights_synth_t is array (0 to NBINPUTS_SYNTH - 1) of std_logic_vector(DATASIZE_SYNTH - 1 downto 0);

end package neuron_synth_pkg;
