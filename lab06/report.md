# Rapport LABO6 HPC
## Bastien Pillonel
## Date : 27.04.2024
### Config : 
- Distrib PopOS 64bits (Ubuntu 22.04)
- AMD Ryzen 7 5800X 8-Core Processor
- AMD K19 (Zen3) architecture
- gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
- Library SIMD : immintrin.h

### Introduction

Dans ce laboratoire il est demandé d'analyzer la consommation du programme du lab04 sur la segmentation vectorisée et non-vectorisée. Il est demandé de choisir au moins 3 méthodes listée dans la donnée.

### Préparation de l'executable

Dans le CMakeLists j'ai set les flags de compilation suivants:

```
# Non vectorize version
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -Wall -fno-tree-vectorize")

# Vectorize version
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -Wall -fno-inline -march=native -mavx2")
```

Dans le code du fichier kmeans.c mettre définir ou non le define VECTORIZE en fonction de quel code est executé.

### Analyze globale de la consommation avec l'outil perf

Pour cette partie j'ai écrit le script [meas_perf_global.sh](./code/meas_perf_global.sh) qui permet de mesurer les consommation sur les 4 images avec 3 taille de cluster différent. 

En faisant un ```perf list```j'ai remarqué que seul l'event power/energy-pkg/ était disponible sur ma machine.

Résultats non vectorize:

```sh
bpil@pop-os:~/HEIG/HPC/HPC_LAB/lab06/code$ ./meas_perf_global.sh 

 Performance counter stats for 'system wide':

              2.45 Joules power/energy-pkg/                                                     

       0.486036355 seconds time elapsed

Image loaded!
0.104604
Programm ended successfully

 Performance counter stats for 'system wide':

              6.23 Joules power/energy-pkg/                                                     

       0.470984326 seconds time elapsed

Image loaded!
0.217715
Programm ended successfully

 Performance counter stats for 'system wide':

             15.81 Joules power/energy-pkg/                                                     

       0.848607319 seconds time elapsed

Image loaded!
0.600488
Programm ended successfully

 Performance counter stats for 'system wide':

              1.60 Joules power/energy-pkg/                                                     

       0.086818931 seconds time elapsed

Image loaded!
0.018949
Programm ended successfully

 Performance counter stats for 'system wide':

              2.18 Joules power/energy-pkg/                                                     

       0.116570511 seconds time elapsed

Image loaded!
0.050000
Programm ended successfully

 Performance counter stats for 'system wide':

              4.17 Joules power/energy-pkg/                                                     

       0.229608251 seconds time elapsed

Image loaded!
0.163873
Programm ended successfully

 Performance counter stats for 'system wide':

              1.31 Joules power/energy-pkg/                                                     

       0.077083444 seconds time elapsed

Image loaded!
0.013289
Programm ended successfully

 Performance counter stats for 'system wide':

              1.87 Joules power/energy-pkg/                                                     

       0.098616794 seconds time elapsed

Image loaded!
0.034763
Programm ended successfully

 Performance counter stats for 'system wide':

              3.48 Joules power/energy-pkg/                                                     

       0.190529104 seconds time elapsed

Image loaded!
0.116595
Programm ended successfully

 Performance counter stats for 'system wide':

              0.35 Joules power/energy-pkg/                                                     

       0.019925492 seconds time elapsed

Image loaded!
0.004903
Programm ended successfully

 Performance counter stats for 'system wide':

              0.54 Joules power/energy-pkg/                                                     

       0.030432667 seconds time elapsed

Image loaded!
0.011130
Programm ended successfully

 Performance counter stats for 'system wide':

              1.11 Joules power/energy-pkg/                                                     

       0.059531033 seconds time elapsed

Image loaded!
0.036329
Programm ended successfully
bpil@pop-os:~/HEIG/HPC/HPC_LAB/lab06/code$ 
```



