cmake_minimum_required(VERSION 3.0)
project(HPC_lab04_smid)

# Configuration générale
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Ajoutez ici vos fichiers source pour chaque exécutable
set(EDGE_SRC
    src/main_sobel.c
    src/image.c
    src/sobel.c
)


set(SEG_SRC
    src/main_segmentation.c
    src/k-means.c
    src/image.c
)


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -g -Wall -fno-inline")

include_directories(
    ../lib/stb
    include/
)

# Configuration de l'exécutable 1
add_executable(edge_detection ${EDGE_SRC})

target_link_libraries(edge_detection m)

# Configuration de l'exécutable 2
add_executable(segmentation ${SEG_SRC})

target_link_libraries(segmentation m)
