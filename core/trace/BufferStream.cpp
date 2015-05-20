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

#include "BufferStream.h"
#include "Logger.h"

namespace trace
{

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

BufferStreamFactory::BufferStreamFactory(AbstractLogger& logger) noexcept
    : m_logger(logger)
{}

BufferStream::pointer BufferStreamFactory::create(LogContext&& context) const noexcept
{
    return std::make_shared<BufferStream>(m_logger, std::move(context));
}

}

