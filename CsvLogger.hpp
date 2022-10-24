/*
 * @author:	Aaron Bacher
 * @date:	2022-10-24
 *
 * @brief:	CsvLogger class to handle csv writing
 */

#pragma once

#include "Logger.hpp"

class LogEntryCSV : public LogEntry{
public:
    LogEntryCSV(const std::string& content);
    void constructEntry() override;
private:
    std::string m_content;
};

class CsvLogger : public Logger{
public:
    CsvLogger(std::string logFileName);
    ~CsvLogger();

    // create log entries
    // write entries to m_logEntries
    void log(const std::string &logEntry);
    void log(const char* logEntry);

private:

    // calls printToConsole and printToFile
    void print(std::unique_ptr<LogEntry> entry, bool enforceConsoleWriting=false) override;
};