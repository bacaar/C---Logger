#include "TextLogger.hpp"

#include <iostream>
#include <ctime>

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

LogEntryText::LogEntryText(LogLevel logLevel, std::string msg, std::string customTimeStr, time_t rawTime)
    :m_logLevel(logLevel), m_msg(msg), m_customTimeStr(customTimeStr), m_rawTime(rawTime)
{}

void LogEntryText::constructEntry(){

    m_entry = m_msg;

    // add logLevel as string at front
    addLogLevel(m_entry);

    // add time as string at front
    addTime(m_entry);
}

void LogEntryText::addLogLevel(std::string& msg){

    // create empty string and write LogLevel into it
    std::string levelStr = "";
    logLevelToStr(levelStr, m_logLevel);

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

void LogEntryText::addTime(std::string& msg){

    // custom time
    std::string customTimeStr = "";
    if(m_customTimeStr != ""){
        // TODO format
        if(m_customTimeStr != " "){
            msg = m_customTimeStr + " - " + msg;
        }
    }
    else{
        // real time
        std::string rawTimeStr = "";
        if(m_rawTime == 0){
            time(&m_rawTime);
        }
        struct tm * timeinfo;
        char buffer[80];
        timeinfo = localtime(&m_rawTime);

        strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
        rawTimeStr = std::string(buffer);
        // TODO format
        msg = rawTimeStr + " - " + msg;
    }
}

TextLogger::TextLogger(std::string logFileName, LogLevel newLogLevel, bool enableConsolePrinting, bool useCustomTime)
    :Logger(logFileName, enableConsolePrinting), m_logLevel(newLogLevel), m_useCustomTime(useCustomTime)
{
    // if no specific logFileName provided, use default
    if(logFileName == ""){
        logFileName = "log0.log";
    }

    // if logfile has no file extension, add it
    if(logFileName.substr(logFileName.size()-4, 4) != ".log"){
        logFileName += ".log";
    }

    setup(logFileName);

    // check if passed log-level is valid (and get loglevel as string), else return
    std::string levelStr = "";
    if(!logLevelToStr(levelStr, m_logLevel)){
        std::string errMsg = "Undefined LogLevel. Logger is terminating.";
        std::unique_ptr<LogEntryText> entry = std::make_unique<LogEntryText>(LogLevel::Error, errMsg, "", 0);
        print(move(entry));
        return;
    }

    std::string initMsg = "Starting logger with log level " + levelStr;
    std::unique_ptr<LogEntryText> entry = std::make_unique<LogEntryText>(LogLevel::Info, initMsg, "", 0);
    print(move(entry));
    
}

TextLogger::~TextLogger(){

    std::string infoMsg = "TextLogger has been shut down";
    
    std::unique_ptr<LogEntryText> entry = std::make_unique<LogEntryText>(LogLevel::Info, infoMsg, "", 0);
    print(move(entry), true);
    m_logFile << "------------------------------------------\n\n";
}

void TextLogger::log(const std::string &logEntry, LogLevel logLevel, std::string timeStr){
    
    if(logLevel <= m_logLevel) {

        time_t rawTime = 0;
        if(!m_useCustomTime){
            time (&rawTime);
        }

        std::unique_ptr<LogEntryText> entry = std::make_unique<LogEntryText>(logLevel, logEntry, timeStr, rawTime);

        if(isHandledByThreader()){
            #if ENABLE_MULTITHREADING
            // write to queue
            m_queueMutex.lock();
            m_logEntries.push(move(entry));
            m_queueMutex.unlock();
            #endif
        } else {
            // write to log
            print(move(entry));
        }
    }
}

void TextLogger::log(const char* logEntry, LogLevel logLevel, std::string timeStr){
    std::string msg(logEntry);  // convert char* to std::string
    log(msg, logLevel, timeStr);
}

void TextLogger::setLogLevel(LogLevel newLogLevel){
    m_logLevel = newLogLevel;
}

void TextLogger::print(std::unique_ptr<LogEntry> entry, bool enforceConsoleWriting){

    entry->constructEntry();
    std::string msg = entry->getEntry();

    // make entries at different locations
    if(m_enableConsolePrinting || enforceConsoleWriting){
        printToConsole(msg);
    }

    printToFile(msg);
}