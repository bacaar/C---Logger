#include "LogThreader.hpp"

#include <iostream>

#include "Logger.hpp"

LogThreader::LogThreader(){
    m_loggerRunning = true;
    t = std::thread(&LogThreader::logging, this);
}

LogThreader::~LogThreader(){
    m_loggerRunning = false;
    t.join();   // wait for logging method to finish
    std::cout << "LogThreader has been shut down" << std::endl;
}

void LogThreader::logging(){

    while(true){
        bool canExit = !m_loggerRunning;    // if logger is still running, no chance for exit

        for(int i = 0; i < m_handledLoggers.size(); ++i){

            // do one item per queue per iteration, so that every logger gets processed equally
            int size = m_handledLoggers[i]->getQueueSize();
            if(size > 0){
                std::unique_ptr<LogEntry> entry = m_handledLoggers[i]->getQueueItem();

                // wirte to console and/or file
                m_handledLoggers[i]->print(move(entry));
                size--;
            }
            if(size > 0) canExit = false;   // if m_loggerRunning is false, then even if only one logger has some items in it's queue, a new iteration is needed
        }

        if(canExit){
            break;
        }
    }
}

void LogThreader::addLogger(std::shared_ptr<Logger> logger){
    m_handledLoggers.push_back(logger);
    logger->m_isHandledByThreader = true;
}