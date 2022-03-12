#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#define SEED 2020

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
    int iter = 0, a = 1;
    bool have_send = false;

    while(a < world_size){
        a *= 2;
        iter++;
    }
    a = 1;

    for(long long int i = 0; i < partial_size; i++){
        x = rand_max - rand_r(&seed);
        y = rand_max - rand_r(&seed);
        if(radius_2 - (rand_max_2 - x) * x - (rand_max_2 - y) * y <= radius)
            partial_sum++;
    }

    for(int j = 0; j < iter; j++){
        a *= 2;
        if (world_rank % a == 0)
        {
            long long int other_sum = 0;
            if(world_rank + a / 2 < world_size){
                MPI_Recv(&other_sum, 1, MPI_LONG_LONG_INT, world_rank + a / 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                partial_sum += other_sum;
            }
        }
        else if (world_rank % a != 0 && !have_send)
        {
            have_send = true;
            MPI_Send(&partial_sum, 1, MPI_LONG_LONG_INT, world_rank - a / 2, 0, MPI_COMM_WORLD);
        }
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
