/**
 * @file examples.cpp
 * @author SergeiNA (you@domain.com)
 * @brief Examples for using async library
 * 
 * Library works as bulkmt but input data controls by user:
 *  - size of input data
 *  - data income time
 *  - for output used only one ThreadManager. All connects
 * work with same loggers
 * @version 0.1
 * @date 2020-01-19
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "async.h"

#include <iostream>
#include <thread>

int main(int, char *[]) {

    // task example
    std::size_t bulk = 3;
    auto init1 = async::connect(bulk);
    auto init2 = async::connect(bulk);

    async::receive(init1, "1", 1);
    async::receive(init2, "1\n", 2);
    async::receive(init1, "\n2\n3\n4\n5\n6\n{\n7\n", 15);
    async::receive(init1, "8\n9\n10\n}\n11\n", 11);
    async::disconnect(init1);
    async::disconnect(init2);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    std::cout << "^^ Task initial example ^^\n";

    auto h = async::connect(2);
    auto h2 = async::connect(3);
    auto h3 = async::connect(4);
    auto h4 = async::connect(5);

    std::thread th0([](void* h){
        async::receive(h, "1\n2\n3\n4\n5\n6\n7\n", 14);
        async::receive(h, "1\n2\n3\n4\n5\n6\n7\n", 14);
        async::receive(h, "1\n2\n3\n4\n5\n6\n7\n", 14);
        },h);

    std::thread th1([](void* h){
        async::receive(h, "7\n6\n5\n4\n3\n2\n1\n", 14);
        async::receive(h, "7\n6\n5\n4\n3\n2\n1\n", 14);
        },h2);

    std::thread th3([](void* h){
        async::receive(h, "{\n1\n2\n3\n4\n3\n2\n1\n}\n", 18);
        async::receive(h, "1\n2\n3\n4\n3\n2\n1\n", 14);
        },h3);


    std::thread th4([](void* h){
        async::receive(h, "{\n3\n4\n5\n6\n5\n3\n4\n}\n", 18);
        async::receive(h, "3\n4\n5\n6\n5\n3\n4\n3\n", 16);
        async::receive(h, "{\n3\n4\n5\n6\n5\n3\n4\n}\n", 18);
        async::receive(h, "3\n4\n5\n6\n5\n3\n4\n3\n", 16);
        },h4);
    
    th4.join();
    th0.join();
    th1.join();
    async::disconnect(h);
    async::disconnect(h2);

    th3.join();

    async::disconnect(h3);
    async::disconnect(h4);


    return 0;
}
