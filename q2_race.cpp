#include <iostream>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 4
#define ITERATIONS 1000000

long long counter = 0;

void* increment(void* arg)
{
    for (int i = 0; i < ITERATIONS; i++)
    {
        counter++;
    }

    return nullptr;
}

int main()
{
    pthread_t threads[NUM_THREADS];

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

    return 0;
}