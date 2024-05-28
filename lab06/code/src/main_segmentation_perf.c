#include "k-means.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

typedef struct
{
    int ctl_fd;
    int ack_fd;
    bool enable;
} PerfManager;
const char *enable_cmd = "enable";
const char *disable_cmd = "disable";
const char *ack_cmd = "ack\n";

void send_command(PerfManager *pm, const char *command)
{
    if (pm->enable)
    {
        write(pm->ctl_fd, command, strlen(command));
        char ack[5];
        read(pm->ack_fd, ack, 5);
        assert(strcmp(ack, ack_cmd) == 0);
    }
}

void PerfManager_init(PerfManager *pm)
{
    char *ctl_fd_env = getenv("PERF_CTL_FD");
    char *ack_fd_env = getenv("PERF_ACK_FD");
    if (ctl_fd_env && ack_fd_env)
    {
        pm->enable = true;
        pm->ctl_fd = atoi(ctl_fd_env);
        pm->ack_fd = atoi(ack_fd_env);
    }
    else
    {
        pm->enable = false;
        pm->ctl_fd = -1;
        pm->ack_fd = -1;
    }
}

void PerfManager_pause(PerfManager *pm)
{
    send_command(pm, disable_cmd);
}
void PerfManager_resume(PerfManager *pm)
{
    send_command(pm, enable_cmd);
}

int main(int argc, char **argv)
{
    PerfManager pmon;
    PerfManager_init(&pmon);
    PerfManager_resume(&pmon);

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

    PerfManager_pause(&pmon);

    return 0;
}
