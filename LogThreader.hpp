/*
 * @author:	Aaron Bacher
 * @date:	2022-10-24
 *
 * @brief:	Class to handle multiple log-files in single thread
 */

#pragma once

#include <vector>
#include <memory>

#include "Logger.hpp"   // no forward declaration as we need ENABLE_MULTITHREADING

#if ENABLE_MULTITHREADING
// multithreading
#include <thread>
#include <mutex>

class LogThreader {
public:
    LogThreader();
    ~LogThreader();

    // add new logger to be handled
    void addLogger(std::shared_ptr<Logger> logger);

private:
    void logging();                         // runs in separate thread; continuously writes entries of m_logEntries to file and/or console
    bool m_loggerRunning;                   // is set to true by constructor and to false by destructor; keeps logging() function running 

    std::thread t;                          // logging thread: started in constructor, joined in destructor

    std::vector<std::shared_ptr<Logger>> m_handledLoggers;
};
#endif