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

// Timer class, inspired from TheCherno (https://www.youtube.com/watch?v=YG4jexlSAjc)
class Timer{
public:
    Timer(){
        start();
    }

    // setting timer to zero
    void start(){
        m_startTimePoint = std::chrono::high_resolution_clock::now();
    }

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

    Logger logger("", LogLevel::Debug);
    logger.log("Default logger", LogLevel::Info);

    Logger customLogger("customLog.log", LogLevel::Debug, false, true);
    customLogger.log("Custom logger", LogLevel::Info, std::to_string(3.14));

    Logger csvLogger("kinState.csv");
    csvLogger.log("t,s,v,a");

    Timer timer;    // measure time until program is ready to continue with something else (-> displayed time is not time needed for logging)
    for(int i = 0; i < 10; ++i){
        logger.log("Message " + std::to_string(i), LogLevel::Debug);
        customLogger.log("Custom logger message " + std::to_string(i), LogLevel::Debug);
        customLogger.log("Custom logger message " + std::to_string(i), LogLevel::Debug, std::to_string(i/10.0));
        csvLogger.log("i,1,2,3");
    }
    
    timer.stop();
}

// function to test wheter it makes a difference printing a long text piece-wise or at once
void testConsolePrinting(){
    
    std::string word = "HalloWelt\n";
    int amount = 1000;

    // Test 1: combine string first and print it at once
    Timer timer;
    std::string text = "";
    for(int i = 0; i < amount; ++i){
        text += word;
    }
    std::cout << text;
    int t1 = timer.stop();
    std::cout << std::endl;

    // Test 2: print text piece-wise
    timer.start();
    for(int i = 0; i < amount; ++i){
        std::cout << text;
    }
    int t2 = timer.stop();

    std::cout << "Duration 1: " << t1 << " us\n";
    std::cout << "Duration 2: " << t2 << " us\n";
}

int main(){

    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";

    // test use of Logging class, watch timing
    testLoggerClass();

    testConsolePrinting();

    return 0;
}