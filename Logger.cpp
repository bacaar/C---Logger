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

// declare static member variables once
#if ENABLE_MULTITHREADING
    std::mutex Logger::s_consoleMutex;
#endif
std::vector<std::string> Logger::s_openLogFiles;

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

Logger::Logger(LogLevel newLogLevel, std::string logFileName, bool enableConsolePrinting, bool useCustomTime)
    :m_logLevel(newLogLevel), m_enableConsolePrinting(enableConsolePrinting), m_useCustomTime(useCustomTime)
{

    // if no specific logFileName provided, use default
    if(logFileName == ""){
        logFileName = "log0.log";
    }

    // if logfile has no file extension, add it
    if(logFileName.substr(logFileName.size()-4, 4) != ".log"){
        logFileName += ".log";
    }

    // check if log directory exists and create it if not
    char cwd[256];
    getcwd(cwd, 256);

    #ifdef __linux__
        std::string logFileDir = std::string(cwd) + "/log";
        m_logFilePath = logFileDir + std::string("/") + logFileName;
    #elif _WIN32
        std::string logFileDir = std::string(cwd) + "\\log";
        m_logFilePath = logFileDir + "\\log0.log";
    #endif

    // check that no other logger is already writing to that file
    if (std::find(s_openLogFiles.begin(), s_openLogFiles.end(), m_logFilePath) != s_openLogFiles.end()){
        std::string errMsg = "-----------\nERROR: log-file already in use by other Logger!\n-----------";
        printToConsole(errMsg, true);
    }
    else{
        s_openLogFiles.push_back(m_logFilePath);
    }

    if (!boost::filesystem::exists(logFileDir)) {
        boost::filesystem::create_directory(logFileDir);
    }

    // (create and) open log-file
    m_logFile.open(m_logFilePath, std::ios::out | std::ios::app);

    // check if logfile creation and opening as been successful
    if(!m_logFile.is_open()){
        std::string errMsg = "-----------\nERROR: Could not open log-file!\n-----------";
        printToConsole(errMsg, true);
    }

    // check if passed log-level is valid (and get loglevel as string), else return
    std::string levelStr = "";
    if(!logLevelToStr(levelStr, m_logLevel)){
        std::string errMsg = "Undefined LogLevel. Logger is terminating.";
        writeToLog(errMsg, LogLevel::Error);
        return;
    }

    std::string initMsg = "Starting logger with log level " + levelStr;
    writeToLog(initMsg, LogLevel::Info);

    #if ENABLE_MULTITHREADING
        // start logging in separate thread
        m_loggerRunning = true;
        t = std::thread(&Logger::logging, this);
        //t.detach(); // leave it running in the background
    #endif
}

Logger::~Logger(){
    
    #if ENABLE_MULTITHREADING
        m_loggerRunning = false;
        t.join();
    #endif

    std::string infoMsg = "Logger has been shut down";
    writeToLog(infoMsg, LogLevel::Info);

    m_logFile << "------------------------------------------\n\n";
    m_logFile.close();

    // remove logfile from s_openLogFiles
    s_openLogFiles.erase(std::find(s_openLogFiles.begin(), s_openLogFiles.end(), m_logFilePath));
}

#if ENABLE_MULTITHREADING
void Logger::logging(){

    unsigned int nLogEntries = m_logEntries.size();

    while(m_loggerRunning || nLogEntries > 0){  // keeps running until m_loggerRunning is set to false and after that until queue is empty
        if(nLogEntries > 0){
            // create new log entry from first entry in queue
            m_queueMutex.lock();
            LogEntry entry = m_logEntries.front();
            m_logEntries.pop();
            m_queueMutex.unlock();

            writeToLog(entry.msg, entry.logLevel, entry.rawTime, entry.customTimeStr);
        }

        nLogEntries = m_logEntries.size();
    }
}
#endif

void Logger::log(const std::string &logEntry, LogLevel logLevel, std::string timeStr){
    
    if(logLevel <= m_logLevel) {

        time_t rawTime = 0;
        if(!m_useCustomTime){
            time (&rawTime);
        }

        #if ENABLE_MULTITHREADING
            // create new entry in m_logEntries
            m_queueMutex.lock();
            m_logEntries.push({logLevel, logEntry, timeStr, rawTime});
            m_queueMutex.unlock();
        #else
            // write to log
            writeToLog(logEntry, logLevel, rawTime, timeStr);
        #endif
    }
}

void Logger::log(const char* logEntry, LogLevel logLevel, std::string timeStr){
    std::string msg(logEntry);  // convert char* to std::string
    log(msg, logLevel, timeStr);
}

void Logger::setLogLevel(LogLevel newLogLevel){
    m_logLevel = newLogLevel;
}

void Logger::writeToLog(const std::string& msg_, LogLevel logLevel, time_t rawTime, std::string timeStr){

    std::string msg(msg_);      // to make mutable but not change source content

    // add logLevel as string at front
    addLogLevel(msg, logLevel);

    // make entries at different locations
    if(m_enableConsolePrinting){
        printToConsole(msg, (logLevel == LogLevel::Error ? true : false));
    }

    addTime(msg, rawTime, timeStr);
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

void Logger::addTime(std::string& msg, time_t rawTime, std::string& timeStr){
    
    if(!m_useCustomTime || timeStr == ""){
        // get current date and time as string

        if(rawTime == 0){
            time(&rawTime);
        }

        struct tm * timeinfo;
        char buffer[80];
        timeinfo = localtime(&rawTime);

        strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
        timeStr = std::string(buffer);
    }

    msg = timeStr + std::string(" - ") + msg;
}

void Logger::printToConsole(std::string& msg, bool error){

    #if ENABLE_MULTITHREADING
        s_consoleMutex.lock();
    #endif

    if(error){
        std::cerr << msg << std::endl;
    } else {
        std::cout << msg << std::endl;
    }

    #if ENABLE_MULTITHREADING
        s_consoleMutex.unlock();
    #endif
}

void Logger::printToFile(std::string& msg){

    // print to file
    m_logFile << msg << std::endl;
}