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

## **Partie 2: Platform designer**

Dans cette partie on an utilisee le platform designer pour créer notre IP AXI4-Lite.

tout d'abord on à ajouter les signaux de l'interface AXI4-Lite, ensuite on a ajouté les signaux de les entrées7sorites du conduit pour notre connexion avec l'IP et enfin les signaux de clock et reset. 

Une fois notre IP créée on peux l'ajouter et la connecter aux signaux present des anciens laboratoires les clocks, les resets et le HPS.

## **Partie 3: Code VHDL top**

Dans cette partie on a du modifier le code VHDL top pour ajouter notre IP et la connecter aux signaux de l'ancien laboratoire comme au partie précédente.

## **Partie 4: Code C**

Le code C est très simple, il suit les contraintes du laboratoire, c'est à dire de clear les leds, d'écrire les hex 0 à 3 et de lire les boutons grâce à l'edge detection.

Les autres fonctionnalités sont pas implémentées.

Mais peuvent être implémentées en ajoutant des fonctions dans le code C sans trop de difficulté en suivant le plan d'adressage de l'IP fourni.

Les fonctionnalités à implémenter seraient les suivantes:

- set les leds
- clear les leds
- lire les boutons

## **Conclusion**

Ce laboratoire m'a permis d'apprendre a créer une IP AXI4-Lite et de la connecter à un code VHDL et un code C.
