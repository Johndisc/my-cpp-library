//
// Created by CGCL on 2022/4/26.
//

#ifndef THREAD_TEST_BLOCKING_DEQUE_H
#define THREAD_TEST_BLOCKING_DEQUE_H

#include <deque>
#include <mutex>

using namespace std;

template<typename T>
class Blocking_deque {
private:
    deque<T> b_deque;
    mutex mtx;

public:
    Blocking_deque()= default;
    void push_back(const T &val);
    void push_front(const T &val);
    bool pop_back(T &t);
    bool pop_front(T &t);
    size_t size();
    bool empty();
};


#endif //THREAD_TEST_BLOCKING_DEQUE_H
