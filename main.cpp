#include <iostream>
#include <cstdlib>
#include <vector>
#include <thread>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include "queue.hpp"
#include "monitor.hpp"

using namespace std;

#define QUEUE_NUM 3
#define QUEUE_SIZE 20
#define CONSUMER_NUM 3
#define PRODUCER_NUM 3
#define PRODUCER_S_NUM 1
#define PRODUCER_O_NUM 1
#define MS_TO_US 1000

char *sem_empty_name[QUEUE_NUM] = {"sem_empty_0", "sem_empty_1", "sem_empty_2"};
char *sem_full_name[QUEUE_NUM] = {"sem_full_0", "sem_full_1", "sem_full_2"};

sem_t *sem_empty[QUEUE_NUM];
sem_t *sem_full[QUEUE_NUM];

char com[] = {'A', 'B', 'C'};
double pr = -1.0;
int times = 20;
int sleep_ms_time = 500;

void initialize_semaphores() {
    for (int i = 0; i < QUEUE_NUM; i++) {
        sem_empty[i] = sem_open(sem_empty_name[i], O_CREAT, QUEUE_SIZE);
        sem_full[i] = sem_open(sem_full_name[i], O_CREAT, 0);
    }
}

void produce(Monitor<Queue>::monitor_helper q, int pri, int queue_num, sem_t *empty, sem_t *full) {
    std::cout << "producer " << queue_num << std::endl;
    int counter = 0;
    char *signs = static_cast<char *>(malloc(sizeof(char) * (COM_LEN + 1)));
    signs[COM_LEN] = '\0';

    while (counter != times) {
        for (int j = 0; j < COM_LEN; j++) signs[j] = com[rand() % COM_LEN];
        usleep(MS_TO_US * sleep_ms_time);
        sem_wait(empty);
        q->push(signs, pri);
        printf("Added %s(0) to q[%d]\n", signs, queue_num);
        sem_post(full);
        counter++;
    }
    free(signs);
}

void consume(Monitor<Queue>::monitor_helper q, int queue_num, sem_t *empty, sem_t *full) {
    std::cout << "consumer " << queue_num << std::endl;
    int counter = 0;

    while (counter != times) {
        usleep(MS_TO_US * sleep_ms_time);
        sem_wait(full);
        char *tmp = q->pop();
        printf("Consumed %s from q[%d]\n", tmp, queue_num);
        sem_post(empty);
        counter++;
    }
}


int main() {
    std::cout << "Hello, World!" << std::endl;
    auto **q = (Queue **) malloc(QUEUE_NUM * sizeof(Queue *));
    for (int i = 0; i < QUEUE_NUM; i++) q[i] = new Queue(QUEUE_SIZE);

    srand(time(nullptr));
    initialize_semaphores();

    Monitor<Queue> *queues_monitors[QUEUE_NUM];
    for (int i = 0; i < QUEUE_NUM; ++i) queues_monitors[i] = new Monitor<Queue>();
    for (int i = 0; i < QUEUE_NUM; ++i) queues_monitors[i]->setMCl(q[i]);
    int users_num = CONSUMER_NUM + PRODUCER_NUM + PRODUCER_S_NUM + PRODUCER_O_NUM;
    std::vector<std::thread> threads;


    /** Producers **/
    threads.emplace_back([&]() {
        std::cout << "producer " << 0 << std::endl;
        int counter = 0;
        char *signs = static_cast<char *>(malloc(sizeof(char) * (COM_LEN + 1)));
        signs[COM_LEN] = '\0';

        while (counter != times) {
            for (int j = 0; j < COM_LEN; j++) signs[j] = com[rand() % COM_LEN];
            usleep(MS_TO_US * sleep_ms_time);
            sem_wait(sem_empty[0]);
            queues_monitors[0]->operator->()->push(signs, NORMAL);
            printf("Added %s(0) to q[%d]\n", signs, 0);
            sem_post(sem_full[0]);
            counter++;
        }
        free(signs);
    });

    threads.emplace_back([&]() {
        std::cout << "producer " << 1 << std::endl;
        int counter = 0;
        char *signs = static_cast<char *>(malloc(sizeof(char) * (COM_LEN + 1)));
        signs[COM_LEN] = '\0';

        while (counter != times) {
            for (int j = 0; j < COM_LEN; j++) signs[j] = com[rand() % COM_LEN];
            usleep(MS_TO_US * sleep_ms_time);
            sem_wait(sem_empty[1]);
            queues_monitors[1]->operator->()->push(signs, NORMAL);
            printf("Added %s(0) to q[%d]\n", signs, 1);
            sem_post(sem_full[1]);
            counter++;
        }
        free(signs);
    });

    threads.emplace_back([&]() {
        std::cout << "producer " << 2 << std::endl;
        int counter = 0;
        char *signs = static_cast<char *>(malloc(sizeof(char) * (COM_LEN + 1)));
        signs[COM_LEN] = '\0';

        while (counter != times) {
            for (int j = 0; j < COM_LEN; j++) signs[j] = com[rand() % COM_LEN];
            usleep(MS_TO_US * sleep_ms_time);
            sem_wait(sem_empty[2]);
            queues_monitors[2]->operator->()->push(signs, NORMAL);
            printf("Added %s(0) to q[%d]\n", signs, 2);
            sem_post(sem_full[2]);
            counter++;
        }
        free(signs);
    });
    /** ************************************************** **/



    /** Consumers **/
    threads.emplace_back([&]() {
        std::cout << "consumer " << 0 << std::endl;
        int counter = 0;

        while (counter != times) {
            usleep(MS_TO_US * sleep_ms_time);
            sem_wait(sem_full[0]);
            char *tmp = queues_monitors[0]->operator->()->pop();
            printf("Consumed %s from q[%d]\n", tmp, 0);
            sem_post(sem_empty[0]);
            counter++;
        }
    });

    threads.emplace_back([&]() {
        std::cout << "consumer " << 1 << std::endl;
        int counter = 0;

        while (counter != times) {
            usleep(MS_TO_US * sleep_ms_time);
            sem_wait(sem_full[1]);
            char *tmp = queues_monitors[1]->operator->()->pop();
            printf("Consumed %s from q[%d]\n", tmp, 1);
            sem_post(sem_empty[1]);
            counter++;
        }
    });

    threads.emplace_back([&]() {
        std::cout << "consumer " << 2 << std::endl;
        int counter = 0;

        while (counter != times) {
            usleep(MS_TO_US * sleep_ms_time);
            sem_wait(sem_full[2]);
            char *tmp = queues_monitors[2]->operator->()->pop();
            printf("Consumed %s from q[%d]\n", tmp, 2);
            sem_post(sem_empty[2]);
            counter++;
        }
    });
    /** ************************************************** **/



    /** Special Producer **/
    threads.emplace_back([&]() {
        printf("Special producer\n");
        int counter = 0;
        char *signs = static_cast<char *>(malloc(sizeof(char) * (COM_LEN + 1)));
        signs[COM_LEN] = '\0';
        while (counter != times) {
            int queue_number = rand() % QUEUE_NUM;
            for (int j = 0; j < COM_LEN; j++) signs[j] = com[rand() % COM_LEN];
            sem_wait(sem_empty[queue_number]);
            queues_monitors[queue_number]->operator->()->push(signs, HIGH);
            sem_post(sem_full[queue_number]);
            printf("Added %s(1) to q[%d]\n", signs, queue_number);
            usleep(MS_TO_US * sleep_ms_time);
            counter++;
        }
    });
    /** ************************************************** **/



    /** Protective Producer (O) **/
    threads.emplace_back([&]() {
        printf("Protective producer O\n");
        int sizes[QUEUE_NUM];
        int counter = 0;
        while(counter != times) {
            for (int j = 0; j < QUEUE_NUM; j++) sizes[j] = queues_monitors[j]->operator->()->size();
            const int N = sizeof(sizes) / sizeof(int);
            int queue_num = distance(sizes, std::max_element(sizes, sizes + N));
            char *signs = "   ";
            sem_wait(sem_empty[queue_num]);
            queues_monitors[queue_num]->operator->()->push(signs, ULTRA);
            printf("Added %s(%d) to q[%d]\n", signs, ULTRA, queue_num);
            sem_post(sem_full[queue_num]);
            counter++;
        }
    });
    /** ************************************************** **/



    for (auto &t : threads) {
        t.join();
    }

    for (int i = 0; i < QUEUE_NUM; ++i) queues_monitors[i]->operator->()->print_queue();
    return 0;
}
