\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 2: Utilisation des I/O de la partie FPGA via le HPS**

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

- **01/03/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Lors de ce laboratoire il a été demandé de réaliser un simple programme pour gérer les entrées et sorties de la DE1-SoC.

## **Partie 1: Utilisation des I/O de la partie FPGA via le HPS**

j'ai commencé par utiliser le laboratoire précédent comme base et grâce au platform designer j'ai ajouter des PIO pour tout les composants de la DE1-SoC.

voici un tableau récapitulatif des PIO utilisés avec leur adresse:

| Nom du PIO | Adresse offset | taille (bit) |
|------------|---------|--------|
| LED        | 0x0000  | 10     |
| KEY        | 0x0010  | 4      |
| SW         | 0x0020  | 10     |
| HEX0_3     | 0x0030  | 28     |
| HEX4_5     | 0x0040  | 14     |

Ces adresses sont les adresses de base par rapport au bridge, il faut y ajouter l'adresse de base (**0xFF200000**) de la partie HPS pour obtenir l'adresse finale.

Il a fallu lier le bridge la clock et les PIO entre eux pour que le programme puisse les utiliser.

## **Partie 2: Utilisation des I/O depuis notre programme**

J'ai ensuite créé un programme en C pour utiliser les PIO.

Le programme est basée sur la donnée fournie il est assez simple.

J'ai pu récupérer des fonctions de mes ancien laboratoires pour lire et écrire dans les PIO ceci a grandement simplifié la tâche.

Rien de spécial à signaler pour cette partie.

## **Tests**

Grâce à **Altera Monitor Program** j'ai pu tester le programme et que tout fonctionne correctement.

J'ai eu des problèmes car certaines fois même en modifiants des fichier le programme ne se mettait pas à jour, j'ai du redémarrer le projet pour que les modifications soient prises en compte.

J'ai aussi eu l'occasion de tester grâce au debugger le code assembleur généré par le compilateur et m'assurer que tout fonctionne correctement c'est notamment dans cette partie que je me suis rendu compte que le programme ne se mettait pas à jour.

## **Conclusion**

Ce laboratoire m'a permis de me familiariser avec les PIO et de comprendre comment les utiliser et les instancier dans un programme.
