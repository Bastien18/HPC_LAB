\usepackage{fvextra}
\begin{Highlighting}[breaklines,numbers=left]

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

### Analyse globale de la consommation avec l'outil perf

Pour cette partie j'ai écrit le script [meas_perf_global.sh](./code/meas_perf_global.sh) qui permet de mesurer les consommation sur les 4 images avec 3 taille de cluster différent. 

En faisant un ```perf list```j'ai remarqué que seul l'event power/energy-pkg/ était disponible sur ma machine.

Résultats non vectorize:

[./code/power_global_perf_nonv.txt](./code/power_global_perf_nonv.txt)

Résultat vectorize:

[./code/power_global_perf_vect.txt](./code/power_global_perf_vect.txt)

On voit une baisse de consomation de moitié lorsque l'on utilise la version vectorisé. On voit aussi que les images avec le plus de pixel ainsi que le nombre de cluster élevé consomme le plus ce qui fait naturelement sense puisque l'on sait quelles demanderont plus d'instructions répétées pour chaque pixel.

### Analyse globale de la consommation avec l'outil likwid

Pour cette partie j'ai créer le script [meas_likwid_global.sh](./code/meas_likwid_global.sh) qui permet de lancer likwid-powermeter et de mesurer le maximum et le minimum de consomation de mon code.

Je n'ai malheuresement pas réussi à lancer le script l'erreur:

```sh
ERROR - [/home/bpil/likwid-5.3.0/src/power.c:power_init:246] No such file or directory.
Cannot get access to RAPL counters
The AMD K19 (Zen3) architecture does not support reading power data or access is locked
```

Survient à chaque appel de likwid. Je n'ai pas réussi à trouver une solution. J'ai constaté que le group de performance CLOCK se lance sans msg d'erreur mais je constate une valeur de consommation du compteur PWR null à chaque mesure.

### Analyse détaillé likwid

Le script [meas_likwid_marker.sh](./code/meas_likwid_marker.sh) permet de lancer une simple analyse à l'aide du groupe de test CLOCK qui inclut un compteur PWR. L'executable [./code/build/segmentation_likwid](./code/build/segmentation_likwid) ajoute un groupe de profiling global dans un premier temps pour voir si les compteurs fonctionnent.

Les resultats dans le fichier [./code/power_marker_likwid_nonv.txt](./code/power_marker_likwid_nonv.txt) montre qu'ici encore tout les events RAPL_PKG_ENERGY sont à 0. Je n'arrive donc pas à faire le profiling du code à l'aide de likwid.

Il est probable que l'architecture de mon processeur ne soit pas adapté pour ce genre de mesure.

### Analyse détaillé perf

Le fichier [./code/src/main_segmentation_perf.c](./code/src/main_segmentation_perf.c) possède tout le code permettant la mesure détailé via likwid comme indiqué dans la consigne du labo. Il est compilé sous l'executable [./code/build/segmentation_perf](./code/build/segmentation_perf). Un premier group de test est placé de manière globale.

J'ai de nouveau rencontrer un problème. Entre la première mesure effectuée avec perf globale et celle-ci une mise à jour de mon système d'exploitation s'est déroulé suite à laquelle j'ai perdu l'utilitaire perf que j'avais installer auparavant. Je n'ai pas réussi à réinstaller le packet linux-tools-6.6.10-76060610-generic malgrès l'aide de Bruno. J'ai néanmoins essayé de contacter un élève ayant eu le même problème que moi mais sans suite.

### Analyse détaillé powercap

Enfin j'ai essayé d'utiliser powercap pour faire le profiling. L'executable [./code/build/segmentation_powercap](./code/build/segmentation_powercap) permet la mesure de la consommation énergétique de l'application segmentation. On peut ensuite simplement lancé l'application pour avoir la valeur de consommation. 

En faisant un petit test sur une image je me suis rendu compte que seul le package 0 sur la zone PACKAGE était disponible. Le soucis c'est que je n'arrive pas à avoir de différence entre les deux mesures.

```sh 
bpil@pop-os:~/HEIG/HPC/HPC_LAB/lab06/code$ sudo ./build/segmentation_powercap ../img/half-life.png 4 ../img/output/half-life-seg4.png
Initializing RAPL package 0
Total packages : 1
Energy on package 0 : 58727121835
Image loaded!
Programm ended successfully

Energy on package 0 : 58727121835
Energy Used : 0.000000
```

Malheureusement je n'ai pas réussi à comprendre pourquoi seule la mesure sur la zone PACKAGE et package 0 était disponible.

### Conclusion

Je suis assez perplexe sur ce laboratoire. La durée d'une semaine est une grosse contrainte sachant que les utilitaire (malgrès une utilisation dans les autres laboratoire) se révèle être plus compliqué que prévue.

Je dois avouer que j'ai de la peine à comprendre l'utilisation de ces outils de profiling s'il ne sont pas compatible sur toutes les architectures. Je comprends la contrainte mais à quoi bon essayer de profiler du code si les processeurs récent ne peuvent pas le supporter. La machine de laboratoire reste une option laborieuse sachant que les laboratoires ne sont pas vraiment faisable dans les périodes mise à disposition.

Je remércie néanmoins Bruno d'avoir consacré beaucoup de son temps à dépanner mes soucis d'utilisation des outils de profiling.






