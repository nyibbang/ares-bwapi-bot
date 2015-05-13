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

class CompleteLayout final : public traces::AbstractLayout
{
    public:
        std::string format(const traces::LogContext& context, const std::string& message) override
        {
            return dateTime() + " | " + context.level + " | "
                + context.file + ":" + std::to_string(context.line) + " | " + message;
        }
};

class BasicLayout final : public traces::AbstractLayout
{
    public:
        std::string format(const traces::LogContext& context, const std::string& message) override
        {
            return context.level + " : " + message;
        }
};

class OstreamLogger final : public traces::AbstractLogger
{
    public:
        OstreamLogger(std::ostream& os)
            : m_ostream(os)
        {}

    private:
        void log(const traces::LogContext&, const std::string& message) override
        {
            if (!message.empty()) {
                m_ostream.write(message.c_str(), message.size());
            }
        }

        std::ostream& m_ostream;
};

class CompositeLogger final : public traces::AbstractLogger
{
    public:
        CompositeLogger(AbstractLogger& primaryLogger, AbstractLogger& secondaryLogger)
            : m_primaryLogger(primaryLogger)
            , m_secondaryLogger(secondaryLogger)
        {}

    private:
        void log(const traces::LogContext& context, const std::string& message) override
        {
            m_primaryLogger.log(context, message);
            m_secondaryLogger.log(context, message);
        }

        AbstractLogger& m_primaryLogger;
        AbstractLogger& m_secondaryLogger;
};

class ConditionalAuxiliaryLogger final : public traces::AbstractLogger
{
    public:
        ConditionalAuxiliaryLogger(std::unique_ptr<AbstractLogger>& auxiliaryLogger)
            : m_auxiliaryLogger(auxiliaryLogger)
        {}

        void log(const traces::LogContext& context, const std::string& message) override
        {
            if (m_auxiliaryLogger) {
                m_auxiliaryLogger->log(context, message);
            }
        }

    private:
        std::unique_ptr<AbstractLogger>& m_auxiliaryLogger;
};

class LayoutLogger final : public traces::AbstractLogger
{
    public:
        LayoutLogger(AbstractLogger& logger, traces::AbstractLayout& layout)
            : m_logger(logger)
            , m_layout(layout)
        {}

        void log(const traces::LogContext& context, const std::string& message) override
        {
            m_logger.log(context, m_layout.format(context, message));
        }

    private:
        AbstractLogger& m_logger;
        traces::AbstractLayout& m_layout;
};

}

namespace traces
{

class BufferStreamFactory final
{
    public:
        BufferStreamFactory(AbstractLogger& logger)
            : m_logger(logger)
        {}

        BufferStream::pointer createBufferStream(const std::string& level, const std::string& file, int line)
        {
            return std::make_shared<BufferStream>(m_logger, LogContext({level, file, line}));
        }

    private:
        AbstractLogger& m_logger;
};

BufferStream::BufferStream(AbstractLogger& logger, LogContext&& context)
    : m_logger(logger)
    , m_context(std::move(context))
{}

BufferStream::~BufferStream()
{
    m_logger.log(m_context, m_buffer.str());
}

BufferStream::pointer operator<<(BufferStream::pointer bfs, BufferStream::stream_manipulator manip)
{
    if (not bfs) return bfs;
    bfs->m_buffer << manip;
    return bfs;
}

#ifdef ARES_DEBUG_BUILD
BufferStream::pointer Facade::debug(const std::string& file, int line)
{
    return instance().m_fileBSF->createBufferStream("DEBUG", file, line);
}
#endif

BufferStream::pointer Facade::info(const std::string& file, int line)
{
    return instance().m_fileBSF->createBufferStream("INFO", file, line);
}

BufferStream::pointer Facade::warning(const std::string& file, int line)
{
    return instance().m_compositeBSF->createBufferStream("WARNING", file, line);
}

BufferStream::pointer Facade::error(const std::string& file, int line)
{
    return instance().m_compositeBSF->createBufferStream("ERROR", file, line);
}

void Facade::resetAuxiliaryLogger()
{
    instance().m_auxiliaryLogger.reset();
}

Facade::Facade()
    : m_fileStream(homePath() + "/AresBWAPIBot.log")
    , m_fileLogger(new OstreamLogger(m_fileStream))
    , m_fileLayout(new CompleteLayout())
    , m_layoutFileLogger(new LayoutLogger(*m_fileLogger, *m_fileLayout))
    , m_auxiliaryLayout(new BasicLayout())
    , m_conditionalAuxiliaryLogger(new ConditionalAuxiliaryLogger(m_auxiliaryLogger))
    , m_layoutAuxiliaryLogger(new LayoutLogger(*m_conditionalAuxiliaryLogger, *m_auxiliaryLayout))
    , m_layoutCompositeLogger(new CompositeLogger(*m_layoutFileLogger, *m_layoutAuxiliaryLogger))
    , m_fileBSF(new BufferStreamFactory(*m_layoutFileLogger))
    , m_compositeBSF(new BufferStreamFactory(*m_layoutCompositeLogger))
{}

Facade& Facade::instance()
{
    static Facade instance;
    return instance;
}

}

