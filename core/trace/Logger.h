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

namespace trace
{

struct LogContext;
class AbstractLayout;

class AbstractLogger
{
    public:
        virtual ~AbstractLogger() {}
        virtual void log(const LogContext& context, const std::string& message) = 0;
};

class OstreamLogger final : public AbstractLogger
{
    public:
        OstreamLogger(std::ostream& os);
        void log(const LogContext&, const std::string& message) override;

    private:
        std::ostream& m_ostream;
};

class CompositeLogger final : public AbstractLogger
{
    public:
        CompositeLogger(AbstractLogger& primaryLogger, AbstractLogger& secondaryLogger);
        void log(const LogContext& context, const std::string& message) override;

    private:
        AbstractLogger& m_primaryLogger;
        AbstractLogger& m_secondaryLogger;
};

class ConditionalAuxiliaryLogger final : public AbstractLogger
{
    public:
        ConditionalAuxiliaryLogger(std::unique_ptr<AbstractLogger>& auxiliaryLogger);
        void log(const LogContext& context, const std::string& message) override;

    private:
        std::unique_ptr<AbstractLogger>& m_auxiliaryLogger;
};

class LayoutLogger final : public AbstractLogger
{
    public:
        LayoutLogger(AbstractLogger& logger, AbstractLayout& layout);
        void log(const LogContext& context, const std::string& message) override;

    private:
        AbstractLogger& m_logger;
        AbstractLayout& m_layout;
};

}

