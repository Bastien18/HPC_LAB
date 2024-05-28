#include "k-means.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "./powercap/inc/powercap.h"
#include "./powercap/inc/powercap-rapl.h"

static const char *to_string(powercap_rapl_zone zone)
{
    switch (zone)
    {
    case POWERCAP_RAPL_ZONE_PACKAGE:
        return "package (CPU socket)";
    case POWERCAP_RAPL_ZONE_CORE:
        return "core (CPU)";
    case POWERCAP_RAPL_ZONE_UNCORE:
        return "uncore (GPU, etc.)";
    case POWERCAP_RAPL_ZONE_DRAM:
        return "DRAM";
    case POWERCAP_RAPL_ZONE_PSYS:
        return "system (PSys)";
    default:
        return "unknown";
    }
}

int main(int argc, char **argv)
{
    // On récupère le nombre de package disponible dans l’architecture
    uint32_t npackages = powercap_rapl_get_num_instances();
    uint64_t *energy_uj1 = (uint64_t *)malloc(sizeof(int) * npackages);
    uint64_t *energy_uj2 = (uint64_t *)malloc(sizeof(int) * npackages);

    // On créer une instance pour chaque package
    powercap_rapl_pkg pkg[npackages];

    // Initialisation des packages
    for (size_t i = 0; i < npackages; i++)
    {
        printf("Initializing RAPL package %ld\n", i);
        if (powercap_rapl_init(i, &pkg[i], true))
        {
            printf("Error initializing RAPL, you may need privileged access\n");
            powercap_rapl_destroy(&pkg[i]);
        }
    }

    // On défini la zone que l’on souhaite analyser, ici psys
    powercap_rapl_zone zone = POWERCAP_RAPL_ZONE_PACKAGE;

    printf("Total packages : %d\n", npackages);

    // On vérifie si les packages supporte la zone qu’on souhaite analysée.
    bool supported[npackages];
    bool has_one_supported = false;

    for (size_t i = 0; i < npackages; i++)
    {
        supported[i] = powercap_rapl_is_zone_supported(&pkg[i], zone);
        if (supported[i] != 1)
        {
            printf("RAPL is not supported on package %ld\n", i);
        }
        else
        {
            has_one_supported = true;
        }
    }

    if (!has_one_supported)
    {
        printf("No supported package for %s\n", to_string(zone));
        return EXIT_FAILURE;
    }

    // Maintenant on lance la mesure de consommation énergétique
    for (size_t j = 0; j < npackages; ++j)
    {
        if (supported[j])
        {
            if (powercap_rapl_get_energy_uj(&pkg[j], zone, &energy_uj1[j]))
            {
                printf(" Failed to get energy on package %ld\n", j);
                break;
            }
            else
            {
                printf("Energy on package %ld : %ld\n", j, energy_uj1[j]);
            }
        }
    }

    struct img_1D_t *img;
    int nb_cluster = 0;

    if (argc != 4)
    {
        fprintf(stderr, "Usage : %s <img_src.png> <nb_clusters> <img_dest.png>\n", argv[0]);
        return EXIT_FAILURE;
    }

    nb_cluster = atoi(argv[2]);
    if (nb_cluster <= 0)
    {
        printf("The number of clusters should be greater than 0\n");
        return 1;
    }

    img = load_image_1D(argv[1]);

    printf("Image loaded!\n");

    kmeans(img, nb_cluster);

    save_image(argv[3], img);

    free_image(img);

    printf("Programm ended successfully\n\n");

    // On remesure la consomation énergétique
    for (size_t j = 0; j < npackages; j++)
    {
        if (supported[j])
        {
            if (powercap_rapl_get_energy_uj(&pkg[j], zone, &energy_uj2[j]))
            {
                printf(" failed to get energy on package %ld\n", j);
                break;
            }
            else
            {
                printf("Energy on package %ld : %ld\n", j, energy_uj1[j]);
            }
        }
    }

    printf("Energy Used : %f\n", (energy_uj2[1]-energy_uj1[1])/1000000.0);

    for (size_t i = 0; i < npackages; i++)
    {
        powercap_rapl_destroy(&pkg[i]);
    }

    return 0;
}
