#include <iostream>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 4
#define ITERATIONS 1000000

long long counter = 0;

pthread_mutex_t mutex;

void* increment(void* arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        pthread_mutex_lock(&mutex);

        counter++;

        pthread_mutex_unlock(&mutex);
    }

    return nullptr;
}

int main()
{
    pthread_t threads[NUM_THREADS];

    pthread_mutex_init(&mutex, nullptr);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], nullptr, increment, nullptr);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], nullptr);
    }

    cout << "Expected value = "
         << NUM_THREADS * ITERATIONS << endl;

    cout << "Actual value   = "
         << counter << endl;

    pthread_mutex_destroy(&mutex);

    return 0;
}