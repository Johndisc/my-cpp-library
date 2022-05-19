//
// Created by CGCL on 2022/3/25.
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(int num) {
    thread_num = num;
    srand(time(NULL));
    is_end = false;
    for (int i = 0; i < num; ++i) {
        tv.emplace_back(&ThreadPool::stealingWorker, this, i);
        privateq.emplace_back(new Blocking_deque<Task>());
//        tv.emplace_back(&ThreadPool::worker, this);
    }
}

ThreadPool::~ThreadPool() {
    unique_lock<mutex> uniqueLock(mtx);
    is_end = true;
    uniqueLock.unlock();
    cond.notify_all();
    for(auto &ea:tv) {
        ea.join();
    }
}

void ThreadPool::worker() {
    while (!is_end || !taskq.empty()) {
        unique_lock<mutex> uniqueLock(mtx);
        cond.wait(uniqueLock, [this] { return !taskq.empty() || is_end; });
        if (taskq.empty())
            return;
        Task task = taskq.front();
        taskq.pop();
        uniqueLock.unlock();
        task();
    }
}

void ThreadPool::stealingWorker(int id) {
    unique_lock<mutex> uniqueLock(mtx);
    cond.wait(uniqueLock);
    while (!all_empty()) {
//        if (is_end)
//            return;
        Task task;
        auto res = getOneTask(task, id);
        if (res) {
//            uniqueLock.unlock();
            task();
        }
    }
}

bool ThreadPool::all_empty() {
    for (auto &item: privateq) {
        if (!item->empty())
            return false;
    }
    return true;
}

bool ThreadPool::getOneTask(Task &task,int id) {
    auto &local_q = privateq[id];
    auto res = local_q->pop_front(task);
    if (res) {
        return true;
    }
    //steal
    res = stealOneTask(task);
    if (res) {
        return true;
    }
    return false;
}

bool ThreadPool::stealOneTask(Task &task) {
    auto start_id = rand() % thread_num;
    for (int i = 0; i < thread_num; ++i) {
        auto idx = (i + start_id) % thread_num;
        auto &q = privateq[idx];
        auto res = q->pop_back(task);
        if (res) {
            printf("stealing %d\n", idx);
            return true;
        }
    }
    return false;
}
