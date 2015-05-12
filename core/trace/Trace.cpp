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

#if defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__)
#define ON_WINDOWS
#include <windows.h>
#include <shlobj.h>
#endif
#include <ctime>

namespace
{

std::string homePath()
{
#ifdef ON_WINDOWS
    WCHAR wpath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathW(0, CSIDL_PROFILE, 0, 0, wpath)))
    {
        std::wstring path(wpath);
        return std::string(path.begin(), path.end());
    }
#endif
    return std::getenv("HOME");
}

std::string dateTime()
{
    char time[20];
    time_t rawtime = std::time(nullptr);
    strftime(time, 20, "%Y-%m-%d %X", localtime(&rawtime));
    return time;
}

std::string layoutMessage(const std::string& message, const std::string& location, const std::string& level)
{
    return dateTime() + " | " + level + " | " + location + " | " + message;
}

class OstreamLogger final : public traces::AbstractLogger
{
    public:
        OstreamLogger(std::ostream& os)
            : m_os(os)
        {}

        void layout(const std::string& file, int line, const std::string& level)
        {
            m_location = std::string(file) + ':' + std::to_string(line);
            m_level = level;
        }

    private:
        void log(const std::string& message) override
        {
            if (!message.empty()) {
                m_os << layoutMessage(message, m_location, m_level) << std::flush;
            }
        }

        std::ostream& m_os;
        std::string m_location;
        std::string m_level;
};

class CompositeLogger final : public traces::AbstractLogger
{
    public:
        CompositeLogger(std::unique_ptr<AbstractLogger>& auxiliaryLogger, std::ostream& os)
            : m_auxiliaryLogger(auxiliaryLogger)
            , m_os(os)
        {}

        void layout(const std::string& file, int line, const std::string& level)
        {
            if (m_auxiliaryLogger) m_auxiliaryLogger->layout(file, line, level);
            std::ostringstream oss;
            oss << line;
            m_location = std::string(file) + ':' + oss.str();
            m_level = level;
        }

    private:
        void log(const std::string& message) override
        {
            if (message.empty()) return;

            if (m_auxiliaryLogger) m_auxiliaryLogger->log(message);
            std::string laidoutMessage = layoutMessage(message, m_location, m_level);
            m_os.write(laidoutMessage.c_str(), laidoutMessage.size());
        }

        std::unique_ptr<AbstractLogger>& m_auxiliaryLogger;
        std::ostream& m_os;
        std::string m_location;
        std::string m_level;
};

}

namespace traces
{

Stream::Stream(AbstractLogger& logger)
    : m_logger(logger)
{}

Stream::~Stream()
{
    m_logger.log(m_buffer.str());
}

Stream& Stream::layout(const std::string& file, int line, const std::string& level)
{
    m_logger.layout(file, line, level);
    return *this;
}

Stream& Stream::operator<<(stream_manipulator manip)
{
    m_buffer << manip;
    if ((manip == &std::endl<char, std::char_traits<char>> or manip == &std::ends<char, std::char_traits<char>>)
            and not m_buffer.str().empty()) {
        m_logger.log(m_buffer.str());
        m_buffer.str(std::string());
    }
    return *this;
}

#ifdef ARES_DEBUG_BUILD
Stream& Facade::debug(const std::string& file, int line)
{
    return instance().m_debugStream.layout(file, line, "DEBUG");
}
#endif

Stream& Facade::info(const std::string& file, int line)
{
    return instance().m_infoStream.layout(file, line, "INFO");
}

Stream& Facade::warning(const std::string& file, int line)
{
    return instance().m_warnStream.layout(file, line, "WARNING");
}

Stream& Facade::error(const std::string& file, int line)
{
    return instance().m_errorStream.layout(file, line, "ERROR");
}

void Facade::resetAuxiliaryLogger()
{
    instance().m_auxiliaryLogger.reset();
}

Facade::Facade()
    : m_fileStream(homePath() + "/AresBWAPIBot.log")
    , m_fileLogger(new OstreamLogger(m_fileStream))
    , m_compositeLogger(new CompositeLogger(m_auxiliaryLogger, m_fileStream))
#ifdef ARES_DEBUG_BUILD
    , m_debugStream(*m_fileLogger)
#endif
    , m_infoStream(*m_fileLogger)
    , m_warnStream(*m_compositeLogger)
    , m_errorStream(*m_compositeLogger)
{}

Facade& Facade::instance()
{
    static Facade instance;
    return instance;
}

}

