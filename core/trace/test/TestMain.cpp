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
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <gtest/gtest.h>
#include <ctime>
#include <future>
#include <list>

namespace
{

const std::string SEPARATOR_REGEX = " \\| ";
const std::string DATE_REGEX = "\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2}";
const std::string LEVEL_REGEX = "(DEBUG|INFO|WARNING|ERROR)";
const std::string LOCATION_REGEX = "[^:]+:\\d+";
const std::string TEST_MESSAGE_TEMPLATE = "test message n°%1% from thread %2%";
const unsigned int THREADS_COUNT = 10;
const unsigned int TRACES_PER_THREAD = 100;

decltype(ARES_INFO()) stream(int type)
{
    if (type == 0) return ARES_INFO();
    if (type == 1) return ARES_WARNING();
    return ARES_ERROR();
}

void loopTrace()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 2);

    auto&& threadId = std::this_thread::get_id();
    for (int i = 1; i <= TRACES_PER_THREAD; ++i)
    {
        stream(dis(gen)) << boost::format(TEST_MESSAGE_TEMPLATE) % i % threadId;
    }
}

bool lineMatchesLayout(const std::string& line)
{
    static const std::string layoutRegexStr(
            DATE_REGEX + SEPARATOR_REGEX
            + LEVEL_REGEX + SEPARATOR_REGEX
            + LOCATION_REGEX + SEPARATOR_REGEX
            + boost::str(boost::format(TEST_MESSAGE_TEMPLATE) % "\\d+" % "\\d+"));
    static const boost::regex layoutRegex(layoutRegexStr);
    return boost::regex_match(line, layoutRegex);
}

}

TEST(CoreTraceTest, TraceAreThreadSafe)
{
    // Start threads that will trace
    std::list<std::future<void>> futureList;
    for (int i = 0; i < THREADS_COUNT; ++i) {
        futureList.push_back(std::move(std::async(std::launch::async, loopTrace)));
    }
    futureList.clear();

    // Open the log output
    std::ifstream logFile;
    logFile.open(std::getenv("HOME") + std::string("/AresBWAPI.log"));

    // Read everyline and check if they match the layout
    std::string line;
    unsigned int count = 0;
    while (std::getline(logFile, line))
    {
        EXPECT_PRED1(lineMatchesLayout, line);
        ++count;
    }

    // Check that we have the right count of lines
    EXPECT_EQ(THREADS_COUNT * TRACES_PER_THREAD, count);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


