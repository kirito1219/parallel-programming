#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#define SEED 2020
#define PROCESS_NUM 20

const unsigned long long int rand_max = RAND_MAX;
const unsigned long long int rand_max_2 = rand_max * 2;
const unsigned long long int radius = rand_max * rand_max;
const unsigned long long int radius_2 = radius * 2;

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    // TODO: init MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    long long int partial_size = tosses / world_size;
    long long int partial_sum = 0;
    double x, y;
    unsigned int seed = 2020 * world_rank;
    if (world_rank > 0)
    {
        for(long long int i = 0; i<partial_size; i++){
            x = rand_max - rand_r(&seed);
            y = rand_max - rand_r(&seed);
            if(radius_2 - (rand_max_2 - x) * x - (rand_max_2 - y) * y <= radius)
                partial_sum++;
        }
        MPI_Send(&partial_sum, 1, MPI_LONG_LONG_INT, 0, 0, MPI_COMM_WORLD);
    }
    else if (world_rank == 0)
    {
   	    long long int global_sum = 0;
        long long int counts[PROCESS_NUM];
        MPI_Request requests[PROCESS_NUM];
        for(long long int i = 0; i<partial_size; i++){
            x = rand_max - rand_r(&seed);
            y = rand_max - rand_r(&seed);
            if(radius_2 - (rand_max_2 - x) * x - (rand_max_2 - y) * y <= radius)
                partial_sum++;
        }
        for(int i = 1; i < world_size; i++)
            MPI_Irecv(&counts[i - 1], 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD, &requests[i - 1]);
        MPI_Waitall(world_size - 1, requests, MPI_STATUS_IGNORE);
        global_sum += partial_sum;
        for(int i = 0; i < world_size - 1; i++)
            global_sum += counts[i];
    }

    if (world_rank == 0)
    {
        // TODO: process PI result
        pi_result = 4.0 * (double)partial_sum / (double)tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }

    MPI_Finalize();
    return 0;
}
