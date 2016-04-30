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


namespace ares
{
namespace core
{
namespace log
{

namespace abc
{
class Layout;
class Logger;
}

using LoggerPtr = std::unique_ptr<abc::Logger>;
using LayoutPtr = std::unique_ptr<abc::Layout>;

class Facade final
{
    public:
#ifdef ARES_DEBUG_BUILD
        static BufferStream::pointer debug(const std::string& file, int line);
#endif
        static BufferStream::pointer info(const std::string& file, int line);
        static BufferStream::pointer warning(const std::string& file, int line);
        static BufferStream::pointer error(const std::string& file, int line);

        static void initializeAuxiliaryLogger(LoggerPtr auxLogger);
        static void resetAuxiliaryLogger();

    private:
        Facade();
        static Facade& instance();

        std::ofstream m_fileStream;
        LoggerPtr m_auxiliaryLogger;
        LoggerPtr m_fileLogger;
        LayoutPtr m_fileLayout;
        LoggerPtr m_layoutFileLogger;
        LayoutPtr m_auxiliaryLayout;
        LoggerPtr m_conditionalAuxiliaryLogger;
        LoggerPtr m_layoutAuxiliaryLogger;
        LoggerPtr m_layoutCompositeLogger;
        std::unique_ptr<BufferStreamFactory> m_fileBSF;
        std::unique_ptr<BufferStreamFactory> m_compositeBSF;
};

}
}
}
