/**
 * @file command_handler.cpp
 * @author SergeiNA (you@domain.com)
 * @brief Method definitoin of QueueCommand and CommandHandler
 * @version 1.0
 * @date 2020-01-05
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include "command_handler.h"

void QueueCommand::addCommand(std::string cmd)
{
    if(pack.first.empty())
        pack.second = getUnixTime();
    pack.first.emplace_back(std::move(cmd));
    if (pack.first.size() == block_size_ && !nested)
    {
        notify();
    }
}

void QueueCommand::subscribe(std::shared_ptr<ThreadManager> tmanager)
{
    subs.emplace_back(tmanager);
}
/**
 * @brief Sent cmd message to all subscribers
 * 
 * Send all commands as a packege with timestamp to all subs
 * If metrics_ exist collect package information
 * Clear pack of commands (delete commands)
 * 
 * Note: If vector of commands is empty 
 * "queuery is empty" - will be send as command
 * and "empty" as a timestamp
 * 
 */
void QueueCommand::notify()
{
    if(empty())
        return;
    for (size_t i = 0; i < subs.size(); i++)
        subs[i]->push(pack);
    
    if(metrics_)
       metrics_->collect(pack.first);

    pack.first.clear();
    pack.first.resize(0);
}

void QueueCommand::set_nested(bool nested_){
    nested = nested_;
}

rawTimestamp QueueCommand::getUnixTime()
{
    using namespace std::chrono;
    return system_clock::now().time_since_epoch();
}

/**
 * @brief handle all type of input commands
 * 
 * If process get block type command ('{' or '}')
 * depends on current state It will operate differently
 * 
 * - Current state is 'regular':
 *      get usual command -> call addCommand for current command
 *      get '{' block start indicator -> call notify, switch state to 
 *          'nested' and  increment braces_count
 *      get '}' block end indicator -> throw exception
 * 
 * - Current state is 'nested':
 *      get usual command -> call addCommand for current command
 *      get '{' block start indicator -> increment braces_count
 *      get '}' block end indicator -> decrement braces_count
 *          if braces_count == 0 -> switch state to regular and call notify
 * 
 * Also accumulate number of handled lines
 * 
 * @param cmd input command
 */

void CommandHandler::process(std::string &&cmd)
{
    ++nstrings;
    if (cmd == "{")
    {
        if (!isNested())
        {
            state = (size_t)cmdState::nested;
            queueCmd_->set_nested(true);
            queueCmd_->notify();
        }
        ++braces_count;
    }
    else if (cmd == "}")
    {
        if (!braces_count)
            throw std::invalid_argument("} can not be first bracket");
        --braces_count;
        if (!braces_count)
        {
            state = (size_t)cmdState::regular;
            queueCmd_->set_nested(false);
            queueCmd_->notify();
        }
    }
    else
        queueCmd_->addCommand(cmd);
}

/**
 * @brief Read all commands from stdin and call process on them
 * 
 * Read form is input stream while is not empty
 * Call dumpRemains() when while loop is end
 * and if metrics_ exist collect metrics
 * 
 * @param is input stdin stream
 */
void CommandHandler::Run(std::istream &is = std::cin)
{
    while (is && !is.eof())
    {
        std::string temp;
        std::getline(is, temp);
        if(temp.empty()&&is.eof())
            break;
        process(std::move(temp));
    }
}

CommandHandler::~CommandHandler(){
}

void CommandHandler::operator=(CommandHandler&& lhs){
    queueCmd_ = std::move(lhs.queueCmd_);
    metrics_ = std::move(lhs.metrics_);
    state = lhs.state;
    braces_count = lhs.braces_count;
    nstrings = lhs.nstrings;
}

void CommandHandler::dumpRemains(){
    if (!isNested()&&!queueCmd_->empty())
        queueCmd_->notify();
    if(metrics_)
        metrics_->collect({},nstrings);
}