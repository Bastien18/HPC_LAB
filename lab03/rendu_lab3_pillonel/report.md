# Rapport LABO3 HPC
## Bastien Pillonel
## Date : 4.04.2024
### Config : 
- Distrib PopOS 64bits (Ubuntu 22.04)
- AMD Ryzen 7 5800X 8-Core Processor
- AMD K19 (Zen3) architecture
- gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0

## Partie 1

Tout le code se trouve sur godbolt accessible via le lien:

[https://godbolt.org/z/87aaceYnK](https://godbolt.org/z/87aaceYnK)

Compilateur utilisé : x86-64 gcc 11.4
0.1068
Les explications de chaques examples se trouvent en commentaire sur godbolt.

## Partie 2

Pour cette partie j'ai décidé d'optimisé ma fonction filter_pixel_1D. 

Le but sera de reduire son temps d'execution au maximimum. Pour les mesures je ré-utilise likwid-perfctr du labo précédant puisqu'il me permettait d'avoir un aperçu du temps. J'ai donc recréé un nouveau groupe de performance [HPC_LAB3_TIME](./code/src/HPC_LAB3_TIME.txt) et placé les deux marker likwid autour de l'appele à filter_pixel_1D.

### Résultat du code compilé en -O0

| Image traité | Temps d'execution [s] |
| ------------ | --------------------- |
| half-life    | 0.4519                |
| medailon     | 0.1332                |
| nyc          | 0.1146                |


### Optimisation manuelle 1

Certaines valeures sont recalculé à chaque tour de boucle pour rien. Notament la position du pixel_y ainsi que l'offset vertical du kernel. On peut donc précalculé ces valeurs.

La version utilisé est celle commentée dans le fichier [sobel.c](./code/src/sobel.c) par VERSION OPTIMISEE MANUELLEMENT 1.

### Résultat optimisation manuelle 1

| Image traité | Temps d'execution [s] |
| ------------ | --------------------- |
| half-life    | 0.4257                |
| medailon     | 0.1222                |
| nyc          | 0.1089                |

On obtient ici une amélioration au niveau du temps d'environs 10%.

### Optimisation manuelle 2

Il serait aussi possible d'utiliser des variables afin d'éviter les accès mémoires répété.

La version utilisé est celle commentée dans le fichier [sobel.c](./code/src/sobel.c) par VERSION OPTIMISEE MANUELLEMENT 2.

### Résultat optimisation manuelle 2

| Image traité | Temps d'execution [s] |
| ------------ | --------------------- |
| half-life    | 0.4274                |
| medailon     | 0.1226                |
| nyc          | 0.1068                |

Cette optimisation n'a pas l'air d'être efficace j'obtiens des résultats moins bons pour les deux premier et une petite amélioration pour le dernier.

Je ne pense pas pouvoir optimiser plus que cela la fonction. On possibilité serait d'utiliser des instructions de type SIMD qui se prête bien au traitement de données matricielles.

### Optimisation du compilateur

L'optimiation manuelle 1 qui a améliorer le temps correspondrait au flag -ftree-loop-distribution qui aide le compilateur à ditribuer les calculs des boucles sur plusieurs itérations. -O3 active ce flags je vais donc recompiler le code en -O3 pour voir le résultat.

### Resultats optimisation du compilateur

| Image traité | Temps d'execution [s] |
| ------------ | --------------------- |
| half-life    | 0.1334                |
| medailon     | 0.0451                |
| nyc          | 0.0516                |

On obtient une nette amélioration du temps d'execution. Il faut prendre en compte qu'en -O3 beacoup d'amélioration sont activée comme le loop-unrolling (pas utilisé en manuelle car la fonction prend un kernel de taille variable), l'utilisation d'instruction simd, le réordonnage de bloc, etc...




