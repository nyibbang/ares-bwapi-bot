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
#define ON_WINDOWS 1
#include <windows.h>
#include <shlobj.h>
#endif
#include <chrono>
#include <ctime>

namespace threading
{

std::tm localtime(const std::time_t& time) noexcept
{
    std::tm result;
#if ON_WINDOWS
    localtime_s(&result, &time);
#else
    localtime_r(&time, &result);
#endif
    return result;
}

}

namespace
{

std::string homePath() noexcept
{
#if ON_WINDOWS
    WCHAR wpath[MAX_PATH];
    if(SUCCEEDED(SHGetFolderPathW(0, CSIDL_PROFILE, 0, 0, wpath)))
    {
        std::wstring path(wpath);
        return std::string(path.begin(), path.end());
    }
#endif
    return std::getenv("HOME");
}

std::string dateTime() noexcept
{
    using sc = std::chrono::system_clock;
    auto nowTm = threading::localtime(sc::to_time_t(sc::now()));
    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %X", &nowTm);
    return timeStr;
}

class CompleteLayout final : public trace::AbstractLayout
{
    public:
        std::string format(const trace::LogContext& context, const std::string& message) const override
        {
            return dateTime() + " | " + context.level + " | "
                + context.file + ":" + std::to_string(context.line) + " | " + message;
        }
};

class BasicLayout final : public trace::AbstractLayout
{
    public:
        std::string format(const trace::LogContext& context, const std::string& message) const override
        {
            return context.level + " : " + message;
        }
};

class OstreamLogger final : public trace::AbstractLogger
{
    public:
        OstreamLogger(std::ostream& os) noexcept
            : m_ostream(os)
        {}

    private:
        void log(const trace::LogContext&, const std::string& message) override
        {
            if (!message.empty()) {
                m_ostream.write(message.c_str(), message.size());
            }
        }

        std::ostream& m_ostream;
};

class CompositeLogger final : public trace::AbstractLogger
{
    public:
        CompositeLogger(trace::AbstractLogger& primaryLogger, trace::AbstractLogger& secondaryLogger) noexcept
            : m_primaryLogger(primaryLogger)
            , m_secondaryLogger(secondaryLogger)
        {}

    private:
        void log(const trace::LogContext& context, const std::string& message) override
        {
            m_primaryLogger.log(context, message);
            m_secondaryLogger.log(context, message);
        }

        trace::AbstractLogger& m_primaryLogger;
        trace::AbstractLogger& m_secondaryLogger;
};

class ConditionalAuxiliaryLogger final : public trace::AbstractLogger
{
    public:
        ConditionalAuxiliaryLogger(std::unique_ptr<trace::AbstractLogger>& auxiliaryLogger) noexcept
            : m_auxiliaryLogger(auxiliaryLogger)
        {}

        void log(const trace::LogContext& context, const std::string& message) override
        {
            if (m_auxiliaryLogger) {
                m_auxiliaryLogger->log(context, message);
            }
        }

    private:
        std::unique_ptr<trace::AbstractLogger>& m_auxiliaryLogger;
};

class LayoutLogger final : public trace::AbstractLogger
{
    public:
        LayoutLogger(trace::AbstractLogger& logger, trace::AbstractLayout& layout) noexcept
            : m_logger(logger)
            , m_layout(layout)
        {}

        void log(const trace::LogContext& context, const std::string& message) override
        {
            m_logger.log(context, m_layout.format(context, message));
        }

    private:
        trace::AbstractLogger& m_logger;
        trace::AbstractLayout& m_layout;
};

}

namespace trace
{

class BufferStreamFactory final
{
    public:
        BufferStreamFactory(AbstractLogger& logger) noexcept
            : m_logger(logger)
        {}

        BufferStream::pointer create(LogContext&& context) const noexcept
        {
            return std::make_shared<BufferStream>(m_logger, std::move(context));
        }

    private:
        AbstractLogger& m_logger;
};

BufferStream::BufferStream(AbstractLogger& logger, LogContext&& context) noexcept
    : m_logger(logger)
    , m_context(std::move(context))
{}

BufferStream::~BufferStream()
{
    try {
        m_logger.log(m_context, m_buffer.str() + '\n');
    }
    catch (const std::exception& ex) {
        // Maybe warn the user of the error in some way, to do later
    }
}

BufferStream::pointer operator<<(BufferStream::pointer bfs, BufferStream::stream_manipulator manip) noexcept
{
    if (not bfs) return bfs;
    bfs->m_buffer << manip;
    return bfs;
}

#ifdef ARES_DEBUG_BUILD
BufferStream::pointer Facade::debug(const std::string& file, int line) noexcept
{
    return instance().m_fileBSF->create({"DEBUG", file, line});
}
#endif

BufferStream::pointer Facade::info(const std::string& file, int line) noexcept
{
    return instance().m_fileBSF->create({"INFO", file, line});
}

BufferStream::pointer Facade::warning(const std::string& file, int line) noexcept
{
    return instance().m_compositeBSF->create({"WARNING", file, line});
}

BufferStream::pointer Facade::error(const std::string& file, int line) noexcept
{
    return instance().m_compositeBSF->create({"ERROR", file, line});
}

void Facade::resetAuxiliaryLogger() noexcept
{
    instance().m_auxiliaryLogger.reset();
}

Facade::Facade() noexcept
    : m_fileStream(homePath() + "/AresBWAPI.log")
    , m_fileLogger(new OstreamLogger(m_fileStream))
    , m_fileLayout(new CompleteLayout())
    , m_layoutFileLogger(new LayoutLogger(*m_fileLogger, *m_fileLayout))
    , m_auxiliaryLayout(new BasicLayout())
    , m_conditionalAuxiliaryLogger(new ConditionalAuxiliaryLogger(m_auxiliaryLogger))
    , m_layoutAuxiliaryLogger(new LayoutLogger(*m_conditionalAuxiliaryLogger, *m_auxiliaryLayout))
    , m_layoutCompositeLogger(new CompositeLogger(*m_layoutFileLogger, *m_layoutAuxiliaryLogger))
    , m_fileBSF(new BufferStreamFactory(*m_layoutFileLogger))
    , m_compositeBSF(new BufferStreamFactory(*m_layoutCompositeLogger))
{
    m_fileStream.exceptions(std::ios_base::failbit);
}

Facade& Facade::instance() noexcept
{
    static Facade instance;
    return instance;
}

}

