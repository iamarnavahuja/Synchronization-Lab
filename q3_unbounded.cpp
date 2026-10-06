#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>

using namespace std;

#define PRODUCERS 2
#define CONSUMERS 2
#define ITEMS_PER_PRODUCER 10

struct Node
{
    int value;
    Node* next;
};

Node* head = nullptr;
Node* tail = nullptr;

int items = 0;

pthread_mutex_t mutex;
pthread_cond_t notEmpty;

void putItem(int value)
{
    Node* newNode = new Node;

    newNode->value = value;
    newNode->next = nullptr;

    if (tail == nullptr)
    {
        head = tail = newNode;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }

    items++;
}

int getItem()
{
    Node* temp = head;

    int value = temp->value;

    head = head->next;

    if (head == nullptr)
    {
        tail = nullptr;
    }

    delete temp;

    items--;

    return value;
}

void* producer(void* arg)
{
    int id = *(int*)arg;

    for (int i = 0; i < ITEMS_PER_PRODUCER; i++)
    {
        int value = id * 100 + i;

        pthread_mutex_lock(&mutex);

        putItem(value);

        cout << "Producer " << id
             << " produced " << value
             << " (items=" << items << ")" << endl;

        pthread_cond_signal(&notEmpty);

        pthread_mutex_unlock(&mutex);

        usleep(20000);
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

        while (items == 0)
        {
            cout << "Consumer " << id
                 << ": BUFFER EMPTY, waiting..." << endl;

            pthread_cond_wait(&notEmpty, &mutex);
        }

        int value = getItem();

        cout << "Consumer " << id
             << " consumed " << value
             << " (items=" << items << ")" << endl;

        pthread_mutex_unlock(&mutex);

        usleep(70000);
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
    pthread_cond_destroy(&notEmpty);

    cout << "All unbounded-buffer operations completed." << endl;

    return 0;
}