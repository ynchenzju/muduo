#include <iostream>
#include <thread>
#include "ThreadPool.h"
#include "CountDownLatch.h"

void print() {
    std::hash<std::thread::id> hasher;
    long int pid = hasher(std::this_thread::get_id());
    printf("tid=%ld\n", pid);
}
int main() {
    ThreadPool pool("new_pool");
    pool.setMaxQueueSize(4);
    pool.start(2);
    pool.run(print);
    pool.run(print);
    CountDownLatch latch(1);
    pool.run(std::bind(&CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
