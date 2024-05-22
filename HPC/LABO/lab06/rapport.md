\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 6: Mesure de consommation énergétique**

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

- **22/05/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Dans ce laboratiore nous allons mesurer la consommation énergétique d'un programme en utilisant différents outils de mesures.

## **Programme de test**

Le programme de test est un programme C qui remplit un tableau de taille definie avec des 1, puis effectue une somme de tous les éléments du tableau. Le programme est le suivant:

```c

int main() {
    float *array = (float *)malloc(SIZE * sizeof(float));
    
    for (size_t i = 0; i < SIZE; i++) {
        array[i] = 1.0f;
    }

    float sum = sum_non_vectorized(array, SIZE);
    printf("Somme non vectorisée : %f\n", sum);

    free(array);
    return 0;
}
```

Deux versions de la fonction de somme sont disponibles, une version non vectorisée et une version vectorisée, elles sont dans les fichiers 'lab06.c' et 'lab06_vec.c' respectivement.

## **Mesure de consommation énergétique**

### **likwid-powermeter**

La première méthode de mesure de consommation énergétique est l'utilisation de l'outil `likwid-powermeter`. Cet outil permet de mesurer la consommation énergétique d'un programme en utilisant les compteurs de performance du processeur.


voici la commande pour mesurer la consommation énergétique des programmes non vectorisés et vectorisés :

```bash
$ likwid-power.sh
```

Et voici les résultats obtenus :

| Mesures | Non-Vectoriser | Vectoriser |
|-------------|----------------|------------|
|Runtime (s)|0.0208941|0.00642727|
|PKG (Joules)|0.0491333|0.0325317|
|PKG (Watt)|2.35154|5.06152|
|PP0 (Joules)|0.0143433|0.0186768|
|PP0 (Watt)|0.686475|2.90586|
|PP1 (Joules)|0.173218|0.105957|
|PP1 (Watt)|8.29028|16.4856|
|DRAM (Joules)|0.00683594|0.0055542|
|DRAM (Watt)|0.327171|0.864162|
|PLATFORM (Joules)|0.173218|0.105957|
|PLATFORM (Watt)|8.29028|16.4856|

### **perf**

La deuxième méthode de mesure de consommation énergétique est l'utilisation de l'outil `perf`. Cet outil permet de mesurer la consommation énergétique d'un programme en utilisant les compteurs de performance du processeur.

voici la commande pour mesurer la consommation énergétique des programmes non vectorisés et vectorisés :

```bash
$ perf stat -e power/energy-pkg/ -e power/energy-pp0/ -e power/energy-pp1/ -e power/energy-ram/ -e power/energy-pkg/ -e power/energy-cores/ -e power/energy-gpu/ ./lab06
```

Et voici les résultats obtenus :





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