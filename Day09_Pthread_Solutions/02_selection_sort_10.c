#include <stdio.h>
#include <pthread.h>

#define N 10

void *selection_sort(void *param)
{
    int *arr = (int *)param;

    for (int i = 0; i < N - 1; i++) {
        int min_index = i;

        for (int j = i + 1; j < N; j++) {
            if (arr[j] < arr[min_index])
                min_index = j;
        }

        if (min_index != i) {
            int temp = arr[i];
            arr[i] = arr[min_index];
            arr[min_index] = temp;
        }
    }

    return NULL;
}

int main(void)
{
    int arr[N];
    pthread_t tid;

    printf("Enter %d integers: ", N);
    for (int i = 0; i < N; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "Invalid input\n");
            return 1;
        }
    }

    if (pthread_create(&tid, NULL, selection_sort, arr) != 0) {
        fprintf(stderr, "pthread_create failed\n");
        return 1;
    }

    /* Wait until the sorting thread finishes before printing. */
    if (pthread_join(tid, NULL) != 0) {
        fprintf(stderr, "pthread_join failed\n");
        return 1;
    }

    printf("Sorted array: ");
    for (int i = 0; i < N; i++)
        printf("%d ", arr[i]);
    putchar('\n');

    return 0;
}
