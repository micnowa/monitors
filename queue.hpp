//
// Created by Michał Nowaliński on 12/9/19.
//

#ifndef MONITORS_QUEUE_HPP
#define MONITORS_QUEUE_HPP

#define COM_LEN 3

typedef enum {
    NORMAL, HIGH, ULTRA
} priority;

//char priority_characters[] = {'0', '1', '2'};

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
