#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int countOnes(int array[],int size){
    int count = 0;
    for (int i = 0; i < size; ++i) {
        if (array[i] == 1) {
            count++;
        }
    }
    return count;    
}

void* count(void* args) {
    int* tArgs = (int*)args;
    int start = tArgs[0];
    int end = tArgs[1];
    int* array = (int*)tArgs[2];
    int* result = (int*)tArgs[3];

    int count = 0;
    for (int i = start; i < end; ++i) {
        if (array[i] == 1) {
            count++;
        }
    }

    *result += count;

}

int parallel_count(int* array, int array_size, int num_threads) {
    pthread_t threads[num_threads];
    int result = 0;
    int part_size = array_size / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        int start = i * part_size;
        int end;
        if((i == num_threads - 1)){
            end = array_size;
        }else{
            end = (i + 1) * part_size;
        }

        int* tArgs = malloc(4 * sizeof(int));
        tArgs[0] = start;
        tArgs[1] = end;
        tArgs[2] = (int)array;
        tArgs[3] = (int)&result;

        pthread_create(&threads[i], NULL, count, (void*)tArgs);
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    return result;
}

int main() {
    srand(time(NULL));
    size_t size = 1000000;
    int *array = (int*)malloc(size * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    for(int i=0;i<size;i++){
        array[i] = rand()%6;
    }
    int actual = 0;
    for(int i = 0; i < size; i++){
        if(array[i]==1){
            actual++;
        }
    }     
    int num_threads = 32;
    clock_t start, end,mid;
    double cpu_time_used;
    for(int i=1;i<=100;i++){
        int res = parallel_count(array, size, num_threads);
        if(actual!=res){
            printf("The program returned %d the correct answer is: %d\n",res,actual);
            break;
        }
        
    }
    start = clock();
    int res;
    for (int i=1;i<100;i++){
        res = parallel_count(array, size, num_threads);
    }
    
    mid = clock();
    cpu_time_used = ((double) (mid - start)) / CLOCKS_PER_SEC;
    start = clock();
        for (int i=1;i<100;i++){
        res = countOnes(array,size);
    }
    end = clock();
    double cpu_time_used1 = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Parallel with race took : %f seconds\n", cpu_time_used);
    printf("Sequential took : %f seconds", cpu_time_used1);
    free(array);
    return 0;
}
