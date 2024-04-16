\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 4: SIMD**

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

- **10/03/2024**

\pagebreak

\hfill\break

\hfill\break

\center

*\[Page de mise en page, laissée vide par intention\]*

\raggedright

\pagebreak

## **Introduction**

Dans ce laboratoire il faut optimiser du code en utilisantr des operations SIMD

## **Optimisation du code**

### fonction 1 **distance**

Cette fonction calcule la distance euclidean entre deux pixels. Au lieu d'utiliser les coordonnées, on utilise la valeur RGB pour évaluer la distance.

Pour optimiser cette fonction on va charger les valeurs RGB des deux pixels dans des registres SIMD, puis on va soustraire les valeurs des deux pixels, on va ensuite multiplier les valeurs obtenues et les additionner pour obtenir la distance euclidean.

Grâce aux operations SIMD on peut effectuer ces opérations en parallèle sur les 4 composantes RGB(3 composants + 1 composant de padding).

### fonction 2 **kmeans_pp**

Cette fonction implémente l'algorithme kmeans++ pour initialiser les centres des clusters.

Cette fonction peux être optimisée en utilisant les operations SIMD pour calculer la distance euclidean entre les pixels et les centres des clusters sans fair appel a la fonction distance.

De plus les allocations dynamique peuvent être évitées en sauvegardant les valeurs RGB des pixels dans les registres SIMD, ici aussi on a un bit de padding pour avoir 4 valeurs dans un registre SIMD, ce dernier pourrait servir pour des images en RGBA.



![graph](graph.png){ width=80% }



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

```