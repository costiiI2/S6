--| Library |-------------------------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
library osvvm;
    context osvvm.OsvvmContext;
    use osvvm.TbUtilPkg.all;
    use osvvm.AlertLogPkg.all;
    use osvvm.TranscriptPkg.all;
    use osvvm.ReportPkg.all;
    use osvvm.RandomPkg.all;
    use osvvm.CoveragePkg.all;
use work.project_logger_pkg.all;
use work.neuron_pkg.all;
use work.neuron_tb_pkg.all;
use work.neuron_bfm_pkg.all;


library tlmvm;
    context tlmvm.tlmvm_context;

--------------------------------------------------------------------------------

--| Package |-------------------------------------------------------------------
package test_neuron_pkg is

    --| Test |------------------------------------------------------------------
    procedure test_random(signal   clk       : in  std_logic;
                          signal   observed  : in  neuron_observed_t;
                          signal   stimulis  : out neuron_stimulis_t;
                          constant TEST_TYPE : in  neuron_test_type_t);
    ----------------------------------------------------------------------------

end test_neuron_pkg;
--------------------------------------------------------------------------------

--| Body |----------------------------------------------------------------------
package body test_neuron_pkg is

    --| Test |------------------------------------------------------------------
    procedure test_random(signal   clk       : in  std_logic;
                          signal   observed  : in  neuron_observed_t;
                          signal   stimulis  : out neuron_stimulis_t;
                          constant TEST_TYPE : in  neuron_test_type_t) is
        
                            constant DATASIZE : integer := observed.result'length;
                            variable stimulis_v : neuron_stimulis_t(inputs(stimulis.inputs'range)(DATASIZE-1 downto 0),
                            weights(stimulis.weights'range)(DATASIZE-1 downto 0),
                            valid(stimulis.valid'range));
        -- We want input and weight random -> 2 value random -> 2 bits vector
        variable rand_v     : integer_vector(1 downto 0);
        variable cov        : CoverageIDType;

        constant NBINPUTS : integer := stimulis.weights'length;
    begin

        -- User notification
        logger.log_note(
            "" & CR &
            ">> Test random " & to_string(TEST_TYPE)
        );

        -- Create coverage
        cov := NewId("Cov1");
        --
        AddCross(cov, GenBin(0, 200), GenBin(0, 25));

        -- Create random value for input and weight and call the bfm to write them, stop when enough is covered
        loop
            raise_objection;
            -- loop trough all the input
            tab_loop1: for i in 0 to NBINPUTS - 1 loop
                -- Get random value for the current input and weight
                rand_v := GetRandPoint(cov);
                stimulis_v.inputs(i)  := std_logic_vector(to_unsigned(rand_v(1), stimulis_v.inputs(i)'length));
                stimulis_v.weights(i) := std_logic_vector(to_unsigned(rand_v(0), stimulis_v.weights(i)'length));
                -- Add the random value to the coverage
                ICover(cov, rand_v);
            end loop;

            -- BFM to write the value in the DUT
            case(TEST_TYPE) is
                when EACH_CLK =>
                    neuron_write_each_clk(stimulis_v, clk, observed, stimulis);
                when PAUSE =>
                    neuron_write_with_pause(stimulis_v, clk, observed, stimulis);
                when FOLLOW =>
                    neuron_write_following(stimulis_v, clk, observed, stimulis);
                when others =>
                    null;
            end case;

            drop_objection;

            -- Exit when enough value have been send according to the coverage
            exit when IsCovered(cov);
        end loop;

    end test_random;
    ----------------------------------------------------------------------------

end test_neuron_pkg;
--------------------------------------------------------------------------------