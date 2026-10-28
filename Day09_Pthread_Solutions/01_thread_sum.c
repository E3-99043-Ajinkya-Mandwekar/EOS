#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct range {
    int start;
    int end;
};

void *thread_sum(void *param)
{
    struct range *r = (struct range *)param;
    long long sum = 0;

    for (int i = r->start; i <= r->end; i++)
        sum += i;

    /* Return the result through allocated memory so main can read it. */
    long long *result = malloc(sizeof(*result));
    if (result == NULL)
        return NULL;

    *result = sum;
    return result;
}

int main(void)
{
    pthread_t tid;
    struct range r;
    long long *result = NULL;

    printf("Enter start and end: ");
    if (scanf("%d %d", &r.start, &r.end) != 2 || r.start > r.end) {
        fprintf(stderr, "Invalid range. Start must be <= end.\n");
        return 1;
    }

    if (pthread_create(&tid, NULL, thread_sum, &r) != 0) {
        perror("pthread_create");
        return 1;
    }

    if (pthread_join(tid, (void **)&result) != 0) {
        fprintf(stderr, "pthread_join failed\n");
        return 1;
    }

    if (result == NULL) {
        fprintf(stderr, "Thread could not allocate result memory\n");
        return 1;
    }

    printf("Sum from %d to %d = %lld\n", r.start, r.end, *result);
    free(result);
    return 0;
}
