#include<iostream>
#include <stdlib.h> 
#include <time.h>
#include <pthread.h>
#include <iomanip>
using namespace std;

pthread_mutex_t lock;
long long int sum = 0;

typedef struct
{
   int thread_id;
   int start;
   int end;
} Arg;

void *montecarlo(void *arg){

    Arg *data = (Arg *)arg;
    int thread_id = data->thread_id;
    int start = data->start;
    int end = data->end;
    long long local_sum = 0;
    double x,y;
    unsigned int seed = time(NULL);

    for(int i = start; i < end ;i++){
        x = (double)rand_r(&seed)/RAND_MAX;
        y = (double)rand_r(&seed)/RAND_MAX;
        if ( x*x + y*y <= 1.0){
            local_sum++;
        }
    }
    pthread_mutex_lock(&lock);
    sum += local_sum;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]){
  
  long long int toss = atol(argv[2]);
  int thd_count = atoi(argv[1]);
  
  srand(time(NULL));
  
  
  pthread_t *thd;
  thd = (pthread_t *)malloc(thd_count * sizeof(pthread_t));
  pthread_mutex_init(&lock, NULL);
  
  
  int part = toss / thd_count; 
  Arg arg[thd_count]; 
  for (int i = 0; i < thd_count; i++){     
      arg[i].thread_id = i;
      arg[i].start = part * i;
      arg[i].end = part * (i + 1);    
      pthread_create(&thd[i], NULL, montecarlo, (void *)&arg[i]);
   }

    
  for (int i = 0; i < thd_count; i++){     
    pthread_join(thd[i], NULL);
  }
  pthread_mutex_destroy(&lock);
  free(thd);
  
  double result = 4.0 * (double)sum / (double)toss;
  cout <<result<< endl;
  
  
  return 0;
}