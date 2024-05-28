cmake_minimum_required(VERSION 3.0)
project(HPC_lab06)

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

set(SEG_LIKWID_SRC
    src/main_segmentation_likwid.c
    src/k-means.c
    src/image.c
)

set(SEG_PERF_SRC
    src/main_segmentation_perf.c
    src/k-means.c
    src/image.c
)

set(SEG_POWERCAP_SRC
    src/main_segmentation_powercap.c
    src/k-means.c
    src/image.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -Wall -fno-inline")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -fno-inline -march=native -mavx2")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g -Wall -fno-inline -DLIKWID_PERFMON")

include_directories(
    ../lib/stb
    include/
)

# Ajouter le chemin où trouver likwid-config.cmake
set(CMAKE_PREFIX_PATH "/home/bpil/likwid-5.3.0/")

# Utiliser find_package pour localiser LIKWID
find_package(Likwid REQUIRED CONFIG)

if (LIKWID_FOUND)
    message(STATUS "Found LIKWID: ${LIKWID_INCLUDE_DIRS}, ${LIKWID_LIBRARIES}")
    include_directories(${LIKWID_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "LIKWID not found. Please install LIKWID or set LIKWID_DIR.")
endif()

set(LIBRARY_PATH "./src/powercap")
set(LIBRARY_NAME "powercap")

# Configuration de l'exécutable 1
#add_executable(edge_detection ${EDGE_SRC})

#target_link_libraries(edge_detection m)

# Configuration de l'exécutable 2
add_executable(segmentation ${SEG_SRC})

target_link_libraries(segmentation m)

# Configuration de l'exécutable 3
add_executable(segmentation_likwid ${SEG_LIKWID_SRC})

target_link_libraries(segmentation_likwid m ${LIKWID_LIBRARIES})

# Configuration de l'exécutable 4
add_executable(segmentation_perf ${SEG_PERF_SRC})

target_link_libraries(segmentation_perf m)

# Configuration de l'exécutable 5
add_library(${LIBRARY_NAME} STATIC IMPORTED)
set_target_properties(${LIBRARY_NAME} PROPERTIES IMPORTED_LOCATION /home/bpil/HEIG/HPC/HPC_LAB/lab06/code/src/powercap/libpowercap.a)
add_executable(segmentation_powercap ${SEG_POWERCAP_SRC})

target_link_libraries(segmentation_powercap PRIVATE ${LIBRARY_NAME} m)


