\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 03 – Utilisation des interruptions**

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

- **24/03/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Lors de ce laboratoire il a été demandé d'implémenter les interruptions sur la carte DE1-SoC.

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

## **Partie 2: Initialisation des interruption**

J'ai ensuite ajouté les interruptions pour les boutons KEY, grâce au platform designer j'ai ou activer les interruptions pour les HPS et ajouter aux KEY des interruptions de type **rising edge**.

J'ai ensuite complété le programme pour qu'il puisse gérer les interruptions comme vu dans le laboratioire Timer du cour ARE.

Une fois cela fait j'ai pu tester le programme et voir que les interruptions fonctionnaient correctement, j'ai donc pu programmer le code pour qu'il reponde aux exigences du laboratoire.

## **Tests**

Grâce à **Altera Monitor Program** j'ai pu tester le programme et que tout fonctionne correctement.

J'ai eu des problèmes car pour une certaines raison inconnue le programme des fois ne fonctionne pas avec des **printf()**. J'ai donc utiliser le debugger pour voir les valeurs des variables et voir si le programme fonctionnait correctement et repondait aux exigences du laboratoire.

## **Conclusion**

Ce laboratoire m'a permis de me familiariser avec les interruptions et de comprendre comment les utiliser et les instancier dans un programme.
