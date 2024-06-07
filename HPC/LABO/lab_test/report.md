\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire test: uftrace**

\hfill\break

\hfill\break

Département: **TIC**

Unité d'enseignement: **HPC**

\hfill\break

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

- **BOUGNON Kévin, CECCHET Costantino**

Professeur:

- **DASSATTI Alberto**
  
Assistant:

- **DA ROCHA CARVALHO Bruno**

Date:

- **28/02/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Uftrace est un programme d'analyse de performance. Ce programme permet de record des timestamps des entrées et sorties de fonctions, il permet aussi d'enregistrer les arguments des entrées et sorties des fonctions et aussi permet d'enregistrer les fonctions user et kernel.

## **Installation**

Ce programme est trés facilment instalable sur linux.

Une fois le repo cloné il faut exectuer les commandes suivantes :
'''bash
sudo misc/install-deps.sh  # optionel pour debloquer des features avancées
./configure		   # --prefix peux être utilisé pour changer le path d'installation
make
sudo make install
'''

Sur debian ceci est encore plus simple :

'''bash
apt install uftrace
'''

## **Guide d'utilisation**

Pour utiliser ce programme il faut avoir des symbols de debug avec les option de compilation **-pg** et **-P**.

Puis plusieurs modes sont disponible, le plus basic est :

'''bash
uftrace <nom_programme> <arguments>
'''

Ceci va génerer un graph d'appel de fonction avec le temps d'execution de chaque call.
'''bash
# DURATION     TID     FUNCTION
   0.449 us [ 69217] | __monstartup();
   0.206 us [ 69217] | __cxa_atexit();
            [ 69217] | main() {
  12.722 us [ 69217] |   __fprintf_chk();
  13.622 us [ 69217] | } /* main */
'''
d'autre option sont disponible et sont expliquée sur le git.

Une option tres utile est celle de **-A** et **-R**, qui sont les options pour sauvgarder les paramétres d'appel et de retour d'une fonciton.

l'example donné sur le git explique très bien le foncitonnement de ces options grâce a fibonacci.

'''bash
$ uftrace record -A fib@arg1 -R fib@retval tests/t-fibonacci 5

$ uftrace replay
# DURATION    TID     FUNCTION
   2.853 us [22080] | __monstartup();
   2.194 us [22080] | __cxa_atexit();
            [22080] | main() {
   2.706 us [22080] |   atoi();
            [22080] |   fib(5) {
            [22080] |     fib(4) {
            [22080] |       fib(3) {
   7.473 us [22080] |         fib(2) = 1;
   0.419 us [22080] |         fib(1) = 1;
  11.452 us [22080] |       } = 2; /* fib */
   0.460 us [22080] |       fib(2) = 1;
  13.823 us [22080] |     } = 3; /* fib */
            [22080] |     fib(3) {
   0.424 us [22080] |       fib(2) = 1;
   0.437 us [22080] |       fib(1) = 1;
   2.860 us [22080] |     } = 2; /* fib */
  19.600 us [22080] |   } = 5; /* fib */
  25.024 us [22080] | } /* main */
'''

Ceci peux être très utile pour voir si dans notre programme des valeurs d'entrée pausent problème a nos algorhitme.

## **avantages et inconveniants**

Ce programme est très pratique car il montre le call graph et le temps par fonction ce qui nous permet très rapidement de trouver la ou notre code passe le plus de temps. De plus on peux ajouter une option pour avoir les appels aux fonctions kernel **-K** ce qui peux nous montrer si nos fonction sont lentes a cause des appels kernel ou du côter user.

L'inconvenient est la restriction de plateformme utilisable que linux et le peux de programme analysable que du C,C++,Rust et python. Mais néanmoins ça reste de excellent language de programmation comparer au javomis.

## **Conclusion**

Ce laboratoire nous a permis de voir un programme de plus de profiling, ce dernier est très facilment utilisable et plus simple d'analyse que d'autres programmes de profiling, il reste basique et donc, n'est pas pratique pour faire du profiling très pousser.
