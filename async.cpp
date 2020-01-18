/**
 * @file async.cpp
 * @author SergeiNA (you@domain.com)
 * @brief Definition of core async functions
 * @version 0.1
 * @date 2020-01-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "async.h"
#include "async_adaptor.h"

namespace async {

    std::once_flag f1;

    auto fileLogThreadManager = std::make_shared<ThreadManager>();
    auto terminalLogThreadManager = std::make_shared<ThreadManager>();
    /**
     * @brief used for intilize fileLogThreadManager and terminalLogThreadManager once
     * 
     */
    void threadManagerInitilizer(){
        /// call once and subscribe loggers to corresponding threadManagers
        std::call_once(
            f1,
            [](std::shared_ptr<ThreadManager>& file,
               std::shared_ptr<ThreadManager>& terminal ){
                    file->subscribe(std::make_shared<log_observer>());
                    file->subscribe(std::make_shared<log_observer>());
                    terminal->subscribe(std::make_shared<terminal_observer>());
                },
                fileLogThreadManager,
                terminalLogThreadManager
             );
    }

    handle_t connect(std::size_t bulk) {
        threadManagerInitilizer();
        return new AsyncAdaptor({fileLogThreadManager, terminalLogThreadManager},bulk);
    }

    void receive(handle_t handle, const char *data, std::size_t size) {
        AsyncAdaptor* asyncAdaptor = static_cast<AsyncAdaptor*>(handle);
        asyncAdaptor->collectData(data,size);
    }

    void disconnect(handle_t handle) {
        AsyncAdaptor* asyncAdaptor = static_cast<AsyncAdaptor*>(handle);
        delete asyncAdaptor;
    }

}
