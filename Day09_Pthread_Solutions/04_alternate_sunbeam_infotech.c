#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static int sunbeam_turn = 1; /* Start with SUNBEAM. */

void *print_sunbeam(void *arg)
{
    (void)arg;
    for (;;) {
        pthread_mutex_lock(&lock);

        while (!sunbeam_turn)
            pthread_cond_wait(&cond, &lock);

        puts("SUNBEAM");
        fflush(stdout);
        sunbeam_turn = 0;
        pthread_cond_broadcast(&cond);

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void *print_infotech(void *arg)
{
    (void)arg;
    for (;;) {
        pthread_mutex_lock(&lock);

        while (sunbeam_turn)
            pthread_cond_wait(&cond, &lock);

        puts("INFOTECH");
        fflush(stdout);
        sunbeam_turn = 1;
        pthread_cond_broadcast(&cond);

        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, print_sunbeam, NULL) != 0 ||
        pthread_create(&t2, NULL, print_infotech, NULL) != 0) {
        fprintf(stderr, "Could not create threads\n");
        return 1;
    }

    /* This program intentionally runs continuously. Stop with Ctrl+C. */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&lock);
    return 0;
}
