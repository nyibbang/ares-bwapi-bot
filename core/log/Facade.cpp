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

#include "Facade.h"
#include "config.h"
#include "Layout.h"
#include "Logger.h"
#include "Utils.h"

namespace ares
{
namespace core
{
namespace log
{

Facade::Facade()
    : m_fileStream(utils::homePath() + "/AresBWAPI.log")
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

Facade& Facade::instance()
{
    static Facade instance;
    return instance;
}

void Facade::initializeAuxiliaryLogger(LoggerPtr auxLogger)
{
    instance().m_auxiliaryLogger = std::move(auxLogger);
}

void Facade::resetAuxiliaryLogger()
{
    instance().m_auxiliaryLogger.reset();
}

#ifdef ARES_DEBUG_BUILD
BufferStream::pointer Facade::debug(const std::string& file, int line)
{
    return instance().m_fileBSF->create({"DEBUG", file, line});
}
#endif

BufferStream::pointer Facade::info(const std::string& file, int line)
{
    return instance().m_fileBSF->create({"INFO", file, line});
}

BufferStream::pointer Facade::warning(const std::string& file, int line)
{
    return instance().m_compositeBSF->create({"WARNING", file, line});
}

BufferStream::pointer Facade::error(const std::string& file, int line)
{
    return instance().m_compositeBSF->create({"ERROR", file, line});
}

}
}
}
