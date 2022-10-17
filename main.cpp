#include "Logger.hpp"

int main(){

    Logger logger(LogLevel::Debug);
    logger.log("Default logger", LogLevel::Info);

    Logger customLogger(LogLevel::Debug, "customLog.log", true, true);
    customLogger.log("Custom logger", LogLevel::Info, std::to_string(3.14));

    return 0;
}