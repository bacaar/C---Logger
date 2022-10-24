/*
 * @author:	Aaron Bacher
 * @date:	2022-04-11
 *
 * @brief:	CsvLogger class to handle all logging
 *
 * @note:	-
 *
 */

#include "CsvLogger.hpp"

#include <iostream>
#include <ctime>

#include <unistd.h>
#include <boost/filesystem.hpp>

LogEntryCSV::LogEntryCSV(const std::string& content)
    :m_content(content)
{}

void LogEntryCSV::constructEntry(){
    m_entry = m_content;
}

CsvLogger::CsvLogger(std::string logFileName)
    :Logger(logFileName, false)
{
    // if no specific logFileName provided, use default
    if(logFileName == ""){
        logFileName = "csv0.csv";
    }

    // if logfile has no file extension, add it
    if(logFileName.substr(logFileName.size()-4, 4) != ".csv"){
        logFileName += ".csv";
    }

    setup(logFileName);
}

CsvLogger::~CsvLogger(){
    
    #if ENABLE_MULTITHREADING
        m_loggerRunning = false;
        t.join();
    #endif

    std::string infoMsg = "CsvLogger has been shut down";
    printToConsole(infoMsg);

    // m_logFile is closed in Base class
}


void CsvLogger::log(const std::string &logEntry){

    std::unique_ptr<LogEntryCSV> entry = std::make_unique<LogEntryCSV>(logEntry);
    #if ENABLE_MULTITHREADING
        // create new entry in m_logEntries
        m_queueMutex.lock();
        m_logEntries.push(move(entry));
        m_queueMutex.unlock();
    #else
        // write to log
        print(move(entry));
    #endif
}

void CsvLogger::log(const char* logEntry){
    std::string msg(logEntry);      // convert char* to std::string
    log(msg);
}

void CsvLogger::print(std::unique_ptr<LogEntry> entry, bool enforceConsoleWriting){

    entry->constructEntry();
    std::string msg = entry->getEntry();
    if(enforceConsoleWriting) printToConsole(msg);
    printToFile(msg);
}

