#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define BUFFER_SIZE 5
#define PRODUCERS 2
#define CONSUMERS 2
#define ITEMS_PER_PRODUCER 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;
int count = 0;

pthread_mutex_t mutex;
pthread_cond_t notFull;
pthread_cond_t notEmpty;

void putItem(int item)
{
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    count++;
}

int getItem()
{
    int item = buffer[out];
    out = (out + 1) % BUFFER_SIZE;
    count--;
    return item;
}

void* producer(void* arg)
{
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++)
    {
        int item = id * 100 + i;

        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE)
        {
            cout << "Producer " << id
                 << ": BUFFER FULL, waiting..." << endl;

            pthread_cond_wait(&notFull, &mutex);
        }

        putItem(item);

        cout << "Producer " << id
             << " produced " << item
             << " (items=" << count << ")" << endl;

        pthread_cond_signal(&notEmpty);

        pthread_mutex_unlock(&mutex);

        usleep(10000);
    }

    return nullptr;
}

void* consumer(void* arg)
{
    int id = *(int*)arg;

    int totalItems =
        (PRODUCERS * ITEMS_PER_PRODUCER) / CONSUMERS;

    for (int i = 0; i < totalItems; i++)
    {
        pthread_mutex_lock(&mutex);

        while (count == 0)
        {
            cout << "Consumer " << id
                 << ": BUFFER EMPTY, waiting..." << endl;

            pthread_cond_wait(&notEmpty, &mutex);
        }

        int item = getItem();

        cout << "Consumer " << id
             << " consumed " << item
             << " (items=" << count << ")" << endl;

        pthread_cond_signal(&notFull);

        pthread_mutex_unlock(&mutex);

        usleep(80000);
    }

    return nullptr;
}

int main()
{
    pthread_t producers[PRODUCERS];
    pthread_t consumers[CONSUMERS];

    int producerID[PRODUCERS] = {1, 2};
    int consumerID[CONSUMERS] = {1, 2};

    pthread_mutex_init(&mutex, nullptr);
    pthread_cond_init(&notFull, nullptr);
    pthread_cond_init(&notEmpty, nullptr);

    for (int i = 0; i < CONSUMERS; i++)
    {
        pthread_create(&consumers[i],
                       nullptr,
                       consumer,
                       &consumerID[i]);
    }

    usleep(100000);

    for (int i = 0; i < PRODUCERS; i++)
    {
        pthread_create(&producers[i],
                       nullptr,
                       producer,
                       &producerID[i]);
    }

    for (int i = 0; i < PRODUCERS; i++)
    {
        pthread_join(producers[i], nullptr);
    }

    for (int i = 0; i < CONSUMERS; i++)
    {
        pthread_join(consumers[i], nullptr);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&notFull);
    pthread_cond_destroy(&notEmpty);

    cout << "All bounded-buffer operations completed." << endl;

    return 0;
}