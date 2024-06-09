-----------------------------------------------------------------------
-- HEIG-VD, Haute Ecole d'Ingenierie et de Gestion du canton de Vaud
-- Institut REDS, Reconfigurable & Embedded Digital Systems
--
-- File         : axi4lite_slave.vhd
-- Author       : E. Messerli    27.07.2017
-- Description  : slave interface AXI  (without burst)
-- used for     : SOCF lab
--| Modifications |-----------------------------------------------------------
-- Ver   Auteur Date         Description
-- 1.0   EMI    09.08.2017   Group process for Write adresse channel
--                           Modify Write data channel
-- 1.1   EMI    13.08.2017   Change signal name axi_awaddr_i
-- 1.2   EMI    14.08.2017   Simulate with axi_dummy_tb.vhd  => 
--                             seem to be correct !
--
--
------------------------------------------------------------------------------

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;

entity axi4lite_slave is
    generic (
        -- Users to add parameters here

        -- User parameters ends

        -- Width of S_AXI data bus
        AXI_DATA_WIDTH  : integer   := 32;  -- 32 or 64 bits
        -- Width of S_AXI address bus
        AXI_ADDR_WIDTH  : integer   := 12
    );
    port (
        clk_i           : in  std_logic;
        reset_i         : in  std_logic;
        -- AXI4-Lite 
        axi_awaddr_i    : in  std_logic_vector(AXI_ADDR_WIDTH-1 downto 0);
        axi_awprot_i    : in  std_logic_vector( 2 downto 0);
        axi_awvalid_i   : in  std_logic;
        axi_awready_o   : out std_logic;
        axi_wdata_i     : in  std_logic_vector(AXI_DATA_WIDTH-1 downto 0);
        axi_wstrb_i     : in std_logic_vector((AXI_DATA_WIDTH/8)-1 downto 0);
        axi_wvalid_i    : in  std_logic;
        axi_wready_o    : out std_logic;
        axi_bresp_o     : out std_logic_vector(1 downto 0);
        axi_bvalid_o    : out std_logic;
        axi_bready_i    : in  std_logic;
        axi_araddr_i    : in  std_logic_vector(AXI_ADDR_WIDTH-1 downto 0);
        axi_arprot_i    : in  std_logic_vector( 2 downto 0);
        axi_arvalid_i   : in  std_logic;
        axi_arready_o   : out std_logic;
        axi_rdata_o     : out std_logic_vector(AXI_DATA_WIDTH-1 downto 0);
        axi_rresp_o     : out std_logic_vector(1 downto 0);
        axi_rvalid_o    : out std_logic;
        axi_rready_i    : in  std_logic;
        -- FIFO
        data     : IN STD_LOGIC_VECTOR (31 DOWNTO 0);
        rdreq    : IN STD_LOGIC;
        wrreq    : IN STD_LOGIC;
        empty    : OUT STD_LOGIC;
        full     : OUT STD_LOGIC;
        q        : OUT STD_LOGIC_VECTOR (31 DOWNTO 0)

    );
end entity axi4lite_slave;

architecture rtl of axi4lite_slave is

    signal reset_s : std_logic;

    --signal for the AXI slave
    --intern signal for output
    signal axi_awready_s       : std_logic;
    signal axi_wready_s        : std_logic;
    signal axi_bresp_s         : std_logic_vector(1 downto 0);
    signal axi_waddr_done_s    : std_logic;
    signal axi_bvalid_s        : std_logic;
    signal axi_arready_s       : std_logic;
    signal axi_rresp_s         : std_logic_vector(1 downto 0);
    signal axi_raddr_done_s    : std_logic;
    signal axi_rvalid_s        : std_logic;

    -- local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH
    -- ADDR_LSB is used for addressing 32/64 bit registers/memories
    -- ADDR_LSB = 2 for 32 bits (n downto 2)
    -- ADDR_LSB = 3 for 64 bits (n downto 3)
    constant ADDR_LSB  : integer := (AXI_DATA_WIDTH/32)+ 1;
    constant DATA_ZERO : unsigned(AXI_DATA_WIDTH-1 downto 0) := (others => '0');

     --intern signal for the axi interface
    signal axi_waddr_mem_s     : std_logic_vector(AXI_ADDR_WIDTH-1 downto ADDR_LSB);
    signal axi_data_wren_s     : std_logic;
    signal axi_write_done_s    : std_logic;
    signal axi_araddr_mem_s    : std_logic_vector(AXI_ADDR_WIDTH-1 downto ADDR_LSB);
    signal axi_data_rden_s     : std_logic;
    signal axi_read_done_s     : std_logic;
    signal axi_rdata_s         : std_logic_vector(AXI_DATA_WIDTH-1 downto 0);

    signal dummy_cnt : unsigned(15 downto 0);

    signal byte_index   : integer;

    --debug signal
    signal local_addr_s :std_logic_vector(AXI_ADDR_WIDTH-1-ADDR_LSB downto 0);

    --user declarations
    constant CST_ADDR_0_FOR_TST  : std_logic_vector(31 downto 0) := x"BADB100D";
    -- signal for the kernel
    signal kern_reg_0_3_s  : std_logic_vector(31 downto 0);
    signal kern_reg_4_7_s  : std_logic_vector(31 downto 0);
    signal kern_reg_8_s    : std_logic_vector(31 downto 0);


    -- FIFO's
    COMPONENT fifo
        PORT (
            clock    : IN STD_LOGIC;
            data     : IN STD_LOGIC_VECTOR (31 DOWNTO 0);
            rdreq    : IN STD_LOGIC;
            wrreq    : IN STD_LOGIC;
            empty    : OUT STD_LOGIC;
            full     : OUT STD_LOGIC;
            q        : OUT STD_LOGIC_VECTOR (31 DOWNTO 0)
        );
    END COMPONENT;
    
    --img fifo
    signal img_fifo_data_in    : std_logic_vector(31 downto 0);
    signal img_fifo_write_en   : std_logic;
    signal img_fifo_read_en    : std_logic;
    signal img_fifo_data_out   : std_logic_vector(31 downto 0);
    signal img_fifo_full       : std_logic;
    signal img_fifo_empty      : std_logic;

    -- convolution fifo
    signal process_fifo_data_in    : std_logic_vector(31 downto 0);
    signal process_fifo_write_en   : std_logic;
    signal process_fifo_read_en    : std_logic;
    signal process_fifo_data_out   : std_logic_vector(31 downto 0);
    signal process_fifo_full       : std_logic;
    signal process_fifo_empty      : std_logic;
    signal comp_head : integer := 0;

    -- output fifo
    signal out_fifo_data_in    : std_logic_vector(31 downto 0);
    signal out_fifo_write_en   : std_logic;
    signal out_fifo_read_en    : std_logic;
    signal out_fifo_data_out   : std_logic_vector(31 downto 0);
    signal out_fifo_full       : std_logic;
    signal out_fifo_empty      : std_logic;
    signal out_head : integer := 0;
    signal out_data_s : std_logic_vector(31 downto 0);



    
  

begin

      
    -- FIFO for the image

    img_fifo : fifo
        port map (
            clock    => clk_i,
            data     => img_fifo_data_in,
            rdreq    => img_fifo_read_en,
            wrreq    => img_fifo_write_en,
            empty    => img_fifo_empty,
            full     => img_fifo_full,
            q        => img_fifo_data_out
        );

    -- FIFO for the image convolution result
    process_fifo : fifo
        port map (
            clock    => clk_i,
            data     => process_fifo_data_in,
            rdreq    => process_fifo_read_en,
            wrreq    => process_fifo_write_en,
            empty    => process_fifo_empty,
            full     => process_fifo_full,
            q        => process_fifo_data_out
        );

    -- FIFO for the output
    out_fifo : fifo
        port map (
            clock    => clk_i,
            data     => out_fifo_data_in,
            rdreq    => out_fifo_read_en,
            wrreq    => out_fifo_write_en,
            empty    => out_fifo_empty,
            full     => out_fifo_full,
            q        => out_fifo_data_out
        );

    reset_s  <= reset_i;
  
-----------------------------------------------------------
--  Affectation for debug
    local_addr_s <=  axi_araddr_mem_s(AXI_ADDR_WIDTH-1 downto ADDR_LSB);
  
-----------------------------------------------------------
-- Write adresse channel

    -- Implement axi_awready generation and
    -- Implement axi_awaddr memorizing
    --   memorize address when S_AXI_AWVALID is valid.
    process (reset_s, clk_i)
    begin
        if reset_s = '1' then
            axi_awready_s    <= '0';
            axi_waddr_done_s <= '0';   
            axi_waddr_mem_s  <= (others => '0');
        elsif rising_edge(clk_i) then
            axi_waddr_done_s <= '0';
            if (axi_awready_s = '1' and axi_awvalid_i = '1')  then --and axi_wvalid_i = '1') then  modif EMI 10juil
                -- slave is ready to accept write address when
                -- there is a valid write address
                axi_awready_s    <= '0';
                axi_waddr_done_s <= '1';
                -- Write Address memorizing
                axi_waddr_mem_s  <= axi_awaddr_i(AXI_ADDR_WIDTH-1 downto ADDR_LSB);
            elsif axi_write_done_s = '1' then
                axi_awready_s    <= '1';
            end if;
        end if;
    end process;
    axi_awready_o <= axi_awready_s;

-----------------------------------------------------------
-- Write data channel
    -- Implement axi_wready generation
    process (reset_s, clk_i)
    begin
        if reset_s = '1' then
            axi_wready_s    <= '0';
            -- axi_data_wren_s <= '0';
        elsif rising_edge(clk_i) then
            -- axi_data_wren_s <= '0';
            --if (axi_wready_s = '0' and axi_wvalid_i = '1' and axi_awready_s = '1' ) then --axi_awvalid_i = '1') then
            if (axi_wready_s = '1' and axi_wvalid_i = '1') then --modif EMI 10juil
                -- slave is ready to accept write address when
                -- there is a valid write address and write data
                -- on the write address and data bus. This design
                -- expects no outstanding transactions.
                axi_wready_s <= '0';
                -- axi_data_wren_s <= '1';
            elsif axi_waddr_done_s = '1' then
                axi_wready_s <= '1';
            end if;
        end if;
    end process;
    axi_wready_o <= axi_wready_s;

    -- Implement memory mapped register select and write logic generation
    -- The write data is accepted and written to memory mapped registers when
    -- axi_awready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted. Write strobes are used to
    -- select byte enables of slave registers while writing.
    -- These registers are cleared when reset is applied.
    -- Slave register write enable is asserted when valid address and data are available
    -- and the slave is ready to accept the write address and write data.
    axi_data_wren_s <= axi_wready_s and axi_wvalid_i ; --and axi_awready_s and axi_awvalid_i ;

        -- Offset (idx) | Data                        | RW
    -- -------------|-----------------------------|-----
    -- 0x00	    (0) | Constant (0xBADB100D)   | R
    -- --kernel registers-------------------------------
    -- 0x04	    (1) | kernel[0-3]             | R/W
    -- 0x08	    (2) | kernel[4-7]             | R/W
    -- 0x0C	    (3) | kernel[8],-,-,-         | R/W
    -- --image registers--------------------------------
    -- 0x10	    (4) | set img[0-3]      	  | W

    -- 0x1C     (7) | return value            | R
    -- --control registers------------------------------
    -- 0x20     (8) | Can write (1) / Can read (0) | R
  
    ---------------------------------------------------------------------
    process (reset_s, clk_i)
        --number address to access 32 or 64 bits data
        variable int_waddr_v : natural;
    begin
        if reset_s = '1' then
            kern_reg_0_3_s <= (others => '0');
            kern_reg_4_7_s <= (others => '0');
            kern_reg_8_s   <= (others => '0');
            axi_write_done_s <= '1';
            img_fifo_data_in <= (others => '0');
            img_fifo_write_en <= '0';

        elsif rising_edge(clk_i) then
            axi_write_done_s <= '0';
            img_fifo_write_en <= '0';

            ---------------------------------------------------------------------
            if axi_data_wren_s = '1' then
                axi_write_done_s <= '1';
                int_waddr_v   := to_integer(unsigned(axi_waddr_mem_s));
                case int_waddr_v is
                    ----- KERNEL REGISTERS -------------------------------------
                    when 1 =>
                        for byte_index in 0 to (AXI_DATA_WIDTH/8-1) loop
                            if ( axi_wstrb_i(byte_index) = '1' ) then
                                kern_reg_0_3_s(byte_index*8+7 downto byte_index*8) <= axi_wdata_i(byte_index*8+7 downto byte_index*8);
                            end if;
                        end loop;
                    when 2 =>
                        for byte_index in 0 to (AXI_DATA_WIDTH/8-1) loop
                            if ( axi_wstrb_i(byte_index) = '1' ) then
                                kern_reg_4_7_s(byte_index*8+7 downto byte_index*8) <= axi_wdata_i(byte_index*8+7 downto byte_index*8);
                            end if;
                        end loop;
                    when 3 =>
                        for byte_index in 0 to (AXI_DATA_WIDTH/8-1) loop
                            if ( axi_wstrb_i(byte_index) = '1' ) then
                                kern_reg_8_s(byte_index*8+7 downto byte_index*8) <= axi_wdata_i(byte_index*8+7 downto byte_index*8);
                            end if;
                        end loop;
                    ----- IMAGE REGISTERS FOR FIFO --------------------------------
                    when 4 =>
                        for byte_index in 0 to (AXI_DATA_WIDTH/8-1) loop
                            if ( axi_wstrb_i(byte_index) = '1' ) then
                                -- fill the fifo element with the data [0-3]
                                img_fifo_data_in(byte_index*8+7 downto byte_index*8) <= axi_wdata_i(byte_index*8+7 downto byte_index*8);
                            end if;
                        end loop;
                        img_fifo_write_en <= '1';
                    --------------------------------------------------------------------
                    when others => null;
                end case;
            end if;
        end if;
    end process;

-----------------------------------------------------------
-- Write respond channel

    -- Implement write response logic generation
    -- The write response and response valid signals are asserted by the slave
    -- when axi_wready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted.
    -- This marks the acceptance of address and indicates the status of
    -- write transaction.

    process (reset_s, clk_i)
    begin
        if reset_s = '1' then
            axi_bvalid_s <= '0';
            axi_bresp_s  <= "00"; --need to work more on the responses
        elsif rising_edge(clk_i) then
            --if (axi_awready_s ='1' and axi_awvalid_i ='1' and axi_wready_s ='1' and axi_wvalid_i ='1' then -- supprimer: axi_bready_i ='0' ) then
            if axi_data_wren_s = '1' then
                axi_bvalid_s <= '1';
                axi_bresp_s  <= "00";
            elsif (axi_bready_i = '1') then --  and axi_bvalid_s = '1') then
                axi_bvalid_s <= '0';
            end if;
        end if;
    end process;
    axi_bvalid_o <= axi_bvalid_s;
    axi_bresp_o <= axi_bresp_s;

-----------------------------------------------------------
-- Read address channel

    -- Implement axi_arready generation
    -- axi_arready is asserted for one S_AXI_ACLK clock cycle when
    -- S_AXI_ARVALID is asserted. axi_awready is
    -- de-asserted when reset (active low) is asserted.
    -- The read address is also memorised when S_AXI_ARVALID is
    -- asserted. axi_araddr is reset to zero on reset assertion.
    process (reset_s, clk_i)
    begin
        if reset_s = '1' then
           axi_arready_s    <= '1';
           axi_raddr_done_s <= '0';
           axi_araddr_mem_s <= (others => '1');
        elsif rising_edge(clk_i) then
            if axi_arready_s = '1' and axi_arvalid_i = '1' then
                axi_arready_s    <= '0';
                axi_raddr_done_s <= '1';
                -- Read Address memorization
                axi_araddr_mem_s <= axi_araddr_i(AXI_ADDR_WIDTH-1 downto ADDR_LSB);
            elsif (axi_raddr_done_s = '1' and axi_rvalid_s = '0') then
                axi_raddr_done_s <= '0';
            elsif axi_read_done_s = '1' then
                axi_arready_s    <= '1';
            end if;
        end if;
    end process;
    axi_arready_o <= axi_arready_s;

-----------------------------------------------------------
-- Read data channel

    -- Implement axi_rvalid generation
    -- axi_rvalid is asserted for one S_AXI_ACLK clock cycle when both
    -- S_AXI_ARVALID and axi_arready are asserted. The slave registers
    -- data are available on the axi_rdata bus at this instance. The
    -- assertion of axi_rvalid marks the validity of read data on the
    -- bus and axi_rresp indicates the status of read transaction.axi_rvalid
    -- is deasserted on reset. axi_rresp and axi_rdata are
    -- cleared to zero on reset.
    process (reset_s, clk_i)
    begin
        if reset_s = '1' then
            -- axi_raddr_done_s <= '0';
            axi_rvalid_s    <= '0';
            axi_read_done_s <= '0';
            axi_rresp_s     <= "00";
        elsif rising_edge(clk_i) then
            -- if axi_arready_s = '0' and axi_arvalid_i = '1' then     --  modif EMI 10juil
            --     axi_raddr_done_s <= '1';
            --if (axi_arready_s = '1' and axi_arvalid_i = '1' and axi_rvalid_s = '0') then
            axi_read_done_s <= '0';
            if (axi_raddr_done_s = '1' and axi_rvalid_s = '0') then   --  modif EMI 10juil
                -- Valid read data is available at the read data bus
                axi_rvalid_s    <= '1';
                -- axi_raddr_done_s <= '0';                                   --  modif EMI 10juil
                axi_rresp_s  <= "00"; -- 'OKAY' response
            elsif (axi_rvalid_s = '1' and axi_rready_i = '1') then
                -- Read data is accepted by the master
                axi_rvalid_s    <= '0';
                axi_read_done_s <= '1';
            end if;
        end if;
    end process;
    axi_rvalid_o <= axi_rvalid_s;
    axi_rresp_o <= axi_rresp_s;

    -- Implement memory mapped register select and read logic generation
    -- Slave register read enable is asserted when valid address is available
    -- and the slave is ready to accept the read address.
    axi_data_rden_s <= axi_raddr_done_s and (not axi_rvalid_s);

    process (axi_araddr_mem_s,
            kern_reg_0_3_s,
            kern_reg_4_7_s,
            kern_reg_8_s,
            out_fifo_data_out,
            out_fifo_empty,
            img_fifo_full
       )
        --number address to access
        variable int_raddr_v  : natural;
    begin
        int_raddr_v   := to_integer(unsigned(axi_araddr_mem_s));
        axi_rdata_s <= x"A5A5A5A5"; --default value
        out_fifo_read_en <= '0';
        case int_raddr_v is
            when 0 =>
                axi_rdata_s <= CST_ADDR_0_FOR_TST;
            when 1 =>
                axi_rdata_s <= kern_reg_0_3_s;
            when 2 =>
                axi_rdata_s <= kern_reg_4_7_s;
            when 3 =>
                axi_rdata_s <= kern_reg_8_s;
            when 7 => 
                axi_rdata_s <= out_fifo_data_out;
                out_fifo_read_en <= '1';
            when 8 =>
                -- read / write
                axi_rdata_s <= (others => '0');
                axi_rdata_s(0) <= not img_fifo_full;
                axi_rdata_s(1) <= not out_fifo_empty;

                axi_rdata_s(2) <= process_fifo_full;
                axi_rdata_s(3) <= process_fifo_empty;
                axi_rdata_s(4) <= out_fifo_full;
                axi_rdata_s(5) <= img_fifo_empty;
            when others =>
                axi_rdata_s <= x"A5A5A5A5";
        end case;
    end process;

    process (reset_s, clk_i)
    begin
        if reset_s = '1' then
            axi_rdata_o <= (others => '0');
        elsif rising_edge(clk_i) then
            if axi_data_rden_s = '1' then
                -- When there is a valid read address (S_AXI_ARVALID) with
                -- acceptance of read address by the slave (axi_arready),
                -- output the read dada
                -- Read address mux
                axi_rdata_o <= axi_rdata_s;
            end if;
        end if;
    end process;

-----------------------------------------------------------

-- convolution computation
PROCESSING_FIFO : process(clk_i, reset_s)
begin
    if reset_s = '1' then
        comp_head <= 0;
        img_fifo_read_en <= '0';
        process_fifo_write_en <= '0';
        process_fifo_data_in <= (others => '0');

    elsif rising_edge(clk_i) then
        -- calculate the output of element i in ram * kernel and store it in ram_comp
        if img_fifo_empty = '0' and  process_fifo_full = '0' then
                    -- read the first element of the fifo
            if comp_head = 0 then
                process_fifo_data_in <= std_logic_vector(resize(signed(img_fifo_data_out(7 downto 0)) * signed(kern_reg_0_3_s(7 downto 0)) +
                                                                signed(img_fifo_data_out(15 downto 8)) * signed(kern_reg_0_3_s(15 downto 8)) +
                                                                signed(img_fifo_data_out(23 downto 16)) * signed(kern_reg_0_3_s(23 downto 16)) +
                                                                signed(img_fifo_data_out(31 downto 24)) * signed(kern_reg_0_3_s(31 downto 24)), 64)(31 downto 0));

                comp_head <= 1;

            elsif comp_head = 1 then
                process_fifo_data_in <= std_logic_vector(resize(signed(img_fifo_data_out(7 downto 0)) * signed(kern_reg_4_7_s(7 downto 0)) +
                                                                signed(img_fifo_data_out(15 downto 8)) * signed(kern_reg_4_7_s(15 downto 8)) +
                                                                signed(img_fifo_data_out(23 downto 16)) * signed(kern_reg_4_7_s(23 downto 16)) +
                                                                signed(img_fifo_data_out(31 downto 24)) * signed(kern_reg_4_7_s(31 downto 24)), 64)(31 downto 0));
                comp_head <= 2;
                                                              
            elsif comp_head = 2 then
                process_fifo_data_in <= std_logic_vector(resize(signed(img_fifo_data_out(7 downto 0)) * signed(kern_reg_8_s(7 downto 0)) +
                                                                signed(img_fifo_data_out(15 downto 8)) * signed(kern_reg_8_s(15 downto 8)) +
                                                                signed(img_fifo_data_out(23 downto 16)) * signed(kern_reg_8_s(23 downto 16)) +
                                                                signed(img_fifo_data_out(31 downto 24)) * signed(kern_reg_8_s(31 downto 24)), 64)(31 downto 0));
                comp_head <= 0;
                                                              
            end if;
                        process_fifo_write_en <= '1';
                        img_fifo_read_en <= '1';
                    end if;
                end if;
            end process;
            

RESULT_FIFO : process(clk_i, reset_s)
begin
    if reset_s = '1' then
        out_fifo_write_en <= '0';
        out_head <= 0;
        out_data_s <= (others => '0');
        out_fifo_data_in <= (others => '0');
    elsif rising_edge(clk_i) then
        out_fifo_write_en <= '0';
        process_fifo_read_en <= '0';

        if process_fifo_empty = '0' and out_fifo_full = '0' then
            if out_head = 0 then
                out_data_s <= process_fifo_data_out;
                process_fifo_read_en <= '1';
                out_head <= 1;
            end if;
            if out_head = 1 then
                out_data_s <= std_logic_vector(signed(out_data_s) + signed(process_fifo_data_out));
                process_fifo_read_en <= '1';
                out_head <= 2;
            elsif out_head = 2 then
                out_data_s <= std_logic_vector(signed(out_data_s) + signed(process_fifo_data_out));
                process_fifo_read_en <= '1';
                out_head <= 3;
            elsif out_head = 3 then
                -- output the result to the output fifo
                out_fifo_data_in <= out_data_s;
                out_fifo_write_en <= '1';
                out_head <= 4;
            elsif out_head = 4 then
                out_head <= 0;
            end if;
                
        end if;
        
    end if;
end process;

end rtl;
