#include "CsvLogger.hpp"

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

    std::string infoMsg = "CsvLogger has been shut down";
    printToConsole(infoMsg);

}


void CsvLogger::log(const std::string &logEntry){

    std::unique_ptr<LogEntryCSV> entry = std::make_unique<LogEntryCSV>(logEntry);
    if(isHandledByThreader()){
        // write to queue
        m_queueMutex.lock();
        m_logEntries.push(move(entry));
        m_queueMutex.unlock();
    } else {
        // write to log
        print(move(entry));
    }
}

void CsvLogger::log(const char* logEntry){
    std::string msg(logEntry);      // convert char* to std::string
    log(msg);
}

void CsvLogger::print(std::unique_ptr<LogEntry> entry, bool enforceConsoleWriting){

    entry->constructEntry();
    std::string msg = entry->getEntry();

    // check amount of columns
    if(m_nColumns == 0){    // first time
        for (int i = 0; i < msg.size(); ++i){
            if(msg[i] == ',') ++m_nColumns;
        }
        ++m_nColumns;   // there is always one more column than semicolons

    }
    else {  // every other time
        int count = 1;  // there is always one more column than semicolons
        for (int i = 0; i < msg.size(); ++i){
            if(msg[i] == ',') ++count;
        }
        if(count != m_nColumns){
            std::string warningMsg = "WARNING! Amount of columns (" + std::to_string(count) + ") does not correspond to columns in first row (" + std::to_string(m_nColumns) + ")";
            printToConsole(warningMsg);
        }
    }

    if(enforceConsoleWriting) printToConsole(msg);

    printToFile(msg);
}

