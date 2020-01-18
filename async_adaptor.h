/**
 * @file async_adaptor.h
 * @author SergeiNA (you@domain.com)
 * @brief Async adapter class for bulkmt
 * @version 1.0
 * @date 2020-01-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once

#include "command_handler.h"

namespace async
{
    class AsyncAdaptor{
    public:
        /**
         * @brief Construct a new Async Adaptor object
         * with vector of ThreadManager and bulk size
         * 
         * @param subs vector of subscribers
         * @param sizeb bulk size
         */
        AsyncAdaptor(std::vector<std::shared_ptr<ThreadManager>> subs, std::size_t sizeb);
        /**
         * @brief Destroy the Async Adaptor object and dump all remainds in QueueCommand
         * 
         */
        ~AsyncAdaptor();
        /**
         * @brief collect data from receiver
         * 
         * Accumulate chars and send to CommandHandler
         * 
         * Accumulate input chars, separate by last end line
         * first part send to CommandHandler, second store in 
         * dataStream
         * 
         * @param data input data
         * @param size size of input data
         */
        void collectData(const char *data, std::size_t size);
    private:
        CommandHandler cmdHandler;///< command processor
        std::string dataStream;///< contains remaind chars after separating
    };
    
} // namespace async
