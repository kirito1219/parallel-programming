#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

int fnz(long long int *partial_sums, int world_size){
    int diff = 0;
    for(int i = 1; i < world_size; i++)
        diff |= (partial_sums[i] == 0);
    if(diff)
        return 0;
    else
        return 1;
    return 0;
}

int main(int argc, char **argv)
{
    // --- DON'T TOUCH ---
    MPI_Init(&argc, &argv);
    double start_time = MPI_Wtime();
    double pi_result;
    long long int tosses = atoi(argv[1]);
    int world_rank, world_size;
    // ---

    MPI_Win win;

    // TODO: MPI init
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    long long int partial_size = tosses/ world_size;
    long long int partial_sum = 0;
    long long int global_sum = 0;
    double x, y;
    unsigned int seed = 2020 * world_rank;

    for(long long int i = 0; i < partial_size; i++){
        x = (double)rand_r(&seed) / RAND_MAX;
        y = (double)rand_r(&seed) / RAND_MAX;
        if(x*x + y*y <= 1.0)
            partial_sum++;
    }

    if (world_rank == 0)
    {
        // Master
        long long int *partial_sums;
        MPI_Alloc_mem(world_size * sizeof(long long int), MPI_INFO_NULL, &partial_sums);
        for(int i = 0; i<world_size; i++)
            partial_sums[i] = 0;
        partial_sums[0] = partial_sum;

        MPI_Win_create(partial_sums, world_size * sizeof(long long int), sizeof(long long int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        
        int ready = 0;
        while(!ready){
            MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, win);
            ready = fnz(partial_sums, world_size);
            MPI_Win_unlock(0, win);
        }
        for(int i = 0; i < world_size; i++)
            global_sum += partial_sums[i];
        MPI_Free_mem(partial_sums);
    }
    else
    {
        // Workers
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, win);
        MPI_Put(&partial_sum, 1, MPI_LONG_LONG_INT, 0, world_rank, 1, MPI_LONG_LONG_INT, win);
        MPI_Win_unlock(0, win);
    }

    MPI_Win_free(&win);

    if (world_rank == 0)
    {
        // TODO: handle PI result
        pi_result = 4.0 * (double)global_sum / (double)tosses;
        // --- DON'T TOUCH ---
        double end_time = MPI_Wtime();
        printf("%lf\n", pi_result);
        printf("MPI running time: %lf Seconds\n", end_time - start_time);
        // ---
    }
    
    MPI_Finalize();
    return 0;
}
