/**
 * @file async_adaptor.cpp
 * @author SergeiNA (you@domain.com)
 * @brief AsyncAdaptor definition 
 * @version 1.0
 * @date 2020-01-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "async_adaptor.h"

#include <memory>
#include <iostream>
#include <sstream>

namespace async{

    AsyncAdaptor::AsyncAdaptor(std::vector<std::shared_ptr<ThreadManager>> subs,
                               std::size_t bsize){
    
        // create QueueCommnad
        auto q_command = std::make_unique<QueueCommand>(bsize);

        // subscribe threadManagers to QueueCommnad
        for(auto&& s:subs)
            q_command->subscribe(s);

        // create CommandHandler with prepared QueueCommnad
        cmdHandler = CommandHandler(std::move(q_command));
    }

    void AsyncAdaptor::collectData(const char *data, std::size_t size){

        // preparation for copy and copy
        std::string buffer = dataStream;
        buffer.resize(buffer.size() + size);
        std::copy(data, data+size, std::next(buffer.begin(), dataStream.size()));

        // check for a '\n'
        std::size_t lastEndlPos =buffer.find_last_of('\n');

        if (lastEndlPos == std::string::npos){
            dataStream = buffer;
            return;
        }

        // split string to active part and reminder
        std::string buffer_to_parse = buffer.substr(0, lastEndlPos+1);
        dataStream = buffer.substr(lastEndlPos+1,std::string::npos);

        // start processing
        std::istringstream iss{buffer_to_parse.data()};
        cmdHandler.Run(iss);
    }

    AsyncAdaptor::~AsyncAdaptor()
    {
        cmdHandler.dumpRemains();
    }
}
