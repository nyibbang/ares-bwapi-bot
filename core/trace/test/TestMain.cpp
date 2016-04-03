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
#include "Logger.h"
#include "Utils.h"
#include <boost/format.hpp>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <ctime>
#include <future>
#include <list>
#include <random>

namespace
{

const unsigned int THREADS_COUNT = 20;
const unsigned int TRACES_PER_THREAD = 1000;
const unsigned int INFO_THREADS_COUNT = THREADS_COUNT / 3;

const std::string SEPARATOR_REGEX = " \\| ";
const std::string DATE_REGEX = "[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}";
const std::string LEVEL_REGEX = "(DEBUG|INFO|WARNING|ERROR)";
const std::string AUXILIARY_LEVEL_REGEX = "(WARNING|ERROR)";
const std::string LOCATION_REGEX = "[^:]+:[0-9]+";
const std::string TEST_MESSAGE_TEMPLATE = "test message n°%1% from thread %2%";
const std::string TEST_MESSAGE_REGEX = boost::str(boost::format(TEST_MESSAGE_TEMPLATE) % "[0-9]+" % "[0-9]+");
const std::string completeLayoutRegex(DATE_REGEX + SEPARATOR_REGEX
                                      + LEVEL_REGEX + SEPARATOR_REGEX
                                      + LOCATION_REGEX + SEPARATOR_REGEX
                                      + TEST_MESSAGE_REGEX + "\n?");
const std::string basicLayoutRegex(LEVEL_REGEX + " : " + TEST_MESSAGE_REGEX + "\n?");

decltype(ARES_INFO()) stream(int type)
{
    if (type == 0) return ARES_INFO();
    if (type == 1) return ARES_WARNING();
    return ARES_ERROR();
}

void loopTrace(int type)
{
    auto&& threadId = std::this_thread::get_id();
    for (auto i = 1u; i <= TRACES_PER_THREAD; ++i)
    {
        stream(type) << boost::format(TEST_MESSAGE_TEMPLATE) % i % threadId;
    }
}

class MockLogger : public trace::abc::Logger
{
    public:
        MOCK_METHOD2(log, void(const trace::LogContext&, const std::string&));
};

class EncapsulatedLogger : public trace::abc::Logger
{
    public:
        EncapsulatedLogger(trace::abc::Logger& logger)
            : m_logger(logger)
        {}

        void log(const trace::LogContext& context, const std::string& message) override {
            m_logger.log(context, message);
        }

    private:
        trace::abc::Logger& m_logger;
};

}

TEST(CoreTraceTest, TracesLayoutAuxiliaryLoggerAndThreadSafe)
{
    // Set the auxiliary logger to a mock object and expect calls on it (info traces are not logged into auxiliary)
    MockLogger mockLogger;
    trace::Facade::initializeAuxiliaryLogger(trace::LoggerPtr(new EncapsulatedLogger(mockLogger)));
#if GTEST_USES_POSIX_RE
    auto auxiliaryMessageMatcher = ::testing::MatchesRegex(basicLayoutRegex);
#else
    auto auxiliaryMessageMatcher = ::testing::_;
#endif
    EXPECT_CALL(mockLogger, log(::testing::_, auxiliaryMessageMatcher))
        .Times((THREADS_COUNT - INFO_THREADS_COUNT) * TRACES_PER_THREAD);

    // Start threads that will trace
    std::list<std::future<void>> futureList;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 2);
    for (auto i = 0u; i < THREADS_COUNT; ++i) {
        int type = 0;
        // The first INFO_THREADS_COUNT threads will log only info traces, the rest will choose between warning and error
        if (i >= INFO_THREADS_COUNT) {
            type = dis(gen);
        }
        futureList.push_back(std::move(std::async(std::launch::async, loopTrace, type)));
    }

    /* Wait for all threads to end (note: on visual studio 2012, the destructor of std::future
       does not block as it should, this is a bug. Instead we have to call wait explicitly) */
    for (auto&& future : futureList) {
        future.wait();
    }

    // Open the log output
    std::ifstream logFile;
    logFile.open(utils::homePath() + std::string("/AresBWAPI.log"));

    // Read each line and check if they match the layout
    std::string line;
    unsigned int count = 0;
    while (std::getline(logFile, line))
    {
#if GTEST_USES_POSIX_RE
        ASSERT_THAT(line, ::testing::MatchesRegex(completeLayoutRegex));
#endif
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


