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

Logger::Logger(std::string logFileName, bool enableConsolePrinting)
    :m_enableConsolePrinting(enableConsolePrinting)
{

}

Logger::~Logger(){
    
    #if ENABLE_MULTITHREADING
        m_loggerRunning = false;
        t.join();
    #endif

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

void Logger::printToConsole(const std::string& msg){

    #if ENABLE_MULTITHREADING
        s_consoleMutex.lock();
    #endif

    // note: I used to differentiate between error messages (then printed with cerr <<) and non-error messages (printed with cout <<)
    // but as this might affect printing order (which was initially the objective of differentiating), I removed it
    std::cout << msg << std::endl;

    #if ENABLE_MULTITHREADING
        s_consoleMutex.unlock();
    #endif
}

void Logger::printToFile(const std::string& msg){

    // print to file
    m_logFile << msg << std::endl;
}