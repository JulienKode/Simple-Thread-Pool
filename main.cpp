#include <iostream>
#include <chrono>
#include <thread>
#include "ThreadPool.hpp"

void func() {
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    std::cout << "I'm a simple thread" << std::endl;
}

int main(int argc, const char * argv[]) {
    ThreadPool pool;

    pool.add(&func);
    pool.add(&func);
    pool.add(&func);
    pool.add(&func);
    std::cout << "I'm the main thread" << std::endl;
    return 0;
}
