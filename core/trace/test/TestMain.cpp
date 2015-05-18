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

#include "Trace.h"
#include <ctime>
#include <future>
#include <list>

namespace
{

decltype(ARES_INFO()) stream(int type)
{
    if (type == 0) return ARES_INFO();
    if (type == 1) return ARES_WARNING();
    return ARES_ERROR();
}

void startTracing()
{
    auto&& threadId = std::this_thread::get_id();
    for (int i = 1; i <= 100; ++i)
    {
        const auto randResult = std::rand() % 3;
        stream(randResult) << "test message n°" << i << " from thread " << threadId;
    }
}
}

int main(int argc, char** argv)
{
    std::srand(std::time(nullptr));
    std::list<std::future<void>> futureList;
    for (int i = 0; i < 10; ++i) {
        futureList.push_back(std::move(std::async(std::launch::async, startTracing)));
    }
    return 0;
}


