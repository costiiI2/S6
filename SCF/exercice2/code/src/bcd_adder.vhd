-----------------------------------------------------------------------
-- HEIG-VD, Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
-- Institut REDS
--
-- Composant    : bcd_adder
-- Description  : Additionneur BCD.
--                Un paramètre générique permet de définir le nombre de digits.
-- Auteur       : Yann Thoma
-- Date         : 01.03.2017
-- Version      : 1.0
--
-- Modification : -
--
-----------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use IEEE.math_real.all;
use work.bcd_pkg.all;
use work.bcd_configuration_package.all;

entity bcd_adder is
    generic (
        NDIGITS : integer := 4
    );
    port (
        input0_i  : in  bcd_number(NDIGITS - 1 downto 0);
        input1_i  : in  bcd_number(NDIGITS - 1 downto 0);
        result_o  : out bcd_number(NDIGITS downto 0);
        hamming_o : out std_logic_vector(integer(ceil(log2(real(4*NDIGITS)))) downto 0)
    );
end bcd_adder;



architecture struct of bcd_adder is


    component generic_full_adder is
        generic(
            Basis_g         : natural := 10 -- Number Basis_g 2^N_bits_c >= Basis_g
        );
        port(
            Carry_i       : in  std_logic;
            Op_A_i        : in  std_logic_vector(BCX_Bits(Basis_g)-1 downto 0);
            Op_B_i        : in  std_logic_vector(BCX_Bits(Basis_g)-1 downto 0);
            Res_o         : out std_logic_vector(BCX_Bits(Basis_g)-1 downto 0);
            Carry_o       : out std_logic
        );

    end component;

    signal carry_s : std_logic := '0';
        
begin

-- op_1 add 2 BDC numbers
    gen_adder: for i in 0 to NDIGITS - 1 generate
        full_adder: generic_full_adder
            generic map(
                Basis_g => 10
            )
            port map(
                Carry_i => carry_s,
                Op_A_i  => input0_i(i),
                Op_B_i  => input1_i(i),
                Res_o   => result_o(i),
                Carry_o => carry_s
            );
    end generate gen_adder;

    result_o(NDIGITS-1) <= carry_s;

-- op_2 hamming distance

    hamm_calc: process(input0_i, input1_i)
        variable hamming_v : integer := 0;
    begin
        for i in 0 to NDIGITS - 1 loop
            if input0_i(i) /= input1_i(i) then
                hamming_v := hamming_v + 1;
            end if;
        end loop;
        hamming_o <= std_logic_vector(to_unsigned(hamming_v, hamming_o'length));
    end process hamm_calc;
    

end struct;
