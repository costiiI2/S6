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
- **YANN Thomas**
  
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

Il a fallut tout d'abord modifier le code VHDL fourni pour ajouter les fonctions des entrées/sorties demandées pour la communication avec le programme en C.

C'est à dire le clear des leds, l'écriture des hex 0 à 3 et le plus dur l'edge detection pour les boutons.

J'ai tout simplement repris le template du byteenable utilisé dans les autres fonctions pour le clear des leds, pour l'écriture des leds j'ai adaptée le code pour les hex 4_5 et changer les valeurs de sorties.

Pour la partie edge detection j'ai simplement créé une table de fonctions synchrone qui détecte les fronts montants et descendants des boutons et les stocke dans un registre:

- un reset
- un set
- un clear
- un maintien de l'état

Le code VHDL résultant est donc constitué de:

- un registre pour sauvegarder l'état des boutons au coup de clock avant.
- un registre pour sauvegarder l'état des boutons au coup de clock actuel(deja present dans le code).
- un registre pour sauver l'état de edge capture des boutons, le meme qui sera utiliser pour le clear de cette dernière.

Il ne faut pas oublier d'ajouter ce dernier registre dans le canal de lecture de l'IP.

## **Partie 2: Platform designer**

Dans cette partie j'ai utiliser le platform designer pour créer notre IP AXI4-Lite.

tout d'abord j'ai ajouter les signaux de l'interface AXI4-Lite, ensuite j'ai ajouter les signaux des entrées/sorites du conduit pour notre connexion avec l'IP et enfin les signaux de clock et reset. 

Une fois notre IP créée on peux l'ajouter et la connecter aux signaux present des anciens laboratoires les clocks, les resets et le HPS.

## **Partie 3: Code VHDL top**

Dans cette partie on a du modifier le code VHDL top pour ajouter notre IP et la connecter aux signaux de l'ancien laboratoire comme aux parties précédentes.


\pagebreak

## **Partie 4: Code C**

Le code C est très simple, il suit les contraintes du laboratoire.

Les fonctions implémentées sont les suivantes:

- set les leds
- lecture/écriture du edge capture des boutons
- écriture des 7 segments


Les autres fonctionnalités sont pas implémentées.

Mais peuvent être implémentées en ajoutant des fonctions dans le code C sans trop de difficulté en suivant le plan d'adressage de l'IP fourni.

Les fonctionnalités à implémenter seraient les suivantes:

- set les leds
- clear les leds
- lire les boutons

## **Conclusion**

Ce laboratoire m'a permis d'apprendre a créer une IP utilisant l'AXI4-Lite et de la connecter à un code VHDL et un code C.

J'ai eu quelque problème de compréhension a l'instantiation de l'IP dans le code VHDL top et dans le platform designer, le reste c'est passé sans trop de difficulté.