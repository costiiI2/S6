# rapport lab01

Lors de ce laboratiore, nous avons du appliquer des filtres a des images pour faire une detection de contours.

en voici les résultats:

### medallion
```bash
$ time ./lab01 ../images/medalion.png out_.png 1

real    0m0.149s
user    0m0.117s
sys     0m0.032s

$ time  ./lab01 ../images/medalion.png out_chained.png 2
start conversion

real    0m0.403s
user    0m0.319s
sys     0m0.083s
```

### half-life
```bash
$ time ./lab01 ../images/half-life.png out_.png 1

real    0m0.325s
user    0m0.294s
sys     0m0.031s
$ time ./lab01 ../images/half-life.png out_.png 2
start conversion

real    0m1.387s
user    0m1.028s
sys     0m0.359s
```

### nyc

```bash
$ time ./lab01 ../images/nyc.png out_.png 1

real    0m0.149s
user    0m0.137s
sys     0m0.013s
$ time ./lab01 ../images/nyc.png out_.png 2
start conversion

real    0m0.369s
user    0m0.317s
sys     0m0.052s
```

### small
```bash
$ time ./lab01 ../images/50x50.png out_.png 1


real    0m0.035s
user    0m0.025s
sys     0m0.011s
$ time ./lab01 ../images/50x50.png out_.png 2
start conversion

real    0m0.082s
user    0m0.066s
sys     0m0.016s
```

![comparaison](../lab01/comp.png)

Nous voyons que la liste chainée ralentit le programme.

ceci est du au fait de devoir parcourir la liste chainée lors de l'application du filtre, dans notre programme nous essayons de minimiser le nombre de parcours de la liste chainée en utilisant des pointeurs judicieusement placés.

Mais neanmoins le parcours doit quand meme se faire et plus l'image est grande plus le temps de parcours est long.