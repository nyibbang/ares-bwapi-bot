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

    m_logFile.open(homePath + "/AresBWAPIBot.log");
#if defined(DEBUG) || defined(_DEBUG)
    m_coutFile.open(homePath + "/BroodWar.cout");
    m_cerrFile.open(homePath + "/BroodWar.cerr");
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

#if defined(DEBUG) || defined(_DEBUG)
CompositeStream& MessageLogger::debug()
{
    return MessageLogger::instance().debugStream();
}
#endif

CompositeStream& MessageLogger::info()
{
    return MessageLogger::instance().infoStream();
}

CompositeStream& MessageLogger::warning()
{
    return MessageLogger::instance().warningStream();
}

CompositeStream& MessageLogger::error()
{
    return MessageLogger::instance().errorStream();
}

#if defined(DEBUG) || defined(_DEBUG)
CompositeStream& MessageLogger::debugStream()
{
    std::vector<std::streambuf*> compositeBuffers({m_logFile.rdbuf()});
    static CompositeStream debugStream(compositeBuffers);
    return debugStream;
}
#endif
CompositeStream& MessageLogger::infoStream()
{
    std::vector<std::streambuf*> compositeBuffers({
#if defined(DEBUG) || defined(_DEBUG)
                                                       m_coutFile.rdbuf(),
#endif
                                                       m_logFile.rdbuf()
                                                  });
    static CompositeStream infoStream(compositeBuffers);
    return infoStream;
}

CompositeStream& MessageLogger::warningStream()
{
    std::vector<std::streambuf*> compositeBuffers({
#if defined(DEBUG) || defined(_DEBUG)
                                                       m_cerrFile.rdbuf(),
#endif
                                                       m_logFile.rdbuf()
                                                  });
    static CompositeStream warningStream(compositeBuffers);
    return warningStream;
}

CompositeStream& MessageLogger::errorStream()
{
    std::vector<std::streambuf*> compositeBuffers({
#if defined(DEBUG) || defined(_DEBUG)
                                                       m_cerrFile.rdbuf(),
#endif
                                                       m_logFile.rdbuf()
                                                  });
    static CompositeStream errorStream(compositeBuffers);
    return errorStream;
}

#if defined(DEBUG) || defined(_DEBUG)
void MessageLogger::debug(const char* msg)
{
    MessageLogger::debug() << msg;
}
#endif

void MessageLogger::info(const char* msg)
{
    MessageLogger::info() << msg;
}

void MessageLogger::warning(const char* msg)
{
    MessageLogger::warning() << msg;
}

void MessageLogger::error(const char* msg)
{
    MessageLogger::warning() << msg;
}

