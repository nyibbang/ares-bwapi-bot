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

#include "config.h"
#include "BufferStream.h"
#include <memory>
#include <fstream>

namespace trace
{

class AbstractLayout;
class AbstractLogger;

class Facade final
{
    public:
#ifdef ARES_DEBUG_BUILD
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

