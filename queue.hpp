//
// Created by Michał Nowaliński on 12/9/19.
//

#ifndef MONITORS_QUEUE_HPP
#define MONITORS_QUEUE_HPP

#define COM_LEN 3
#define NORMAL_CHAR '0'
#define HIGH_CHAR '1'
#define ULTRA_CHAR '2'


typedef enum {
    NORMAL, HIGH, ULTRA
} priority;



class Queue {
private:
    int max;
    char *str[COM_LEN];
    int *priorities;

    bool _is_initialized();

    bool _is_empty();

    bool _is_full();

public:
    Queue();

    Queue(int max_size);

    bool push(const char *c, int priority);

    char *pop();

    int size();

    void print_queue();

    bool destroy_queue();

    void flush_queue();

};


#endif
