/*
 * example.cpp
 *
 * testing logger class
 * showing how to use
 */

#include <iostream>
#include <chrono>

#include <filesystem>

#include "Logger.hpp"

namespace fs = std::filesystem;

// Timer class, inspired by TheCherno (https://www.youtube.com/watch?v=YG4jexlSAjc)
class Timer{
public:
    Timer(){
        start();
    }

    // setting timer to zero
    void start(){
        m_startTimePoint = std::chrono::high_resolution_clock::now();
    }

    // stop timing, return duration
    int stop(){
        auto endTimePoint = std::chrono::high_resolution_clock::now();

        auto start_us = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimePoint).time_since_epoch().count();
        auto end_us = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

        int duration = end_us - start_us;

        return duration;
    }

private:

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimePoint;
}; 

void testLoggerClass(){

    /* instantiate 3 different loggers */

    // one with default settings
    std::shared_ptr<TextLogger> logger = std::make_shared<TextLogger>("", LogLevel::Debug);
    logger->log("Default logger", LogLevel::Info);

    // one with custom time usage
    std::shared_ptr<TextLogger> customLogger = std::make_shared<TextLogger>(fs::current_path().string() + "/log/customLog.log", LogLevel::Debug, true, false, true);
    customLogger->log("Custom logger", LogLevel::Info, std::to_string(3.14));

    // one csv logger
    std::shared_ptr<CsvLogger> csvLogger = std::make_shared<CsvLogger>("kinState.csv");
    csvLogger->log("t,s,v,a");

    /* TESTING */
    // test all three loggers simultaneously
    Timer timer;    // measure time until program is ready to continue with something else (-> displayed time is not time needed for logging)
    for(int i = 0; i < 10; ++i){
        logger->log("Message " + std::to_string(i), LogLevel::Debug);
        customLogger->log("Custom logger message " + std::to_string(i), LogLevel::Debug);
        customLogger->log("Custom logger message " + std::to_string(i), LogLevel::Debug, " ");
        customLogger->log("Custom logger message " + std::to_string(i), LogLevel::Debug, std::to_string(i/10.0));
        csvLogger->log("i,1,2,3");
    }
    
    int t1 = timer.stop();
    std::cout << "without separate thread: " << t1 << " us" << std::endl;

    #if ENABLE_MULTITHREADING
    // test all three loggers simultaneously again, now with multithreading
    LogThreader threader;
    threader.addLogger(logger);
    threader.addLogger(customLogger);
    threader.addLogger(csvLogger);

    timer.start();
    for(int i = 10; i < 20; ++i){
        logger->log("Message " + std::to_string(i), LogLevel::Debug);
        customLogger->log("Custom logger message " + std::to_string(i), LogLevel::Debug);
        customLogger->log("Custom logger message " + std::to_string(i), LogLevel::Debug, " ");
        customLogger->log("Custom logger message " + std::to_string(i), LogLevel::Debug, std::to_string(i/10.0));
        csvLogger->log("i,1,2,3");
    }
    
    int t2 = timer.stop();
    std::cout << "with separate thread: " << t2 << " us" << std::endl;
    #endif
}

int main(){

    // test use of Logging class, check timing
    testLoggerClass();

    return 0;
}