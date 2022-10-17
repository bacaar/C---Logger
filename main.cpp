#include "Logger.hpp"

int main(){

    Logger logger(LogLevel::Debug);
    logger.log("Logger Started", LogLevel::Debug);

    return 0;
}