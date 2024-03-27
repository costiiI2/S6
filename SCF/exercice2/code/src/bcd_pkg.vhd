-----------------------------------------------------------------------
-- HEIG-VD, Haute Ecole d'Ingenerie et de Gestion du Canton de Vaud
-- Institut REDS
--
-- Composant    : bcd_pkg
-- Description  : Paquetage déclarant deux types pour l'usage de nombres
--                codés en BCD.
-- Auteur       : Yann Thoma
-- Date         : 01.03.2017
-- Version      : 1.0
--
-- Modification : -
--
-----------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;

package bcd_pkg is

subtype bcd_digit is std_logic_vector(3 downto 0);

type bcd_number is array(integer range <>) of bcd_digit;

end package bcd_pkg;
