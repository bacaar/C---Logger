#include "Logger.hpp"

int main(){

    Logger logger(LogLevel::Debug);
    logger.log("Default logger", LogLevel::Info);

    Logger customLogger(LogLevel::Debug, "customLog.log", true, true);
    customLogger.log("Custom logger", LogLevel::Info, std::to_string(3.14));

    for(int i = 0; i < 10; ++i){
        logger.log("Message " + std::to_string(i), LogLevel::Debug);
        customLogger.log("Custom logger message " + std::to_string(i), LogLevel::Debug);
        customLogger.log("Custom logger message " + std::to_string(i), LogLevel::Debug, std::to_string(i/10.0));
    }

    return 0;
}