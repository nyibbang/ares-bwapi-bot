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
#include <iostream>

#ifdef ARES_DISABLE_LOG
    #define ARES_NOOP_STREAM if (false) std::cout
    #define ARES_DEBUG()     ARES_NOOP_STREAM
    #define ARES_INFO()      ARES_NOOP_STREAM
    #define ARES_WARNING()   ARES_NOOP_STREAM
    #define ARES_ERROR()     ARES_NOOP_STREAM
#else
    #include "Facade.h"
    #ifdef ARES_DEBUG_BUILD
        #define ARES_DEBUG()   ares::core::log::Facade::debug(__FILE__, __LINE__)
    #else
        #define ARES_DEBUG()   if(false) std::cout
    #endif
    #define ARES_INFO()    ares::core::log::Facade::info(__FILE__, __LINE__)
    #define ARES_WARNING() ares::core::log::Facade::warning(__FILE__, __LINE__)
    #define ARES_ERROR()   ares::core::log::Facade::error(__FILE__, __LINE__)
#endif

