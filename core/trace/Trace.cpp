/*
 * AresBWAPIBot : A bot for SC:BW using BWAPI library;
 * Copyright (C) 2015 Vincent PALANCHER; Florian LEMEASLE
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "Trace.h"
#ifdef ON_WINDOWS
#include <windows.h>
#include <shlobj.h>
#include <ctime>

namespace
{

std::string homePath()
{
    WCHAR wpath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathW(0, CSIDL_PROFILE, 0, 0, wpath)))
    {
        std::wstring path(wpath);
        return std::string(path.begin(), path.end());
    }
    return std::string();
}

std::string dateTime()
{
    char time[20];
    time_t rawtime = std::time(nullptr);
    strftime(time, 20, "%Y-%m-%d %X", localtime(&rawtime));
    return time;
}

std::string formateMessage(std::string message, std::string location, std::string level)
{
    std::string formatedMessage = dateTime() + '|' + level + '|' + location + '|' + message;
    return formatedMessage;
}

}


traces::StreamLogger::StreamLogger(std::ostream& os)
: m_os(os)
{}

void traces::StreamLogger::layout(const char* file, int line, const char* level)
{
    std::ostringstream oss;
    oss << line;
    m_location = std::string(file) + ':' + oss.str();
    m_level = level;
}

void traces::StreamLogger::log(const std::string& message)
{
    if(!message.empty()) m_os << formateMessage(message, m_location, m_level) << std::flush;
}

traces::CompositeLogger::CompositeLogger(std::unique_ptr<AbstractLogger>& coutLogger, std::ostream& os)
    : m_coutLogger(coutLogger)
      , m_os(os)
{}

void traces::CompositeLogger::layout(const char* file, int line, const char* level)
{
    if (m_coutLogger) m_coutLogger->layout(file, line, level);
    std::ostringstream oss;
    oss << line;
    m_location = std::string(file) + ':' + oss.str();
    m_level = level;
}

void traces::CompositeLogger::log(const std::string& message)
{
    if(!message.empty())
    {
        if (m_coutLogger) m_coutLogger->log(message);
        std::string formatedMessage = formateMessage(message, m_location, m_level);
        m_os.write(formatedMessage.c_str(), formatedMessage.size());
    }
}

traces::Stream::Stream(AbstractLogger& logger)
: m_logger(logger)
{}

traces::Stream::~Stream() {
    m_logger.log(m_buffer.str());
}

traces::Stream& traces::Stream::layout(const char* file, int line, const char* level)
{
    m_logger.layout(file, line, level);
    return *this;
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
traces::Stream& traces::Facade::debug(const char* file, int line)
{
    return instance().m_debugStream.layout(file, line, "DEBUG");
}
#endif

traces::Stream& traces::Facade::info(const char* file, int line)
{
    return instance().m_infoStream.layout(file, line, "INFO");
}

traces::Stream& traces::Facade::warning(const char* file, int line)
{
    return instance().m_warnStream.layout(file, line, "WARNING");
}

traces::Stream& traces::Facade::error(const char* file, int line)
{
    return instance().m_errorStream.layout(file, line, "ERROR");
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

#endif

