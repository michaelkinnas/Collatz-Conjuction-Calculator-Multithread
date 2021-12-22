#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NTHREADS 6

#define START_NUMBER 1
#define END_NUMBER 100000000
#define PRINTOUT_STEP 10000000

void* cc(void *arg);

struct variables {
    long long start;
    long long end;
    long long checking_step;
    long long longest_num;
    long long longest_steps;
};

int main(int argc, char** argv) {
    clock_t start, end;
    start = clock();
    pthread_t threads[NTHREADS];
    struct variables vars_arr[NTHREADS];   
    
    //initialize some variables for all threads;
    for (int i = 0; i < NTHREADS; i++) {        
        vars_arr[i].checking_step = PRINTOUT_STEP;
        vars_arr[i].longest_steps = 0;
        vars_arr[i].longest_num = 0;
    }

    //divide number ranges to threads for start and stop
    for (int i = 0; i < NTHREADS; i++) {        
        vars_arr[i].start = (i * END_NUMBER / NTHREADS) + 1;
        //printf("start:\t %d\n", vars_arr[i].start);      
        vars_arr[i].end = (1 + i) * END_NUMBER / NTHREADS;
        //printf("end:\t %d\n", vars_arr[i].end);
    }

    //create threads and start working
    int count[NTHREADS];
    for (int i = 0; i < NTHREADS; i++) {
        count[i] = i;
        pthread_create(&threads[i], NULL, cc, &vars_arr[count[i]]);
    }

    //wait for all threads to finish
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //compare output of all threads to pick the best number
    long long longest_num = vars_arr[0].longest_num;
    long long longest_steps = vars_arr[0].longest_steps;
    for (int i = 1; i < NTHREADS; i++) {
        if (vars_arr[i].longest_steps > longest_steps) {
            longest_num = vars_arr[i].longest_num;
            longest_steps = vars_arr[i].longest_steps;
        }
    }    
    printf("\n\n%lld is the number with the most amount of steps: %lld\n", longest_num, longest_steps);
    end = clock();
    double duration = ((double)end - start) / CLOCKS_PER_SEC;     
    printf("Time taken to calculate: %.4f sec", duration);

    return 0;
}

//simple collatz conjecture algorithm by Michael Kinnas
//arg is struct pass
void* cc(void *arg) {
    struct variables *variables_ptr = (struct variables*) arg;
    long long next;
    long long counter;

    printf("Started new thread %d\n", pthread_self());

    for (long long i = variables_ptr->start; i <= variables_ptr->end; i++) {
        next = i;
        counter = 0;
        //printf("\n%lld", next); //uncomment for specific number details
        if (i % variables_ptr->checking_step == 0) {
            printf("Finished calculating number %lld, from thread %d\n", i, pthread_self());
            //printf("Just finished checking number %lld, currently %lld is the number with the most amount of steps: %lld from thread %d\n", i, variables_ptr->longest_num, variables_ptr->longest_steps, pthread_self());       
        }
        while (next != 1 && next > 0) {
            if (next % 2 == 0) { //even
                counter++;
                next = next / 2;
                //printf("\n%lld", next); //uncomment for specific number details
            } else {
                counter++;
                next = (next * 3) + 1;
                //printf("\n%lld", next); //uncomment for specific number details
            }           
        }
        if (counter > variables_ptr->longest_steps) {
            variables_ptr->longest_steps = counter;
            variables_ptr->longest_num = i;
        }
        //printf(", Steps: %d", counter); //uncomment for specific number details        
    }
}