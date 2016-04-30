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

#include <ostream>
#include <memory>


namespace ares
{
namespace core
{
namespace log
{

struct LogContext;

namespace abc
{

class Layout;

class Logger
{
    public:
        virtual ~Logger() {}
        virtual void log(const LogContext& context, const std::string& message) = 0;
};

}

class OstreamLogger final : public abc::Logger
{
    public:
        OstreamLogger(std::ostream& os);
        void log(const LogContext&, const std::string& message) override;

    private:
        std::ostream& m_ostream;
};

class CompositeLogger final : public abc::Logger
{
    public:
        CompositeLogger(abc::Logger& primaryLogger, abc::Logger& secondaryLogger);
        void log(const LogContext& context, const std::string& message) override;

    private:
        abc::Logger& m_primaryLogger;
        abc::Logger& m_secondaryLogger;
};

class ConditionalAuxiliaryLogger final : public abc::Logger
{
    public:
        ConditionalAuxiliaryLogger(std::unique_ptr<abc::Logger>& auxiliaryLogger);
        void log(const LogContext& context, const std::string& message) override;

    private:
        std::unique_ptr<abc::Logger>& m_auxiliaryLogger;
};

class LayoutLogger final : public abc::Logger
{
    public:
        LayoutLogger(abc::Logger& logger, abc::Layout& layout);
        void log(const LogContext& context, const std::string& message) override;

    private:
        abc::Logger& m_logger;
        abc::Layout& m_layout;
};

}
}
}
