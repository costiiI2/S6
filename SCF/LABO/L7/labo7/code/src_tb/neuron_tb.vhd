-------------------------------------------------------------------------------
-- HEIG-VD, Haute Ecole d'Ingenierie et de Gestion du canton de Vaud
-- Institut REDS, Reconfigurable & Embedded Digital Systems
--
-- Fichier      : neuron_tb.vhd
--
-- Description  : Testbench for a neuron
--
-- Auteur       : L. Fournier
-- Date         : 10.04.2024
-- Version      : 1.0
--
-- Utilisé dans : Laboratoire de  SCF
--
--| Modifications |------------------------------------------------------------
-- Version   Auteur      Date               Description
-- 1.0       LFR         10.04.2024         First version.
-------------------------------------------------------------------------------

--| Librarys |-----------------------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
use work.project_logger_pkg.all;
use work.neuron_pkg.all;
use work.neuron_tb_pkg.all;
use work.test_neuron_pkg.all;
library osvvm;
    context osvvm.OsvvmContext;
    use osvvm.TbUtilPkg.all;
    use osvvm.AlertLogPkg.all;
    use osvvm.TranscriptPkg.all;
    use osvvm.ReportPkg.all;
    use osvvm.RandomPkg.all;
    use osvvm.CoveragePkg.all;

library tlmvm;
    context tlmvm.tlmvm_context;

-------------------------------------------------------------------------------

--| Entity |-------------------------------------------------------------------
entity neuron_tb is
    generic (
        DATASIZE  : integer := 8;
        NBINPUTS  : integer := 16;
        DUV       : integer := 0;
        TESTCASE  : integer := 0;
        COMMA_POS : integer := 0
    );
end neuron_tb;
-------------------------------------------------------------------------------

--| Architecture |-------------------------------------------------------------
architecture testbench of neuron_tb is

    --| Signals |--------------------------------------------------------------
    -- Tests
    signal clk_sti       : std_logic;
    signal rst_sti       : std_logic;
    signal stimulis_sti  : neuron_stimulis_t(inputs(NBINPUTS - 1 downto 0)(DATASIZE - 1 downto 0),
                                             weights(NBINPUTS - 1 downto 0)(DATASIZE - 1 downto 0),
                                             valid(NBINPUTS - 1 downto 0));
    signal observed_obs  : neuron_observed_t(ready(NBINPUTS - 1 downto 0),
                                             result(DATASIZE - 1 downto 0));
    signal reference_ref : std_logic_vector(DATASIZE-1 downto 0);
    -- Simulation
    signal sim_end_s : boolean := false;
    ---------------------------------------------------------------------------

    --| Components |-----------------------------------------------------------
    -- DUT
    component neuron
        generic(
            COMMA_POS : integer := 0
        );
        port(
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
    end component neuron;
    ---------------------------------------------------------------------------


    -- Create a specialized FIFO package with the same data width for holding
    -- the results.
    -- Very importan: Use the specialized subtypes, not the generic ones
    package result_transaction_pkg is new tlm_unbounded_fifo_pkg
        generic map (element_type => std_logic_vector(DATASIZE - 1 downto 0));
    
    shared variable result_fifo : result_transaction_pkg.tlm_fifo_type;


    procedure final_report(finish_status: finish_status_t) is
    begin
        if finish_status = NO_BEAT then
            logger.log_error("No more heart beat. The system seems down");
        end if;
        logger.final_report;
    end final_report;

begin

    --| Components instanciation |---------------------------------------------
    -- Use the dut with the specified architecture
    DUT_GEN :
        if(DUV = 0) generate
            DUT_INST : entity work.neuron(combinational)
            generic map(
                COMMA_POS => COMMA_POS
            )
            port map(
                clk_i     => clk_sti,
                rst_i     => rst_sti,
                inputs_i  => stimulis_sti.inputs,
                weights_i => stimulis_sti.weights,
                valid_i   => stimulis_sti.valid,
                ready_o   => observed_obs.ready,
                result_o  => observed_obs.result,
                ready_i   => stimulis_sti.ready,
                valid_o   => observed_obs.valid
            );
        elsif(DUV = 1) generate
            DUT_INST : entity work.neuron(sequential)
            generic map(
                COMMA_POS => COMMA_POS
            )
            port map(
                clk_i     => clk_sti,
                rst_i     => rst_sti,
                inputs_i  => stimulis_sti.inputs,
                weights_i => stimulis_sti.weights,
                valid_i   => stimulis_sti.valid,
                ready_o   => observed_obs.ready,
                result_o  => observed_obs.result,
                ready_i   => stimulis_sti.ready,
                valid_o   => observed_obs.valid
            );
        elsif(DUV = 2) generate
            DUT_INST : entity work.neuron(pipeline)
            generic map(
                COMMA_POS => COMMA_POS
            )
            port map(
                clk_i     => clk_sti,
                rst_i     => rst_sti,
                inputs_i  => stimulis_sti.inputs,
                weights_i => stimulis_sti.weights,
                valid_i   => stimulis_sti.valid,
                ready_o   => observed_obs.ready,
                result_o  => observed_obs.result,
                ready_i   => stimulis_sti.ready,
                valid_o   => observed_obs.valid
            );
    end generate DUT_GEN;
    ---------------------------------------------------------------------------


	monitor: simulation_monitor
	generic map (drain_time => 50000 ns,beat_time => 50000 ns,final_reporting => final_report);

	clk_proc : clock_generator(clk_sti, CLK_PERIOD);


    --| Simulation process |---------------------------------------------------
    stimulis_proc : process is

        --| Reset sequence |---------------------------------------------------
        procedure reset_seq(signal reset     : out std_logic;
                            signal stimulis  : out neuron_stimulis_t) is
        begin
            reset            <= '1';
            for i in stimulis.inputs'range loop
                stimulis.inputs(i) <= (stimulis.inputs(i)'range => '0');
            end loop;
            for i in stimulis.weights'range loop
                stimulis.weights(i) <= (stimulis.weights(i)'range => '0');
            end loop;
            stimulis.valid   <= (stimulis.valid'range => '0');
            stimulis.ready   <= '0';
            cycle_fall(clk_sti, 1);
            reset            <= '0';
            cycle_fall(clk_sti, 1);
        end reset_seq;
        -----------------------------------------------------------------------

    begin

        -- User notification
        logger.log_note(
            "" & CR &
            ">> Start of simulation"
        );

        -- Reset system at the beginning
        reset_seq(rst_sti, stimulis_sti);

        case TESTCASE is
            when 0 => -- run all tests
                test_random(clk_sti, observed_obs, stimulis_sti, EACH_CLK);
                test_random(clk_sti, observed_obs, stimulis_sti, PAUSE);
                test_random(clk_sti, observed_obs, stimulis_sti, FOLLOW);
            when 1 =>
                test_random(clk_sti, observed_obs, stimulis_sti, EACH_CLK);
            when 2 =>
                test_random(clk_sti, observed_obs, stimulis_sti, PAUSE);
            when 3 =>
                test_random(clk_sti, observed_obs, stimulis_sti, FOLLOW);
            when others =>
                null;
        end case;

        wait;
    end process stimulis_proc;
    ---------------------------------------------------------------------------

    --| verif process |--------------------------------------------------------
    verif_proc : process is
        variable result_v : std_logic_vector(DATASIZE - 1 downto 0);
    begin

        -- Check the reference with the observed result on each valid result
        loop
            cycle_rise(clk_sti, 1);

            result_transaction_pkg.blocking_get(result_fifo, result_v);

            while (observed_obs.valid = '0') or stimulis_sti.ready = '0' loop
                cycle_rise(clk_sti, 1);
            end loop;

                if (observed_obs.result /= result_v) then
                logger.log_error(
                    ""                                                      & CR &
                    ">> Result observe = " & to_string(observed_obs.result) & CR &
                    ">> Result attendu = " & to_string(result_v)
                );
            end if;
        end loop;
        wait;
    end process verif_proc;
    ---------------------------------------------------------------------------

    --| ref process |----------------------------------------------------------
    ref_proc : process is
        variable reference_v : std_logic_vector(DATASIZE-1 downto 0) := (others => '0');
        variable starters_v : std_logic_vector(NBINPUTS - 1 downto 0);
        constant all_ones : std_logic_vector(NBINPUTS - 1 downto 0) := (others => '1');
    begin
        -- init the ref to 0 at start
        reference_ref <= reference_v;

        -- Each time all the inputs are valid, calculate ref
        loop

            starters_v := (others => '0');

            while (starters_v /= all_ones) and not(sim_end_s) loop
                -- check the valid on rising edge
                cycle_rise(clk_sti, 1);
                starters_v := starters_v or (stimulis_sti.valid and observed_obs.ready);
            end loop;

            --if(signed(stimulis_sti.valid) = to_signed(-1, stimulis_sti.valid'length)) then
            if (starters_v = all_ones) then
                calc_ref(stimulis_sti, reference_v, COMMA_POS);
                reference_ref <= reference_v;
                result_transaction_pkg.blocking_put(result_fifo, reference_v);
            end if;

            -- update the ref on falling edge
            cycle_fall(clk_sti, 1);
        end loop;
        wait;
    end process ref_proc;
    ---------------------------------------------------------------------------

end testbench;
-------------------------------------------------------------------------------
