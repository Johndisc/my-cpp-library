//
// Created by CGCL on 2022/3/25.
//

#ifndef THREAD_TEST_THREADPOOL_H
#define THREAD_TEST_THREADPOOL_H

#include <thread>
#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include "Blocking_deque.h"
#include "Blocking_deque.cpp"

using namespace std;
typedef function<void()> Task;

inline int func(int start, int end)
{
    int sum = 0;
    for (int i = start; i < end; ++i) {
        sum += i;
    }
    return sum;
}

class ThreadPool {
public:
    explicit ThreadPool(int num);

    ~ThreadPool();

    template<class F, class ...Args>
    auto enqueue(F &&f, Args&&... args) -> future<decltype(f(args...))> {
        using return_type = decltype(f(args...));
        auto task = make_shared<packaged_task < return_type() >>
        (bind(forward<F>(f), forward<Args>(args)...));
        future <return_type> result = task->get_future();
        unique_lock<mutex> uniqueLock(mtx);
        taskq.push([task]{ (*task)(); });
        uniqueLock.unlock();

        cond.notify_one();
        return result;
    }

    auto ws_enqueue()
    {
        int s = 0, e = 50;
        vector <vector<future<int>>> result{10};
        for (int i = 0; i < thread_num; ++i) {
            for (int j = 0; j < 20; ++j) {
                auto task = make_shared<packaged_task < int() >>
                ([a = s, b = e] { return func(a, b); });
                result[i].push_back(task->get_future());
                privateq[i]->push_back([task] { (*task)(); });
                s = e;
                e += 50;
            }
        }
        cond.notify_all();
        return result;
    }

private:
    bool is_end;
    vector <thread> tv;
    queue<Task> taskq;
    mutex mtx;
    condition_variable cond;
    vector <Blocking_deque<Task>*> privateq;
    int thread_num;

    void worker();
    bool all_empty();
    void stealingWorker(int id);
    bool getOneTask(Task &task, int id);
    bool stealOneTask(Task &task);
};


#endif //THREAD_TEST_THREADPOOL_H
