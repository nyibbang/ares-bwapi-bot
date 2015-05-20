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

#include "LogContext.h"
#include <memory>
#include <sstream>

namespace trace
{

class AbstractLogger;

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

class BufferStreamFactory final
{
    public:
        BufferStreamFactory(AbstractLogger& logger) noexcept;
        BufferStream::pointer create(LogContext&& context) const noexcept;

    private:
        AbstractLogger& m_logger;
};

}

