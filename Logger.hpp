/*
 * @author:	Aaron Bacher
 * @date:	2022-04-11
 *
 * @brief:	Logger class to handle all logging
 *
 * @note:	- time stamp still has to be included
 *
 */

#pragma once

#include <string>
#include <fstream>

// selection from https://www.ibm.com/docs/en/cognos-analytics/10.2.2?topic=SSEP7J_10.2.2/com.ibm.swg.ba.cognos.ug_rtm_wb.10.2.2.doc/c_n30e74.html
enum LogLevel {
    Error,
    Warning,
    Info,
    Debug
};

// logger class
class Logger{
public:
    Logger(LogLevel newLogLevel);
    ~Logger();

    // create log entries
    void log(const std::string &logEntry, LogLevel logLevel);
    void log(const char* logEntry, LogLevel logLevel);

    // allows to change loglevel after logger-construction. 
    // e.g. to increase loglevel temporarely
    void setLogLevel(LogLevel newLogLevel);

private:
    LogLevel m_logLevel;
    std::ofstream m_logFile;

    // calls printToConsole and printToFile
    void makeEntry(std::string& msg, LogLevel logLevel);

    // adds log Level as string in front of message
    void addLogLevel(std::string& msg, LogLevel logLevel);

    // handle printing to console with time stamps etc.
    // prints errors only
    void printToConsole(std::string& msg, bool error = false);

    // prints entry to logfile
    void printToFile(std::string& msg);

};