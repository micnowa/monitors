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
#define MS_TO_US 1000

char *sem_empty_name[QUEUE_NUM] = {"sem_empty_0", "sem_empty_1", "sem_empty_2"};
char *sem_full_name[QUEUE_NUM] = {"sem_full_0", "sem_full_1", "sem_full_2"};

sem_t *sem_empty[QUEUE_NUM];
sem_t *sem_full[QUEUE_NUM];

Monitor<Queue> *queues_monitors[QUEUE_NUM];

char com[] = {'A', 'B', 'C'};
double pr = 0.30;
int times = 20;
int sleep_ms_time = 500;
int freq = 500;

void initialize_semaphores() {
    for (int i = 0; i < QUEUE_NUM; i++) {
        sem_empty[i] = sem_open(sem_empty_name[i], O_CREAT, QUEUE_SIZE);
        sem_full[i] = sem_open(sem_full_name[i], O_CREAT, 0);
    }
}

void producer(int q_n) {
    std::cout << "producer " << q_n << std::endl;
    int counter = 0;
    char *signs = static_cast<char *>(malloc(sizeof(char) * (COM_LEN + 1)));
    signs[COM_LEN] = '\0';

    while (counter != times) {
        for (int j = 0; j < COM_LEN; j++) signs[j] = com[rand() % COM_LEN];
        usleep(MS_TO_US * sleep_ms_time);
        sem_wait(sem_empty[q_n]);
        queues_monitors[q_n]->operator->()->push(signs, NORMAL);
        printf("Added %s(%d) to q[%d]\n", signs, NORMAL, q_n);
        sem_post(sem_full[q_n]);
        counter++;
    }
    free(signs);
}

void consumer(int q_n) {
    std::cout << "consumer " << q_n << std::endl;
    int counter = 0;
    int waiting = 0;
    while (counter != times) {
        if (waiting) {
            usleep(MS_TO_US * sleep_ms_time);
            waiting--;
        }
        sem_wait(sem_full[q_n]);
        char *tmp = queues_monitors[q_n]->operator->()->pop();
        printf("Consumed %s from q[%d]\n", tmp, q_n);
        sem_post(sem_empty[q_n]);
        if (tmp == nullptr) continue;
        if (tmp[COM_LEN] == ULTRA_CHAR) waiting += 5;

        usleep(MS_TO_US * freq);
        double r = ((double) rand() / (RAND_MAX));
        if (r < pr) {
            for (int m = 0; m < COM_LEN - 1; m++) tmp[m] = tmp[m + 1];
            tmp[COM_LEN - 1] = com[rand() % COM_LEN];
            char *new_tmp = static_cast<char *>(malloc(sizeof(char) * (COM_LEN + 1)));
            for (int m = 0; m < COM_LEN; m++) new_tmp[m] = tmp[m];
            new_tmp[COM_LEN] = '\0';
            sem_wait(sem_empty[q_n]);
            queues_monitors[q_n]->operator->()->push(tmp, NORMAL);
            printf("Added %s(0) to q[%d]\n", tmp, q_n);
            sem_post(sem_full[q_n]);
            free(new_tmp);
        }
        free(tmp);
        counter++;
    }
}

void special_producer() {
    std::cout<<"Special producer"<<std::endl;
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
}

void protective_producer() {
    printf("Protective producer O\n");
    int sizes[QUEUE_NUM];
    int counter = 0;
    while (counter != times) {
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
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    auto **q = (Queue **) malloc(QUEUE_NUM * sizeof(Queue *));
    for (int i = 0; i < QUEUE_NUM; i++) q[i] = new Queue(QUEUE_SIZE);

    srand(time(nullptr));
    initialize_semaphores();

    for (int i = 0; i < QUEUE_NUM; ++i) queues_monitors[i] = new Monitor<Queue>();
    for (int i = 0; i < QUEUE_NUM; ++i) queues_monitors[i]->setMCl(q[i]);
    std::vector<std::thread> threads;

    /** Producers **/
    threads.emplace_back([&]() { producer(0); });
    threads.emplace_back([&]() { producer(1); });
    threads.emplace_back([&]() { producer(2); });

    /** Consumers **/
    threads.emplace_back([&]() { consumer(0); });
    threads.emplace_back([&]() { consumer(1); });
    threads.emplace_back([&]() { consumer(2); });

    /** Special Producer **/
    threads.emplace_back([&]() { special_producer(); });

    /** Protective Producer (O) **/
    threads.emplace_back([&]() { protective_producer(); });

    for (auto &t : threads) t.join();

    for (int i = 0; i < QUEUE_NUM; ++i) queues_monitors[i]->operator->()->print_queue();
    return 0;
}
