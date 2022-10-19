/*
 * main.cpp
 *
 * just testing of Logger class
 */

#include <iostream>
#include <chrono>
#include <fstream>

#include <thread>
#include <mutex>

#include "Logger.hpp"

// Timer class from TheCherno (https://www.youtube.com/watch?v=YG4jexlSAjc)
class Timer{
public:
    Timer(){
        m_startTimePoint = std::chrono::high_resolution_clock::now();
    }

    ~Timer(){
        stop();
    }

private:
    void stop(){
        auto endTimePoint = std::chrono::high_resolution_clock::now();

        auto start_us = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
        auto end_us = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

        auto duration = end_us - start_us;

        std::cout << "Duration: " << duration << " us" << std::endl;
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;
}; 

void testLoggerClass(){

    Logger logger(LogLevel::Debug);
    logger.log("Default logger", LogLevel::Info);

    Logger customLogger(LogLevel::Debug, "customLog.log", false, true);
    customLogger.log("Custom logger", LogLevel::Info, std::to_string(3.14));

    {
        Timer timer;    // measure time until program is ready to continue with something else (-> displayed time is not time needed for logging)
        for(int i = 0; i < 1000; ++i){
            logger.log("Message " + std::to_string(i), LogLevel::Debug);
            customLogger.log("Custom logger message " + std::to_string(i), LogLevel::Debug);
            customLogger.log("Custom logger message " + std::to_string(i), LogLevel::Debug, std::to_string(i/10.0));
        }

    }
}

int main(){

    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";

    // test use of Logging class, watch timing
    testLoggerClass();

    return 0;
}