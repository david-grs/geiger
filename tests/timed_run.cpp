#include "geiger/geiger.h"

#include <gtest/gtest.h>

#include <thread>
#include <chrono>

struct TimedRun : public ::testing::TestWithParam<std::chrono::milliseconds>
{
    using clock = std::chrono::high_resolution_clock;

    static void SetUpTestCase()
    {
        geiger::init();
    }

    static void TearDownTestCase() {}

    void SetUp() override
    {
        m_start = clock::now();
    }

    void TearDown() override {}

    auto ms_elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - m_start);
    }

protected:
    clock::time_point m_start;
    geiger::suite<> m_suite;
};

TEST_P(TimedRun, OneIteration__SameDuration)
{
    m_suite.add("sleep", [this]() { std::this_thread::sleep_for(GetParam()); });
    m_suite.run(1);

    ASSERT_EQ(GetParam().count(), ms_elapsed().count());
}

TEST_P(TimedRun, TwoIterations__TwiceLonger)
{
    m_suite.add("sleep", [this]() { std::this_thread::sleep_for(GetParam()); });
    m_suite.run(2);

    ASSERT_EQ(GetParam().count() * 2, ms_elapsed().count());
}

TEST_P(TimedRun, OneSecond__OneSecondIfShorter)
{
    m_suite.add("sleep", [this]() { std::this_thread::sleep_for(GetParam()); });
    m_suite.run(std::chrono::seconds(1));

    auto test_duration = GetParam();

    if (test_duration < std::chrono::seconds(1))
        ASSERT_NEAR(1000, ms_elapsed().count(), 100);
    else
        ASSERT_NEAR(test_duration.count(), ms_elapsed().count(), 100);
}

INSTANTIATE_TEST_CASE_P(FewTestDurations,
                        TimedRun,
                        ::testing::Values(std::chrono::milliseconds(1),
                                          std::chrono::milliseconds(9),
                                          std::chrono::milliseconds(85),
                                          std::chrono::milliseconds(357),
                                          std::chrono::milliseconds(901),
                                          std::chrono::milliseconds(2155)));

