-------------------------------------------------------------------------------
--    Copyright 2015 HES-SO HEIG-VD REDS
--    All Rights Reserved Worldwide
--
--    Licensed under the Apache License, Version 2.0 (the "License");
--    you may not use this file except in compliance with the License.
--    You may obtain a copy of the License at
--
--        http://www.apache.org/licenses/LICENSE-2.0
--
--    Unless required by applicable law or agreed to in writing, software
--    distributed under the License is distributed on an "AS IS" BASIS,
--    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
--    See the License for the specific language governing permissions and
--    limitations under the License.
-------------------------------------------------------------------------------
--
-- File        : tlm_fifo_example_tb.vhd
-- Description : This package offers an example of how to use TLM FIFOs to
--               help data transmission between two processes.
--               This kind of mechanisms are very useful for testing designs
--               That take data as input and generates output from it,
--               typically packets managers for communication protocols.
--               It also allows to pass information from one process to
--               another without timing constraints.
--
-- Author      : Yann Thoma
-- Team        : REDS institute
-- Date        : 05.06.13
--
--
--| Modifications |------------------------------------------------------------
-- Ver  Date      Who   Description
-- 1.0  05.06.13  YTA   First version
-- 
-------------------------------------------------------------------------------


library ieee;
use ieee.std_logic_1164.all;

-- This package only declares a type, used by the example testbench
package user_pkg is
    
    type user_type is
    record
        data: std_logic_vector(31 downto 0);
        addr: std_logic_vector(7 downto 0);
    end record user_type;
    
end user_pkg;



library ieee;
use ieee.std_logic_1164.all;
library tlmvm;
use tlmvm.tlm_fifo_pkg;

-- Create a specialized package that will offer FIFOs with 32-bits data in it
package my_tlm_pkg is new tlm_fifo_pkg
  generic map (element_type => std_logic_vector(31 downto 0),
                nb_max_data => 20);




library ieee;
use ieee.std_logic_1164.all;
use work.user_pkg.all;
library tlmvm;
use tlmvm.tlm_fifo_pkg;

-- Create a specialized package that will offer FIFOs with the specific
-- user type of data in it
package user_tlm_pkg is new tlm_fifo_pkg
  generic map (element_type => user_type,
                nb_max_data => 20);




library ieee;
use ieee.std_logic_1164.all;
use work.user_pkg.all;
library tlmvm;
use tlmvm.tlm_unbounded_fifo_pkg;

-- Create a specialized package that will offer unbounded FIFOs with the
-- specific user type of data in it
package unbounded_user_tlm_pkg is new tlm_unbounded_fifo_pkg
  generic map (element_type => user_type);

-------------------------------------------------------------------------------
-- So now, we're ready to use the 3 types of FIFOs
-------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

use work.user_pkg.all;
use work.user_tlm_pkg.all;
use work.my_tlm_pkg.all;

entity tlm_fifo_no_objection_example_tb is
end tlm_fifo_no_objection_example_tb;


architecture testbench of tlm_fifo_no_objection_example_tb is

    -- Clock period
    constant CLK_PERIOD : time := 10 ns;

    -- Clock signal
	signal clk_sti : std_logic;

    -- One FIFO    
    shared variable fifo: work.my_tlm_pkg.tlm_fifo_type;

    -- And another one
    shared variable fifo_user: work.user_tlm_pkg.tlm_fifo_type;

    -- And a last one
    shared variable unbounded_fifo_user:
                          work.unbounded_user_tlm_pkg.tlm_fifo_type;
    

    -- A simple procedure to wait a certain number of clock cycles
	procedure cycle(nb_cycles: integer := 1) is
	begin
		for i in 1 to nb_cycles loop
			wait until rising_edge(clk_sti);
		end loop;
	end cycle;

begin

	clk_proc: process is
	begin
		for i in 1 to 1000 loop
			clk_sti <= '0';
			wait for CLK_PERIOD/2;
			clk_sti <= '1';
			wait for CLK_PERIOD/2;
		end loop;
		wait;
	end process;
    

-------------------------------------------------------------------------------
-- Test of the first FIFO
-------------------------------------------------------------------------------

    -- Write process number 1
    write_proc1: process is
        variable ok: boolean;
    begin
        for i in 1 to 30 loop
            cycle;
            while (fifo.is_full) loop
                cycle;
            end loop;
            fifo.put(std_logic_vector(to_unsigned(i,32)),ok);
            if (not ok) then
                report "FIFO Full";
            end if;
        end loop;
        wait;
    end process;
    
    -- Read process number 1
    read_proc1: process is
        variable ok: boolean;
        variable data : std_logic_vector(31 downto 0);
    begin
		cycle(200);
        while (true) loop
            cycle;
            while (fifo.is_empty) loop
                cycle;
            end loop;
            fifo.get(data,ok);
            if (ok) then
                report "read_proc1. Read a data: " &
                       integer'image(to_integer(unsigned(data)));
            end if;
        end loop;
    end process;

-------------------------------------------------------------------------------
-- Test of the second FIFO
-------------------------------------------------------------------------------

    -- Write process number 2
    write_proc2: process is
        variable ok: boolean;
        variable sample : user_type;
    begin
		cycle(400);
        for i in 1 to 30 loop
            cycle;
            while (fifo_user.is_full) loop
                cycle;
            end loop;
            sample.data := std_logic_vector(to_unsigned(i,32));
            sample.addr := std_logic_vector(to_unsigned(i,8));
            fifo_user.put(sample,ok);
            if (not ok) then
                report "FIFO Full";
            end if;
        end loop;
        wait;
    end process;
    
    -- Read process number 2
    read_proc2: process is
        variable ok: boolean;
        variable sample : user_type;
    begin
        cycle(200);
        while (true) loop
            cycle;
            while (fifo_user.is_empty) loop
                cycle;
            end loop;
            fifo_user.get(sample,ok);
            if (ok) then
                report "read_proc2. Read a sample. Data: " &
                       integer'image(to_integer(unsigned(sample.data))) &
                       ". Addr: " &
                       integer'image(to_integer(unsigned(sample.addr)));
            end if;
        end loop;
    end process;


-------------------------------------------------------------------------------
-- Test of the third FIFO
-------------------------------------------------------------------------------

    -- Write process number 3
    write_proc3: process is
        variable ok: boolean;
        variable sample : user_type;
    begin
		cycle(600);
        for i in 1 to 30 loop
            cycle;
            while (unbounded_fifo_user.is_full) loop
                cycle;
            end loop;
            sample.data := std_logic_vector(to_unsigned(i,32));
            sample.addr := std_logic_vector(to_unsigned(i,8));
            unbounded_fifo_user.put(sample,ok);
            if (not ok) then
                report "FIFO Full";
            end if;
        end loop;
        wait;
    end process;

    -- Read process number 3
    read_proc3: process is
        variable ok: boolean;
        variable sample : user_type;
    begin
        cycle(200);
        while (true) loop
            cycle;
            while (unbounded_fifo_user.is_empty) loop
                cycle;
            end loop;
            unbounded_fifo_user.get(sample,ok);
            if (ok) then
                report "read_proc3. Read a sample. Data: " &
                       integer'image(to_integer(unsigned(sample.data))) &
                       ". Addr: " &
                       integer'image(to_integer(unsigned(sample.addr)));
            end if;
        end loop;
    end process;

end testbench;


