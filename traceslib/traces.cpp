#include "traces.h"

#ifdef ON_LINUX
#   include <cstdlib>
#else
#   include <windows.h>
#   include <shlobj.h>
#endif


namespace
{

std::string homePath()
{
#ifndef ON_LINUX
    WCHAR wpath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathW(0, CSIDL_PROFILE, 0, 0, wpath)))
    {
        std::wstring path(wpath);
        return std::string(path.begin(), path.end());
    }
    return std::string();
#else
    return getenv("HOME");
#endif
}

}


traces::StreamLogger::StreamLogger(std::ostream& os)
: m_os(os)
{}

void traces::StreamLogger::log(const std::string& message)
{
    m_os << message << std::flush;
}

traces::CompositeLogger::CompositeLogger(std::unique_ptr<AbstractLogger>& coutLogger, std::ostream& os)
    : m_coutLogger(coutLogger)
      , m_os(os)
{}

void traces::CompositeLogger::log(const std::string& message)
{
    if (m_coutLogger) m_coutLogger->log(message);
    m_os.write(message.c_str(), message.size());
}

traces::Stream::Stream(AbstractLogger& logger)
: m_logger(logger)
{}

traces::Stream::~Stream() {
    m_logger.log(m_buffer.str());
}

traces::Stream& traces::Stream::operator<<(stream_manipulator manip)
{
    m_buffer << manip;
    if ((manip == &std::endl<char, std::char_traits<char>> or manip == &std::ends<char, std::char_traits<char>>)
            and not m_buffer.str().empty()) {
        m_logger.log(m_buffer.str());
        m_buffer.str(std::string());
    }
    return *this;
}

#ifdef DEBUG
traces::Stream& traces::Facade::debug()
{
    return instance().m_debugStream;
}
#endif

traces::Stream& traces::Facade::info()
{
    return instance().m_infoStream;
}

traces::Stream& traces::Facade::warning()
{
    return instance().m_warnStream;
}

traces::Stream& traces::Facade::error()
{
    return instance().m_errorStream;
}

void traces::Facade::resetAuxiliaryLogger()
{
    instance().m_coutLogger.reset();
}

traces::Facade::Facade()
    : m_fileStream(homePath() + "/tracesBWAPIBot.log")
    , m_noInfoLogger(m_fileStream)
    , m_infoLogger(m_coutLogger, m_fileStream)
#ifdef DEBUG
    , m_debugStream(m_noInfoLogger)
#endif
    , m_infoStream(m_noInfoLogger)
    , m_warnStream(m_infoLogger)
    , m_errorStream(m_infoLogger)
{}

traces::Facade& traces::Facade::instance()
{
    static Facade instance;
    return instance;
}
