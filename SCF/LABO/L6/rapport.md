\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 6: Driver de base**

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

- **22/04/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Lors de ce laboratoire il a été demandé de réaliser un driver pour faire marcher l'IP faite dans le laboratoire précédent.

## **Implémentation**

Avant de faire le driver j'ai rajouter un noeud au DT pour l'IP.

```c
    gnegne {
		compatible = "SCF";
		status = "okay";
	};
```

Grâce a ca le driver peux être instancier.

Les commande pour recompiler le DT reste les même que le laboratoire précédent, et les étapes aussi.

Le driver va map l'adresse de l'IP et cree une entrée (/dev/de1_io) et grâce a cette entrée on peux lire et écrire sur l'IP depuis notre driver.

On utilise IOCTL pour changer l'index des registre de l'IP avant de lire ou écrire.

Les fonctions sont tres simple une lecture, une écriture et une pour IOCTL, rien de particulier.

notre driver peux être utiliser que par un seul processus a la fois, pour cela on utilise une variable qui va nous dire si le driver est en cours d'utilisation.

## **Programme C**

Le programme en C est très simple, il ouvre le fichier /dev/de1_io et écrit et lit sur l'IP.

Il a fallu adapter les fonctions de lecture et d'écriture pour qu'elles prennent en compte l'index des registres et pas les offsets.

Pour passer d'un mode d'utilisation à l'autre il suffit de changer la valeur de la variable `MODE`au début du fichier.

## **Conclusion**

Ce laboratoire m'a permis de comprendre comment utiliser un driver pour faire marcher une IP et comment utiliser IOCTL.

