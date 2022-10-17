/*
 * @author:	Aaron Bacher
 * @date:	2022-04-11
 *
 * @brief:	Logger class to handle all logging
 *
 * @note:	-
 *
 */

#include "Logger.hpp"

#include <iostream>
#include <ctime>

#include <unistd.h>
#include <boost/filesystem.hpp>

bool logLevelToStr(std::string& str, LogLevel logLevel){
    switch (logLevel)
    {
    case LogLevel::Error:
        str = "ERROR";
        return true;
    case LogLevel::Warning:
        str = "WARNING";
        return true;
    case LogLevel::Info:
        str = "INFO";
        return true;
    case LogLevel::Debug:
        str = "DEBUG";
        return true;
    }

    str = "UNDEFINED";
    return false;
}

Logger::Logger(LogLevel newLogLevel)
    :m_logLevel(newLogLevel)
{

    // check if log directory exists and create it if not
    char cwd[256];
    getcwd(cwd, 256);

    #ifdef __linux__
        std::string logFileDir = std::string(cwd) + "/log";
        std::string logFileName = logFileDir + "/log0.log";
    #elif _WIN32
        std::string logFileDir = std::string(cwd) + "\\log";
        std::string logFileName = logFileDir + "\\log0.log";
    #endif

    if (!boost::filesystem::exists(logFileDir)) {
        boost::filesystem::create_directory(logFileDir);
    }

    // (create and) open log-file
    m_logFile.open(logFileName, std::ios::out | std::ios::app);

    // check if logfile creation and opening as been successful
    if(!m_logFile.is_open()){
        std::string errMsg = "-----------\nERROR: Could not open log-File!\n-----------";
        printToConsole(errMsg, true);
    }

    // check if passed log-level is valid (and get loglevel as string), else return
    std::string levelStr = "";
    if(!logLevelToStr(levelStr, m_logLevel)){
        std::string errMsg = "Undefined LogLevel. Logger is terminating.";
        makeEntry(errMsg, LogLevel::Error);
        return;
    }

    std::string initMsg = "Starting logger with log level " + levelStr;
    makeEntry(initMsg, LogLevel::Info);
}

Logger::~Logger(){
    std::string infoMsg = "Logger has been shut down";
    makeEntry(infoMsg, LogLevel::Info);

    m_logFile << "------------------------------------------\n\n";
    m_logFile.close();
}

void Logger::log(const std::string &logEntry, LogLevel logLevel){
    if(logLevel <= m_logLevel) {
        std::string msg(logEntry); // to not modify original message
        makeEntry(msg, logLevel);
    }
}

void Logger::log(const char* logEntry, LogLevel logLevel){
    if(logLevel <= m_logLevel) {
        std::string msg(logEntry);
        makeEntry(msg, logLevel);
    }
}

void Logger::setLogLevel(LogLevel newLogLevel){
    m_logLevel = newLogLevel;
}

void Logger::makeEntry(std::string& msg, LogLevel logLevel){

    // add logLevel as string at front
    addLogLevel(msg, logLevel);

    // make entries at different locations
    printToConsole(msg, (logLevel == LogLevel::Error ? true : false));
    printToFile(msg);
}

void Logger::addLogLevel(std::string& msg, LogLevel logLevel){

    // create empty string and write LogLevel into it
    std::string levelStr = "";
    logLevelToStr(levelStr, logLevel);

    // add colon
    levelStr += ":";

    // all log entries should have same length, so fill up space
    // TODO does not work properly for Warnings
    for(int i = 0; i < 12-levelStr.length(); ++i){
        levelStr += " ";
    }

    // combine everything
    msg = levelStr + msg;
}


void Logger::printToConsole(std::string& msg, bool error){

    if(error){
        std::cerr << msg << std::endl;
    } else {
        std::cout << msg << std::endl;
    }
}

void Logger::printToFile(std::string& msg){

    // get current date and time as string
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
    std::string timeStr(buffer);

    // print to file
    m_logFile << timeStr << " - " << msg << std::endl;

}