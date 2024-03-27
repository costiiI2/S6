------------------------------------------------------------------------------
-- HEIG-VD, Haute Ecole d'Ingenierie et de Gestion du canton de Vaud
-- Institute REDS: Reconfigurable & Embedded Digital Systems
--
-- File			: GenericFullAdder.vhd
-- Description  : Cascadable full adder for one digit coded in binary (i.e. BCD)
--                Work with any number basis with condition 2^N_bits >= Basis
--
-- Author		: A. Fivaz
-- Date			: 10.03.2011
-- Version		: 0.0
--
-- Subject		: Semester thesis: Decimal Computation in FPGA
--
--| Updates |-----------------------------------------------------------------
-- Version   Author   Date               Description
--
------------------------------------------------------------------------------

library ieee;
    use ieee.std_logic_1164.all;
    use ieee.numeric_std.all;
	use work.bcd_configuration_package.all;

entity generic_full_adder is
	generic(
		Basis_g			: natural := 10	-- Number Basis_g 2^N_bits_c >= Basis_g
	);
    port(
        Carry_i 	    : in std_logic;
        Op_A_i			: in std_logic_vector(BCX_Bits(Basis_g)-1 downto 0);
        Op_B_i			: in std_logic_vector(BCX_Bits(Basis_g)-1 downto 0);
        Res_o			: out std_logic_vector(BCX_Bits(Basis_g)-1 downto 0);
        Carry_o     	: out std_logic
    );
	
end generic_full_adder;

architecture Mixed of generic_full_adder is

	constant N_bits_c	: natural := BCX_Bits(Basis_g);

-- Internal signals
	signal Carry_i_s	: Unsigned(0 downto 0);
	signal Op_A_s		: Unsigned(N_bits_c downto 0);
	signal Op_B_s		: Unsigned(N_bits_c downto 0);	
	signal Comp_s    	: Unsigned(N_bits_c downto 0);
	signal Somme_1_s	: Unsigned(N_bits_c downto 0);
	signal Somme_2_s	: Unsigned(N_bits_c downto 0);	

begin

	Carry_i_s(0) <= Carry_i;
	Op_A_s <= Unsigned('0' & Op_A_i(N_bits_c-1 downto 0));
	Op_B_s <= Unsigned('0' & Op_B_i(N_bits_c-1 downto 0));

-- First binary adder
	Somme_1_s <= Op_A_s + Op_B_s + Carry_i_s;

-- Comparator output
	--Comp_s <= Somme_1_s(4) OR Somme_1_s(3) AND Somme_1_s(2) OR  Somme_1_s(3) AND Somme_1_s(1); -- For decimal arithmetic
	--Comp_s <= To_Unsigned(6, N_bits_c) when (Somme_1_s > 9) else To_Unsigned(0, N_bits_c); -- For decimal arithmetic
	Comp_s <= To_Unsigned((2**N_bits_c)-Basis_g, N_bits_c+1) when (Somme_1_s > (Basis_g-1)) else To_Unsigned(0, N_bits_c+1);

-- Second binary adder (automatically removed when 2**N_bits_c=Basis_g)
	Somme_2_s <= Somme_1_s + Comp_s;	
	Carry_o <= Somme_2_s(N_bits_c);
	Res_o <= Std_Logic_Vector(Somme_2_s(N_bits_c-1 downto 0));

end Mixed;
