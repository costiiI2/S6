# rapport lab01

Lors de ce laboratoire, nous avons du appliquer des filtres a des images pour faire une detection de contours.

en voici les résultats:

![comparaison](../lab01/comp.png)

Nous voyons que la liste chaînée est bien plus lente,ceci est du au fait de devoir parcourir la liste chaînée lors de l'application du filtre, dans notre programme nous essayons de minimiser le nombre de parcours de la liste chaînée en utilisant des pointeurs judicieusement placés.

Mais néanmoins le parcours doit quand meme se faire et plus l'image est grande plus le temps de parcours est long.

### temps d'execution pour les images de test

```bash
$ time  ./lab01 ../images/medalion.png med_.png 1
width: 1267
height: 919

real    0m0.131s
user    0m0.089s
sys     0m0.042s

$ time  ./lab01 ../images/medalion.png med_chain.png 2
start conversion

real    0m19.137s
user    0m19.067s
sys     0m0.069s

$ time  ./lab01 ../images/half-life.png h.png 1
width: 2000
height: 2090

real    0m0.288s
user    0m0.258s
sys     0m0.030s

$ time  ./lab01 ../images/half-life.png hal_chain.png 2
start conversion

real    1m49.654s
user    1m49.343s
sys     0m0.300s

$ time  ./lab01 ../images/nyc.png ny.png 1
width: 1150
height: 710

real    0m0.106s
user    0m0.102s
sys     0m0.004s

$ time  ./lab01 ../images/nyc.png ny_chain.png 2
start conversion

real    0m11.807s
user    0m11.740s
sys     0m0.066s

$ time  ./lab01 ../images/small_img.png small_img.png 1
width: 348
height: 348

real    0m0.040s
user    0m0.037s
sys     0m0.004s

$ time  ./lab01 ../images/small_img.png small_img_chain.png 2
start conversion

real    0m0.573s
user    0m0.546s
sys     0m0.028s

```

### Machine de test

Tout les tests ont été effectués sur un ordinateur avec les caractéristiques suivantes:

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
power management:



```