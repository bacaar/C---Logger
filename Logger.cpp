#include "Logger.hpp"

#include <iostream>

#include <unistd.h>
#include <boost/filesystem.hpp>

#if ENABLE_MULTITHREADING
#include <thread>
#include <chrono>
#endif

// declare static member variables once
std::vector<std::string> Logger::s_openLogFiles;

Logger::Logger(std::string logFileName, bool enableConsolePrinting)
    :m_enableConsolePrinting(enableConsolePrinting), m_isHandledByThreader(false)
{}

Logger::~Logger(){

    m_logFile.close();

    // remove logfile from s_openLogFiles
    s_openLogFiles.erase(std::find(s_openLogFiles.begin(), s_openLogFiles.end(), m_logFilePath));
}

void Logger::setup(std::string logFileName){

    // check if log directory exists and create it if not
    char cwd[256];
    getcwd(cwd, 256);

    #ifdef __linux__
        std::string logFileDir = std::string(cwd) + "/log";
        m_logFilePath = logFileDir + std::string("/") + logFileName;
    #elif _WIN32
        std::string logFileDir = std::string(cwd) + "\\log";
        m_logFilePath = logFileDir + std::string("\\") + logFileName;
    #endif

    // check that no other logger is already writing to that file
    if (std::find(s_openLogFiles.begin(), s_openLogFiles.end(), m_logFilePath) != s_openLogFiles.end()){
        std::string errMsg = "-----------\nERROR: log-file already in use by other Logger!\n-----------";
        printToConsole(errMsg);
    }
    else{
        s_openLogFiles.push_back(m_logFilePath);
    }

    if (!boost::filesystem::exists(logFileDir)) {
        boost::filesystem::create_directory(logFileDir);
    }

    if(m_logFilePath.substr(m_logFilePath.size()-4, 4) == ".log"){          // append mode
        m_logFile.open(m_logFilePath, std::ios::out | std::ios::app);
    }
    else if(m_logFilePath.substr(m_logFilePath.size()-4, 4) == ".csv"){     // delete file content and write afterwards
        m_logFile.open(m_logFilePath, std::ios::out);
    }
    else{
        std::string errMsg = "-----------\nERROR: Unknown log-file type!\n-----------";
        printToConsole(errMsg);
    }

    // check if logfile creation and opening as been successful
    if(!m_logFile.is_open()){
        std::string errMsg = "-----------\nERROR: Could not open log-file!\n-----------";
        printToConsole(errMsg);
    }
}

#if ENABLE_MULTITHREADING
std::unique_ptr<LogEntry> Logger::getQueueItem(){

    std::unique_ptr<LogEntry> entry;

    m_queueMutex.lock();
    if(m_logEntries.size() == 0){
        return std::unique_ptr<LogEntry>(nullptr);
    }
    else{
        entry = move(m_logEntries.front());
        m_logEntries.pop();
    }
    m_queueMutex.unlock();
    return move(entry);
}

int Logger::getQueueSize(){
    int size;

    // needed for stability, else it often pauses on exception during runtime (and for a try-catch block I would need a exception transport system between the threads 
    // with which I don't want to be bothered right now)
    std::this_thread::sleep_for(std::chrono::nanoseconds(1));

    m_queueMutex.lock();
    size =  m_logEntries.size();
    m_queueMutex.unlock();
    
    return size;
}
#endif

void Logger::printToConsole(const std::string& msg){

    // note: I used to differentiate between error messages (then printed with cerr <<) and non-error messages (printed with cout <<)
    // but as this might affect printing order (which was initially the objective of differentiating), I removed it
    std::cout << msg << std::endl;
}

void Logger::printToFile(const std::string& msg){

    // print to file
    m_logFile << msg << std::endl;
}