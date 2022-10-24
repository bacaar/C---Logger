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
#include <queue>
#include <memory>

class LogEntry{
public:
    virtual void constructEntry() = 0;
    std::string getEntry() { return m_entry; }
protected:
    std::string m_entry;
};

// logger class
class Logger{
public:
    Logger(std::string logFileName, bool enableConsolePrinting=false);      // for normal text logs
    ~Logger();

protected:
    bool m_isHandledByThread;       // defines if logger should work on its own (false) or if logging is done by LogThreader in separate thread (true)

    std::queue<std::unique_ptr<LogEntry>> m_logEntries;      // if handled by LogThreader, log-method writes into this buffer instead of writing directly to file and/or console

    bool m_enableConsolePrinting;

    std::string m_logFilePath;
    std::ofstream m_logFile;

    // method to intialize file
    void setup(std::string logFileName);

    // handle printing to console with time stamps etc.
    void printToConsole(const std::string& msg);

    // prints entry to logfile
    void printToFile(const std::string& msg);

private:

    static std::vector<std::string> s_openLogFiles; // holds paths to all currently open log files in order to make sure that not two loggers are writing to same one

    virtual void print(std::unique_ptr<LogEntry> entry, bool enforceConsoleWriting=false) = 0;

};