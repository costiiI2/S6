\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 5: Profiling**

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

- **CECCHET Costantino**

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

## HIGHWAY


the bench mark will be **./hw help**in the root folder of the project.

## time mesure

```bash
costi@Cos:~/Desktop/highway$ time ./hw help

...

real	0m0.014s
user	0m0.000s
sys	0m0.029s

```     

### perf stat fast fetch

```bash
 Performance counter stats for './hw help':

             10.97 msec task-clock                       #    2.418 CPUs utilized             
                45      context-switches                 #    4.103 K/sec                     
                11      cpu-migrations                   #    1.003 K/sec                     
               418      page-faults                      #   38.109 K/sec                     
        26,614,572      cycles                           #    2.426 GHz                       
        15,505,893      instructions                     #    0.58  insn per cycle            
         3,283,758      branches                         #  299.377 M/sec                     
            78,552      branch-misses                    #    2.39% of all branches           

       0.004536999 seconds time elapsed

       0.004070000 seconds user
       0.006783000 seconds sys
```

Ici on peut voir le nombre d'instruction par cycle, il n'est pas très bon, mais il pourrait être possible de l'améliorer.

## perf record

```bash
$ perf record -g ./hw help
[ perf record: Woken up 1 times to write data ]
[ perf record: Captured and wrote 0.007 MB perf.data (2 samples) ]

$ perf report
```

Grâce à perf record et perf report on peut voir les fonctions qui sont les plus utilisées et les fonctions qui sont les plus appelées.

Mais nous remarquons bien que la fonction **search_buffer** est la fonction qui est la plus appelée.
   
## hotspot

En utilisant hotspot on peut voir ou le programme passe le plus de temps.

Dans le fichier search.c on a la fonction **search_buffer** qui est appeler le plus souvent.

Cette fonction est applée dans le thread **search_thread** qui est un des deux threads qui sont crées., le deuxième thread sert a l'affichage des résultats.

Cette partie du thread l.169 **worker.c** appelle la fonction search qui cherche un pattern dans un buffer, pour ce faire une fois le buffer chargé, on appelle la fonction **search_buffer** qui est la fonction la plus appelée, qui fait la recherche du pattern dans le buffer.

Apres analyse cette fonction est deja très bien optimiser même les sous appelle de fonction le sont, mais il serait possible de mieux paralléliser le code en plusieurs threads.

Il serait aussi possible de changer l'algorithme de recherche pour un algorithme plus rapide tel que **Boyer-Moore** ou **Knuth-Morris-Pratt**.

On pourrait aussi créer une fonction **memchr()** qui pourrait être optimisée pour notre cas spécifique.

Ces optimisations reste très compliquées et demandent beaucoup de temps et de ressources.

ce programme est déjà très bien optimisé.

## conclusion

Le programme est déjà très bien optimisé, il est difficile de trouver des optimisations qui pourraient être faites.

Mais nous avons appris à utiliser des outils de profiling qui nous permettent de voir ou le programme passe le plus de temps et de voir les fonctions qui sont les plus appelées.

Ceci pourrait permettre par la suite d'optimiser certaines parties du code, qui ne sont pas evidentes à première vue.

## **Environnement d'execution**

Le système décrit dispose d'un processeur Intel Core i7-8550U avec 8 threads, répartis sur 4 cœurs physiques. La fréquence du processeur est de 1,80 GHz avec une fréquence mesurée de 1432,548 MHz. 

Voici plus ample information sur le processeur:

```bash
$ cat /proc/cpuinfo 
processor	: 0
vendor_id	: GenuineIntel
cpu family	: 6
model		: 142
model name	: Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
stepping	: 10
microcode	: 0xf4
cpu MHz		: 1432.548
cache size	: 8192 KB
physical id	: 0
siblings	: 8
core id		: 0
cpu cores	: 4
apicid		: 0
initial apicid	: 0
fpu		: yes
fpu_exception	: yes
cpuid level	: 22
wp		: yes
flags		: fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefetch cpuid_fault epb invpcid_single pti ssbd ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid ept_ad fsgsbase tsc_adjust sgx bmi1 avx2 smep bmi2 erms invpcid mpx rdseed adx smap clflushopt intel_pt xsaveopt xsavec xgetbv1 xsaves dtherm ida arat pln pts hwp hwp_notify hwp_act_window hwp_epp md_clear flush_l1d arch_capabilities
vmx flags	: vnmi preemption_timer invvpid ept_x_only ept_ad ept_1gb flexpriority tsc_offset vtpr mtf vapic ept vpid unrestricted_guest ple pml ept_mode_based_exec
bugs		: cpu_meltdown spectre_v1 spectre_v2 spec_store_bypass l1tf mds swapgs itlb_multihit srbds mmio_stale_data retbleed gds
bogomips	: 3999.93
clflush size	: 64
cache_alignment	: 64
address sizes	: 39 bits physical, 48 bits virtual

$ likwid-topology 
--------------------------------------------------------------------------------
CPU name:	Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
CPU type:	Intel Kabylake processor
CPU stepping:	10
********************************************************************************
Hardware Thread Topology
********************************************************************************
Sockets:		1
Cores per socket:	4
Threads per core:	2
--------------------------------------------------------------------------------
HWThread	Thread		Core		Socket		Available
0		0		0		0		*
1		0		1		0		*
2		0		2		0		*
3		0		3		0		*
4		1		0		0		*
5		1		1		0		*
6		1		2		0		*
7		1		3		0		*
--------------------------------------------------------------------------------
Socket 0:		( 0 4 1 5 2 6 3 7 )
--------------------------------------------------------------------------------
********************************************************************************
Cache Topology
********************************************************************************
Level:			1
Size:			32 kB
Cache groups:		( 0 4 ) ( 1 5 ) ( 2 6 ) ( 3 7 )
--------------------------------------------------------------------------------
Level:			2
Size:			256 kB
Cache groups:		( 0 4 ) ( 1 5 ) ( 2 6 ) ( 3 7 )
--------------------------------------------------------------------------------
Level:			3
Size:			8 MB
Cache groups:		( 0 4 1 5 2 6 3 7 )
--------------------------------------------------------------------------------
********************************************************************************
NUMA Topology
********************************************************************************
NUMA domains:		1
--------------------------------------------------------------------------------
Domain:			0
Processors:		( 0 4 1 5 2 6 3 7 )
Distances:		10
Free memory:		542.145 MB
Total memory:		7695.82 MB
--------------------------------------------------------------------------------

```