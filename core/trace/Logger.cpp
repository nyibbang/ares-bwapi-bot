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

#include "Logger.h"
#include "Layout.h"
#include <mutex>

namespace trace
{

OstreamLogger::OstreamLogger(std::ostream& os)
    : m_ostream(os)
{}

void OstreamLogger::log(const LogContext&, const std::string& message)
{
    if (!message.empty())
    {
        static std::mutex osMutex;
        std::lock_guard<std::mutex> osGuard(osMutex);
        m_ostream << message << std::flush;
    }
}

CompositeLogger::CompositeLogger(AbstractLogger& primaryLogger, AbstractLogger& secondaryLogger)
    : m_primaryLogger(primaryLogger)
    , m_secondaryLogger(secondaryLogger)
{}

void CompositeLogger::log(const LogContext& context, const std::string& message)
{
    m_primaryLogger.log(context, message);
    m_secondaryLogger.log(context, message);
}

ConditionalAuxiliaryLogger::ConditionalAuxiliaryLogger(std::unique_ptr<AbstractLogger>& auxiliaryLogger)
    : m_auxiliaryLogger(auxiliaryLogger)
{}

void ConditionalAuxiliaryLogger::log(const LogContext& context, const std::string& message)
{
    if (m_auxiliaryLogger) {
        m_auxiliaryLogger->log(context, message);
    }
}

LayoutLogger::LayoutLogger(AbstractLogger& logger, AbstractLayout& layout)
    : m_logger(logger)
    , m_layout(layout)
{}

void LayoutLogger::log(const LogContext& context, const std::string& message)
{
    m_logger.log(context, m_layout.format(context, message));
}

}

