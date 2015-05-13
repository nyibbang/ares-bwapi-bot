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

#ifndef ARES_TESTS

#include <memory>
#include <fstream>
#include <sstream>

namespace traces
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
        virtual std::string format(const traces::LogContext& context, const std::string& message) = 0;
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

        BufferStream(AbstractLogger& logger, LogContext&& m_context);
        ~BufferStream();

        template<class T> friend BufferStream::pointer operator<<(BufferStream::pointer bfs, T&& t);
        friend BufferStream::pointer operator<<(BufferStream::pointer bfs, stream_manipulator manip);

    private:
        AbstractLogger& m_logger;
        LogContext m_context;
        std::ostringstream m_buffer;
};

template<class T> BufferStream::pointer operator<<(BufferStream::pointer bfs, T&& t)
{
    if (not bfs) return bfs;
    bfs->m_buffer << std::forward<T>(t);
    return bfs;
}

BufferStream::pointer operator<<(BufferStream::pointer bfs, BufferStream::stream_manipulator manip);

class Facade final
{
    public:
#ifdef ARES_DEBUG_BUILD
        static BufferStream::pointer debug(const std::string& file, int line);
#endif
        static BufferStream::pointer info(const std::string& file, int line);
        static BufferStream::pointer warning(const std::string& file, int line);
        static BufferStream::pointer error(const std::string& file, int line);

        static void resetAuxiliaryLogger();

        template<class TAuxiliaryLogger, class... TArgs>
        static void initializeAuxiliaryLogger(TArgs&&... args)
        {
            instance().m_auxiliaryLogger.reset(new TAuxiliaryLogger(std::forward<TArgs>(args)...));
        }

    private:
        Facade();
        static Facade& instance();

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

#ifdef ARES_DEBUG_BUILD
#define ARES_DEBUG()   traces::Facade::debug(__FILE__, __LINE__)
#else
#define ARES_DEBUG()   if(false) std::cout
#endif
#define ARES_INFO()    traces::Facade::info(__FILE__, __LINE__)
#define ARES_WARNING() traces::Facade::warning(__FILE__, __LINE__)
#define ARES_ERROR()   traces::Facade::error(__FILE__, __LINE__)

#else // ARES_TESTS is defined
    #define ARES_NOOP_STREAM if (false) std::cout
    #define ARES_DEBUG()     ARES_NOOP_STREAM
    #define ARES_INFO()      ARES_NOOP_STREAM
    #define ARES_WARNING()   ARES_NOOP_STREAM
    #define ARES_ERROR()     ARES_NOOP_STREAM
#endif

