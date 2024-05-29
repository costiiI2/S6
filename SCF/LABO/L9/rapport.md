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

- un registre de idx_read (2 bytes)/ idx_write (2 bytes)
- un registre can_read (1 byte) / can_write (1 byte) 
- un registre taille liste (2 bytes)
- un registre nombre de valeurs dans la liste (2 bytes)

- lors de l'écriture de l'image on incrémente le nombre de valeurs dans la liste et l'idx_write
- lors de la lecture de l'image on décrémente le nombre de valeurs dans la liste et on incrémente l'idx_read
- les deux idx sont modulo la taille de la liste

- si # valeurs dans la liste == taille liste alors can_write = 0
- si # valeurs dans la liste == 0 alors can_read = 0
- return value est stocké dans le tableau de convolution a la dernière position
- registre du nombre de valeur a calculer
- registre offset a calculer


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
                
        }
        if(can_read){
                img_result[i++] = read(0x1C) + read(0x1C) + read(0x1C) + read(0x1C);
        }
}

psuedo code vhdl:

list[N][3]

img <= base_addr + 0x10 +(idx_write * 4)


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
                idx_write++;
                if(idx_write == taille_liste){
                        idx_write <= 0
                }
                values_in_list++;
                valeurs_a_calculer++;
                if(values_in_list == taille_liste){
                        can_write <= 0
                }

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
