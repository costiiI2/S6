\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 9: Convolution**

\hfill\break

\hfill\break

Département: **TIC**

Unité d'enseignement: **SCF**

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\hfill\break

\raggedright

Auteur(s):

- **CECCHET Costantino**

Professeur:

- **DASSATTI Alberto**
- **YANN Thomas**
  
Assistant:

- **JACCARD Anthony I.**

Date:

- **22/05/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Ce laboratoire va nous permettre de développer de A à Z un système complet pour calculer une convolution sur FPGA.


-- Address map
    -- Offset (idx) | Data                        | RW
    -- -------------|-----------------------------|-----
    -- 0x00	    (0) | Constant (0xBADB100D)   | R
    -- --kernel registers-------------------------------
    -- 0x04	    (1) | kernel[0-3]             | R/W
    -- 0x08	    (2) | kernel[4-7]             | R/W
    -- 0x0C	    (3) | kernel[8],-,-,-         | R/W
    -- --image registers--------------------------------
    -- 0x10	    (4) | set img[0-3]      	  | W

    -- 0x1C         (7) | return value            | R
    -- --control registers------------------------------
    -- 0x20     (8) | Can read  / Can write  | R

vhdl :

- un registre can_read (1 byte) / can_write (1 byte) 




pseudo code c:

set_kernel(kernel){
        write(0x4) = kernel[0-3]
        ...0x8 = kernel[4-7]
        ...0xC = kernel[8]
}

set_img(img){
        write(0x10) = img[0-3]
        ...0x10 = img[4-7]
        ...0x10 = img[8]
}

while(i < taille img_res){
        if(can_write){
                set_img(img[j++])//4 premier pixel
                set_img(img[j++])//4 deuxieme pixel
                set_img(img[j++])//4 troisieme pixel
                increment_pixel();
                
        }
        if(can_read){
                img_result[i++] = read(0x1C);
                increment_pixel();
        }
}

psuedo code vhdl:

list[N][3]



calcul :process (clk)
off_set_a_calculer <= base_addr + offset;
begin
        if rising_edge(clk) then
                if valeurs_a_calculer > 0 then
                        -- calculated value
                        img[(idx_read + offset_a calculer)] <= img[]*kernel[]
                        valeurs_a_calculer--;
                        offset_a_calculer++;
                        if(offset_a_calculer == taille_liste){
                                offset_a_calculer <= 0
                        }
                end if
        end if
end process

case setting img pixels:
                set img[8] <= reg_32
                

case reading img pixels:
                reg_32 <= img[idx_read + 8 + 2]-- getting last 8 bit value in register
                idx_read++;
                if(idx_read == taille_liste){
                        idx_read <= 0
                }
                values_in_list--;
                if(values_in_list == 0){
                        can_read <= 0
                }

case setting kernel values:
                kernel[0-3] <= reg_32 


´´´vhdl
-- definition of the scfifo 256x32
GENERIC MAP (
		add_ram_output_register => "OFF",
		intended_device_family => "Cyclone V",
		lpm_numwords => 256,
		lpm_showahead => "OFF",
		lpm_type => "scfifo",
		lpm_width => 32,
		lpm_widthu => 8,
		overflow_checking => "ON",
		underflow_checking => "ON",
		use_eab => "ON"
	)
´´´