#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct array {
    int *arr;
    int size;
};

void *bubble_sort(void *param)
{
    struct array *data = (struct array *)param;

    for (int i = 0; i < data->size - 1; i++) {
        int swapped = 0;

        for (int j = 0; j < data->size - 1 - i; j++) {
            if (data->arr[j] > data->arr[j + 1]) {
                int temp = data->arr[j];
                data->arr[j] = data->arr[j + 1];
                data->arr[j + 1] = temp;
                swapped = 1;
            }
        }

        if (!swapped)
            break;
    }

    return NULL;
}

int main(void)
{
    struct array data;
    pthread_t tid;

    printf("Enter number of integers: ");
    if (scanf("%d", &data.size) != 1 || data.size <= 0) {
        fprintf(stderr, "Size must be a positive integer\n");
        return 1;
    }

    data.arr = malloc((size_t)data.size * sizeof(*data.arr));
    if (data.arr == NULL) {
        perror("malloc");
        return 1;
    }

    printf("Enter %d integers: ", data.size);
    for (int i = 0; i < data.size; i++) {
        if (scanf("%d", &data.arr[i]) != 1) {
            fprintf(stderr, "Invalid input\n");
            free(data.arr);
            return 1;
        }
    }

    if (pthread_create(&tid, NULL, bubble_sort, &data) != 0) {
        fprintf(stderr, "pthread_create failed\n");
        free(data.arr);
        return 1;
    }

    if (pthread_join(tid, NULL) != 0) {
        fprintf(stderr, "pthread_join failed\n");
        free(data.arr);
        return 1;
    }

    printf("Sorted array: ");
    for (int i = 0; i < data.size; i++)
        printf("%d ", data.arr[i]);
    putchar('\n');

    free(data.arr);
    return 0;
}
