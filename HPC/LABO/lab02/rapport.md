\topmargin = -30pt
\textheight = 625pt

\center

# **Laboratoire 2: likwid**

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

- **DASSATI Alberto**
  
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

```bash

array perf on 100x100 image

+-----------------------------------+--------------+
|               Metric              |  HWThread 0  |
+-----------------------------------+--------------+
|        Runtime (RDTSC) [s]        |       0.0011 |
|        Runtime unhalted [s]       |       0.0007 |
|            Clock [MHz]            |    1197.2860 |
|                CPI                |       0.4147 |
|             Energy [J]            |       0.0038 |
|             Power [W]             |       3.4010 |
|          Energy DRAM [J]          |       0.0007 |
|           Power DRAM [W]          |       0.6034 |
|            DP [MFLOP/s]           |      53.9394 |
|          AVX DP [MFLOP/s]         |            0 |
|          Packed [MUOPS/s]         |            0 |
|          Scalar [MUOPS/s]         |      53.9394 |
|  Memory load bandwidth [MBytes/s] |     713.1326 |
|  Memory load data volume [GBytes] |       0.0008 |
| Memory evict bandwidth [MBytes/s] |      25.3088 |
| Memory evict data volume [GBytes] | 2.816000e-05 |
|    Memory bandwidth [MBytes/s]    |     738.4414 |
|    Memory data volume [GBytes]    |       0.0008 |
|       Operational intensity       |       0.0730 |
+-----------------------------------+--------------+


linked list perf on 100x100 image

+-----------------------------------+------------+
|               Metric              | HWThread 0 |
+-----------------------------------+------------+
|        Runtime (RDTSC) [s]        |     0.0161 |
|        Runtime unhalted [s]       |     0.0111 |
|            Clock [MHz]            |  1541.9603 |
|                CPI                |     0.8392 |
|             Energy [J]            |     0.0515 |
|             Power [W]             |     3.1966 |
|          Energy DRAM [J]          |     0.0087 |
|           Power DRAM [W]          |     0.5422 |
|            DP [MFLOP/s]           |     3.7040 |
|          AVX DP [MFLOP/s]         |          0 |
|          Packed [MUOPS/s]         |          0 |
|          Scalar [MUOPS/s]         |     3.7040 |
|  Memory load bandwidth [MBytes/s] |  1215.9650 |
|  Memory load data volume [GBytes] |     0.0196 |
| Memory evict bandwidth [MBytes/s] |   279.4912 |
| Memory evict data volume [GBytes] |     0.0045 |
|    Memory bandwidth [MBytes/s]    |  1495.4562 |
|    Memory data volume [GBytes]    |     0.0241 |
|       Operational intensity       |     0.0025 |
+-----------------------------------+------------+

half life 2000X2090 1d
+-----------------------------------+------------+
|               Metric              | HWThread 0 |
+-----------------------------------+------------+
|        Runtime (RDTSC) [s]        |     0.1351 |
|        Runtime unhalted [s]       |     0.2518 |
|            Clock [MHz]            |  3853.8412 |
|                CPI                |     0.3739 |
|             Energy [J]            |     1.9739 |
|             Power [W]             |    14.6140 |
|          Energy DRAM [J]          |     0.0655 |
|           Power DRAM [W]          |     0.4849 |
|            DP [MFLOP/s]           |   185.6797 |
|          AVX DP [MFLOP/s]         |          0 |
|          Packed [MUOPS/s]         |          0 |
|          Scalar [MUOPS/s]         |   185.6797 |
|  Memory load bandwidth [MBytes/s] |   736.0696 |
|  Memory load data volume [GBytes] |     0.0994 |
| Memory evict bandwidth [MBytes/s] |   346.2958 |
| Memory evict data volume [GBytes] |     0.0468 |
|    Memory bandwidth [MBytes/s]    |  1082.3654 |
|    Memory data volume [GBytes]    |     0.1462 |
|       Operational intensity       |     0.1715 |
+-----------------------------------+------------+


half life 2000X2090 linkedList

+-----------------------------------+------------+
|               Metric              | HWThread 0 |
+-----------------------------------+------------+
|        Runtime (RDTSC) [s]        |   109.5668 |
|        Runtime unhalted [s]       |   213.9491 |
|            Clock [MHz]            |  3937.0027 |
|                CPI                |     3.0855 |
|             Energy [J]            |  1337.5980 |
|             Power [W]             |    12.2081 |
|          Energy DRAM [J]          |    42.4966 |
|           Power DRAM [W]          |     0.3879 |
|            DP [MFLOP/s]           |     0.2288 |
|          AVX DP [MFLOP/s]         |          0 |
|          Packed [MUOPS/s]         |          0 |
|          Scalar [MUOPS/s]         |     0.2288 |
|  Memory load bandwidth [MBytes/s] |   402.1063 |
|  Memory load data volume [GBytes] |    44.0575 |
| Memory evict bandwidth [MBytes/s] |   171.8875 |
| Memory evict data volume [GBytes] |    18.8332 |
|    Memory bandwidth [MBytes/s]    |   573.9938 |
|    Memory data volume [GBytes]    |    62.8907 |
|       Operational intensity       |     0.0004 |
+-----------------------------------+------------+


med 1D 1267X919

+-----------------------------------+------------+
|               Metric              | HWThread 0 |
+-----------------------------------+------------+
|        Runtime (RDTSC) [s]        |    21.5682 |
|        Runtime unhalted [s]       |    38.8379 |
|            Clock [MHz]            |  3706.7681 |
|                CPI                |     3.1187 |
|             Energy [J]            |   339.6508 |
|             Power [W]             |    15.7478 |
|          Energy DRAM [J]          |    24.7045 |
|           Power DRAM [W]          |     1.1454 |
|            DP [MFLOP/s]           |     0.3237 |
|          AVX DP [MFLOP/s]         |          0 |
|          Packed [MUOPS/s]         |          0 |
|          Scalar [MUOPS/s]         |     0.3237 |
|  Memory load bandwidth [MBytes/s] |  2640.9210 |
|  Memory load data volume [GBytes] |    56.9599 |
| Memory evict bandwidth [MBytes/s] |  1237.0837 |
| Memory evict data volume [GBytes] |    26.6816 |
|    Memory bandwidth [MBytes/s]    |  3878.0047 |
|    Memory data volume [GBytes]    |    83.6415 |
|       Operational intensity       |     0.0001 |
+-----------------------------------+------------+

med LinkedList 1267X919

+-----------------------------------+------------+
|               Metric              | HWThread 0 |
+-----------------------------------+------------+
|        Runtime (RDTSC) [s]        |     0.0437 |
|        Runtime unhalted [s]       |     0.0702 |
|            Clock [MHz]            |  3296.7614 |
|                CPI                |     0.3742 |
|             Energy [J]            |     0.4641 |
|             Power [W]             |    10.6292 |
|          Energy DRAM [J]          |     0.0253 |
|           Power DRAM [W]          |     0.5802 |
|            DP [MFLOP/s]           |   160.0219 |
|          AVX DP [MFLOP/s]         |          0 |
|          Packed [MUOPS/s]         |          0 |
|          Scalar [MUOPS/s]         |   160.0219 |
|  Memory load bandwidth [MBytes/s] |  1057.4279 |
|  Memory load data volume [GBytes] |     0.0462 |
| Memory evict bandwidth [MBytes/s] |   572.2354 |
| Memory evict data volume [GBytes] |     0.0250 |
|    Memory bandwidth [MBytes/s]    |  1629.6632 |
|    Memory data volume [GBytes]    |     0.0711 |
|       Operational intensity       |     0.0982 |
+-----------------------------------+------------+


nyc 1150X710

+-----------------------------------+------------+
|               Metric              | HWThread 0 |

+-----------------------------------+------------+
|               Metric              | HWThread 0 |
+-----------------------------------+------------+
|        Runtime (RDTSC) [s]        |     0.0292 |
|        Runtime unhalted [s]       |     0.0510 |
|            Clock [MHz]            |  3580.3446 |
|                CPI                |     0.3877 |
|             Energy [J]            |     0.4334 |
|             Power [W]             |    14.8648 |
|          Energy DRAM [J]          |     0.0120 |
|           Power DRAM [W]          |     0.4124 |
|            DP [MFLOP/s]           |   168.0232 |
|          AVX DP [MFLOP/s]         |          0 |
|          Packed [MUOPS/s]         |          0 |
|          Scalar [MUOPS/s]         |   168.0232 |
|  Memory load bandwidth [MBytes/s] |   661.8081 |
|  Memory load data volume [GBytes] |     0.0193 |
| Memory evict bandwidth [MBytes/s] |   119.9101 |
| Memory evict data volume [GBytes] |     0.0035 |
|    Memory bandwidth [MBytes/s]    |   781.7181 |
|    Memory data volume [GBytes]    |     0.0228 |
|       Operational intensity       |     0.2149 |
+-----------------------------------+------------+




 LinkedList 1150X710

+-----------------------------------+------------+
|               Metric              | HWThread 0 |
+-----------------------------------+------------+
|        Runtime (RDTSC) [s]        |    12.4036 |
|        Runtime unhalted [s]       |    24.4077 |
|            Clock [MHz]            |  3965.7995 |
|                CPI                |     3.0660 |
|             Energy [J]            |   139.0184 |
|             Power [W]             |    11.2079 |
|          Energy DRAM [J]          |     3.9324 |
|           Power DRAM [W]          |     0.3170 |
|            DP [MFLOP/s]           |     0.3947 |
|          AVX DP [MFLOP/s]         |          0 |
|          Packed [MUOPS/s]         |          0 |
|          Scalar [MUOPS/s]         |     0.3947 |
|  Memory load bandwidth [MBytes/s] |   209.3547 |
|  Memory load data volume [GBytes] |     2.5968 |
| Memory evict bandwidth [MBytes/s] |    70.6243 |
| Memory evict data volume [GBytes] |     0.8760 |
|    Memory bandwidth [MBytes/s]    |   279.9790 |
|    Memory data volume [GBytes]    |     3.4728 |
|       Operational intensity       |     0.0014 |
+-----------------------------------+------------+




```

For array perf 100x100 we have a maxperf 53.9394 MFLOP/s and a max bandwidth of 738.4414 MBytes/s and an operational intensity of 0.0730

For linked list perf 100x100 we have a maxperf 3.7040 MFLOP/s and a max bandwidth of 1495.4562 MBytes/s and an operational intensity of 0.0025

For half life 2000x2090 we have a maxperf 185.6797 MFLOP/s and a max bandwidth of 1082.3654 MBytes/s and an operational intensity of 0.1715

For half life 2000x2090 linked list we have a maxperf 0.2288 MFLOP/s and a max bandwidth of 573.9938 MBytes/s and an operational intensity of 0.0004

For medaillon 1267x919  we have a maxperf 160.0219 MFLOP/s and a max bandwidth of 1629.6632 MBytes/s and an operational intensity of 0.0982

For medaillon 1267x919 linked list we have a maxperf 0.3237 MFLOP/s and a max bandwidth of 3878.0047 MBytes/s and an operational intensity of 0.0001

For nyc 1150x710 we have a maxperf 168.0232 MFLOP/s and a max bandwidth of 781.7181 MBytes/s and an operational intensity of 0.2149

For nyc 1150x710 linked list we have a maxperf 0.3947 MFLOP/s and a max bandwidth of 279.9790 MBytes/s and an operational intensity of 0.0014


53.9394 0.0730 sml1D
3.7040 0.0025 smlLL
185.6797 0.1715 hl1D
0.2288 0.0004 hlLL
160.0219 0.0982 med1D
0.3237 0.0001 medLL
168.0232 0.2149 nyc1D
0.3947 0.0014 nycLL

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