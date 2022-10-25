/*
 * @author:	Aaron Bacher
 * @date:	2022-10-24
 *
 * @brief:	Derived Logger class to handle text logging (normal .log-files)
 */

#pragma once

#include "Logger.hpp"

// selection from https://www.ibm.com/docs/en/cognos-analytics/10.2.2?topic=SSEP7J_10.2.2/com.ibm.swg.ba.cognos.ug_rtm_wb.10.2.2.doc/c_n30e74.html
enum LogLevel {
    Error,
    Warning,
    Info,
    Debug
};

class LogEntryText : public LogEntry{
public:
    LogEntryText(LogLevel logLevel,
                 std::string msg,
                 std::string customTimeStr = "",
                 time_t rawTime = 0);
    
    void constructEntry() override;

private:
    // adds log Level as string in front of message
    void addLogLevel(std::string& msg);

    // adds time as string in front of message
    void addTime(std::string& msg);

    LogLevel m_logLevel;
    std::string m_msg;
    std::string m_customTimeStr = "";
    time_t m_rawTime = 0;
};

class TextLogger : public Logger{
public:
    TextLogger(std::string logFileName, LogLevel newLogLevel, bool enableConsolePrinting=false, bool useCustomTime=false);      // for normal text logs
    ~TextLogger();

    // create log entries
    // write entries to m_logEntries
    void log(const std::string &logEntry, LogLevel logLevel, std::string timeStr = "");
    void log(const char* logEntry, LogLevel logLevel, std::string timeStr = "");      // wrapper for above method

    // allows to change loglevel after logger-construction. 
    // e.g. to increase loglevel temporarely
    void setLogLevel(LogLevel newLogLevel);

private:

    LogLevel m_logLevel;

    bool m_useCustomTime;

    // construct entry, give command to write to console and/or file
    void print(std::unique_ptr<LogEntry> entry, bool enforceConsoleWriting=false) override;
};