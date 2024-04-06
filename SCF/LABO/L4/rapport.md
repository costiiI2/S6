\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 04 – IP AXI4-Lite avec FPGA I/O**

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
- **YANN Thoma**
  
Assistant:

- **JACCARD Anthony I.**

Date:

- **06/04/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Lors de ce laboratoire il a été demandé d'implementer les entrées/sorties en utilisant une IP AXI4-Lite pour communiquer avec la partie FPGA de la carte DE1-SoC.

## **Partie 1: Code VHDL**

Il a fallut tout d'abord modifier le code VHDL fourni pour ajouter les entrées/sorties demandées pour la communication avec le programme en C.

C'est à dire le clear des leds, l'écriture des hex 0 à 3 et le plus dur l'edge detection pour les boutons.



## **Partie 2: Utilisation des I/O depuis notre programme**



## **Tests**

Grâce à **Altera Monitor Program** j'ai pu tester le programme et que tout fonctionne correctement.

J'ai eu des problèmes car certaines fois même en modifiants des fichier le programme ne se mettait pas à jour, j'ai du redémarrer le projet pour que les modifications soient prises en compte.

J'ai aussi eu l'occasion de tester grâce au debugger le code assembleur généré par le compilateur et m'assurer que tout fonctionne correctement c'est notamment dans cette partie que je me suis rendu compte que le programme ne se mettait pas à jour.

## **Conclusion**

Ce laboratoire m'a permis de me familiariser avec les PIO et de comprendre comment les utiliser et les instancier dans un programme.
