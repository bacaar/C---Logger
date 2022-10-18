/*
 * @author:	Aaron Bacher
 * @date:	2022-04-11
 *
 * @brief:	Logger class to handle all logging
 */

#pragma once

#include <string>
#include <fstream>
#include <vector>

#define ENABLE_MULTITHREADING 1
#ifdef ENABLE_MULTITHREADING
    #include <thread>
    #include <mutex>
#endif

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
    Logger(LogLevel newLogLevel, std::string logFileName = "", bool enableConsolePrinting=false, bool useCustomTime=false);
    ~Logger();

    // create log entries
    void log(const std::string &logEntry, LogLevel logLevel, std::string timeStr = "");
    void log(const char* logEntry, LogLevel logLevel, std::string timeStr = "");      // wrapper for above method

    // allows to change loglevel after logger-construction. 
    // e.g. to increase loglevel temporarely
    void setLogLevel(LogLevel newLogLevel);

private:
    LogLevel m_logLevel;
    std::string m_logFilePath;
    std::ofstream m_logFile;

    static std::vector<std::string> s_openLogFiles; // holds paths to all currently open log files in order to make sure that not two loggers are writing to same one

    bool m_enableConsolePrinting;
    bool m_useCustomTime;

#ifdef ENABLE_MULTITHREADING
    // multithreading
    static std::mutex s_consoleMutex;   // as there is only one console for whole programm, mutex for console writing has to be static, so same for all logger instances
    std::mutex m_fileMutex;             // as each logger writes to separate file, mutex for file writing is independent of other instances
#endif

    // calls printToConsole and printToFile
    void makeEntry(const std::string& msg, LogLevel logLevel, std::string timeStr = "");

    // adds log Level as string in front of message
    void addLogLevel(std::string& msg, LogLevel logLevel);

    // adds time as string in front of message
    void addTime(std::string& msg, std::string& timeStr);

    // handle printing to console with time stamps etc.
    void printToConsole(std::string& msg, bool error = false);

    // prints entry to logfile
    void printToFile(std::string& msg);

};