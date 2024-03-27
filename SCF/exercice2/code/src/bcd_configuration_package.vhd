------------------------------------------------------------------------------
-- HEIG-VD, Haute Ecole d'Ingenierie et de Gestion du canton de Vaud
-- Institute REDS: Reconfigurable & Embedded Digital Systems
--
-- File			: bcd_configuration_package.vhd
-- Description  : Provide data types, functions and configuration parameters
--
-- Author		: A. Fivaz
-- Date			: 20.03.2011
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
	
	
package bcd_configuration_package is

	-- Define constants used to configure the implementation
	constant Basis_c : natural := 10;

	-- Define binary coded signal types
	--type BCX_vector is array (natural range <>) of Unsigned(N_bits-1 downto 0);

	-- Define generic functions
	function BCX_Bits(basisX : in natural) return natural ; -- return bit size of binary number coded basis X
	function BCY_Digits(basisY, basisX, N_Digits_X : in natural) return natural ; -- return digit size of binary number coded basis Y
	function Minimum(value1, value2 : in natural) return natural ;
	function Count_in_BCX(number, basis : in natural) return natural ; -- used to count, i.e. in decimal: +1 +1 .. + 7 +1 .. + (7+7*2^4)

end bcd_configuration_package;


package body bcd_configuration_package is

function BCX_Bits(basisX : in natural) return natural is
	-- Nb of bits needed to binary encode a digit in basis X
	variable N_bits_v      : integer;
	-- 2^N_bits_v
	variable Max_Value_v : integer;
begin
	Max_Value_v	:= 0;
	N_bits_v	:= 0;
 	while (Max_Value_v < basisX) loop
		N_bits_v	:= N_bits_v + 1;
		Max_Value_v	:= 2**N_bits_v;
	end loop;
return N_bits_v;
end BCX_Bits;

function Minimum(value1, value2 : in natural) return natural is
begin
    if value1 < value2 then return value1;
    else return value2;
    end if;
end Minimum;

function BCY_Digits(basisY, basisX, N_Digits_X : in natural) return natural is
	-- Nb of digits needed to binary encode a BCX number in basis Y
	variable N_digits_v : integer;
	-- 2^N_bits_v
	variable Max_Value_v : integer;
begin
	Max_Value_v	:= 1;
	N_digits_v	:= 1;
	while (Max_Value_v < basisX**N_Digits_X) loop
		N_digits_v	:= N_digits_v + 1;
		Max_Value_v	:= basisY**N_digits_v;
	end loop;
return N_digits_v;
end BCY_Digits;

function Count_in_BCX(number, basis : in natural) return natural is
	--used to count, i.e. in decimal: +1 +1 .. + 7 +1 .. + (7+7*2^4)
	variable N_digits_v, N_Steps_v	: natural;
	variable Count_Nr_v, N_bits_v	: natural;
	variable Count_v, Step_v		: natural;	
	
	
begin
	N_digits_v	:= 1;
	N_Steps_v	:=1;
	N_bits_v	:= BCX_Bits(basis);
	Count_v		:= 0;
	Step_v		:= 1;
	Count_Nr_v	:= 0;

	while (Count_v < number) loop
		Count_v := Count_v + 1;
		if (Count_v REM basis = 0) then
		-- Count_v=basis**N_digits_v*N_inc_v
			-- in decinal this loop adds +2^0 + 2^4 + 2^8 + 2^12 for a number of 4 digits		
			while (basis**N_digits_v <= number) loop
				Step_v := Step_v + ((2**N_bits_v)-basis) * (2**N_bits_v)**(N_digits_v-1);
				N_Steps_v := N_Steps_v + 1;
				N_digits_v := N_digits_v + 1;
			end loop;
			Count_Nr_v := Step_v;		
		else	
			Count_Nr_v := 1;
		end if;

	end loop;
	
return Count_Nr_v;
end Count_in_BCX;

end bcd_configuration_package;
