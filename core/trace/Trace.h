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

#pragma once

#include <iostream>

#if !(ARES_TESTS)

#include <memory>
#include <fstream>
#include <sstream>

namespace trace
{

class BufferStreamFactory;

struct LogContext
{
    std::string level;
    std::string file;
    int line;
};

class AbstractLayout
{
    public:
        virtual ~AbstractLayout() {}
        virtual std::string format(const LogContext& context, const std::string& message) const = 0;
};

class AbstractLogger
{
    public:
        virtual ~AbstractLogger() {}
        virtual void log(const LogContext& context, const std::string& message) = 0;
};

class BufferStream final
{
    public:
        using stream_manipulator = std::ostream&(&)(std::ostream&);
        using pointer = std::shared_ptr<BufferStream>;

        BufferStream(AbstractLogger& logger, LogContext&& m_context) noexcept;
        ~BufferStream();

        template<class T> friend BufferStream::pointer operator<<(BufferStream::pointer bfs, T&& t) noexcept;
        friend BufferStream::pointer operator<<(BufferStream::pointer bfs, stream_manipulator manip) noexcept;

    private:
        AbstractLogger& m_logger;
        LogContext m_context;
        std::ostringstream m_buffer;
};

template<class T> BufferStream::pointer operator<<(BufferStream::pointer bfs, T&& t) noexcept
{
    if (not bfs) return bfs;
    bfs->m_buffer << std::forward<T>(t);
    return bfs;
}

BufferStream::pointer operator<<(BufferStream::pointer bfs, BufferStream::stream_manipulator manip) noexcept;

class Facade final
{
    public:
#if ARES_DEBUG_BUILD
        static BufferStream::pointer debug(const std::string& file, int line) noexcept;
#endif
        static BufferStream::pointer info(const std::string& file, int line) noexcept;
        static BufferStream::pointer warning(const std::string& file, int line) noexcept;
        static BufferStream::pointer error(const std::string& file, int line) noexcept;

        static void resetAuxiliaryLogger() noexcept;

        template<class TAuxiliaryLogger, class... TArgs>
        static void initializeAuxiliaryLogger(TArgs&&... args) noexcept
        {
            try {
                instance().m_auxiliaryLogger.reset(new TAuxiliaryLogger(std::forward<TArgs>(args)...));
            }
            catch (const std::exception& ex) {
                // AuxiliaryLogger constructor threw an exception, nothing we can do, just pass. Maybe treat later
            }
        }

    private:
        Facade() noexcept;
        static Facade& instance() noexcept;

        std::ofstream m_fileStream;
        std::unique_ptr<AbstractLogger> m_auxiliaryLogger;
        std::unique_ptr<AbstractLogger> m_fileLogger;
        std::unique_ptr<AbstractLayout> m_fileLayout;
        std::unique_ptr<AbstractLogger> m_layoutFileLogger;
        std::unique_ptr<AbstractLayout> m_auxiliaryLayout;
        std::unique_ptr<AbstractLogger> m_conditionalAuxiliaryLogger;
        std::unique_ptr<AbstractLogger> m_layoutAuxiliaryLogger;
        std::unique_ptr<AbstractLogger> m_layoutCompositeLogger;
        std::unique_ptr<BufferStreamFactory> m_fileBSF;
        std::unique_ptr<BufferStreamFactory> m_compositeBSF;
};

}

#if ARES_DEBUG_BUILD
#define ARES_DEBUG()   trace::Facade::debug(__FILE__, __LINE__)
#else
#define ARES_DEBUG()   if(false) std::cout
#endif
#define ARES_INFO()    trace::Facade::info(__FILE__, __LINE__)
#define ARES_WARNING() trace::Facade::warning(__FILE__, __LINE__)
#define ARES_ERROR()   trace::Facade::error(__FILE__, __LINE__)

#else // ARES_TESTS is defined
    #define ARES_NOOP_STREAM if (false) std::cout
    #define ARES_DEBUG()     ARES_NOOP_STREAM
    #define ARES_INFO()      ARES_NOOP_STREAM
    #define ARES_WARNING()   ARES_NOOP_STREAM
    #define ARES_ERROR()     ARES_NOOP_STREAM
#endif

