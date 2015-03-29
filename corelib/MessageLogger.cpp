#include "MessageLogger.h"
#include <iostream>

#if defined(_WIN32) || defined(_WIN64) || defined(__GNUC__) || defined(__MINGW32__)
#   include <windows.h>
#   include <shlobj.h>
#elif defined(__unix__) || defined(__linux__) || defined(__CYGWIN__)
#   include <cstdlib>
#endif

MessageLogger& MessageLogger::instance()
{
    static MessageLogger _instance;
    return _instance;
}

MessageLogger::MessageLogger()
{
    std::string homePath = getHomePath();
    if(homePath.empty())
    {
        std::cout << "MessageLogger::getHomePath get null string." << std::endl;
        return;
    }
#if defined(DEBUG) || defined(_DEBUG)
    m_coutFile.open(homePath + "/BroodWar.cout");
    m_cerrFile.open(homePath + "/BroodWar.cerr");
#endif
    m_logFile.open(homePath + "/AresBWAPIBot.log");
}

MessageLogger::~MessageLogger()
{
    closeIfOpen(m_logFile);
#if defined(DEBUG) || defined(_DEBUG)
    closeIfOpen(m_coutFile);
    closeIfOpen(m_cerrFile);
#endif
}

std::string MessageLogger::getHomePath()
{
#if defined(_WIN32) || defined(_WIN64) || defined(__GNUC__) || defined(__MINGW32__)
    WCHAR wpath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathW(0, CSIDL_PROFILE, 0, 0, wpath)))
    {
        std::wstring path(wpath);
        return std::string(path.begin(), path.end());
    }
    return "";
#elif defined(__unix__) || defined(__linux__) || defined(__CYGWIN__)
    return getenv("HOME");
#else
    return "";
#endif
}


void MessageLogger::write(std::ofstream& os, const char* msg)
{
    if(os.is_open())
    {
        os << msg << std::endl;
    }
}

void MessageLogger::write(FILE_TYPE type, const char* msg)
{
    switch(type)
    {
        case LOG:
            write(m_logFile, msg);
            break;
#if defined(DEBUG) || defined(_DEBUG)
        case COUT:
            write(m_coutFile, msg);
            break;

        case CERR:
            write(m_cerrFile, msg);
            break;
#endif
        default:
            break;
    }
}

void MessageLogger::closeIfOpen(std::ofstream& os)
{
    if(os.is_open())
        os.close();
}

#if defined(DEBUG) || defined(_DEBUG)
void MessageLogger::debug(const char* msg)
{
    MessageLogger::instance().write(LOG, msg);
}
#endif

void MessageLogger::info(const char* msg)
{
    MessageLogger::instance().write(LOG, msg);
#if defined(DEBUG) || defined(_DEBUG)
    MessageLogger::instance().write(COUT, msg);
#endif
}

void MessageLogger::warning(const char* msg)
{
    MessageLogger::instance().write(LOG, msg);
#if defined(DEBUG) || defined(_DEBUG)
    MessageLogger::instance().write(CERR, msg);
#endif
}

void MessageLogger::error(const char* msg)
{
    MessageLogger::instance().write(LOG, msg);
#if defined(DEBUG) || defined(_DEBUG)
    MessageLogger::instance().write(CERR, msg);
#endif
}


