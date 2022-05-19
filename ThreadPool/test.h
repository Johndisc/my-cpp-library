#include "ThreadPool.h"

template<class F, class... Args>
auto eenqueue(F f, Args... args) -> future<decltype(f(args...))> {
    using return_type = decltype(f(args...));
    future <return_type> result = async(bind(f, args...));
    return result;
}

void tptest()
{
    int res, sum = 0;
    for (int i = 0; i < 10000; ++i) {
        sum += i;
    }
    cout << sum << endl;
    sum = 0;
    ThreadPool threadPool(10);
    auto result = threadPool.ws_enqueue();
    for (int i = 0; i < 10; ++i) {
        for (auto &item: result[i]) {
            sum += item.get();
        }
    }
    cout << sum << endl;
}