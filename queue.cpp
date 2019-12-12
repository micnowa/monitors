//
// Created by root on 12/9/19.
//

#include "queue.hpp"
#include <cstdlib>
#include <cstring>
#include <cstdio>


bool Queue::push(const char *c, int priority) {
    if (!_is_initialized() || _is_full()) return false;

    int pos = 0;
    /* Rewriting communicates */
    for (int j = 0; j < COM_LEN; j++) {
        char *characters = new char[(strlen(str[j]) + 2)];
        for (int i = 0; i <= (int) strlen(str[j]); i++) {
            if (priorities[i] >= priority && (str[j])[i] != '\0') characters[i] = (str[j])[i];
            else {
                pos = i;
                break;
            }
        }
        characters[pos] = c[j];
        for (int i = pos; i < (int) strlen(str[j]); i++) characters[i + 1] = (str[j])[i];
        characters[strlen(str[j]) + 1] = '\0';
        strcpy(str[j], characters);
        free(characters);
    }

    /* Rewriting priorities */
    int *integers = new int[max];
    for (int i = 0; i < strlen(str[0]); i++) {
        if (i == pos) priorities[i] = priority;
        else if (i > pos) priorities[i] = priorities[i + 1];
    }
    free(integers);
    return true;
}


char *Queue::pop() {
    if (_is_empty()) return nullptr;
    char *c = static_cast<char *>(malloc(sizeof(char) * (COM_LEN + 2)));
    for (int j = 0; j < COM_LEN; j++) {
        c[j] = (str[j])[0];
        for (int i = 0; i < max; i++) (str[j])[i] = (str[j])[i + 1];
        if (j == 0) {
            switch (priorities[0]) {
                case NORMAL:
                    c[COM_LEN] = '0';
                    break;
                case HIGH:
                    c[COM_LEN] = '1';
                    break;
                case ULTRA:
                    c[COM_LEN] = '2';
                    break;
                default:
                    break;
            }

            for (int i = 0; i < max - 1; i++) priorities[i] = priorities[i + 1];
        }
    }
    c[COM_LEN + 1] = '\0';
    return c;
}

int Queue::size() {
    if (_is_empty() || !_is_initialized()) return 0;
    if (_is_full()) return max;
    return strlen(str[0]);
}

bool Queue::destroy_queue() {
    free(str);
    return true;
}


void Queue::print_queue() {
    if (!strlen(str[0])) puts("Queue empty");
    puts("Queue:");
    for (unsigned long i = 0; i < strlen(str[0]); i++) {
        for (unsigned long j = 0; j < COM_LEN; j++) printf("%c", str[j][i]);
        printf(", %d\n", priorities[i]);
    }
}


bool Queue::_is_initialized() { return this != nullptr; }


bool Queue::_is_empty() { return strlen(str[0]) == 0; }


bool Queue::_is_full() { return strlen(str[0]) == max; }

void Queue::flush_queue() {
    for (auto &i : str) {
        delete[] i;
        i = new char[max + 1];
    }
    free(priorities);
    priorities = new int[max];
}

Queue::Queue(int max_size) {
    max = max_size;
    for (auto &i : str) i = new char[max + 1];
    priorities = new int[max];
}

Queue::Queue() {}
