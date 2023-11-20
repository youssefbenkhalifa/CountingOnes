#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* count(void* args) {
    int* tArgs = (int*)args;
    int start = tArgs[0];
    int end = tArgs[1];
    int* array = (int*)tArgs[2];
    int* privatecount = (int*)tArgs[3];

    int count = 0;
    for (int i = start; i < end; ++i) {
        if (array[i] == 1) {
            count++;
        }
    }

    *privatecount = count;

}

int p_count(int* array, int array_size, int num_threads) {
    pthread_t threads[num_threads];
    int private_counts[num_threads];
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
        tArgs[3] = (int)&private_counts[i];

        pthread_create(&threads[i], NULL, count, (void*)tArgs);
    }
    int result = 0;
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
        result += private_counts[i];
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
    int num_threads = 1;
    int actual = 0;
    for(int i = 0; i < size; i++){
        if(array[i]==1){
            actual++;
        }
    } 
    for(int i=1;i<=100;i++){
    int res = p_count(array, size, num_threads);
    if(actual!=res){
        printf("The program returned %d and the correct answer is: %d",res,actual);
    }
    }
    free(array);
    return 0;
}
