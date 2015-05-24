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

#include "Layout.h"
#include "config.h"
#include "LogContext.h"
#include <chrono>
#include <ctime>

namespace threading
{

std::tm localtime(const std::time_t& time)
{
    std::tm result;
#ifdef ARES_MSWINDOWS
    localtime_s(&result, &time);
#else
    localtime_r(&time, &result);
#endif
    return result;
}

}

namespace
{

std::string dateTime()
{
    using sc = std::chrono::system_clock;
    auto nowTm = threading::localtime(sc::to_time_t(sc::now()));
    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %X", &nowTm);
    return timeStr;
}

}

namespace trace
{

std::string CompleteLayout::format(const LogContext& context, const std::string& message) const
{
    return dateTime() + " | " + context.level + " | "
        + context.file + ":" + std::to_string(context.line) + " | " + message;
}

std::string BasicLayout::format(const LogContext& context, const std::string& message) const
{
    return context.level + " : " + message;
}

}

