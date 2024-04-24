library ieee;
use ieee.std_logic_1164.all;

package neuron_pkg is

    type neuron_input_t is array (integer range <>) of std_logic_vector;

    type neuron_weights_t is array (integer range <>) of std_logic_vector;

end package neuron_pkg;
