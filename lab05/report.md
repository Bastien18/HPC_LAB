# Rapport LABO5 HPC
## Bastien Pillonel
## Date : 15.05.2024
### Config : 
- Distrib PopOS 64bits (Ubuntu 22.04)
- AMD Ryzen 7 5800X 8-Core Processor
- AMD K19 (Zen3) architecture
- gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0

## Introduction 

Pour ce labo il est demandé de faire le profiling d'un code open source C au choix. J'ai choisit d'analyser le code d'une application qui check la syntaxe textuelle d'un fichier src en C. 

Lien du code source [C-Code-Syntax-Checke](https://github.com/MM120-i/C-Code-Syntax-Checker/tree/main)

## Modification du code de base

Le code de base présentais pas mal d'erreur et n'étais pas fonctionnel. J'ai donc pris soin de corriger les erreurs d'indexage et autre. La version en ligne sur github n'est donc pas fonctionnelle.

Le programme se compile simplement à l'aide du makefile et de la commande ```make```. Ensuite on lance le programme comme suit ```./Checker <nom_du_fichier_a_analyser>```.


## Profiling
### Perf tool

Pour profiler le code j'ai décidé d'utiliser deux fichiers C. Un avec des erreur [input_error.txt](./C-Code-Syntax-Checker/src/input_error.txt) et un sans erreur [input_clean.c](./C-Code-Syntax-Checker/src/input_clean.c).

Les deux premiers events que je souhaite mesurer sont le nombre d'instruction retirées ainsi que le nombre de cycle cpu effectué. Pour se faire je lance la commande :

```perf stat -e cpu-cycles,instructions ./Checker <input_file>```

Résultat sur input_clean.c:

```
 Performance counter stats for './Checker ./src/input_clean.c':

         4,464,980      cpu-cycles                                                            
         3,859,545      instructions                     #    0.86  insn per cycle            

       0.003168161 seconds time elapsed

       0.000000000 seconds user
       0.003145000 seconds sys
```

Résultat sur input_error.txt:

```
 Performance counter stats for './Checker ./src/input_error.txt':

         3,821,949      cpu-cycles                                                            
         3,061,866      instructions                     #    0.80  insn per cycle            

       0.013873980 seconds time elapsed

       0.003810000 seconds user
       0.000000000 seconds sys
```

On obtient ensuite les ratios suivants:

Input_clean ratio = 0.86 [instr/cycle]
Input_error ratio = 0.80 [instr/cycle]

Ensuite je mesure le ratio des caches miss sur chaque fichier:

Résultat sur input_clean.c:

```
 Performance counter stats for './Checker ./src/input_clean.c':

           967,607      L1-dcache-loads                                                       
            20,274      L1-dcache-load-misses            #    2.10% of all L1-dcache accesses 

       0.002718538 seconds time elapsed

       0.000000000 seconds user
       0.002805000 seconds sys
```

Résultat sur input_error.txt:

```
 Performance counter stats for './Checker ./src/input_error.txt':

           763,950      L1-dcache-loads                                                       
            20,290      L1-dcache-load-misses            #    2.66% of all L1-dcache accesses 

       0.001629154 seconds time elapsed

       0.001754000 seconds user
       0.000000000 seconds sys
```

Input_clean cache misses = 2.10%
Input_error cache misses = 2.66%

Enfin je regarde le nombre de branch misses :

Résultat sur input_clean.c:

```
 Performance counter stats for './Checker ./src/input_clean.c':

            39,560      branch-misses                    #    5.75% of all branches           
           687,736      branch-instructions                                                   

       0.001892185 seconds time elapsed

       0.000000000 seconds user
       0.001987000 seconds sys
```

Résultat sur input_error.txt:

```
 Performance counter stats for './Checker ./src/input_error.txt':

            40,046      branch-misses                    #    8.52% of all branches           
           469,801      branch-instructions                                                   

       0.002201889 seconds time elapsed

       0.002299000 seconds user
       0.000000000 seconds sys
```

Input_clean branch misses = 5.75%
Input_error branch misses = 8.52%


### Hotspot tool

Lancement de la mesure sur hotspot à l'aide de la commande suivante:

```
perf record -o <data_file_location> --call-graph dwarf --aio --sample-cpu <path_to_checker_exec> <file_to_analyze>
```

Je lui demande tout d'abord un aperçu des cycles cpu.

Input_clean.c:

![](./picture/Screenshot%20from%202024-05-18%2008-11-40.png)

Input_error.txt:

![](./picture/Screenshot%20from%202024-05-19%2012-29-47.png)

L'analyse des points chauds ne m'indique pas réelement de problème dans mon programme. Malheureusement je n'ai pas réussi à afficher tous les appels à mes fonctions pour une raison que j'ignore. Néanmoins il semble que l'appel aux fonctions de la shared library ait un overhead assez conséquent. Je checkerai donc les appels aux fonctions de la librairie standard.

### Valgrind

Pour cette partie j'ai décidé d'utiliser l'outil callgrind qui permet de mieux représenter le nombre d'appel aux fonctions. Comme mon programme utilise beaucoup de boucle pour parcourir le fichier source de code C, il est utile de voir quelle sont les fonctions qui sont le plus souvent appelés afin de voir si une optimisation est possible.

Commande utilisé :

```
valgrind --tool=callgrind ./Checker <fichier_a_analyzer>
```

Ensuite pour la visualisation :

```
kcachegrind callgrind.out.<PID_correspondant>
```

Résultat input_clean.c:

![](./picture/Screenshot%20from%202024-05-19%2016-26-42.png)

Résultat input_error.txt:

![](./picture/Screenshot%20from%202024-05-19%2016-27-21.png)

On remarque ici qu'un grand nombre d'appel à strcmp est effectué. Ce qui fait sens puisque notre programme va analyser la syntax du code. Mais ce qui peut potentiellement nous faire perdre du temps si certains appelle sont superflues.

### Optimisation de strcmp

Pour optimiser le nombre d'appelle à strcmp je décide d'observer chaque appel:

Dans la fonction keyword:

```c
// Check if the current word is a C keyword
for (k = 0; k < NUMBER_OF_KEYWORDS; k++)
{
       if (strcmp(currentWord, sync[k]) == 0)
       {
              // Print the keyword and its line number
              printf("Line %d: %s\n", arr[i].lineno, currentWord);
       }
}
```

Ici par exemple comme les keywords sont unique, si l'on trouve un match dans la liste nous n'avons plus besoin de terminer la recherche on peut donc placer un break après le print.

Dans la fonction builtinfunction:

```c
for (l = 0; l < 30; l++)
{
       if (strcmp(currentWord, sync[l]) == 0)
       {
              total += 1; // Increment total if the word is a built in function.
       }
}
```

Ici on ne peut pas vraiment optimiser le nombre de fonction builtin à optimiser car on est obligé de parcourir toute la liste afin de voir si un matching est présent

Pareil pour la fonction function_and_prototype. Néanmoins la recherche du main se fait dans une autre boucle qui reparcours tous le fichier. Cela ralentis extrêmement car on double le percours du fichier pour rien. On peut directement intégrer le check dans la boucle de parcours principale.

La dernière boucle s'occupe de compter le nombre de prototype qui (dans un programme ou chaque fonction est définie et déclarée) doit être égale au nombre de fonction moins 1 (pour le main). Encore une fois cette boucle n'est pas nécessaire et le check peut se faire directement dans la boucle principale.

Dans la fonction de comptage des variables on check si le mot est une variable avant de faire le check de si la ligne est une déclaration/définition de fonction.

```c
// Check if the current word matches any of the synchronization words
for (size_t l = 0; l < 6; l++)
{
       if (strcmp(currnetWord, sync[l]) == 0)
       {
              pos = j;

              // Check if the line does not end with ')' to ensure it is a function declaration
              if ((str[strlength - 3] != ')') && (str[strlength - 2] != ')'))
              {
                     // Loop through the line to count commas and semicolons, indicating variable declarations
                     for (size_t x = 0; x < strlength; x++)
                     {
                     if (str[x] == ',' || str[x] == ';')
                     {
                            total[l]++;
                     }
                     }
              }
       }
}
```

Si l'on inverse les deux vérification on évitera certaines comparaisons inutiles.

### Test de l'optimisation

Le fichier optimisé s'appelle [Checker_optimized.c](./C-Code-Syntax-Checker/src/Checker_optimized.c).

Résultat du nombre d'appel à strcmp input_clean.c:

![](./picture/Screenshot%20from%202024-05-19%2017-51-42.png)

Résultat du nombre d'appel à strcmp input_error.txt:

![](./picture/Screenshot%20from%202024-05-19%2017-52-11.png)

On remarque une légère baisse d'environs 500 appels.

Ensuite je relance le script measure.sh en changeant l'application à profiler pour comparer avec la baseline du début.

```
# started on Sun May 19 17:54:40 2024


 Performance counter stats for './Checker_optimized ./src/input_clean.c':

         2,866,494      cpu-cycles                                                            
         2,429,009      instructions                     #    0.85  insn per cycle            

       0.002637315 seconds time elapsed

       0.002676000 seconds user
       0.000000000 seconds sys


# started on Sun May 19 17:54:40 2024


 Performance counter stats for './Checker_optimized ./src/input_error.txt':

         2,279,781      cpu-cycles                                                            
         1,822,003      instructions                     #    0.80  insn per cycle            

       0.001623332 seconds time elapsed

       0.001738000 seconds user
       0.000000000 seconds sys


# started on Sun May 19 17:54:40 2024


 Performance counter stats for './Checker_optimized ./src/input_clean.c':

           745,492      L1-dcache-loads                                                       
            18,370      L1-dcache-load-misses            #    2.46% of all L1-dcache accesses 

       0.001598304 seconds time elapsed

       0.001687000 seconds user
       0.000000000 seconds sys


# started on Sun May 19 17:54:40 2024


 Performance counter stats for './Checker_optimized ./src/input_error.txt':

           622,549      L1-dcache-loads                                                       
            18,948      L1-dcache-load-misses            #    3.04% of all L1-dcache accesses 

       0.001449956 seconds time elapsed

       0.001463000 seconds user
       0.000000000 seconds sys


# started on Sun May 19 17:54:40 2024


 Performance counter stats for './Checker_optimized ./src/input_clean.c':

            36,485      branch-misses                    #    7.67% of all branches           
           475,465      branch-instructions                                                   

       0.001552869 seconds time elapsed

       0.001639000 seconds user
       0.000000000 seconds sys


# started on Sun May 19 17:54:40 2024


 Performance counter stats for './Checker_optimized ./src/input_error.txt':

            37,198      branch-misses                    #   10.15% of all branches           
           366,386      branch-instructions                                                   

       0.001601821 seconds time elapsed

       0.001681000 seconds user
       0.000000000 seconds sys
```

On observe une diminution de presque moitié du nombre d'instruction ainsi que du nombre de cycle CPU et une très légère augmentation du cache miss et du branch predict ce qui est sûrement dû au fait d'avoir rajouter quelques test en plus dans la boucle principale du check des fonctions et prototype. 

Globalement cela reste une bonne amélioration des performances du code.





