library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.neuron_pkg.all;
use work.neuron_synth_pkg.all;

entity neuron_synth is
    port (
        clk_i     : in  std_logic;
        rst_i     : in  std_logic;
        input_i   : in std_logic_vector(DATASIZE_SYNTH - 1 downto 0);
        weight_i   : in std_logic_vector(DATASIZE_SYNTH - 1 downto 0);
        valid_i   : in  std_logic_vector(NBINPUTS_SYNTH - 1 downto 0);
        ready_o   : out std_logic_vector(NBINPUTS_SYNTH - 1 downto 0);
        result_o  : out std_logic_vector(DATASIZE_SYNTH - 1 downto 0);
        ready_i   : in  std_logic;
        valid_o   : out std_logic;
        -- Selection of input to allow less IOs
        sel_i     : in  integer range NBINPUTS_SYNTH - 1 downto 0
        );
end neuron_synth;

architecture struct of neuron_synth is

    signal inputs_s: neuron_input_t(NBINPUTS_SYNTH - 1 downto 0)(DATASIZE_SYNTH - 1 downto 0);
    signal weights_s: neuron_weights_t(NBINPUTS_SYNTH - 1 downto 0)(DATASIZE_SYNTH - 1 downto 0);
    signal valid_in_s: std_logic_vector(NBINPUTS_SYNTH - 1 downto 0);
    signal ready: std_logic_vector(NBINPUTS_SYNTH - 1 downto 0);

    signal result_s    : std_logic_vector(DATASIZE_SYNTH - 1 downto 0);
    signal valid_out_s : std_logic;
    signal ready_out_s : std_logic_vector(NBINPUTS_SYNTH - 1 downto 0);

    -- Sync registers
    -- They are used simply to ease the evaluation of FMax
    signal inputs_sync_s   : neuron_input_t(NBINPUTS_SYNTH - 1 downto 0)(DATASIZE_SYNTH - 1 downto 0);
    signal weights_sync_s  : neuron_weights_t(NBINPUTS_SYNTH - 1 downto 0)(DATASIZE_SYNTH - 1 downto 0);
    signal valid_in_sync_s : std_logic_vector(NBINPUTS_SYNTH - 1 downto 0);
    signal ready_in_sync_s : std_logic;

    signal result_sync_s    : std_logic_vector(DATASIZE_SYNTH - 1 downto 0);
    signal valid_out_sync_s : std_logic;
    signal ready_out_sync_s : std_logic_vector(NBINPUTS_SYNTH - 1 downto 0);

begin

    real_neuron : entity work.neuron(sequential)
    port map (
        clk_i => clk_i,
        rst_i => rst_i,
        inputs_i => inputs_sync_s,
        weights_i => weights_sync_s,
        valid_i => valid_in_sync_s,
        ready_o => ready,
        result_o => result_s,
        ready_i => ready_in_sync_s,
        valid_o => valid_out_s
    );

    -- Input synchronisation reg
    process(clk_i, rst_i) is
    begin
        if(rst_i = '1') then
            inputs_sync_s   <= (others => (others => '0'));
            weights_sync_s  <= (others => (others => '0'));
            valid_in_sync_s <= (others => '0');
            ready_in_sync_s <= '0';
        elsif(rising_edge(clk_i)) then
            -- This trick enables to fit a quite huge neuron within an FPGA
            inputs_sync_s(sel_i) <= input_i;
            weights_sync_s(sel_i) <= weight_i;

            valid_in_sync_s <= valid_in_s;
            ready_in_sync_s <= ready_i;
        end if;
    end process;

    -- Output synchronisation reg
    process(clk_i, rst_i) is
    begin
        if(rst_i = '1') then
            result_sync_s    <= (others => '0');
            valid_out_sync_s <= '0';
            ready_out_sync_s <= (others => '0');
        elsif(rising_edge(clk_i)) then
            result_sync_s    <= result_s;
            valid_out_sync_s <= valid_out_s;
            ready_out_sync_s <= ready_out_s;
        end if;
    end process;

    process(all) is
    begin
        for i in inputs_s'range loop
            valid_in_s(i) <= valid_i(i);
            ready_out_s(i) <= ready(i);
        end loop;
    end process;

    result_o <= result_sync_s;
    valid_o  <= valid_out_sync_s;
    ready_o  <= ready_out_sync_s;

end struct;