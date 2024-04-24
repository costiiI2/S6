--| Library |-------------------------------------------------------------------
library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
use work.neuron_pkg.all;
use work.neuron_tb_pkg.all;
use work.project_logger_pkg.all;
--------------------------------------------------------------------------------

--| Package declaration |-------------------------------------------------------
package neuron_bfm_pkg is

    --| Procedures |------------------------------------------------------------
    procedure neuron_write_each_clk(variable val      : in  neuron_stimulis_t;
                                    signal   clk      : in  std_logic;
                                    signal   observed : in  neuron_observed_t;
                                    signal   stimulis : out neuron_stimulis_t);

    procedure neuron_write_with_pause(variable val      : in  neuron_stimulis_t;
                                      signal   clk      : in  std_logic;
                                      signal   observed : in  neuron_observed_t;
                                      signal   stimulis : out neuron_stimulis_t);

    procedure neuron_write_following(variable val      : in  neuron_stimulis_t;
                                     signal   clk      : in  std_logic;
                                     signal   observed : in  neuron_observed_t;
                                     signal   stimulis : out neuron_stimulis_t);
    ----------------------------------------------------------------------------

end neuron_bfm_pkg;
--------------------------------------------------------------------------------

--| Package body |--------------------------------------------------------------
package body neuron_bfm_pkg is

    --| Procedures |------------------------------------------------------------
    procedure neuron_write_following(variable val      : in  neuron_stimulis_t;
                                     signal   clk      : in  std_logic;
                                     signal   observed : in  neuron_observed_t;
                                     signal   stimulis : out neuron_stimulis_t) is


        constant DATASIZE : integer := observed.result'length;
        variable stimulis_v : neuron_stimulis_t(inputs(val.inputs'range)(DATASIZE-1 downto 0),
                                                weights(val.weights'range)(DATASIZE-1 downto 0),
                                                valid(val.valid'range));
        constant NBINPUTS : integer := observed.ready'length;
    begin


        stimulis.ready <= '0';
        stimulis.valid <= (stimulis.valid'range => '0');
        cycle_fall(clk, 1);

        -- Loop trough all the input to check if they are ready to get new value
        tab_loop1: for i in 0 to NBINPUTS-1 loop

            -- Set the input and rise the valid flag
            stimulis.inputs(i)  <= val.inputs(i);
            stimulis.weights(i) <= val.weights(i);
            --stimulis.valid      <= (i => '1', others => '0');
            stimulis.valid(i)   <= '1';
            cycle_fall(clk, 1);
        end loop;

        if(observed.valid = '0') then
            -- Wait for the result to be valid
            wait until(observed.valid = '1') for (TIMEOUT);
        end if;

        -- If not valid it mean a timout happen -> error
        if(observed.valid = '0') then
            logger.log_error("neuron never valid");
        end if;

        -- Set ready -> the result is taken
        stimulis.ready <= '1';
        cycle_fall(clk, 1);

    end neuron_write_following;

    procedure neuron_write_with_pause(variable val      : in  neuron_stimulis_t;
                                      signal   clk      : in  std_logic;
                                      signal   observed : in  neuron_observed_t;
                                      signal   stimulis : out neuron_stimulis_t) is

        constant DATASIZE : integer := observed.result'length;
        variable stimulis_v : neuron_stimulis_t(inputs(val.inputs'range)(DATASIZE-1 downto 0),
                                                weights(val.weights'range)(DATASIZE-1 downto 0),
                                                valid(val.valid'range));
        constant NBINPUTS : integer := observed.ready'length;
        variable done_v     : std_logic_vector(NBINPUTS-1 downto 0) := (others => '0');
    begin

        -- Always ready to write data
        stimulis.ready <= '1';

        -- Set all the value and indicate they are valid
        stimulis.inputs  <= val.inputs;
        stimulis.weights <= val.weights;
        stimulis.valid   <= (stimulis.valid'range => '1');

        -- Loop till all the ready have passed high
        done_v := (others => '0');
        loop

            -- Check all the ready at each clock
            for i in 0 to NBINPUTS-1 loop
                if(observed.ready(i) = '1') then
                    -- If a ready is high set the corresponding bit of done to '1'
                    done_v(i) := '1';
                end if;
            end loop;


            -- Write stimulis on falling edge
            cycle_fall(clk, 1);

            if(signed(done_v) = to_signed(-1, done_v'length)) then
                -- Wait a bit to be sure the result is taken before falling ready
                cycle_fall(clk, 1);
                stimulis.valid <= (stimulis.valid'range => '0');
                stimulis.ready <= '0';
            end if;

            -- Exit when all the bit of done_v are '1'
            exit when(signed(done_v) = to_signed(-1, done_v'length));
        end loop;

        cycle_fall(clk, 1);

    end neuron_write_with_pause;

    procedure neuron_write_each_clk(variable val      : in  neuron_stimulis_t;
                                    signal   clk      : in  std_logic;
                                    signal   observed : in  neuron_observed_t;
                                    signal   stimulis : out neuron_stimulis_t) is

        constant DATASIZE : integer := observed.result'length;
        variable stimulis_v : neuron_stimulis_t(inputs(val.inputs'range)(DATASIZE-1 downto 0),
                                                weights(val.weights'range)(DATASIZE-1 downto 0),
                                                valid(val.valid'range));
        constant NBINPUTS : integer := observed.ready'length;
        variable done_v     : std_logic_vector(NBINPUTS-1 downto 0) := (others => '0');
    begin

        -- Set all ready/valid to '1'
        stimulis.inputs  <= val.inputs;
        stimulis.weights <= val.weights;
        stimulis.valid   <= (stimulis.valid'range => '1');
        stimulis.ready   <= '1';

        -- Loop till all the ready have passed high -> all the input have been taken
        done_v := (others => '0');
        loop
            -- Check all the ready at each clock
            for i in 0 to NBINPUTS-1 loop
                if(observed.ready(i) = '1') then
                    -- If a ready is high set the corresponding bit of done to '1'
                    done_v(i) := '1';
                end if;
            end loop;

            -- Write stimulis on falling edge
            cycle_fall(clk, 1);

            -- Exit when all the bit of done_v are '1'
            exit when(signed(done_v) = to_signed(-1, done_v'length));
        end loop;

    end neuron_write_each_clk;
    ----------------------------------------------------------------------------

end neuron_bfm_pkg;
--------------------------------------------------------------------------------