//
// Created by CGCL on 2022/4/26.
//

#include "Blocking_deque.h"

template<typename T>
void Blocking_deque<T>::push_back(const T &val) {
    lock_guard <mutex> locker(mtx);
    b_deque.push_back(val);
}

template<typename T>
void Blocking_deque<T>::push_front(const T &val) {
    lock_guard <mutex> locker(mtx);
    b_deque.push_back(val);
}

template<typename T>
bool Blocking_deque<T>::pop_back(T &t) {
    lock_guard <mutex> locker(mtx);
    if (b_deque.empty())
        return false;
    t = b_deque.back();
    b_deque.pop_back();
    return true;
}

template<typename T>
bool Blocking_deque<T>::pop_front(T &t) {
    lock_guard <mutex> locker(mtx);
    if (b_deque.empty())
        return false;
    t = b_deque.front();
    b_deque.pop_front();
    return true;
}

template<typename T>
size_t Blocking_deque<T>::size() {
    lock_guard <mutex> locker(mtx);
    return b_deque.size();
}

template<typename T>
bool Blocking_deque<T>::empty() {
    lock_guard <mutex> locker(mtx);
    return b_deque.empty();
}